#include "ScrollBar.hpp"

namespace components::computils
{

ScrollBar::ScrollBar(const bool fillerIfNeeded)
    : AbstractComponent({.type = CompType::ScrollBar,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl"})
    , isFillerIfNeeded{fillerIfNeeded}
    , knob("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
    , cornerFiller("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
{
    knob.options.color = utils::hexToVec4("#1eb635ff");
    cornerFiller.options.color = utils::hexToVec4("#d4d4d4ff");
}

void ScrollBar::onPrepareToRender()
{
    // glm::vec4 color = utils::hexToVec4("#970f9739");
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.3f, .3f);
    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uColor", color);
}

void ScrollBar::onRenderDone()
{
    if (!isActive) { return; }
    lwr.render(getState()->projectionMatrix, knob);

    if (!isOppositeActive || !isFillerIfNeeded) { return; }
    lwr.render(getState()->projectionMatrix, cornerFiller);
}

void ScrollBar::onMoveEvent()
{
    if (!isActive) { return; }
    if (!isDragging) { return; }

    const auto& s = getState();
    adjustKnob(s->mouseX, s->mouseY);
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

        adjustKnob(s->mouseX, s->mouseY);
    }
    else
    {
        if (!isActive) { return; }
        isDragging = false;
    }
}

void ScrollBar::onStart()
{
    manuallyAdjustDepthTo(getParent()->getDepth() + 128);
    knob.transform.layer = getDepth() + 1;
}

int ScrollBar::adjustKnob(const int x, const int y)
{
    const auto& thisTransform = getTransformRead();
    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        const auto sPos = thisTransform.pos.x;
        const auto ePos = sPos + thisTransform.scale.x - options.barSize;

        knob.transform.pos.x = x - options.barSize / 2.0f;
        knob.transform.pos.x = std::clamp(knob.transform.pos.x, sPos, ePos);

        scrollValue = utils::remap(knob.transform.pos.x, sPos, ePos, 0.0f, overflow);
    }
    else if (options.orientation == layoutcalc::LdOrientation::Vertical)
    {
        const auto sPos = thisTransform.pos.y;
        const auto ePos = sPos + thisTransform.scale.y - options.barSize;

        knob.transform.pos.y = y - options.barSize / 2.0f;
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

    // // Really should be done just once, but eh. We never know how root will change it's layer

    const auto& parentTransform = getParent()->getTransformRead();
    const int oppositeBarSubtract = isOppositeActive ? options.barSize : 0;
    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        /* Place background bar */
        getTransformRW().pos = {
            parentTransform.pos.x, parentTransform.pos.y + parentTransform.scale.y - options.barSize};
        getTransformRW().scale = {parentTransform.scale.x - oppositeBarSubtract, options.barSize};

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

        /* Calculate position and scale for knob to be in bounds */
        const auto sPos = getTransformRW().pos.x;
        const auto ePos = sPos + getTransformRW().scale.x - options.barSize;
        knob.transform.pos = {std::clamp(knob.transform.pos.x, sPos, ePos), getTransformRW().pos.y};
        knob.transform.scale = {options.barSize, getTransformRW().scale.y};

        /* Value needs to be recalculated here based on new overflow value and knob position. It kinda scrolls by
        itself when container is resized. */
        scrollValue = utils::remap(knob.transform.pos.x, sPos, ePos, 0.0f, overflow);
    }
    else
    {
        /* Place background bar */
        getTransformRW().pos = {
            parentTransform.pos.x + parentTransform.scale.x - options.barSize, parentTransform.pos.y};
        getTransformRW().scale = {options.barSize, parentTransform.scale.y - oppositeBarSubtract};

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

        /* Calculate position and scale for knob to be in bounds */
        const auto sPos = getTransformRW().pos.y;
        const auto ePos = sPos + getTransformRW().scale.y - options.barSize;
        knob.transform.pos = {getTransformRW().pos.x, std::clamp(knob.transform.pos.y, sPos, ePos)};
        knob.transform.scale = {getTransformRW().scale.x, options.barSize};

        /* Value needs to be recalculated here based on new overflow value and knob position. It kinda scrolls by
        itself when container is resized. */
        scrollValue = utils::remap(knob.transform.pos.y, sPos, ePos, 0.0f, overflow);
    }

    if (isOppositeActive && isFillerIfNeeded)
    {
        const auto sPos = getTransformRW().pos.x + getTransformRW().scale.x;
        cornerFiller.transform.pos = {sPos, getTransformRW().pos.y};
        cornerFiller.transform.scale = {options.barSize, options.barSize};
    }

    updateDueToResize = true;

    cornerFiller.transform.markDirty();
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