#include "LayoutCalculator.hpp"
#include "../../Utility.hpp"
#include "LayoutData.hpp"
#include <cstdint>

namespace components::layoutcalc
{
LayoutCalculator::LayoutCalculator(AbstractComponent* comp)
    : root{comp}
{}

void LayoutCalculator::calculate(const int ceva)
{
    /*
        border:
            x - bottom
            y - top
            z - left
            w - right
    */

    // utils::printlni("Calculating layout for ID: {}", root->getId());
    const auto& rootBox = root->getTransformRead();
    glm::vec2 startXY = {0, 0};

    // startXY = {root->layout.borderSize.z + rootBox.pos.x, root->layout.borderSize.x + rootBox.pos.y};
    startXY = {rootBox.pos.x, rootBox.pos.y};

    for (const auto& childNode : root->getNodes())
    {
        if (childNode->layout.sizingPolicy == LayoutData::SizingPolicy::Absolute)
        {
            childNode->getBoxModelRW().scale = childNode->layout.size;
        }

        childNode->getBoxModelRW().pos = startXY;
        startXY.x += childNode->getTransformRead().scale.x;
    }

    const auto bound = getChildrenBound(root->getNodes());
    int diffX = bound.end.x - bound.start.x;
    glm::vec2 addToChild = {0, 0};
    // clang-format off
    switch (root->layout.childPolicy)
    {
    case LayoutData::ChildPolicy::Tight:
        addToChild.x = root->layout.borderSize.z;
        addToChild.y = root->layout.borderSize.y;
        break;
    case LayoutData::ChildPolicy::TightCenter:
    {
        int halfDiffX = diffX / 2;
        addToChild.x = (rootBox.scale.x + rootBox.pos.x) / 2 - halfDiffX;
        break;
    }
    case LayoutData::ChildPolicy::TightReverse:
    {
        addToChild.x = (rootBox.scale.x + rootBox.pos.x) -
                     (diffX + root->layout.borderSize.z + root->layout.borderSize.w);
        break;
    }
    case LayoutData::ChildPolicy::EvenlySpaced:
        break;
    default:
        utils::printlne("Unsupported ChildPolicy {}!", static_cast<int>(root->layout.childPolicy));
    }
    // clang-format on

    for (const auto& childNode : root->getNodes())
    {
        childNode->getBoxModelRW().pos += addToChild;
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
