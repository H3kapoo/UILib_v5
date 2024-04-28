#include "Button.hpp"

#include "../Utility.hpp"

namespace components
{
Button::Button()
    : AbstractComponent({.type = "Button", .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl"})
{}

Button::~Button()
{
    // printlni("Deleting {} id {} ..", getType(), getId());
}

void Button::onPrepareToRender()
{
    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uColor", style.color);
}

void Button::addClickListener(std::function<void(int, int, MouseButton)> func)
{
    mouseClickCb = func;
}

void Button::onClickEvent()
{
    // println("Button element id {} has been clicked!", getId());

    const auto& s = getState();
    if (mouseClickCb) mouseClickCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
};

void Button::onMoveEvent()
{
    // const auto& s = getState();
    // println("For Button {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void Button::onStart()
{
    style.someOption = 10;
    // getBoxModelRW().pos.z = getDepth(); // TODO: Shall not exist
    // printlni("[INF] I am node {} and onStart() called", getId());
}

void Button::onLayoutUpdate()
{
    layoutCalc.calculate();
}

} // namespace components