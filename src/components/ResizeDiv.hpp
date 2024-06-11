#pragma once

#include <cstdint>
#include <utility>

#include "src/components/AbstractComponent.hpp"
#include "src/components/SeparatorBar.hpp"
#include "src/components/layoutCalc/PinchLayoutCalculator.hpp"

namespace components
{

using namespace layoutcalc;

class ResizeDiv : public AbstractComponent
{

public:
    /* General style parameters associated with this component in particular */
    struct Style
    {
        glm::vec4 color{0.0f};
        glm::vec4 hoveredColor{0.0f};
        glm::vec4 borderColor{1.0f};

        /* Note: Keep consistency between PinchDivs!! */
        int16_t separatorSize{10};
    };

    ResizeDiv();
    ~ResizeDiv();

    Style style;

    /**
     * @brief Append components to act as resizeable panes separated by a Separator component.
     *
     * @note Append all necessary panes at once. Once this gets called, subsequent calles will have unexpected
     *       behaviour. This will be fixed later.
     *
     * @param  comp Array of components to append
     */
    void append(std::vector<AbstractComponent*>&& comp);

private:
    /**
     * @brief Called when the PinchBar is clicked. Also calls mouse click on supporting bars.
     *
     * @param x    Mouse X position
     * @param y    Mouse Y position
     * @param btn  Clicked button enum
     */
    void onSeparatorClick(const int16_t x, const int16_t y, const MouseButton btn);

    /**
     * @brief Called when the PinchBar is released. Also calls mouse released on supporting bars.
     *
     * @param x    Mouse X position
     * @param y    Mouse Y position
     * @param btn  Clicked button enum
     */
    void onSeparatorRelease(const int16_t x, const int16_t y, const MouseButton btn);

    /**
     * @brief Called when the PinchBar is in a clicked but dragging state. Responsible for resizing the areas tied to
              that PinchBar, more precisely the PinchPair.
     *
     * @param x      Mouse X position
     * @param y      Mouse Y position
     * @param index  Index of the PinchBar's associated PinchPair
     */
    void onSeparatorDrag(const int16_t x, const int16_t y, const int16_t index);

    /**
     * @brief Called when the mouse exits the area of the PinchBar. Calles exit also on the supporting PinchBars.
     *
     */
    void onSeparatorExit();

    /**
     * @brief Called when the mouse is moving over the PinchBar in order to scan for potential supporting PinchBars. If
              the application doesn't require 3/4-way pinching, this function can be conditionally removed by developer.
     *
     * @param x  Mouse X position
     * @param y  Mouse Y position
     */
    void onMouseMoveOverSeparator(const int16_t x, const int16_t y);

    /**
     * @brief Determine if the separator is within separatorSize of x,y position square wise from each separator end.
     *
     * @param sep  Separator to calculate square box distance to
     * @param x    Mouse X position
     * @param y    Mouse Y position
     * @return     True if within range, false otherwise
     */
    bool isSeparatorClose(AbstractComponent* sep, const int16_t x, const int16_t y);

    /* User shall not be able to use these for now. */
    bool remove();
    bool appendAux();
    bool removeAux();

    /* Events */
    void onPrepareToRender() override;
    void onRenderDone() override;
    void onClickEvent() override;
    void onMoveEvent() override;
    void onMouseExitEvent() override;
    void onMouseEnterEvent() override;
    void onStart() override;
    bool onLayoutUpdate() override;

    using PanePair = std::pair<AbstractComponent*, AbstractComponent*>;
    std::vector<SeparatorBar*> separators;

    /* PinchPair refers to the pair of Panes tied to the Separator at a specific vector (this) index. E.g pinchPair[1]
       gets the two Panes associated with separator number 1 (0 indexed). */
    std::vector<PanePair> panePairs;

    int16_t prevX{0}, prevY{0};

    /* Used to help layout calculator which function to use to calculate the layout. */
    bool firstUpdateAfterAppend{false};
    bool currentlyDragging{false};

    /* Supporting Separator bars for 3/4-way resizing */
    SeparatorBar* suppBarOne{nullptr};
    SeparatorBar* suppBarTwo{nullptr};

    /* Layout related */
    layoutcalc::PinchLayoutCalculator layoutCalc{this};
};
} // namespace components