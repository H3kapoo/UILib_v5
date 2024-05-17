#pragma once

#include <cstdint>

#include "AbstractComponent.hpp"

#include "../Utility.hpp"
#include "../renderer/LightWeightRenderer.hpp"
#include "compUtils/LightWeightDummy.hpp"
#include "layoutCalc/LayoutData.hpp"

namespace components::computils
{
class ScrollBar : public AbstractComponent
{
public:
    struct Options
    {
        layoutcalc::LdOrientation orientation{layoutcalc::LdOrientation::Horizontal};
        int barSize{11};
    };

    ScrollBar();

    void onPrepareToRender() override;
    void onRenderDone() override;
    void onMoveEvent() override;
    void onClickEvent() override;
    void onStart() override;
    void onScroll() override;

    int adjustKnobOnMouseEvent(const int x, const int y);
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
    bool updateDueToResize{true};
    float knobPercentageAlongBg{0};
    float mouseOffset{0};
    float scrollSensitivity{10.0f};
    int8_t knobInset{2};

    renderer::LightWeightRenderer lwr;
    computils::LightWeightDummy knob;
};
} // namespace components::computils