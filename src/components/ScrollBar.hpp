#pragma once

#include <cstdint>

#include "src/components/AbstractComponent.hpp"
#include "src/components/compUtils/LightWeightDummy.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"
#include "src/renderer/LightWeightRenderer.hpp"

namespace components::computils
{
class ScrollBar : public AbstractComponent
{
public:
    /* General options parameters associated with this component in particular */
    struct Options
    {
        layoutcalc::LdOrientation orientation{layoutcalc::LdOrientation::Horizontal};
        int16_t barSize{21};
        int16_t knobInset{2};
        float scrollSensitivity{10.0f};
    };

    ScrollBar();

    /**
     * @brief Calculates new position for the knob along the scrollbar background.
     *
     * @param x  Mouse X position
     * @param y  Mouse Y position
     */
    void adjustKnobOnMouseEvent(const int x, const int y);

    /**
     * @brief Notify scrollbar with the new overflow value. This positions scrollbar in relationship with it's
     *        container and also the scrollbar. Usually called when parent container changes size.
     *
     */
    void notifyLayoutHasChanged();

    void updateOverflow(int newOverflow);

    void setOppositeScrollBarActive();
    void setOppositeScrollBarInactive();
    void setInactive();
    void setActive();

    bool isBarActive();

    int16_t getScrollValue();
    int16_t getDeltaScrollValue();

    Options options;

private:
    /* Events */
    void onPrepareToRender() override;
    void onRenderDone() override;
    void onMoveEvent() override;
    void onClickEvent() override;
    void onStart() override;
    void onScroll() override;

    int16_t overflow{0};
    int16_t scrollValue{0};
    int16_t lastScrollValue{0};

    bool isDragging{false};
    bool isActive{false};
    bool isOppositeActive{false};
    bool updateDueToResize{true};

    float knobPercentageAlongBg{0};
    float mouseOffset{0};

    /* Knob rendering related */
    renderer::LightWeightRenderer lwr;
    computils::LightWeightDummy knob;
};
} // namespace components::computils