#include "LayoutCalculator.hpp"
#include "../../Utility.hpp"

namespace components::layoutcalc
{
LayoutCalculator::LayoutCalculator(AbstractComponent* comp)
    : root{comp}
{}

void LayoutCalculator::calculate(const int ceva)
{
    utils::printlni("Calculating layout for ID: {}", root->getId());
    // const auto middle = root->getBoxModelRead().scale / 2.0f;
    glm::vec2 startXY = {0, 0};
    const auto cache = root->getBoxModelRead().scale;
    for (const auto& childNode : root->getNodes())
    {
        // if (startXY.x + ceva/2.0f > cache.scale.x)
        // {
        //     startXY.x = 0;
        //     startXY.y += 20;
        // }

        childNode->getBoxModelRW().scale = cache / 2.0f;
        childNode->getBoxModelRW().pos = startXY;
        // startXY.x += 200;
    }
}
// maybe just let all have the same options available..
} // namespace components::layoutcalc
