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

// void adjustKnob(const int newMousePos, const float& bgTransformPos, const float& bgTransformScale,
// ) {}

bool ScrollBar::onMouseMove(const int x, const int y)
{
    if (!isActive) { return false; }
    if (!isDragging) { return false; }

    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        const auto sPos = bgd.transform.pos.x;
        const auto ePos = sPos + bgd.transform.scale.x - options.barSize;

        knobd.transform.pos.x = x - options.barSize / 2.0f;
        knobd.transform.pos.x = std::clamp(knobd.transform.pos.x, sPos, ePos);
        knobd.transform.markDirty();

        // scrollValue = utils::remap(knobd.transform.pos.x, sPos, ePos, 0.0f, overflow);
    }

    return true;
}

bool ScrollBar::onMouseClick(const int x, const int y)
{
    if (!isActive) { return false; }

    bool xConstraint = x >= bgd.transform.pos.x && x <= bgd.transform.pos.x + bgd.transform.scale.x;
    bool yConstraint = y >= bgd.transform.pos.y && y <= bgd.transform.pos.y + bgd.transform.scale.y;
    if (!xConstraint || !yConstraint) { return false; }

    const auto sPos = bgd.transform.pos.x;
    const auto ePos = sPos + bgd.transform.scale.x - options.barSize;

    knobd.transform.pos.x = x - options.barSize / 2.0f;
    knobd.transform.pos.x = std::clamp(knobd.transform.pos.x, sPos, ePos);

    // scrollValue = utils::remap(knobd.transform.pos.x, sPos, ePos, 0.0f, overflow);
    // // utils::printlne("sPos {} ePos {} kp {} sv {}", sPos, ePos, knobd.transform.pos.x, scrollValue);
    isDragging = true;

    return true;
}

void ScrollBar::onMouseRelease()
{
    if (!isActive) { return; }

    isDragging = false;
}

void ScrollBar::setupLayout()
{
    if (!isActive) { return; }

    // Really should be done just once, but eh. We never know how root will change it's layer
    bgd.transform.layer = rootTransform.layer + 1;
    knobd.transform.layer = bgd.transform.layer + 1;

    if (options.orientation == layoutcalc::LdOrientation::Horizontal)
    {
        // background bar
        bgd.transform.pos = {rootTransform.pos.x, rootTransform.pos.y + rootTransform.scale.y - options.barSize};
        bgd.transform.scale = {rootTransform.scale.x, options.barSize};

        // knob itself
        const auto sPos = bgd.transform.pos.x;
        const auto ePos = sPos + bgd.transform.scale.x - options.barSize;
        knobd.transform.pos = {std::clamp(knobd.transform.pos.x, sPos, ePos), bgd.transform.pos.y};
        knobd.transform.scale = {options.barSize, bgd.transform.scale.y};

        scrollValue = utils::remap(knobd.transform.pos.x, sPos, ePos, 0.0f, overflow);
    }
    else
    {
        bgd.transform.pos = {rootTransform.pos.x + rootTransform.scale.x - options.barSize, rootTransform.pos.y};
        bgd.transform.scale = {options.barSize, rootTransform.scale.y};
    }

    bgd.transform.markDirty();
    knobd.transform.markDirty();
}

void ScrollBar::show(const glm::mat4& projMatrix)
{
    if (!isActive) { return; }
    lwr.render(projMatrix, bgd);
    lwr.render(projMatrix, knobd);
}

void ScrollBar::setOverflow(int newOverflow)
{
    if (newOverflow > 0) { setActive(); }
    else
    {
        setInactive();
        const auto sPos = bgd.transform.pos.x;
        knobd.transform.pos = {sPos, bgd.transform.pos.y};
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

int ScrollBar::getScrollValue()
{
    return scrollValue;
}

bool ScrollBar::isBarActive()
{
    return isActive;
}
} // namespace components::computils