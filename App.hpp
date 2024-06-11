#pragma once

#include "src/UIState.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/Button.hpp"
#include "src/components/ComponentManager.hpp"
#include "src/components/Div.hpp"
#include "src/components/ResizeDiv.hpp"
#include "src/components/TabSwitcher.hpp"

using namespace components;
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
    void windowMaximizedEvent(int maximized);

private:
    ComponentManager componentManager;
    Div rootDiv;
    Div div2, div3, div4, div5, div6, div7, div8, div9;
    // Button btn;
    TabSwitcher ts, ts2;
    ResizeDiv pdiv, pdiv2, pdiv3;
    std::vector<AbstractComponent*> divs;
};