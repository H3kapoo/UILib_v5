#include "LayoutCalculator.hpp"
#include "../../Utility.hpp"
#include "LayoutData.hpp"
#include <glm/fwd.hpp>

namespace components::layoutcalc
{
#define SKIP_SCROLLBAR(x)                                                                                              \
    if (x->getType() == AbstractComponent::CompType::ScrollBar) { continue; }

LayoutCalculator::LayoutCalculator(AbstractComponent* comp)
    : root{comp}
{}

glm::i16vec2 LayoutCalculator::calculate(const int scrollOffsetX,
    const int scrollOffsetY,
    const bool isHScrollActive,
    const bool isVScrollActive)
{
    const ScrollBarDetails sbDetails{isHScrollActive, isVScrollActive};

    /* Reset positions */
    resetPositions();

    /* Calculate scale */
    calculateAndApplyScale(sbDetails);

    /* Calculate position based on FillPolicy & Orientation */
    calculateAndApplyPosition(sbDetails);

    /* Calculate and add offset to the position based on Align & internalAlign */
    calculateAndApplyAlignOffset(sbDetails);

    /* Overflow calculation */
    const auto overflow = calculateAndApplyOverflow(scrollOffsetX, scrollOffsetY, sbDetails);

    return {overflow.overflowX, overflow.overflowY};
}

void LayoutCalculator::calculateAndApplyScale(const ScrollBarDetails& sbDetails)
{
    auto rootScale = root->getTransformRead().scale;

    /* They are reversed, it's justified */
    rootScale.x -= sbDetails.isVBarActive ? root->layout.scrollBarSize : 0;
    rootScale.y -= sbDetails.isHBarActive ? root->layout.scrollBarSize : 0;
    for (const auto& comp : root->getNodes())
    {
        SKIP_SCROLLBAR(comp)

        if (comp->layout.scaling.horizontal.policy == LdScalePolicy::Absolute)
        {
            comp->getTransformRW().scale.x = comp->layout.scaling.horizontal.value;
        }

        if (comp->layout.scaling.vertical.policy == LdScalePolicy::Absolute)
        {
            comp->getTransformRW().scale.y = comp->layout.scaling.vertical.value;
        }

        if (comp->layout.scaling.horizontal.policy == LdScalePolicy::Relative)
        {
            const auto rootLeftBorder = root->layout.borderSize.left;
            const auto rootRightBorder = root->layout.borderSize.right;
            comp->getTransformRW().scale.x = comp->layout.scaling.horizontal.value *
                                             (rootScale.x - (rootLeftBorder + rootRightBorder));
        }

        if (comp->layout.scaling.vertical.policy == LdScalePolicy::Relative)
        {
            const auto rootTopBorder = root->layout.borderSize.top;
            const auto rootBotBorder = root->layout.borderSize.bottom;
            comp->getTransformRW().scale.y = comp->layout.scaling.vertical.value *
                                             (rootScale.y - (rootTopBorder + rootBotBorder));
        }
    }
}

void LayoutCalculator::calculateAndApplyPosition(const ScrollBarDetails& sbDetails)
{
    glm::vec2 startXY = {0, 0};

    auto remainingSpace = getRemainingSpaceAfterScale(sbDetails);
    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        const auto& childMargin = childNode->layout.marginSize;
        const auto& childTransform = getAdjustedTransform(childNode);
        auto& childPos = childNode->getTransformRW().pos;
        if (root->layout.orientation == LayoutData::Orientation::Horizontal)
        {
            childPos.x = getNextFillPolicyPosition(startXY.x, childTransform.scale.x, remainingSpace.x);
            childPos.x += childMargin.left;
            childPos.y += childMargin.top;
        }
        else if (root->layout.orientation == LayoutData::Orientation::Vertical)
        {
            childPos.y = getNextFillPolicyPosition(startXY.y, childTransform.scale.y, remainingSpace.y);
            childPos.y += childMargin.top;
            childPos.x += childMargin.left;
        }
    }
}

void LayoutCalculator::calculateAndApplyInternalAlignOffset(const Bounds& bound)
{
    glm::vec2 offset = {0, 0};

    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        auto& childPos = childNode->getTransformRead().pos;
        const auto& childTransform = getAdjustedTransform(childNode);
        if (root->layout.orientation == LdOrientation::Horizontal)
        {
            switch (root->layout.internalAlign)
            {
                case LdAlign::Top: {
                    // Nothing to do
                    break;
                }
                case LdAlign::Center: {
                    offset.y += bound.end.y - (childTransform.pos.y + childTransform.scale.y);
                    childPos += offset * 0.5f;
                    break;
                }
                case LdAlign::Bot: {
                    offset.y += bound.end.y - (childTransform.pos.y + childTransform.scale.y);
                    childPos += offset;
                    break;
                }
                case LdAlign::COUNT:
                    break;
            }
        }

        if (root->layout.orientation == LdOrientation::Vertical)
        {
            switch (root->layout.internalAlign)
            {
                case LdAlign::Left: {
                    // Nothing to do
                    break;
                }
                case LdAlign::Center: {
                    offset.x += bound.end.x - (childTransform.pos.x + childTransform.scale.x);
                    childPos += offset * 0.5f;
                    break;
                }
                case LdAlign::Right: {
                    offset.x += bound.end.x - (childTransform.pos.x + childTransform.scale.x);
                    childPos += offset;
                    break;
                }
                case LdAlign::COUNT:
                    break;
            }
        }
        offset = {0, 0};
    }
}

void LayoutCalculator::calculateAndApplyAlignOffset(const ScrollBarDetails& sbDetails)
{
    const auto rootLeftBorder = root->layout.borderSize.left;
    const auto rootRightBorder = root->layout.borderSize.right;
    const auto rootTopBorder = root->layout.borderSize.top;
    const auto rootBotBorder = root->layout.borderSize.bottom;

    const auto& rootBox = root->getTransformRead();
    const auto bound = getChildrenBound(root->getNodes());
    const auto boundsDiff = bound.end - bound.start;

    auto adjustedRootScale = rootBox.scale;

    /* They are reversed, it's justified */
    adjustedRootScale.y -= sbDetails.isHBarActive ? root->layout.scrollBarSize : 0;
    adjustedRootScale.x -= sbDetails.isVBarActive ? root->layout.scrollBarSize : 0;

    glm::vec2 offset = {0, 0};

    /* Align relative to other elements on same level */
    calculateAndApplyInternalAlignOffset(bound);

    /* Align relative to parent */
    switch (root->layout.align.horizontal)
    {
        case LdAlign::Left: {
            offset.x = rootLeftBorder;
            break;
        }
        case LdAlign::Center: {
            int halfDiffX = boundsDiff.x / 2;
            int spaceLeft = adjustedRootScale.x + rootLeftBorder - rootRightBorder;
            offset.x = spaceLeft / 2.0f - halfDiffX;
            break;
        }
        case LdAlign::Right: {
            offset.x = adjustedRootScale.x - (boundsDiff.x + rootRightBorder);
            break;
        }
        case LdAlign::COUNT:
            break;
    }

    switch (root->layout.align.vertical)
    {
        case LdAlign::Top: {
            offset.y = rootTopBorder;
            break;
        }
        case LdAlign::Center: {
            int halfDiffY = boundsDiff.y / 2;
            int spaceLeft = adjustedRootScale.y + rootTopBorder - rootBotBorder;
            offset.y = spaceLeft / 2.0f - halfDiffY;
            break;
        }
        case LdAlign::Bot: {
            offset.y = adjustedRootScale.y - (boundsDiff.y + rootBotBorder);
            break;
        }
        case LdAlign::COUNT:
            break;
    }

    offset += rootBox.pos;
    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        childNode->getTransformRead().pos += offset;
    }
}

LayoutCalculator::OverflowResult LayoutCalculator::calculateAndApplyOverflow(const int16_t scrollOffsetX,
    const int16_t scrollOffsetY,
    const ScrollBarDetails& sbDetails)
{
    /* If Y_BAR is active, it means overflow (if exists) needs to be increased by 'scrollbar' size so we can scroll past
       that bar and show all the content. Without this, some content will be hidden under the scrollbar. This applies
       vice-versa aswell.*/
    const auto xPlus = sbDetails.isHBarActive ? root->layout.scrollBarSize : 0;
    const auto yPlus = sbDetails.isVBarActive ? root->layout.scrollBarSize : 0;

    const auto bounds = getChildrenBound(root->getNodes());

    const auto rootLeftBorder = root->layout.borderSize.left;
    const auto rootRightBorder = root->layout.borderSize.right;
    const auto rootTopBorder = root->layout.borderSize.top;
    const auto rootBotBorder = root->layout.borderSize.bottom;

    const auto& rootAdjustedScale = root->getTransformRead();
    const auto& rts = rootAdjustedScale.scale -
                      glm::vec2{rootLeftBorder + rootRightBorder + yPlus, rootBotBorder + rootTopBorder + xPlus};
    const auto& rtp = rootAdjustedScale.pos + glm::vec2{rootLeftBorder, rootTopBorder};
    const auto rte = rtp + rts;

    const auto rightOverflow = bounds.end.x - rte.x;  // x pos if overflow
    const auto leftOverflow = rtp.x - bounds.start.x; // x pos if overflow
    const auto topOverflow = rtp.y - bounds.start.y;  // y pos if overflow
    const auto bottomOverflow = bounds.end.y - rte.y; // y pos if overflow

    float totalOverflowX = 0;
    float totalOverflowY = 0;

    totalOverflowX += leftOverflow > 0 ? leftOverflow : 0;
    totalOverflowX += rightOverflow > 0 ? rightOverflow : 0;

    totalOverflowY += topOverflow > 0 ? topOverflow : 0;
    totalOverflowY += bottomOverflow > 0 ? bottomOverflow : 0;

    // utils::printlnw("HO {} VO {}", totalOverflowX, totalOverflowY);

    /* Overflow application */
    for (const auto& childNode : root->getNodes())
    {
        auto& childPos = childNode->getTransformRW().pos;

        /* This will push the content in such a way that it doesn't overflow to the left or to the top. Scrollbars will
           be shown instead for scrolling */
        childPos.x += leftOverflow > 0 ? leftOverflow : 0;
        childPos.y += topOverflow > 0 ? topOverflow : 0;

        /* Apply user chosen overflow */
        childPos.x -= scrollOffsetX;
        childPos.y -= scrollOffsetY;
    }

    return OverflowResult{totalOverflowX, totalOverflowY};
}

float LayoutCalculator::getNextFillPolicyPosition(float& bufferPos, float compScale, float remainingSpace)
{
    float nextPos = 0;
    switch (root->layout.fillPolicy)
    {
        case LayoutData::FillPolicy::Tightly: {
            nextPos = bufferPos;
            bufferPos += compScale;
            break;
        }
        case LayoutData::FillPolicy::EvenlySpaced: {
            const auto offset = remainingSpace / (root->getNodes().size() + 1);
            bufferPos += offset;
            nextPos = bufferPos;
            bufferPos += compScale;
            break;
        }
        case LayoutData::FillPolicy::SpaceBetween: {
            // TODO: Treat division by zero. Currently this is ok due to .size() being size_t
            nextPos = bufferPos;
            const auto offset = remainingSpace / (root->getNodes().size() - 1);
            bufferPos += offset + compScale;
            break;
        }
        case LayoutData::FillPolicy::COUNT:
            break;
    }
    return nextPos;
}

void LayoutCalculator::resetPositions()
{
    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        childNode->getTransformRW().pos = {0, 0};
    }
}

glm::vec2 LayoutCalculator::getRemainingSpaceAfterScale(const ScrollBarDetails& sbDetails)
{
    const auto rootLeftBorder = root->layout.borderSize.left;
    const auto rootRightBorder = root->layout.borderSize.right;
    const auto rootTopBorder = root->layout.borderSize.top;
    const auto rootBotBorder = root->layout.borderSize.bottom;

    glm::vec2 accumulatedSize = {0, 0};
    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        const auto& childTransform = getAdjustedTransform(childNode);
        if (root->layout.orientation == LdOrientation::Horizontal)
        {
            accumulatedSize.x += childTransform.scale.x;
            accumulatedSize.y = childTransform.scale.y > accumulatedSize.y ? childTransform.scale.y : accumulatedSize.y;
        }
        else if (root->layout.orientation == LdOrientation::Vertical)
        {
            accumulatedSize.x = childTransform.scale.x > accumulatedSize.x ? childTransform.scale.x : accumulatedSize.x;
            accumulatedSize.y += childTransform.scale.y;
        }
    }

    // TODO: take into accound padd
    const auto& rootScale = root->getTransformRead().scale;
    auto remainingSpace = rootScale - accumulatedSize;
    remainingSpace.x -= (rootLeftBorder + rootRightBorder);
    remainingSpace.y -= (rootTopBorder + rootBotBorder);

    /* They are reversed, it's justified */
    remainingSpace.x -= sbDetails.isVBarActive ? root->layout.scrollBarSize : 0;
    remainingSpace.y -= sbDetails.isHBarActive ? root->layout.scrollBarSize : 0;

    return remainingSpace;
}

LayoutCalculator::Bounds LayoutCalculator::getChildrenBound(const std::vector<AbstractComponent*>& childComps)
{
    Bounds result;
    for (const auto& comp : childComps)
    {
        SKIP_SCROLLBAR(comp)

        /* X related */
        const auto& childTransform = getAdjustedTransform(comp);

        if (childTransform.pos.x <= result.start.x) { result.start.x = childTransform.pos.x; }
        if (childTransform.pos.x + childTransform.scale.x >= result.end.x)
        {
            result.end.x = childTransform.pos.x + childTransform.scale.x;
        }

        /* Y related */
        if (childTransform.pos.y <= result.start.y) { result.start.y = childTransform.pos.y; }
        if (childTransform.pos.y + childTransform.scale.y >= result.end.y)
        {
            result.end.y = childTransform.pos.y + childTransform.scale.y;
        }
    }

    return result;
}

LayoutCalculator::AdjustedTransform LayoutCalculator::getAdjustedTransform(AbstractComponent* comp)
{
    /* This basically extends the components "box" by the margins effectively making the following calculationg think
       the object is objectSize+marginsSize*/
    AdjustedTransform newTransform;
    const auto& childMargin = comp->layout.marginSize;
    newTransform.pos = comp->getTransformRead().pos - glm::vec2(childMargin.left, childMargin.top);
    newTransform.scale = comp->getTransformRead().scale +
                         glm::vec2(childMargin.left + childMargin.right, childMargin.top + childMargin.bottom);
    return newTransform;
}
} // namespace components::layoutcalc
