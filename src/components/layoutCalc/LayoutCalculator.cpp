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
    auto remainingSpace = root->getTransformRead().scale - accumulatedSize;
    remainingSpace.x -= (rootLeftBorder + rootRightBorder);
    remainingSpace.y -= (rootTopBorder + rootBotBorder);
    for (const auto& childNode : root->getNodes())
    {
        if (root->layout.orientation == LayoutData::Orientation::Horizontal)
        {
            if (root->layout.fillPolicy == LayoutData::FillPolicy::EvenlySpaced)
            {
                const auto offset = remainingSpace.x / (root->getNodes().size() + 1);
                startXY.x += offset;
                childNode->getTransformRW().pos = startXY;
                startXY.x += childNode->getTransformRead().scale.x;
            }
            else if (root->layout.fillPolicy == LayoutData::FillPolicy::Tightly)
            {
                childNode->getTransformRW().pos = startXY;
                startXY.x += childNode->getTransformRead().scale.x;
            }
        }
        else if (root->layout.orientation == LayoutData::Orientation::Vertical)
        {
            if (root->layout.fillPolicy == LayoutData::FillPolicy::EvenlySpaced)
            {
                const auto offset = remainingSpace.y / (root->getNodes().size() + 1);
                startXY.y += offset;
                childNode->getTransformRW().pos = startXY;
                startXY.y += childNode->getTransformRead().scale.y;
            }
            else if (root->layout.fillPolicy == LayoutData::FillPolicy::Tightly)
            {
                childNode->getTransformRW().pos = startXY;
                startXY.y += childNode->getTransformRead().scale.y;
            }
        }
    }

    const auto bound = getChildrenBound(root->getNodes());
    const auto boundsDiff = bound.end - bound.start;
    glm::vec2 offset = {0, 0};
    // for (const auto& childNode : root->getNodes())
    // {
    //     // clang-format off
    //     switch (root->layout.childPos)
    //     {
    //         case LayoutData::ChildPos::TopLeft:
    //         {
    //             offset.x = rootLeftBorder;
    //             offset.y = rootTopBorder;
    //             break;
    //         }
    //         case LayoutData::ChildPos::TopCenter:
    //         {
    //             int halfDiffX = boundsDiff.x / 2;
    //             int spaceLeft = rootBox.scale.x + rootLeftBorder - rootRightBorder;
    //             offset.x = spaceLeft / 2.0f - halfDiffX;
    //             offset.y = rootTopBorder;
    //             break;
    //         }
    //         case LayoutData::ChildPos::TopRight:
    //         {
    //             offset.x = rootBox.scale.x - (boundsDiff.x + rootRightBorder);
    //             offset.y = rootTopBorder;
    //             break;
    //         }
    //         case LayoutData::ChildPos::MidLeft:
    //         {
    //             int halfDiffY = boundsDiff.y / 2;
    //             int spaceLeft = rootBox.scale.y + rootTopBorder - rootBotBorder;
    //             offset.y = spaceLeft / 2.0f - halfDiffY;
    //             offset.x = rootLeftBorder;
    //             break;
    //         }
    //         case LayoutData::ChildPos::MidCenter:
    //         {
    //             int halfDiffX = boundsDiff.x / 2;
    //             int halfDiffY = boundsDiff.y / 2;
    //             int spaceLeftX = rootBox.scale.x + rootLeftBorder - rootRightBorder;
    //             int spaceLeftY = rootBox.scale.y + rootTopBorder - rootBotBorder;
    //             offset.x = spaceLeftX / 2.0f - halfDiffX;
    //             offset.y = spaceLeftY / 2.0f - halfDiffY;
    //             break;
    //         }
    //         case LayoutData::ChildPos::MidRight:
    //         {
    //             int halfDiffY = boundsDiff.y / 2;
    //             int spaceLeft = rootBox.scale.y + rootTopBorder - rootBotBorder;
    //             offset.x = rootBox.scale.x - (boundsDiff.x + rootRightBorder);
    //             offset.y = spaceLeft / 2.0f - halfDiffY;
    //             break;
    //         }
    //         case LayoutData::ChildPos::BottomLeft:
    //         {
    //             offset.x = rootLeftBorder;
    //             offset.y = rootBox.scale.y - (boundsDiff.y + rootBotBorder);
    //             break;
    //         }
    //         case LayoutData::ChildPos::BottomCenter:
    //         {
    //             int halfDiffX = boundsDiff.x / 2;
    //             int spaceLeft = rootBox.scale.x + rootLeftBorder - rootRightBorder;
    //             offset.x = spaceLeft / 2.0f - halfDiffX;
    //             offset.y = rootBox.scale.y - (boundsDiff.y + rootBotBorder);
    //             break;
    //         }
    //         case LayoutData::ChildPos::BottomRight:
    //         {
    //             offset.x = rootBox.scale.x - (boundsDiff.x + rootRightBorder);
    //             offset.y = rootBox.scale.y - (boundsDiff.y + rootBotBorder);
    //             break;
    //         }
    //     }
    //     // clang-format on

    //     childNode->getTransformRW().pos += offset;
    // }
}

void LayoutCalculator::calculateAndApplyScale(AbstractComponent* comp)
{
    if (comp->layout.scaling.horizontal.policy == LayoutData::ScalePolicy::Absolute)
    {
        comp->getTransformRW().scale.x = comp->layout.scaling.horizontal.value;
    }

    if (comp->layout.scaling.vertical.policy == LayoutData::ScalePolicy::Absolute)
    {
        comp->getTransformRW().scale.y = comp->layout.scaling.vertical.value;
    }

    if (comp->layout.scaling.horizontal.policy == LayoutData::ScalePolicy::Relative)
    {
        const auto rootLeftBorder = root->layout.borderSize.z;
        const auto rootRightBorder = root->layout.borderSize.w;
        // const auto rootTopBorder = root->layout.borderSize.x;
        // const auto rootBotBorder = root->layout.borderSize.y;
        comp->getTransformRW().scale.x = comp->layout.scaling.horizontal.value *
                                         (root->getTransformRead().scale.x - (rootLeftBorder + rootRightBorder));
    }

    if (comp->layout.scaling.vertical.policy == LayoutData::ScalePolicy::Relative)
    {
        const auto rootTopBorder = root->layout.borderSize.x;
        const auto rootBotBorder = root->layout.borderSize.y;
        comp->getTransformRW().scale.y = comp->layout.scaling.vertical.value *
                                         (root->getTransformRead().scale.y - (rootTopBorder + rootBotBorder));
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
