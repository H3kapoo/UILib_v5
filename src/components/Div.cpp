#include "Div.hpp"

#include "../Utility.hpp"
#include "AbstractComponent.hpp"
#include <unordered_map>

namespace components
{
Div::Div()
    : AbstractComponent("Div")
{}

Div::~Div()
{
    // printlni("Deleting {} id {} ..", getType(), getId());
}

void Div::onPrepareToRender(std::unordered_map<std::string, std::vector<glm::vec4>>& vec4s)
{
    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uColor", style.color);
}

void Div::addClickListener(std::function<void(int, int, MouseButton)> func)
{
    mouseClickCb = func;
}

void Div::onClickEvent()
{
    // println("Div element id {} has been clicked!", getId());

    const auto& s = getState();
    if (mouseClickCb) mouseClickCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
};

void Div::onMoveEvent()
{
    // const auto& s = getState();
    // println("For div {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void Div::onStart()
{
    // getBoxModelRW().pos.z = getDepth(); // TODO: Shall not exist
    // printlni("[INF] I am node {} and onStart() called", getId());
}

void Div::onLayoutUpdate()
{
    layoutCalc.calculate(style.someOption);
}

} // namespace components