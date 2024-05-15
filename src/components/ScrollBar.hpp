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

    ScrollBar(const bool fillerIfNeeded = false);

    void onPrepareToRender() override;
    void onRenderDone() override;
    void onMoveEvent() override;
    void onClickEvent() override;
    void onStart() override;

    int adjustKnob(const int x, const int y);
    void notifyLayoutHasChanged();

    void show(const glm::mat4& projMatrix);

    void updateOverflow(int newOverflow);

    void setOppositeScrollBarActive();
    void setOppositeScrollBarInactive();

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
    bool isOppositeActive{false};
    bool isFillerIfNeeded{false};
    bool updateDueToResize{true};
    float knobPercentageAlongBg{0};

    renderer::LightWeightRenderer lwr;
    computils::LightWeightDummy knob;
    computils::LightWeightDummy cornerFiller;
};
} // namespace components::computils