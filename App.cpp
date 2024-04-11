#include "App.hpp"

App::~App()
{
    for (auto&& o : divs)
        delete o;
}

void App::start(int initialWidth, int initialHeight)
{
    // Div* div2 = new Div();
    // Div* div3 = new Div();
    // Div* div4 = new Div();
    // Div* div5 = new Div();
    // Div* div6 = new Div();

    rootDiv = new Div();
    cm.setRoot(rootDiv);

    divs.push_back(rootDiv);
    for (int i = 0; i < 5'000; i++)
    {
        divs.push_back(new Div());
    }
    rootDiv->append(divs);

    cm.resizeEvent(initialWidth, initialHeight);
}

void App::update()
{
    cm.render();
}

void App::mouseClickEvent(MouseButton button, KeyAction action, ActiveModifiersBits mods)
{
    cm.mouseClickEvent(button, action, mods);
}

void App::mouseMoveEvent(double mouseX, double mouseY)
{
    cm.mouseMoveEvent(mouseX, mouseY);
}

void App::resizeEvent(int newWidth, int newHeight)
{
    cm.resizeEvent(newWidth, newHeight);
}