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

glm::i16vec2 LayoutCalculator::calculate(const int scrollOffsetX, const int scrollOffsetY)
{
    /* Reset positions */
    resetPositions();

    /* Calculate scale */
    calculateAndApplyScale();

    /* Calculate position based on FillPolicy & Orientation */
    calculateAndApplyPosition();

    /* Calculate and add offset to the position based on Align & internalAlign */
    calculateAndApplyAlignOffset();

    const auto bounds = getChildrenBound(root->getNodes());

    const auto& rts = root->getTransformRW().scale;
    const auto& rtp = root->getTransformRW().pos;
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

    for (const auto& childNode : root->getNodes())
    {
        auto& childPos = childNode->getTransformRW().pos;
        childPos.x += leftOverflow > 0 ? leftOverflow : 0;
        childPos.y += topOverflow > 0 ? topOverflow : 0;
    }

    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)
        auto& childPos = childNode->getTransformRW().pos;
        childPos.x -= scrollOffsetX;
        childPos.y -= scrollOffsetY;
    }

    return {totalOverflowX, totalOverflowY};
}

void LayoutCalculator::calculateAndApplyAlignOffset()
{
    const auto rootLeftBorder = root->layout.borderSize.left;
    const auto rootRightBorder = root->layout.borderSize.right;
    const auto rootTopBorder = root->layout.borderSize.top;
    const auto rootBotBorder = root->layout.borderSize.bottom;

    const auto& rootBox = root->getTransformRead();
    const auto bound = getChildrenBound(root->getNodes());
    const auto boundsDiff = bound.end - bound.start;
    glm::vec2 offset = {0, 0};

    switch (root->layout.align.horizontal)
    {
        case LdAlign::Left: {
            offset.x = rootLeftBorder;
            break;
        }
        case LdAlign::Center: {
            int halfDiffX = boundsDiff.x / 2;
            int spaceLeft = rootBox.scale.x + rootLeftBorder - rootRightBorder;
            offset.x = spaceLeft / 2.0f - halfDiffX;
            break;
        }
        case LdAlign::Right: {
            offset.x = rootBox.scale.x - (boundsDiff.x + rootRightBorder);
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
            int spaceLeft = rootBox.scale.y + rootTopBorder - rootBotBorder;
            offset.y = spaceLeft / 2.0f - halfDiffY;
            break;
        }
        case LdAlign::Bot: {
            offset.y = rootBox.scale.y - (boundsDiff.y + rootBotBorder);
            break;
        }
        case LdAlign::COUNT:
            break;
    }

    offset += rootBox.pos;
    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        childNode->getTransformRW().pos += offset;
    }
}

void LayoutCalculator::calculateAndApplyPosition()
{
    const auto& rootBox = root->getTransformRW();
    // glm::vec2 startXY = rootBox.pos;
    glm::vec2 startXY = {0, 0};

    auto remainingSpace = getRemainingSpaceAfterScale();
    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        auto& childPos = childNode->getTransformRW().pos;
        auto& childScale = childNode->getTransformRW().scale;
        if (root->layout.orientation == LayoutData::Orientation::Horizontal)
        {
            childPos.x = getNextFillPolicyPosition(startXY.x, childScale.x, remainingSpace.x);
        }
        else if (root->layout.orientation == LayoutData::Orientation::Vertical)
        {
            childPos.y = getNextFillPolicyPosition(startXY.y, childScale.y, remainingSpace.y);
        }
    }
}

float LayoutCalculator::getNextFillPolicyPosition(float& bufferPos, float& compScale, float& remainingSpace)
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

void LayoutCalculator::calculateAndApplyScale()
{
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
                                             (root->getTransformRead().scale.x - (rootLeftBorder + rootRightBorder));
        }

        if (comp->layout.scaling.vertical.policy == LdScalePolicy::Relative)
        {
            const auto rootTopBorder = root->layout.borderSize.top;
            const auto rootBotBorder = root->layout.borderSize.bottom;
            comp->getTransformRW().scale.y = comp->layout.scaling.vertical.value *
                                             (root->getTransformRead().scale.y - (rootTopBorder + rootBotBorder));
        }
    }
}

void LayoutCalculator::resetPositions()
{
    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        childNode->getTransformRW().pos = {0, 0};
    }
}

glm::vec2 LayoutCalculator::getRemainingSpaceAfterScale()
{
    // TODO: Wrongly calculates remaning usable space. Doesnt take into account
    //  current orientation
    const auto rootLeftBorder = root->layout.borderSize.left;
    const auto rootRightBorder = root->layout.borderSize.right;
    const auto rootTopBorder = root->layout.borderSize.top;
    const auto rootBotBorder = root->layout.borderSize.bottom;

    glm::vec2 accumulatedSize = {0, 0};
    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        const auto& chScale = childNode->getTransformRead().scale;
        if (root->layout.orientation == LdOrientation::Horizontal)
        {
            accumulatedSize.x += chScale.x;
            accumulatedSize.y = chScale.y > accumulatedSize.y ? chScale.y : accumulatedSize.y;
        }
        else if (root->layout.orientation == LdOrientation::Vertical)
        {
            accumulatedSize.x = chScale.x > accumulatedSize.x ? chScale.x : accumulatedSize.x;
            accumulatedSize.y += chScale.y;
        }
    }

    // TODO: take into accound margin/border/padd
    auto remainingSpace = root->getTransformRead().scale - accumulatedSize;
    remainingSpace.x -= (rootLeftBorder + rootRightBorder);
    remainingSpace.y -= (rootTopBorder + rootBotBorder);

    return remainingSpace;
}

LayoutCalculator::Bounds LayoutCalculator::getChildrenBound(const std::vector<AbstractComponent*>& childComps)
{
    // TODO: Take into account MARGINs
    Bounds result;
    for (const auto& comp : childComps)
    {
        SKIP_SCROLLBAR(comp)

        /* X related */
        const auto& childTransform = comp->getTransformRead();
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

    /* Any other FillPolicy will take up the whole axis space */
    // if (root->layout.fillPolicy != LdFillPolicy::Tightly)
    // {
    //     const auto rootLeftBorder = root->layout.borderSize.left;
    //     const auto rootRightBorder = root->layout.borderSize.right;
    //     const auto rootTopBorder = root->layout.borderSize.top;
    //     const auto rootBotBorder = root->layout.borderSize.bottom;
    //     if (root->layout.orientation == LdOrientation::Horizontal)
    //     {
    //         result.start.x = root->getTransformRead().pos.x + rootLeftBorder;
    //         result.end.x = root->getTransformRead().scale.x - rootRightBorder;
    //     }
    //     else if (root->layout.orientation == LdOrientation::Vertical)
    //     {
    //         result.start.y = root->getTransformRead().pos.y + rootTopBorder;
    //         result.end.y = root->getTransformRead().scale.y - rootBotBorder;
    //     }
    // }

    return result;
}
} // namespace components::layoutcalc
