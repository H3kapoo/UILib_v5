#include "Div.hpp"

#include "../Utility.hpp"

namespace components
{
Div::Div()
    : AbstractComponent("Div")
{}

Div::~Div()
{
    println("Deleting {} id {} ..", getType(), getId());
}

void Div::onClickListener(std::function<void(int, int, MouseButton)> func)
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
    println("[INF] I am node {} and onStart() called", getId());
}
} // namespace components