#pragma once

#include <cstdlib>

#include "src/components/AbstractComponent.hpp"
#include "src/components/layoutCalc/LayoutUtils.hpp"

namespace components::layoutcalc
{

/**
 * @brief Stateless class responsible for positioning elements with respect to their parent and user supplied options
 *
 */
class PinchLayoutCalculator
{
public:
    PinchLayoutCalculator(AbstractComponent* comp);
    void calculate(const bool firstUpdateAfterAppendChildren);

private:
    struct AdjustedTransform
    {
        glm::i16vec2 pos;
        glm::i16vec2 scale;
    };

    struct Bounds
    {
        glm::vec2 start{9999};
        glm::vec2 end{-9999};
    };

    struct ScrollBarDetails
    {
        bool isHBarActive{false};
        bool isVBarActive{false};
    };

    struct OverflowResult
    {
        float overflowX{0};
        float overflowY{0};
    };

    void resetPositions();
    void calculateAndApplyAbsoluteScale();
    void calculateAndApplyPosition();
    void calculateAndApplyScale();
    float getNextFillPolicyPosition(float& bufferPos, float compScale, float remainingSpace);
    void calculateNextBasePosition(glm::vec2& currentXY, AbstractComponent* comp);

    void gridCalculateAndApplyScale(const ScrollBarDetails& sbDetails);
    void gridCalculateAndApplyPosition(const ScrollBarDetails& sbDetails);

    glm::vec2 getRemainingSpaceAfterScale();
    Bounds getChildrenBound(const std::vector<AbstractComponent*>& childComps);
    AdjustedTransform getAdjustedTransform(AbstractComponent* comp);
    AdjustedTransform getPaddedRootTransform();

    AbstractComponent* root;
    bool firstUpdateDone{false};
};
} // namespace components::layoutcalc