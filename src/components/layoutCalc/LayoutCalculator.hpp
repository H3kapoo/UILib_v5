#pragma once

#include "../AbstractComponent.hpp"
#include "LayoutUtils.hpp"

namespace components::layoutcalc
{

/**
 * @brief Stateless class responsible for positioning elements with respect to their parent and user supplied options
 *
 */
class LayoutCalculator
{
public:
    LayoutCalculator(AbstractComponent* comp);
    glm::i16vec2 calculate(const int scrollOffsetX = 0, const int scrollOffsetY = 0);

private:
    struct Bounds
    {
        glm::vec2 start{9999};
        glm::vec2 end{-9999};
    };

    float getNextFillPolicyPosition(float& bufferPos, float& compScale, float& remainingSpace);
    void calculateAndApplyAlignOffset();
    void calculateAndApplyPosition();
    void calculateAndApplyScale();
    void calculateNextBasePosition(glm::vec2& currentXY, AbstractComponent* comp);

    void resetPositions();
    glm::vec2 getRemainingUsableSpace();
    Bounds getChildrenBound(const std::vector<AbstractComponent*>& childComps);

    /* TODO: Maybe in the future this root can be removed and class can be made static so we are fully stateless */
    AbstractComponent* root;
};
} // namespace components::layoutcalc