#include "LayoutCalculator.hpp"
#include "../../Utility.hpp"

namespace components::layoutcalc
{
LayoutCalculator::LayoutCalculator(AbstractComponent* comp)
    : root{comp}
{}

void LayoutCalculator::calculate(const bool ceva)
{
    // printlni("Calculating layout for ID: {}", root->getId());
    // const auto middle = root->getBoxModelRead().scale / 2.0f;
    glm::vec2 startXY = {0, 0};
    for (const auto& childNode : root->getNodes())
    {
        childNode->getBoxModelRW().scale = {100, 100};
        childNode->getBoxModelRW().pos = startXY;
        startXY.x += 200;
    }
}
// maybe just let all have the same options available..
} // namespace components::layoutcalc
