#include "ScrollBar.hpp"

#include "layoutCalc/LayoutData.hpp"
#include <cstdint>

namespace components::computils
{

ScrollBar::ScrollBar()
    : AbstractComponent({.type = CompType::ScrollBar,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl"})
    , knob("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
{
    // knob.options.color = utils::hexToVec4("#C0C0C1ff");
    knob.options.color = utils::hexToVec4("#3E3D3Dff");
}

void ScrollBar::onPrepareToRender()
{
    glm::vec4 color = utils::hexToVec4("#252525ff");
    // glm::vec4 color = utils::hexToVec4("#F1F1F1ff");
    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uColor", color);
}

void ScrollBar::onRenderDone()
{
    if (!isActive) { return; }
    lwr.render(getState()->projectionMatrix, knob);
}

void ScrollBar::onMoveEvent()
{
    if (!isActive) { return; }
    if (!isDragging) { return; }

    const auto& s = getState();
    adjustKnobOnMouseEvent(s->mouseX, s->mouseY);
}

void ScrollBar::onClickEvent()
{
    if (!isActive) { return; }

    const auto& s = getState();
    if (s->mouseAction == HIDAction::Pressed && s->clickedButton == MouseButton::Left)
    {
        const auto& thisTransform = getTransformRead();
        bool xConstraint = s->mouseX >= thisTransform.pos.x && s->mouseX <= thisTransform.pos.x + thisTransform.scale.x;
        bool yConstraint = s->mouseY >= thisTransform.pos.y && s->mouseY <= thisTransform.pos.y + thisTransform.scale.y;

        if (!xConstraint || !yConstraint) { return; }
        isDragging = true;

        bool insideKnobX = s->mouseX >= knob.transform.pos.x &&
                           s->mouseX <= knob.transform.pos.x + knob.transform.scale.x;
        bool insideKnobY = s->mouseY >= knob.transform.pos.y &&
                           s->mouseY <= knob.transform.pos.y + knob.transform.scale.y;
        if (options.orientation == layoutcalc::LdOrientation::Horizontal)
        {
            if (insideKnobX && insideKnobY) { mouseOffset = s->mouseX - knob.transform.pos.x; }
            else { mouseOffset = knob.transform.scale.x / 2.0f; }
        }
        else if (options.orientation == layoutcalc::LdOrientation::Vertical)
        {
            if (insideKnobX && insideKnobY) { mouseOffset = s->mouseY - knob.transform.pos.y; }
            else { mouseOffset = knob.transform.scale.y / 2.0f; }
        }

        adjustKnobOnMouseEvent(s->mouseX, s->mouseY);
    }
    else
    {
        if (!isActive) { return; }
        isDragging = false;
        mouseOffset = 0;
    }
}

void ScrollBar::onScroll()
{
    if (!isActive) { return; }

    const auto& thisTransform = getTransformRead();
    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        const auto knobSizeX = std::max(options.barSize, (int16_t)(getTransformRW().scale.x - overflow));
        const auto sPos = thisTransform.pos.x + options.knobInset;
        const auto ePos = sPos + thisTransform.scale.x - (knobSizeX + options.knobInset);

        knob.transform.pos.x -= getState()->scrollDirection * options.scrollSensitivity;
        knob.transform.pos.x = std::clamp(knob.transform.pos.x, sPos, ePos);

        scrollValue = utils::remap(knob.transform.pos.x, sPos, ePos, 0.0f, overflow);
    }
    else if (options.orientation == layoutcalc::LdOrientation::Vertical)
    {
        const auto knobSizeY = std::max(options.barSize, (int16_t)(getTransformRW().scale.y - overflow));
        const int niceifyCornerOffset = isOppositeActive ? options.knobInset : 0;
        const auto sPos = thisTransform.pos.y + options.knobInset;
        const auto ePos = sPos + thisTransform.scale.y - (knobSizeY - niceifyCornerOffset);

        knob.transform.pos.y -= getState()->scrollDirection * options.scrollSensitivity;
        knob.transform.pos.y = std::clamp(knob.transform.pos.y, sPos, ePos);

        scrollValue = utils::remap(knob.transform.pos.y, sPos, ePos, 0.0f, overflow);
    }

    knob.transform.markDirty();

    updateDueToResize = false;
    updateLayout();
}

void ScrollBar::onStart()
{
    /* Note: In order for the scrollbars to render correctly, they need to be rendered in a "above playground" depth
     setting. SB_DEPTH_COUNTUP value was arbitrarly choosen. Otherwise if we don't do this trick, it may happen that
     scrollbars are rendered on top of another one or that an element will render above another scrollbar. This
     eliminates those problems with minimal overhead. */
    static int16_t SB_DEPTH_COUNTUP = 512;
    manuallyAdjustDepthTo(++SB_DEPTH_COUNTUP);
    knob.transform.layer = getDepth() + 1;
}

void ScrollBar::adjustKnobOnMouseEvent(const int x, const int y)
{
    const auto& thisTransform = getTransformRead();
    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        const auto knobSizeX = std::max(options.barSize, (int16_t)(getTransformRW().scale.x - overflow));
        const auto sPos = thisTransform.pos.x + options.knobInset;
        const auto ePos = sPos + thisTransform.scale.x - (knobSizeX + options.knobInset);

        knob.transform.pos.x = x - mouseOffset;
        knob.transform.pos.x = std::clamp(knob.transform.pos.x, sPos, ePos);

        scrollValue = utils::remap(knob.transform.pos.x, sPos, ePos, 0.0f, overflow);
    }
    else if (options.orientation == layoutcalc::LdOrientation::Vertical)
    {
        const auto knobSizeY = std::max(options.barSize, (int16_t)(getTransformRW().scale.y - overflow));
        const int niceifyCornerOffset = isOppositeActive ? options.knobInset : 0;
        const auto sPos = thisTransform.pos.y + options.knobInset;
        const auto ePos = sPos + thisTransform.scale.y - (knobSizeY - niceifyCornerOffset);

        knob.transform.pos.y = y - mouseOffset;
        knob.transform.pos.y = std::clamp(knob.transform.pos.y, sPos, ePos);

        scrollValue = utils::remap(knob.transform.pos.y, sPos, ePos, 0.0f, overflow);
    }

    updateDueToResize = false;
    updateLayout();

    knob.transform.markDirty();
}

void ScrollBar::notifyLayoutHasChanged()
{
    if (!isActive) { return; }

    const auto& parentTransform = getParent()->getTransformRead();
    const auto& parentLayout = getParent()->layout;
    const auto rootLeftBorder = parentLayout.borderSize.left;
    const auto rootRightBorder = parentLayout.borderSize.right;
    const auto rootTopBorder = parentLayout.borderSize.top;
    const auto rootBotBorder = parentLayout.borderSize.bottom;

    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        /* Place background bar */
        getTransformRW().pos = {parentTransform.pos.x + rootLeftBorder,
            parentTransform.pos.y + parentTransform.scale.y - options.barSize - rootBotBorder};
        getTransformRW().scale = {parentTransform.scale.x - (rootRightBorder + rootLeftBorder), options.barSize};

        const auto knobSizeX = std::max(options.barSize, (int16_t)(getTransformRW().scale.x - overflow));

        /* On component resize, knob position along the background scrollbar should be preserved */
        if (updateDueToResize)
        {
            knob.transform.pos = {
                getTransformRead().pos.x + (getTransformRead().scale.x - knobSizeX) * knobPercentageAlongBg,
                getTransformRead().pos.y};
        }
        else
        {
            const auto bgLen = getTransformRW().scale.x - knobSizeX;
            const auto knobPos = (float)knob.transform.pos.x - getTransformRW().pos.x;
            knobPercentageAlongBg = knobPos / bgLen;
        }

        /* Calculate position and scale for knob to be in bounds */
        const auto sPos = getTransformRW().pos.x + options.knobInset;
        const auto ePos = sPos + getTransformRW().scale.x - (knobSizeX + options.knobInset);
        knob.transform.pos = {std::clamp(knob.transform.pos.x, sPos, ePos), getTransformRW().pos.y + options.knobInset};
        knob.transform.scale = {knobSizeX - options.knobInset, getTransformRW().scale.y - options.knobInset * 2};

        /* Value needs to be recalculated here based on new overflow value and knob position. It kinda scrolls by
        itself when container is resized. */
        scrollValue = utils::remap(knob.transform.pos.x, sPos, ePos, 0.0f, overflow);
    }
    else if (options.orientation == layoutcalc::LdOrientation::Vertical)
    {

        /* Place background bar */
        const int oppositeBarSubtract = isOppositeActive ? options.barSize : 0;
        getTransformRW().pos = {parentTransform.pos.x + parentTransform.scale.x - (options.barSize + rootRightBorder),
            parentTransform.pos.y + rootTopBorder};
        getTransformRW().scale = {
            options.barSize, parentTransform.scale.y - (oppositeBarSubtract + rootTopBorder + rootBotBorder)};

        const auto knobSizeY = std::max(options.barSize, (int16_t)(getTransformRW().scale.y - overflow));

        /* On component resize, knob position along the background scrollbar should be preserved */
        if (updateDueToResize)
        {
            knob.transform.pos = {getTransformRW().pos.x,
                getTransformRead().pos.y + (getTransformRead().scale.y - knobSizeY) * knobPercentageAlongBg};
        }
        else
        {
            const auto bgLen = getTransformRW().scale.y - knobSizeY;
            const auto knobPos = (float)knob.transform.pos.y - getTransformRW().pos.y;
            knobPercentageAlongBg = knobPos / bgLen;
        }

        /* Calculate position and scale for knob to be in bounds */
        const int niceifyCornerOffset = isOppositeActive ? options.knobInset : 0;
        const auto sPos = getTransformRW().pos.y + options.knobInset;
        const auto ePos = sPos + getTransformRW().scale.y - (knobSizeY - niceifyCornerOffset);
        knob.transform.pos = {getTransformRW().pos.x + options.knobInset, std::clamp(knob.transform.pos.y, sPos, ePos)};
        knob.transform.scale = {getTransformRW().scale.x - options.knobInset * 2, knobSizeY - options.knobInset};

        /* Value needs to be recalculated here based on new overflow value and knob position. It kinda scrolls by
        itself when container is resized. */
        scrollValue = utils::remap(knob.transform.pos.y, sPos, ePos, 0.0f, overflow);
    }

    updateDueToResize = true;
    knob.transform.markDirty();
}

void ScrollBar::updateOverflow(int newOverflow)
{
    notifyLayoutHasChanged();
    overflow = newOverflow;
}

void ScrollBar::setInactive()
{
    isActive = false;
    scrollValue = 0;
    knobPercentageAlongBg = 0;
    knob.transform.pos = getTransformRW().pos;
    knob.transform.markDirty();
}

void ScrollBar::setActive()
{
    isActive = true;
}

bool ScrollBar::isBarActive()
{
    return isActive;
}

void ScrollBar::setOppositeScrollBarActive()
{
    isOppositeActive = true;
}

void ScrollBar::setOppositeScrollBarInactive()
{
    isOppositeActive = false;
}

int16_t ScrollBar::getScrollValue()
{
    return scrollValue;
}
} // namespace components::computils