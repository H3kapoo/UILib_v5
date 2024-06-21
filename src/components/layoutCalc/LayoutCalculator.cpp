#include "LayoutCalculator.hpp"

#include "src/Utility.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"

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

    if (root->layout.fillPolicy == LdFillPolicy::Grid)
    {
        /* Calculate scale for grid*/
        gridCalculateAndApplyScale(sbDetails);

        /* Calculate position for grid*/
        gridCalculateAndApplyPosition(sbDetails);
    }
    else
    {

        /* Calculate scale */
        calculateAndApplyScale(sbDetails);

        /* Calculate position based on FillPolicy & Orientation */
        calculateAndApplyPosition(sbDetails);

        /* Calculate and add offset to the position based on Align & internalAlign */
        calculateAndApplyAlignOffset(sbDetails);
    }

    /* Overflow calculation */
    const auto overflow = calculateAndApplyOverflow(scrollOffsetX, scrollOffsetY, sbDetails);
    return {overflow.overflowX, overflow.overflowY};
    // return {0, 0};
}

void LayoutCalculator::gridCalculateAndApplyScale(const ScrollBarDetails& sbDetails)
{
    /* We can get 1 pixel off errors due to rounding inaccuracy. We need to counteract this by keeping track of the
       current error. If this error is greater than epsilon, we need to add +1 to the scale of the currently under
       calculation component. */
    glm::vec2 accumulatedScaleError = {0, 0};
    float epsi = 1.0f / root->getNodes().size();

    auto rootScale = getPaddedRootTransform().scale;

    const auto rootLeftBorder = root->layout.border.left;
    const auto rootRightBorder = root->layout.border.right;
    const auto rootTopBorder = root->layout.border.top;
    const auto rootBotBorder = root->layout.border.bottom;

    /* They are reversed, it's justified */
    rootScale.x -= sbDetails.isVBarActive ? root->layout.scrollBarSize : 0;
    rootScale.x -= rootLeftBorder + rootRightBorder;
    rootScale.y -= sbDetails.isHBarActive ? root->layout.scrollBarSize : 0;
    rootScale.y -= rootTopBorder + rootBotBorder;

    const auto& gridConfig = root->layout.grid.config;
    float equalSliceH = 1.0f / gridConfig.cols;
    float equalSliceV = 1.0f / gridConfig.rows;

    float equalSliceValueH = equalSliceH * rootScale.x;
    float equalSliceValueV = equalSliceV * rootScale.y;

    for (const auto& comp : root->getNodes())
    {
        SKIP_SCROLLBAR(comp)

        const auto& childMargins = comp->layout.margin;
        const auto& childSpan = comp->layout.grid.span;
        if (comp->layout.scaling.horizontal.policy == LdScalePolicy::Relative)
        {
            comp->getTransformRW().scale.x = comp->layout.scaling.horizontal.value *
                                             (equalSliceValueH * childSpan.colSpan);

            /* Needed so we don't get pixel imperfect visual artifacts */
            float preRoundX = comp->getTransformRW().scale.x;
            comp->getTransformRW().scale.x = std::round(comp->getTransformRead().scale.x);

            accumulatedScaleError.x += preRoundX - comp->getTransformRW().scale.x;
            if (accumulatedScaleError.x > epsi)
            {
                comp->getTransformRW().scale.x += 1.0f;
                accumulatedScaleError.x = 0.0f;
            }
        }

        if (comp->layout.scaling.vertical.policy == LdScalePolicy::Relative)
        {
            comp->getTransformRW().scale.y = comp->layout.scaling.vertical.value *
                                             (equalSliceValueV * childSpan.rowSpan);

            /* Needed so we don't get pixel imperfect visual artifacts */
            float preRoundY = comp->getTransformRW().scale.y;
            comp->getTransformRW().scale.y = std::round(comp->getTransformRead().scale.y);

            accumulatedScaleError.y += preRoundY - comp->getTransformRW().scale.y;
            if (accumulatedScaleError.y > epsi)
            {
                comp->getTransformRW().scale.y += 1.0f;
                accumulatedScaleError.y = 0.0f;
            }
        }

        if (comp->layout.scaling.horizontal.policy == LdScalePolicy::Absolute)
        {
            comp->getTransformRW().scale.x = comp->layout.scaling.horizontal.value;
        }

        if (comp->layout.scaling.vertical.policy == LdScalePolicy::Absolute)
        {
            comp->getTransformRW().scale.y = comp->layout.scaling.vertical.value;
        }

        // Reduce by margins
        comp->getTransformRW().scale -= glm::vec2(childMargins.left + childMargins.right,
            childMargins.top + childMargins.bottom);
    }
}

void LayoutCalculator::gridCalculateAndApplyPosition(const ScrollBarDetails& sbDetails)
{
    const auto& paddedRoot = getPaddedRootTransform();
    auto rootScale = paddedRoot.scale;

    const auto rootLeftBorder = root->layout.border.left;
    const auto rootRightBorder = root->layout.border.right;
    const auto rootTopBorder = root->layout.border.top;
    const auto rootBotBorder = root->layout.border.bottom;

    /* They are reversed, it's justified */
    rootScale.x -= sbDetails.isVBarActive ? root->layout.scrollBarSize : 0;
    rootScale.x -= rootLeftBorder + rootRightBorder;
    rootScale.y -= sbDetails.isHBarActive ? root->layout.scrollBarSize : 0;
    rootScale.y -= rootTopBorder + rootBotBorder;

    const auto& gridConfig = root->layout.grid.config;
    float equalSliceH = 1.0f / gridConfig.cols;
    float equalSliceV = 1.0f / gridConfig.rows;

    float equalSliceValueH = equalSliceH * rootScale.x;
    float equalSliceValueV = equalSliceV * rootScale.y;

    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        const auto& childMargins = childNode->layout.margin;
        const auto& childGridPos = childNode->layout.grid.pos;

        auto& childPos = childNode->getTransformRW().pos;
        childPos.x += equalSliceValueH * childGridPos.col;
        childPos.y += equalSliceValueV * childGridPos.row;

        /* Make child take root padding into account */
        childNode->getTransformRead().pos += paddedRoot.pos + glm::i16vec2(rootLeftBorder, rootTopBorder);

        // Push by margins
        childNode->getTransformRW().pos += glm::vec2(childMargins.left, childMargins.top);
    }
}

void LayoutCalculator::calculateAndApplyScale(const ScrollBarDetails& sbDetails)
{
    auto rootScale = getPaddedRootTransform().scale;

    /* They are reversed, it's justified */
    rootScale.x -= sbDetails.isVBarActive ? root->layout.scrollBarSize : 0;
    rootScale.y -= sbDetails.isHBarActive ? root->layout.scrollBarSize : 0;

    /* If we have some children with Absolute scale, then Relative scale of the other children of root will be relative
       to the space remaining after subtracting thr Absolute scales from the root. */
    glm::i16vec2 absSubtract = {0, 0};
    const bool isLayoutHorizontal = root->layout.orientation == LdOrientation::Horizontal;
    for (const auto& comp : root->getNodes())
    {
        SKIP_SCROLLBAR(comp)

        if (isLayoutHorizontal && comp->layout.scaling.horizontal.policy == LdScalePolicy::Absolute)
        {
            absSubtract.x += comp->layout.scaling.horizontal.value;
        }

        if (!isLayoutHorizontal && comp->layout.scaling.vertical.policy == LdScalePolicy::Absolute)
        {
            absSubtract.y += comp->layout.scaling.vertical.value;
        }
    }
    rootScale -= absSubtract;

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
            const auto childMargins = comp->layout.margin;
            const auto leftSubtract = root->layout.border.left;
            const auto rightSubtract = root->layout.border.right;
            comp->getTransformRW().scale.x = comp->layout.scaling.horizontal.value *
                                             (rootScale.x - (leftSubtract + rightSubtract));
            comp->getTransformRW().scale.x -= childMargins.left + childMargins.right;

            /* Needed so we don't get pixel imperfect visual artifacts */
            comp->getTransformRW().scale.x = std::round(comp->getTransformRead().scale.x);
        }

        if (comp->layout.scaling.vertical.policy == LdScalePolicy::Relative)
        {
            const auto childMargins = comp->layout.margin;
            const auto topSubtract = root->layout.border.top;
            const auto botSubtract = root->layout.border.bottom;
            comp->getTransformRW().scale.y = comp->layout.scaling.vertical.value *
                                             (rootScale.y - (topSubtract + botSubtract));
            comp->getTransformRW().scale.y -= childMargins.top + childMargins.bottom;

            /* Needed so we don't get pixel imperfect visual artifacts */
            comp->getTransformRW().scale.y = std::round(comp->getTransformRead().scale.y);
        }
    }
}

void LayoutCalculator::calculateAndApplyPosition(const ScrollBarDetails& sbDetails)
{
    glm::vec2 startXY = {0, 0};
    int16_t maxY = 0;
    int16_t maxX = 0;

    auto remainingSpace = getRemainingSpaceAfterScale(sbDetails);
    const auto& paddedRootTransform = getPaddedRootTransform().scale;
    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        const auto& childMargin = childNode->layout.margin;
        glm::i16vec2 nextPos = {0, 0};

        const auto& childTransform = getAdjustedTransform(childNode);
        auto& childPos = childNode->getTransformRW().pos;

        // TODO: 3 lvl deep IFs need to be fixed
        if (root->layout.orientation == LayoutData::Orientation::Horizontal)
        {
            nextPos.x += getNextFillPolicyPosition(startXY.x, childTransform.scale.x, remainingSpace.x);
            nextPos.x += childMargin.left;
            if (root->layout.wrap == LdWrapMode::WrapAround && root->layout.fillPolicy == LdFillPolicy::Tightly)
            {
                if (nextPos.x + childTransform.scale.x > paddedRootTransform.x)
                {
                    startXY.x = childTransform.scale.x;
                    startXY.y += maxY;
                    nextPos.x = childMargin.left;
                    nextPos.y = startXY.y + childMargin.top;
                    maxY = 0;
                }
                maxY = std::max(childTransform.scale.y, maxY);
            }
            childPos = {nextPos.x, startXY.y + childMargin.top};
        }
        else if (root->layout.orientation == LayoutData::Orientation::Vertical)
        {
            nextPos.y += getNextFillPolicyPosition(startXY.y, childTransform.scale.y, remainingSpace.y);
            nextPos.y += childMargin.top;
            if (root->layout.wrap == LdWrapMode::WrapAround && root->layout.fillPolicy == LdFillPolicy::Tightly)
            {
                if (nextPos.y + childTransform.scale.y > paddedRootTransform.y)
                {
                    startXY.y = childTransform.scale.y;
                    startXY.x += maxX;
                    nextPos.y = childMargin.top;
                    nextPos.x = startXY.x + childMargin.left;
                    maxX = 0;
                }
                maxX = std::max(childTransform.scale.x, maxX);
            }
            childPos = {startXY.x + childMargin.left, nextPos.y};
        }
    }
}

void LayoutCalculator::calculateAndApplyAlignOffset(const ScrollBarDetails& sbDetails)
{
    const auto rootLeftBorder = root->layout.border.left;
    const auto rootRightBorder = root->layout.border.right;
    const auto rootTopBorder = root->layout.border.top;
    const auto rootBotBorder = root->layout.border.bottom;

    const auto& rootBox = getPaddedRootTransform();
    const auto bound = getChildrenBound(root->getNodes());
    const auto boundsDiff = bound.end - bound.start;

    auto adjustedRootScale = rootBox.scale;

    /* They are reversed, it's justified */
    adjustedRootScale.y -= sbDetails.isHBarActive ? root->layout.scrollBarSize : 0;
    adjustedRootScale.x -= sbDetails.isVBarActive ? root->layout.scrollBarSize : 0;

    glm::vec2 offset = {0, 0};

    /* Align relative to other elements on same level. In wrap mode, this isnt supported. */
    if (root->layout.wrap == LdWrapMode::NoWrap) { calculateAndApplyInternalAlignOffset(bound); }

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

    const auto rootLeftBorder = root->layout.border.left;
    const auto rootRightBorder = root->layout.border.right;
    const auto rootTopBorder = root->layout.border.top;
    const auto rootBotBorder = root->layout.border.bottom;

    const auto& rootAdjustedTransform = getPaddedRootTransform();
    const auto& rts = rootAdjustedTransform.scale -
                      glm::i16vec2{rootLeftBorder + rootRightBorder + yPlus, rootBotBorder + rootTopBorder + xPlus};
    const auto& rtp = rootAdjustedTransform.pos + glm::i16vec2{rootLeftBorder, rootTopBorder};
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
        /* Position elements one after the other tightly */
        case LayoutData::FillPolicy::Tightly: {
            nextPos = bufferPos;
            bufferPos += compScale;
            break;
        }
        /* Position elements equaly spaced one from another and from parent ends */
        case LayoutData::FillPolicy::EvenlySpaced: {
            const auto offset = remainingSpace / (root->getNodes().size() + 1);
            bufferPos += offset;
            nextPos = bufferPos;
            bufferPos += compScale;
            break;
        }
        /* Position elements equaly spaced one from another but stuck to the parent ends */
        case LayoutData::FillPolicy::SpaceBetween: {
            // TODO: Treat division by zero. Currently this is ok due to .size() being size_t
            nextPos = bufferPos;
            const auto offset = remainingSpace / (root->getNodes().size() - 1);
            bufferPos += offset + compScale;
            break;
        }
        case LayoutData::FillPolicy::Grid:
        // Not supported here
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
    const auto rootLeftBorder = root->layout.border.left;
    const auto rootRightBorder = root->layout.border.right;
    const auto rootTopBorder = root->layout.border.top;
    const auto rootBotBorder = root->layout.border.bottom;

    // glm::vec2 accumulatedSize = {0, 0};
    glm::i16vec2 accumulatedSize = {0, 0};
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

    const auto& rootScale = getPaddedRootTransform().scale;
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
    const auto& childMargin = comp->layout.margin;
    newTransform.pos = comp->getTransformRead().pos - glm::vec2(childMargin.left, childMargin.top);
    newTransform.scale = comp->getTransformRead().scale +
                         glm::vec2(childMargin.left + childMargin.right, childMargin.top + childMargin.bottom);
    return newTransform;
}

LayoutCalculator::AdjustedTransform LayoutCalculator::getPaddedRootTransform()
{
    AdjustedTransform newTransform;
    const auto& rootPadding = root->layout.padding;
    const auto& rootTransform = root->getTransformRead();
    newTransform.pos = rootTransform.pos + glm::vec2(rootPadding.left, rootPadding.top);
    newTransform.scale = rootTransform.scale -
                         glm::vec2(rootPadding.left + rootPadding.right, rootPadding.top + rootPadding.bottom);
    return newTransform;
}
} // namespace components::layoutcalc
