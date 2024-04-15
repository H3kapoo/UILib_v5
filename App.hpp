#pragma once

#include "src/UIState.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/ComponentManager.hpp"
#include "src/components/Div.hpp"
#include "src/inputManagement/Input.hpp"

using namespace components;
using namespace inputManagement;

class App
{
public:
    ~App();

    void start(int initialWidth, int initialHeight);
    void update();

    /* Events handlers */
    void mouseClickEvent(MouseButton button, KeyAction action, ActiveModifiersBits mods);
    void mouseMoveEvent(double mouseX, double mouseY);
    void resizeEvent(int newWidth, int newHeight);

private:
    ComponentManager componentManager;
    Div rootDiv;
    std::vector<AbstractComponent*> divs;
};