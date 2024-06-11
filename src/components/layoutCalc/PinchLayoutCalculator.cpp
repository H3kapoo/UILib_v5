#include "PinchLayoutCalculator.hpp"

#include <algorithm>

#include "src/Utility.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"

namespace components::layoutcalc
{
#define SKIP_SCROLLBAR(x)                                                                                              \
    if (x->getType() == AbstractComponent::CompType::ScrollBar) { continue; }

PinchLayoutCalculator::PinchLayoutCalculator(AbstractComponent* comp)
    : root{comp}
{}

void PinchLayoutCalculator::calculate(const bool firstUpdateAfterAppendChildren)
{
    /* Reset positions */
    resetPositions();

    if (firstUpdateAfterAppendChildren)
    {
        /* Panes come in relative sizes to the parent initially. Transform them in absolute ones that are easier to work
           with down the line. */
        calculateAndApplyAbsoluteScale();
        firstUpdateDone = true;
    }

    /* Calculate scale */
    if (firstUpdateDone) { calculateAndApplyScale(); }

    /* Calculate position based on FillPolicy & Orientation */
    calculateAndApplyPosition();
}

void PinchLayoutCalculator::calculateAndApplyAbsoluteScale()
{
    auto rootScale = root->getTransformRead().scale;
    auto& rootNodes = root->getNodes();
    glm::i16vec2 absSubtract = {0, 0};
    const bool isLayoutHorizontal = root->layout.orientation == LdOrientation::Horizontal;

    /* If we have some children with Absolute scale, then Relative scale of the other children of root will be relative
       to the space remaining after subtracting the Absolute scales from the root. */
    for (const auto& comp : rootNodes)
    {
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

    for (const auto& comp : rootNodes)
    {
        auto& compScale = comp->getTransformRW().scale;

        /* Absolute component parts remain the same vertically & horizontally */
        if (comp->layout.scaling.horizontal.policy == LdScalePolicy::Absolute)
        {
            compScale.x = comp->layout.scaling.horizontal.value;
        }

        if (comp->layout.scaling.vertical.policy == LdScalePolicy::Absolute)
        {
            compScale.y = comp->layout.scaling.vertical.value;
        }

        if (comp->layout.scaling.horizontal.policy == LdScalePolicy::Relative)
        {
            compScale.x = comp->layout.scaling.horizontal.value * rootScale.x;
            compScale.x = std::clamp(compScale.x, comp->layout.scaling.horizontal.min, 20000.0f);

            /* Needed so we don't get pixel imperfect visual artifacts */
            compScale.x = std::round(compScale.x);

            /* If the layout is horizontal we need to switch from Relative initial scaling to Absolute scaling */
            if (isLayoutHorizontal)
            {
                comp->layout.scaling.horizontal.policy = LdScalePolicy::Absolute;
                comp->layout.scaling.horizontal.value = compScale.x;
            }
        }

        if (comp->layout.scaling.vertical.policy == LdScalePolicy::Relative)
        {
            compScale.y = comp->layout.scaling.vertical.value * rootScale.y;
            compScale.y = std::clamp(compScale.y, comp->layout.scaling.vertical.min, 20000.0f);

            /* Needed so we don't get pixel imperfect visual artifacts */
            compScale.y = std::round(compScale.y);

            /* If the layout is vertical we need to switch from Relative initial scaling to Absolute scaling */
            if (!isLayoutHorizontal)
            {
                comp->layout.scaling.vertical.policy = LdScalePolicy::Absolute;
                comp->layout.scaling.vertical.value = compScale.y;
            }
        }
    }
}

void PinchLayoutCalculator::calculateAndApplyScale()
{
    auto rootScale = root->getTransformRead().scale;
    auto& rootNodes = root->getNodes();
    glm::i16vec2 compsSize = {0, 0};

    /* Count how many components have reached their minimum size value. Also calculate how much horizontal space all
       components occupy horizontally or vertically depending on the layout orientation */
    int nonMinnedComps = 0;
    const bool isLayoutHorizontal = root->layout.orientation == LdOrientation::Horizontal;
    for (const auto& comp : rootNodes)
    {
        if (isLayoutHorizontal)
        {
            compsSize.x += comp->layout.scaling.horizontal.value;
            if (comp->getType() != AbstractComponent::CompType::SeparatorBar &&
                comp->layout.scaling.horizontal.value > comp->layout.scaling.horizontal.min)
            {
                nonMinnedComps++;
            }
        }
        else
        {
            compsSize.y += comp->layout.scaling.vertical.value;
            if (comp->getType() != AbstractComponent::CompType::SeparatorBar &&
                comp->layout.scaling.vertical.value > comp->layout.scaling.vertical.min)
            {
                nonMinnedComps++;
            }
        }
    }

    /* Each component non-minned will have to increase/decrease by a small percentage. If we deduce all objects are
       minned, cap this to 1 so we don't get by zero division exception. */
    if (nonMinnedComps == 0) { nonMinnedComps = 1; }

    /* Determine, depending on the orientation, how much each non-minned component should scale. */
    float rootIncrease = isLayoutHorizontal ? rootScale.x - compsSize.x : rootScale.y - compsSize.y;
    float incEqual = rootIncrease / nonMinnedComps;

    glm::i16vec2 runningTotal = {0, 0};
    for (const auto& comp : rootNodes)
    {
        auto& compScale = comp->getTransformRW().scale;

        if (isLayoutHorizontal)
        {
            compScale.y = comp->layout.scaling.vertical.value * rootScale.y;
            compScale.y = std::round(compScale.y);
            runningTotal.x += compScale.x;

            if (comp->getType() == AbstractComponent::CompType::SeparatorBar) { continue; }

            comp->layout.scaling.horizontal.value += incEqual;
            compScale.x = comp->layout.scaling.horizontal.value;
            compScale.x = std::round(compScale.x);
            compScale.x = std::clamp(compScale.x, comp->layout.scaling.horizontal.min, 20'000.0f);
            comp->layout.scaling.horizontal.value = compScale.x;
            runningTotal.x += compScale.x;
        }
        else
        {
            compScale.x = comp->layout.scaling.horizontal.value * rootScale.x;
            compScale.x = std::round(compScale.x);
            runningTotal.y += compScale.y;

            if (comp->getType() == AbstractComponent::CompType::SeparatorBar) { continue; }

            comp->layout.scaling.vertical.value += incEqual;
            compScale.y = comp->layout.scaling.vertical.value;
            compScale.y = std::round(compScale.y);
            compScale.y = std::clamp(compScale.y, comp->layout.scaling.vertical.min, 20'000.0f);
            comp->layout.scaling.vertical.value = compScale.y;
            runningTotal.y += compScale.y;
        }
    }

    /* Accounting for rounding error unable to be caught above. This makes sure the last element doesn't get past the
       parent */
    if (isLayoutHorizontal && (int)runningTotal.x != (int)rootScale.x)
    {
        const auto lastComp = rootNodes[rootNodes.size() - 1];
        lastComp->getTransformRW().scale.x += std::clamp((rootScale.x - runningTotal.x), -1.0f, 1.0f);
    }

    if (!isLayoutHorizontal && (int)runningTotal.y != (int)rootScale.y)
    {
        const auto lastComp = rootNodes[rootNodes.size() - 1];
        lastComp->getTransformRW().scale.y += std::clamp((rootScale.y - runningTotal.y), -1.0f, 1.0f);
    }
}

void PinchLayoutCalculator::calculateAndApplyPosition()
{
    glm::vec2 startXY = root->getTransformRead().pos;
    for (const auto& childNode : root->getNodes())
    {
        auto& childPos = childNode->getTransformRW().pos;
        auto& childScale = childNode->getTransformRW().scale;

        if (root->layout.orientation == LayoutData::Orientation::Horizontal)
        {
            childPos += startXY;
            startXY.x += childScale.x;
        }
        else if (root->layout.orientation == LayoutData::Orientation::Vertical)
        {
            childPos += startXY;
            startXY.y += childScale.y;
        }
    }
}

void PinchLayoutCalculator::resetPositions()
{
    for (const auto& childNode : root->getNodes())
    {
        childNode->getTransformRW().pos = {0, 0};
    }
}

glm::vec2 PinchLayoutCalculator::getRemainingSpaceAfterScale()
{
    const auto rootLeftBorder = root->layout.border.left;
    const auto rootRightBorder = root->layout.border.right;
    const auto rootTopBorder = root->layout.border.top;
    const auto rootBotBorder = root->layout.border.bottom;

    glm::i16vec2 accumulatedSize = {0, 0};
    for (const auto& childNode : root->getNodes())
    {
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

    return remainingSpace;
}

PinchLayoutCalculator::Bounds PinchLayoutCalculator::getChildrenBound(const std::vector<AbstractComponent*>& childComps)
{
    Bounds result;
    for (const auto& comp : childComps)
    {
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

PinchLayoutCalculator::AdjustedTransform PinchLayoutCalculator::getAdjustedTransform(AbstractComponent* comp)
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

PinchLayoutCalculator::AdjustedTransform PinchLayoutCalculator::getPaddedRootTransform()
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
