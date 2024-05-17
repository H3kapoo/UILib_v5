#pragma once

#include "src/UIState.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/Button.hpp"
#include "src/components/ComponentManager.hpp"
#include "src/components/Div.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"
#include "src/inputManagement/Input.hpp"

using namespace components;
using namespace inputManagement;
using namespace layoutcalc;

class App
{
public:
    ~App();

    void start(int initialWidth, int initialHeight);
    void update();

    /* Events handlers */
    void mouseClickEvent(MouseButton button, HIDAction action, ActiveModifiersBits mods);
    void mouseMoveEvent(double mouseX, double mouseY);
    void mouseScrollEvent(double offsetX, double offsetY);
    void keyEvent(int key, HIDAction action, int mods);
    void resizeEvent(int newWidth, int newHeight);

private:
    ComponentManager componentManager;
    Div rootDiv;
    Div div2, div3, div4, div5, div6;
    std::vector<AbstractComponent*> divs;
};