#pragma once

#include "../renderer/LightWeightRenderer.hpp"
#include "AbstractComponent.hpp"
#include "PinchBar.hpp"
#include "compUtils/LightWeightDummy.hpp"
#include "layoutCalc/LayoutData.hpp"
#include "layoutCalc/PinchLayoutCalculator.hpp"
#include <cstdint>

namespace components
{

using namespace layoutcalc;
class PinchDiv : public AbstractComponent
{

public:
    struct Style
    {
        glm::vec4 color{0.0f};
        glm::vec4 hoveredColor{0.0f};
        glm::vec4 borderColor{1.0f};
    };

    PinchDiv();
    ~PinchDiv();

    Style style;

    void append(std::vector<AbstractComponent*>&& comp);

private:
    struct PinchPair
    {
        AbstractComponent* first;
        AbstractComponent* second;
    };

    void separatorClick(int16_t x, int16_t y, MouseButton b, PinchBar* pb);
    void separatorRelease(int16_t x, int16_t y, MouseButton b, PinchBar* pb);
    void separatorClickedMove(int16_t x, int16_t y, int16_t index);
    void separatorMove(int16_t x, int16_t y, PinchBar* pb);

    /* User shall not be able to add or remove children to button */
    bool remove();
    bool appendAux();
    bool removeAux();

    void onPrepareToRender() override;
    void onRenderDone() override;
    void onClickEvent() override;
    void onMoveEvent() override;
    void onMouseExitEvent() override;
    void onMouseEnterEvent() override;
    void onStart() override;
    bool onLayoutUpdate() override;

    // computils::LightWeightDummy imageDummy;
    // renderer::LightWeightRenderer lwr;

    std::vector<PinchBar*> separators;
    std::vector<PinchPair> pinchPairs;

    int16_t prevX{0};
    int16_t prevY{0};
    int16_t delta{0};
    int16_t pinchedId{0};
    int16_t pinchedId2{0};
    int16_t separatorSize{15};

    /* Layout related */
    layoutcalc::PinchLayoutCalculator layoutCalc{this};
};
} // namespace components