#include "PinchLayoutCalculator.hpp"
#include "../../Utility.hpp"
#include "LayoutData.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>

namespace components::layoutcalc
{
#define SKIP_SCROLLBAR(x)                                                                                              \
    if (x->getType() == AbstractComponent::CompType::ScrollBar) { continue; }

PinchLayoutCalculator::PinchLayoutCalculator(AbstractComponent* comp)
    : root{comp}
{}

void PinchLayoutCalculator::calculate(const bool firstUpdate)
{
    /* Reset positions */
    resetPositions();

    if (firstUpdate)
    {
        calculateAndApplyRelativeScale();
        doneFirst = true;
    }

    /* Calculate scale */
    if (doneFirst)
    {
        calculateAndApplyScale();
        calculateAndApplyScale();
    }

    /* Calculate position based on FillPolicy & Orientation */
    calculateAndApplyPosition();
}

void PinchLayoutCalculator::calculateAndApplyRelativeScale()
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
            comp->getTransformRW().scale.x = comp->layout.scaling.horizontal.value * rootScale.x;
            comp->getTransformRW().scale.x = std::clamp(comp->getTransformRW().scale.x,
                comp->layout.scaling.horizontal.min, 20000.0f);

            /* Needed so we don't get pixel imperfect visual artifacts */
            comp->getTransformRW().scale.x = std::round(comp->getTransformRead().scale.x);

            if (isLayoutHorizontal)
            {
                comp->layout.scaling.horizontal.policy = LdScalePolicy::Absolute;
                comp->layout.scaling.horizontal.value = comp->getTransformRead().scale.x;
            }
        }

        if (comp->layout.scaling.vertical.policy == LdScalePolicy::Relative)
        {
            comp->getTransformRW().scale.y = comp->layout.scaling.vertical.value * rootScale.y;

            /* Needed so we don't get pixel imperfect visual artifacts */
            comp->getTransformRW().scale.y = std::round(comp->getTransformRead().scale.y);

            if (!isLayoutHorizontal)
            {
                comp->layout.scaling.vertical.policy = LdScalePolicy::Absolute;
                comp->layout.scaling.vertical.value = comp->getTransformRead().scale.y;
            }
        }
    }
}

void PinchLayoutCalculator::calculateAndApplyScale()
{
    auto rootScale = root->getTransformRead().scale;
    auto& rootNodes = root->getNodes();
    glm::i16vec2 compsSize = {0, 0};

    int nr = 0;
    const bool isLayoutHorizontal = root->layout.orientation == LdOrientation::Horizontal;
    for (const auto& comp : rootNodes)
    {
        if (isLayoutHorizontal)
        {
            compsSize.x += comp->layout.scaling.horizontal.value;
            if (comp->getType() != AbstractComponent::CompType::PinchBar &&
                comp->layout.scaling.horizontal.value > comp->layout.scaling.horizontal.min)
            {
                nr++;
            }
        }
        else
        {
            compsSize.y += comp->layout.scaling.vertical.value;
            if (comp->getType() != AbstractComponent::CompType::PinchBar &&
                comp->layout.scaling.vertical.value > comp->layout.scaling.vertical.min)
            {
                nr++;
            }
        }
    }

    float rootIncrease = 0;
    if (isLayoutHorizontal) { rootIncrease = rootScale.x - compsSize.x; }
    else { rootIncrease = rootScale.y - compsSize.y; }

    // utils::printlne("rootInc {}", rootIncrease);
    if (nr == 0) { nr = 1; }
    float incEqual = rootIncrease / nr;
    for (const auto& comp : rootNodes)
    {
        if (isLayoutHorizontal)
        {
            comp->getTransformRW().scale.y = comp->layout.scaling.vertical.value * rootScale.y;

            /* Needed so we don't get pixel imperfect visual artifacts */
            comp->getTransformRW().scale.y = std::round(comp->getTransformRead().scale.y);
            if (comp->getType() == AbstractComponent::CompType::PinchBar) { continue; }

            comp->layout.scaling.horizontal.value += incEqual;
            comp->getTransformRW().scale.x = comp->layout.scaling.horizontal.value;
            comp->getTransformRW().scale.x = std::round(comp->getTransformRead().scale.x);
            comp->getTransformRW().scale.x = std::clamp(comp->getTransformRW().scale.x,
                comp->layout.scaling.horizontal.min, 20000.0f);
            comp->layout.scaling.horizontal.value = comp->getTransformRead().scale.x;
        }
    }

    // utils::printlne("Size {}", root->getTransformRead().scale.x);
    /* Accounting for rounding error unable to be caught above. This makes sure the last element doesn't get past the
       parent */
    // if (isLayoutHorizontal && (int)runningTotal.x != (int)root->getTransformRead().scale.x && rootNodes.size() >= 2)
    // {
    //     const auto lastComp = rootNodes[rootNodes.size() - 1];
    //     lastComp->getTransformRW().scale.x += (root->getTransformRead().scale.x - runningTotal.x);
    // }

    // if (!isLayoutHorizontal && (int)runningTotal.y != (int)root->getTransformRead().scale.y && rootNodes.size() >= 2)
    // {
    //     const auto lastComp = rootNodes[rootNodes.size() - 1];
    //     lastComp->getTransformRW().scale.y += (root->getTransformRead().scale.y - runningTotal.y);
    // }
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
