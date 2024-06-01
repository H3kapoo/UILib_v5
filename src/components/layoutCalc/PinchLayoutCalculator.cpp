#include "PinchLayoutCalculator.hpp"
#include "../../Utility.hpp"
#include "LayoutData.hpp"
#include <cstdint>

namespace components::layoutcalc
{
#define SKIP_SCROLLBAR(x)                                                                                              \
    if (x->getType() == AbstractComponent::CompType::ScrollBar) { continue; }

PinchLayoutCalculator::PinchLayoutCalculator(AbstractComponent* comp)
    : root{comp}
{}

void PinchLayoutCalculator::calculate()
{
    /* Reset positions */
    resetPositions();

    /* Calculate scale */
    calculateAndApplyScale();

    /* Calculate position based on FillPolicy & Orientation */
    calculateAndApplyPosition();
}

void PinchLayoutCalculator::calculateAndApplyScale()
{
    auto rootScale = getPaddedRootTransform().scale;
    auto& rootNodes = root->getNodes();
    /* If we have some children with Absolute scale, then Relative scale of the other children of root will be relative
       to the space remaining after subtracting thr Absolute scales from the root. */
    glm::i16vec2 absSubtract = {0, 0};
    const bool isLayoutHorizontal = root->layout.orientation == LdOrientation::Horizontal;
    for (const auto& comp : rootNodes)
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

    float totalX = 0;
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
        totalX += comp->getTransformRW().scale.x;
    }

    if ((int)totalX != (int)root->getTransformRead().scale.x && rootNodes.size() >= 2)
    {
        const auto lastComp = rootNodes[rootNodes.size() - 1];
        lastComp->getTransformRW().scale.x += (root->getTransformRead().scale.x - totalX);
    }
}

void PinchLayoutCalculator::calculateAndApplyPosition()
{
    glm::vec2 startXY = {0, 0};

    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        const auto& childMargin = childNode->layout.margin;
        glm::i16vec2 nextPos = {childMargin.left, childMargin.top};

        const auto& childTransform = getAdjustedTransform(childNode);
        auto& childPos = childNode->getTransformRW().pos;

        if (root->layout.orientation == LayoutData::Orientation::Horizontal)
        {
            childPos.x += startXY.x;
            startXY.x += childTransform.scale.x;
        }
        else if (root->layout.orientation == LayoutData::Orientation::Vertical)
        {
            childPos.y += startXY.y;
            startXY.y += childTransform.scale.y;
        }
    }
}

void PinchLayoutCalculator::resetPositions()
{
    for (const auto& childNode : root->getNodes())
    {
        SKIP_SCROLLBAR(childNode)

        childNode->getTransformRW().pos = {0, 0};
    }
}

glm::vec2 PinchLayoutCalculator::getRemainingSpaceAfterScale()
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

    return remainingSpace;
}

PinchLayoutCalculator::Bounds PinchLayoutCalculator::getChildrenBound(const std::vector<AbstractComponent*>& childComps)
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
