#include "ScrollBar.hpp"
#include "layoutCalc/LayoutData.hpp"

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

    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        knob.transform.pos.x -= getState()->scrollDirection * scrollSensitivity;
        knob.transform.pos.x = std::clamp(knob.transform.pos.x, getTransformRead().pos.x,
            getTransformRead().pos.x + getTransformRead().scale.x);
    }
    else if (options.orientation == layoutcalc::LdOrientation::Vertical)
    {
        knob.transform.pos.y -= getState()->scrollDirection * scrollSensitivity;
        knob.transform.pos.y = std::clamp(knob.transform.pos.y, getTransformRead().pos.y,
            getTransformRead().pos.y + getTransformRead().scale.y);
    }

    knob.transform.markDirty();

    updateDueToResize = false;
    updateLayout();
}

void ScrollBar::onStart()
{
    manuallyAdjustDepthTo(getParent()->getDepth() + 128);
    knob.transform.layer = getDepth() + 1;
}

int ScrollBar::adjustKnobOnMouseEvent(const int x, const int y)
{
    /* Note/TODO: Normally 'knobInset' should be taken into account here as well to perfectly determine the knob
       position and scrollValue, however the value is so small we can get away without accounting for it for now.*/
    const auto& thisTransform = getTransformRead();
    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        const auto sPos = thisTransform.pos.x;
        const auto ePos = sPos + thisTransform.scale.x - knob.transform.scale.x;

        knob.transform.pos.x = x - mouseOffset;
        knob.transform.pos.x = std::clamp(knob.transform.pos.x, sPos, ePos);

        scrollValue = utils::remap(knob.transform.pos.x, sPos, ePos, 0.0f, overflow);
    }
    else if (options.orientation == layoutcalc::LdOrientation::Vertical)
    {
        const auto sPos = thisTransform.pos.y;
        const auto ePos = sPos + thisTransform.scale.y - knob.transform.scale.y;

        knob.transform.pos.y = y - mouseOffset;
        knob.transform.pos.y = std::clamp(knob.transform.pos.y, sPos, ePos);

        scrollValue = utils::remap(knob.transform.pos.y, sPos, ePos, 0.0f, overflow);
    }

    updateDueToResize = false;
    updateLayout();

    knob.transform.markDirty();

    return 0;
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

        /* On component resize, knob position along the background scrollbar should be preserved */
        if (updateDueToResize)
        {
            knob.transform.pos = {
                getTransformRead().pos.x + getTransformRead().scale.x * knobPercentageAlongBg, getTransformRW().pos.y};
        }
        else
        {
            const auto bgLen = getTransformRW().scale.x;
            const auto knobPos = (float)knob.transform.pos.x - getTransformRW().pos.x;
            knobPercentageAlongBg = knobPos / bgLen;
        }

        /*
            Bigger overflow -> smaller knob size
            Smaller overflow -> bigger knob size
        */
        const auto knobSizeX = std::max(options.barSize, (int)getTransformRW().scale.x - overflow);

        /* Calculate position and scale for knob to be in bounds */
        const auto sPos = getTransformRW().pos.x + knobInset;
        const auto ePos = sPos + getTransformRW().scale.x - (knobSizeX + knobInset);
        knob.transform.pos = {
            std::clamp(knob.transform.pos.x + knobInset, sPos, ePos), getTransformRW().pos.y + knobInset};
        knob.transform.scale = {knobSizeX - knobInset, getTransformRW().scale.y - knobInset * 2};

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

        /* On component resize, knob position along the background scrollbar should be preserved */
        if (updateDueToResize)
        {
            knob.transform.pos = {
                getTransformRW().pos.x, getTransformRead().pos.y + getTransformRead().scale.y * knobPercentageAlongBg};
        }
        else
        {
            const auto bgLen = getTransformRW().scale.y;
            const auto knobPos = (float)knob.transform.pos.y - getTransformRW().pos.y;
            knobPercentageAlongBg = knobPos / bgLen;
        }

        /*
            Bigger overflow -> smaller knob size
            Smaller overflow -> bigger knob size
        */
        const auto knobSizeY = std::max(options.barSize, (int)getTransformRW().scale.y - overflow);

        /* Calculate position and scale for knob to be in bounds */
        const int niceifyCornerOffset = isOppositeActive ? 0 : knobInset;
        const auto sPos = getTransformRW().pos.y + knobInset;
        const auto ePos = sPos + getTransformRW().scale.y - (knobSizeY + niceifyCornerOffset);
        knob.transform.pos = {
            getTransformRW().pos.x + knobInset, std::clamp(knob.transform.pos.y + knobInset, sPos, ePos)};
        knob.transform.scale = {getTransformRW().scale.x - knobInset * 2, knobSizeY - knobInset};

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

int ScrollBar::getScrollValue()
{
    return scrollValue;
}
} // namespace components::computils