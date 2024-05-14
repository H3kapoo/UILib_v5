#include "ScrollBar.hpp"

namespace components::computils
{

ScrollBar::ScrollBar(const std::function<void(const int x, const int y)> scrollCb)
    : AbstractComponent({.type = CompType::ScrollBar,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl"})
    , scrollViewCb{scrollCb}
    , knobd("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
{
    knobd.options.color = utils::hexToVec4("#1eb635ff");
}

void ScrollBar::onPrepareToRender()
{
    glm::vec4 color = utils::hexToVec4("#970f97ff");
    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uColor", color);
}

void ScrollBar::onRenderDone()
{
    if (!isActive) { return; }
    lwr.render(getState()->projectionMatrix, knobd);
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

int ScrollBar::adjustKnob(const int x, const int y)
{
    const auto& thisTransform = getTransformRead();
    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        const auto sPos = thisTransform.pos.x;
        const auto ePos = sPos + thisTransform.scale.x - options.barSize;

        knobd.transform.pos.x = x - options.barSize / 2.0f;
        knobd.transform.pos.x = std::clamp(knobd.transform.pos.x, sPos, ePos);

        scrollValue = utils::remap(knobd.transform.pos.x, sPos, ePos, 0.0f, overflow);
        scrollViewCb(scrollValue, 0);
    }
    else if (options.orientation == layoutcalc::LdOrientation::Vertical)
    {
        const auto sPos = thisTransform.pos.y;
        const auto ePos = sPos + thisTransform.scale.y - options.barSize;

        knobd.transform.pos.y = y - options.barSize / 2.0f;
        knobd.transform.pos.y = std::clamp(knobd.transform.pos.y, sPos, ePos);

        scrollValue = utils::remap(knobd.transform.pos.y, sPos, ePos, 0.0f, overflow);
        scrollViewCb(0, scrollValue);
    }
    knobd.transform.markDirty();

    return 0;
}

void ScrollBar::notifyLayoutHasChanged()
{
    if (!isActive) { return; }

    // Really should be done just once, but eh. We never know how root will change it's layer
    manuallyAdjustDepthTo(getParent()->getDepth() + 128);
    knobd.transform.layer = getDepth() + 1;

    const auto& parentTransform = getParent()->getTransformRead();
    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        /* Place background bar */
        getTransformRW().pos = {
            parentTransform.pos.x, parentTransform.pos.y + parentTransform.scale.y - options.barSize};
        getTransformRW().scale = {parentTransform.scale.x, options.barSize};

        /* Calculate position and scale for knob to be in bounds */
        const auto sPos = getTransformRW().pos.x;
        const auto ePos = sPos + getTransformRW().scale.x - options.barSize;
        knobd.transform.pos = {std::clamp(knobd.transform.pos.x, sPos, ePos), getTransformRW().pos.y};
        knobd.transform.scale = {options.barSize, getTransformRW().scale.y};

        /* Value needs to be recalculated here based on new overflow value and knob position. It kinda scrolls by
        itself when container is resized. */
        scrollValue = utils::remap(knobd.transform.pos.x, sPos, ePos, 0.0f, overflow);
    }
    else
    {
        /* Place background bar */
        getTransformRW().pos = {
            parentTransform.pos.x + parentTransform.scale.x - options.barSize, parentTransform.pos.y};
        getTransformRW().scale = {options.barSize, parentTransform.scale.y};

        /* Calculate position and scale for knob to be in bounds */
        const auto sPos = getTransformRW().pos.y;
        const auto ePos = sPos + getTransformRW().scale.y - options.barSize;
        knobd.transform.pos = {getTransformRW().pos.x, std::clamp(knobd.transform.pos.y, sPos, ePos)};
        knobd.transform.scale = {getTransformRW().scale.x, options.barSize};

        /* Value needs to be recalculated here based on new overflow value and knob position. It kinda scrolls by
        itself when container is resized. */
        scrollValue = utils::remap(knobd.transform.pos.y, sPos, ePos, 0.0f, overflow);
    }

    knobd.transform.markDirty();
}

void ScrollBar::updateOverflow(int newOverflow)
{
    // return;
    if (newOverflow > 0)
    {
        setActive();
        notifyLayoutHasChanged();
        // if (newOverflow != overflow)
        {
            if (options.orientation == layoutcalc::LdOrientation::Horizontal) { scrollViewCb(scrollValue, 0); }
            else if (options.orientation == layoutcalc::LdOrientation::Vertical) { scrollViewCb(0, scrollValue); }
        }
    }
    else
    {
        setInactive();
        knobd.transform.pos = getTransformRW().pos;
        scrollValue = 0;
    }
    overflow = newOverflow;
}

void ScrollBar::setInactive()
{
    isActive = false;
}

void ScrollBar::setActive()
{
    isActive = true;
}

bool ScrollBar::isBarActive()
{
    return isActive;
}
} // namespace components::computils