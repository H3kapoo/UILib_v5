#pragma once

#include <algorithm>
#include <functional>

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
        int barSize{20};
    };

    ScrollBar(const computils::Transform& root, const std::function<void(const int x, const int y)> scrollCb);

    bool onMouseMove(const int x, const int y);
    bool onMouseClick(const int x, const int y);
    void onMouseRelease();

    void setupLayout();

    void show(const glm::mat4& projMatrix);

    void setOverflow(int newOverflow);

    void setInactive();
    void setActive();
    int getScrollValue();
    bool isBarActive();

    Options options;

private:
    int overflow{0};
    int scrollValue{0};
    bool isDragging{false};
    bool isActive{false};

    std::function<void(const int x, const int y)> scrollViewCb{nullptr};
    const computils::Transform& rootTransform;
    renderer::LightWeightRenderer lwr;
    computils::LightWeightDummy bgd;
    computils::LightWeightDummy knobd;
};
} // namespace components::computils