#pragma once

#include <algorithm>
#include <functional>

#include "AbstractComponent.hpp"

#include "../Utility.hpp"
#include "../renderer/LightWeightRenderer.hpp"
#include "compUtils/LightWeightDummy.hpp"
#include "compUtils/Transform.hpp"
#include "layoutCalc/LayoutData.hpp"

namespace components::computils
{
class ScrollBar : public AbstractComponent
{
public:
    struct Options
    {
        layoutcalc::LdOrientation orientation{layoutcalc::LdOrientation::Horizontal};
        int barSize{20};
    };

    ScrollBar(const std::function<void(const int x, const int y)> scrollCb);

    void onPrepareToRender() override;
    void onRenderDone() override;
    void onMoveEvent() override;
    void onClickEvent() override;

    int adjustKnob(const int x, const int y);
    void notifyLayoutHasChanged();

    void show(const glm::mat4& projMatrix);

    void updateOverflow(int newOverflow);

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
    renderer::LightWeightRenderer lwr;
    // computils::LightWeightDummy bgd;
    computils::LightWeightDummy knobd;
};
} // namespace components::computils