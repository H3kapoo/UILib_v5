#include "ScrollBar.hpp"

namespace components::computils
{

ScrollBar::ScrollBar(const computils::Transform& root, const std::function<void(const int x, const int y)> scrollCb)
    : scrollViewCb{scrollCb}
    , rootTransform{root}
    , bgd("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
    , knobd("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
{
    bgd.options.color = utils::hexToVec4("#970f97ff");
    knobd.options.color = utils::hexToVec4("#1eb635ff");
}

int ScrollBar::adjustKnob(const int x, const int y)
{
    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        const auto sPos = bgd.transform.pos.x;
        const auto ePos = sPos + bgd.transform.scale.x - options.barSize;

        knobd.transform.pos.x = x - options.barSize / 2.0f;
        knobd.transform.pos.x = std::clamp(knobd.transform.pos.x, sPos, ePos);

        const int oldScrollValue = scrollValue;
        scrollValue = utils::remap(knobd.transform.pos.x, sPos, ePos, 0.0f, overflow);
        scrollViewCb(scrollValue - oldScrollValue, 0);
    }
    else if (options.orientation == layoutcalc::LdOrientation::Vertical)
    {
        const auto sPos = bgd.transform.pos.y;
        const auto ePos = sPos + bgd.transform.scale.y - options.barSize;

        knobd.transform.pos.y = y - options.barSize / 2.0f;
        knobd.transform.pos.y = std::clamp(knobd.transform.pos.y, sPos, ePos);

        const int oldScrollValue = scrollValue;
        scrollValue = utils::remap(knobd.transform.pos.y, sPos, ePos, 0.0f, overflow);
        scrollViewCb(0, scrollValue - oldScrollValue);
    }
    knobd.transform.markDirty();

    return 0;
}

bool ScrollBar::onMouseMove(const int x, const int y)
{
    if (!isActive) { return false; }
    if (!isDragging) { return false; }

    adjustKnob(x, y);

    return true;
}

bool ScrollBar::onMouseClick(const int x, const int y)
{
    if (!isActive) { return false; }

    bool xConstraint = x >= bgd.transform.pos.x && x <= bgd.transform.pos.x + bgd.transform.scale.x;
    bool yConstraint = y >= bgd.transform.pos.y && y <= bgd.transform.pos.y + bgd.transform.scale.y;

    if (!xConstraint || !yConstraint) { return false; }
    isDragging = true;

    adjustKnob(x, y);

    return true;
}

void ScrollBar::onMouseRelease()
{
    if (!isActive) { return; }
    isDragging = false;
}

void ScrollBar::notifyLayoutHasChanged()
{
    if (!isActive) { return; }

    // Really should be done just once, but eh. We never know how root will change it's layer
    bgd.transform.layer = rootTransform.layer + 1;
    knobd.transform.layer = bgd.transform.layer + 1;

    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        /* Place background bar */
        bgd.transform.pos = {rootTransform.pos.x, rootTransform.pos.y + rootTransform.scale.y - options.barSize};
        bgd.transform.scale = {rootTransform.scale.x, options.barSize};

        /* Calculate position and scale for knob to be in bounds */
        const auto sPos = bgd.transform.pos.x;
        const auto ePos = sPos + bgd.transform.scale.x - options.barSize;
        knobd.transform.pos = {std::clamp(knobd.transform.pos.x, sPos, ePos), bgd.transform.pos.y};
        knobd.transform.scale = {options.barSize, bgd.transform.scale.y};

        /* Value needs to be recalculated here based on new overflow value and knob position. It kinda scrolls by itself
         * when container is resized. */
        scrollValue = utils::remap(knobd.transform.pos.x, sPos, ePos, 0.0f, overflow);
    }
    else
    {
        /* Place background bar */
        bgd.transform.pos = {rootTransform.pos.x + rootTransform.scale.x - options.barSize, rootTransform.pos.y};
        bgd.transform.scale = {options.barSize, rootTransform.scale.y};

        /* Calculate position and scale for knob to be in bounds */
        const auto sPos = bgd.transform.pos.y;
        const auto ePos = sPos + bgd.transform.scale.y - options.barSize;
        knobd.transform.pos = {bgd.transform.pos.x, std::clamp(knobd.transform.pos.y, sPos, ePos)};
        knobd.transform.scale = {bgd.transform.scale.x, options.barSize};

        /* Value needs to be recalculated here based on new overflow value and knob position. It kinda scrolls by itself
         * when container is resized. */
        scrollValue = utils::remap(knobd.transform.pos.y, sPos, ePos, 0.0f, overflow);
    }

    bgd.transform.markDirty();
    knobd.transform.markDirty();
}

void ScrollBar::updateOverflow(int newOverflow)
{
    if (newOverflow > 0)
    {
        setActive();
        notifyLayoutHasChanged();
        if (newOverflow != overflow)
        {
            if (options.orientation == layoutcalc::LdOrientation::Horizontal) { scrollViewCb(scrollValue, 0); }
            else if (options.orientation == layoutcalc::LdOrientation::Vertical) { scrollViewCb(0, scrollValue); }
        }
    }
    else
    {
        setInactive();
        knobd.transform.pos = bgd.transform.pos;
        scrollValue = 0;
    }
    overflow = newOverflow;
}

void ScrollBar::show(const glm::mat4& projMatrix)
{
    if (!isActive) { return; }
    lwr.render(projMatrix, bgd);
    lwr.render(projMatrix, knobd);
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