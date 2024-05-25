#pragma once

#include "../AbstractComponent.hpp"
#include "LayoutUtils.hpp"
#include <cstdint>
#include <glm/fwd.hpp>

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
    glm::i16vec2 calculate(const int scrollOffsetX = 0,
        const int scrollOffsetY = 0,
        const bool isHScrollActive = false,
        const bool isVScrollActive = false);

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

    OverflowResult calculateAndApplyOverflow(const int16_t scrollOffsetX,
        const int16_t scrollOffsetY,
        const ScrollBarDetails& sbDetails);
    float getNextFillPolicyPosition(float& bufferPos, float compScale, float remainingSpace);
    void calculateAndApplyAlignOffset(const ScrollBarDetails& sbDetails);
    void calculateAndApplyInternalAlignOffset(const Bounds& bounds);
    void calculateAndApplyPosition(const ScrollBarDetails& sbDetails);
    void calculateAndApplyScale(const ScrollBarDetails& sbDetails);
    void calculateNextBasePosition(glm::vec2& currentXY, AbstractComponent* comp);

    void resetPositions();
    glm::vec2 getRemainingSpaceAfterScale(const ScrollBarDetails& sbDetails);
    Bounds getChildrenBound(const std::vector<AbstractComponent*>& childComps);
    AdjustedTransform getAdjustedTransform(AbstractComponent* comp);
    AdjustedTransform getPaddedRootTransform();

    /* TODO: Maybe in the future this root can be removed and class can be made static so we are fully stateless */
    AbstractComponent* root;
};
} // namespace components::layoutcalc