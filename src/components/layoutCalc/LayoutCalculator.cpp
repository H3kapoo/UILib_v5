#include "LayoutCalculator.hpp"
#include "../../Utility.hpp"
#include "LayoutData.hpp"
#include <cstdint>

namespace components::layoutcalc
{
LayoutCalculator::LayoutCalculator(AbstractComponent* comp)
    : root{comp}
{}

void LayoutCalculator::calculate()
{
    /*
        border:
            x - top
            y - bottom
            z - left
            w - right
    */

    const auto rootLeftBorder = root->layout.borderSize.z;
    const auto rootRightBorder = root->layout.borderSize.w;
    const auto rootTopBorder = root->layout.borderSize.x;
    const auto rootBotBorder = root->layout.borderSize.y;

    for (const auto& childNode : root->getNodes())
    {
        calculateAndApplyScale(childNode);
    }

    glm::vec2 accumulatedSize = {0, 0};
    for (const auto& childNode : root->getNodes())
    {
        accumulatedSize += childNode->getTransformRead().scale;
    }

    const auto& rootBox = root->getTransformRead();
    glm::vec2 startXY = rootBox.pos;
    // TODO: take into accound margin/border/padd
    const auto remainingSpace = root->getTransformRead().scale - accumulatedSize;
    for (const auto& childNode : root->getNodes())
    {
        if (root->layout.orientation == LayoutData::Orientation::Horizontal)
        {
            if (root->layout.childPacking == LayoutData::ChildPacking::EvenlySpaced)
            {
                const auto offset = remainingSpace.x / (root->getNodes().size() + 1);
                startXY.x += offset;
                childNode->getTransformRW().pos = startXY;
                startXY.x += childNode->getTransformRead().scale.x;
            }
            else if (root->layout.childPacking == LayoutData::ChildPacking::Tight)
            {
                childNode->getTransformRW().pos = startXY;
                startXY.x += childNode->getTransformRead().scale.x;
            }
        }
        else if (root->layout.orientation == LayoutData::Orientation::Vertical)
        {
            if (root->layout.childPacking == LayoutData::ChildPacking::EvenlySpaced)
            {
                const auto offset = remainingSpace.y / (root->getNodes().size() + 1);
                startXY.y += offset;
                childNode->getTransformRW().pos = startXY;
                startXY.y += childNode->getTransformRead().scale.y;
            }
            else if (root->layout.childPacking == LayoutData::ChildPacking::Tight)
            {
                childNode->getTransformRW().pos = startXY;
                startXY.y += childNode->getTransformRead().scale.y;
            }
        }
    }

    const auto bound = getChildrenBound(root->getNodes());
    const auto boundsDiff = bound.end - bound.start;
    glm::vec2 offset = {0, 0};

    for (const auto& childNode : root->getNodes())
    {
        // clang-format off
        switch (root->layout.childPos)
        {
            case LayoutData::ChildPos::TopLeft:
                break;
            case LayoutData::ChildPos::TopCenter:
            {
                int halfDiffX = boundsDiff.x / 2;
                int spaceLeft = rootBox.scale.x + rootLeftBorder - rootRightBorder;
                offset.x = spaceLeft / 2.0f - halfDiffX;
                offset.y = rootTopBorder;
                break;
            }
            case LayoutData::ChildPos::TopRight:
            {
                offset.x = rootBox.scale.x - (boundsDiff.x + rootRightBorder);
                offset.y = rootTopBorder;
                break;
            }
            case LayoutData::ChildPos::MidLeft:
            {
                int halfDiffY = boundsDiff.y / 2;
                int spaceLeft = rootBox.scale.y + rootTopBorder - rootBotBorder;
                offset.y = spaceLeft / 2.0f - halfDiffY;
                offset.x = rootLeftBorder;
                break;
            }
            case LayoutData::ChildPos::MidCenter:
            {
                int halfDiffX = boundsDiff.x / 2;
                int halfDiffY = boundsDiff.y / 2;
                int spaceLeftX = rootBox.scale.x + rootLeftBorder - rootRightBorder;
                int spaceLeftY = rootBox.scale.y + rootTopBorder - rootBotBorder;
                offset.x = spaceLeftX / 2.0f - halfDiffX;
                offset.y = spaceLeftY / 2.0f - halfDiffY;
                break;
            }
            case LayoutData::ChildPos::MidRight:
                break;
            case LayoutData::ChildPos::BottomLeft:
                break;
            case LayoutData::ChildPos::BottomCenter:
                break;
            case LayoutData::ChildPos::BottomRight:
                break;
        }
        // clang-format on

        childNode->getTransformRW().pos += offset;
    }
}

void LayoutCalculator::calculateAndApplyScale(AbstractComponent* comp)
{
    if (comp->layout.sizingPolicy == LayoutData::SizingPolicy::Absolute)
    {
        comp->getTransformRW().scale = comp->layout.size;
    }
}

void LayoutCalculator::calculateNextBasePosition(glm::vec2& currentXY, AbstractComponent* comp)
{
    // todo: do not calc bound each time
    const auto bound = getChildrenBound(root->getNodes());

    if (root->layout.orientation == LayoutData::Orientation::Horizontal)
    {
        const auto diffX = root->getTransformRead().scale.x - (bound.end.x - bound.start.x);
        const auto addOffset = diffX / root->getNodes().size(); // - comp->getTransformRead().scale.x / 2;
        currentXY.x += addOffset;
    }

    comp->getTransformRW().pos = currentXY;
    return;

    if (root->layout.orientation == LayoutData::Orientation::Horizontal)
    {
        currentXY.x += comp->getTransformRead().scale.x;
    }
    else if (root->layout.orientation == LayoutData::Orientation::Vertical)
    {
        currentXY.y += comp->getTransformRead().scale.y;
    }
}

LayoutCalculator::Bounds LayoutCalculator::getChildrenBound(const std::vector<AbstractComponent*>& childComps)
{
    Bounds result;
    for (const auto& comp : childComps) // 0 = sbCount_
    {
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
    return result;
}
} // namespace components::layoutcalc
