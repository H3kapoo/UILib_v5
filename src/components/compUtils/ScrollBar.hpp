#pragma once

#include <algorithm>

#include "../../Utility.hpp"
#include "../../renderer/LightWeightRenderer.hpp"
#include "../layoutCalc/LayoutData.hpp"
#include "LightWeightDummy.hpp"
#include "Transform.hpp"

namespace components::computils
{
class ScrollBar
{
public:
    struct Options
    {
        layoutcalc::LdOrientation orientation{layoutcalc::LdOrientation::Horizontal};
        int barSize{10};
    };

    ScrollBar(const computils::Transform& root)
        : rootTransform{root}
        , bgd("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
        , knobd("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
    {
        bgd.options.color = utils::hexToVec4("#970f97ff");
        knobd.options.color = utils::hexToVec4("#1eb635ff");
    }

    bool onMouseMove(const int x, const int y)
    {
        if (!isActive) { return false; }
        if (!isDragging) { return false; }

        currX = x;
        currY = y;
        if (options.orientation == layoutcalc::LdOrientation::Horizontal)
        {
            const auto sPos = bgd.transform.pos.x;
            const auto ePos = sPos + bgd.transform.scale.x;
            scrollValue = utils::remap(knobd.transform.pos.x, sPos, ePos - options.barSize, 0.0f, overflow);
            utils::printlne("Scroll value {}", scrollValue);
        }

        prevX = x;
        prevY = y;
        // int deltaY = y - prevY;
        return true;
    }

    bool onMouseClick(const int x, const int y)
    {
        if (!isActive) { return false; }

        prevX = x;
        prevY = y;
        currX = x;
        currY = y;
        bool xConstraint = x >= bgd.transform.pos.x && x <= bgd.transform.pos.x + bgd.transform.scale.x;
        bool yConstraint = y >= bgd.transform.pos.y && y <= bgd.transform.pos.y + bgd.transform.scale.y;
        if (xConstraint && yConstraint)
        {
            utils::printlni("Clicked on scrollbar {} {}", x, y);
            knobd.transform.pos.x = x;
            const auto sPos = bgd.transform.pos.x;
            const auto ePos = sPos + bgd.transform.scale.x;
            scrollValue = utils::remap(knobd.transform.pos.x, sPos, ePos - options.barSize, 0.0f, overflow);
            utils::printlne("Scroll value {}", scrollValue);
            isDragging = true;
            return true;
        }
        return false;
    }

    void onMouseRelease()
    {
        if (!isActive) { return; }

        isDragging = false;
    }

    void onLayoutUpdate()
    {
        if (!isActive) { return; }

        // Really should be done just once, but eh. We never know how root will change it's layer
        bgd.transform.layer = rootTransform.layer + 1;
        knobd.transform.layer = bgd.transform.layer + 1;

        if (options.orientation == layoutcalc::LdOrientation::Horizontal)
        {
            bgd.transform.pos = {rootTransform.pos.x, rootTransform.pos.y + rootTransform.scale.y - options.barSize};
            bgd.transform.scale = {rootTransform.scale.x, options.barSize};

            const auto sPos = bgd.transform.pos.x;
            const auto ePos = sPos + bgd.transform.scale.x;
            utils::printlnw("Max of: {}", overflow);

            knobd.transform.pos = {currX, bgd.transform.pos.y};
            knobd.transform.scale = {options.barSize, bgd.transform.scale.y};

            knobd.transform.pos.x = std::clamp(knobd.transform.pos.x, sPos, ePos - options.barSize);
        }
        else
        {
            bgd.transform.pos = {rootTransform.pos.x + rootTransform.scale.x - options.barSize, rootTransform.pos.y};
            bgd.transform.scale = {options.barSize, rootTransform.scale.y};
        }

        bgd.transform.markDirty();
        knobd.transform.markDirty();
    }

    void show(const glm::mat4& projMatrix)
    {
        if (!isActive) { return; }
        lwr.render(projMatrix, bgd);
        lwr.render(projMatrix, knobd);
    }

    void setOverflow(int overflowVal)
    {
        overflow = overflowVal;
    }

    void setInactive()
    {
        isActive = false;
    }

    void setActive()
    {
        isActive = true;
    }

    int getScrollValue()
    {
        return scrollValue;
    }

    bool isBarActive()
    {
        return isActive;
    }

    Options options;

private:
    int overflow{0};
    int scrollValue{0};
    bool isDragging{false};
    bool isActive{false};
    int prevX{0}, prevY{0};
    int currX{0}, currY{0};
    float sensitivity{0.1f};

    const computils::Transform& rootTransform;
    renderer::LightWeightRenderer lwr;
    computils::LightWeightDummy bgd;
    computils::LightWeightDummy knobd;
};
} // namespace components::computils