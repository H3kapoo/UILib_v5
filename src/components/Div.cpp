#include "Div.hpp"

#include "../Utility.hpp"

namespace components
{
Div::Div()
    : AbstractComponent("Div")
{
    getKeeper().watch("uColor", &color);
}

Div::~Div()
{
    printlni("Deleting {} id {} ..", getType(), getId());
}

void Div::onClickListener(std::function<void(int, int, MouseButton)> func)
{
    mouseClickCb = func;
}

void Div::onClickEvent()
{
    println("Div element id {} has been clicked!", getId());
    const auto& s = getState();

    auto& bm = getBoxModel();
    bm.pos = {s->mouseX - bm.scale.x / 2, s->mouseY - bm.scale.y / 2, 1.0f};

    if (mouseClickCb) mouseClickCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
};

void Div::onMoveEvent()
{
    // const auto& s = getState();
    // println("For div {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void Div::onStart()
{
    printlni("[INF] I am node {} and onStart() called", getId());
}
} // namespace components