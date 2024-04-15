#include "App.hpp"
#include "src/Utility.hpp"

App::~App()
{
    for (auto&& o : divs)
        delete o;
}

void App::start(int initialWidth, int initialHeight)
{
    /* Create objects with known long lifetime */
    // rootDiv = new Div();
    // div2 = new Div();

    /* Root needs to be set before performing any append/rm operation */
    componentManager.setRoot(&rootDiv);
    // rootDiv.append({&div2, &div3, &div4});

    rootDiv.style.color = utils::hexToVec4("#125ea9ff");
    // div2.style.color = utils::hexToVec4("#9212a9ff");
    // rootDiv.layoutOptions.someOption = true;
    // divs.push_back(&rootDiv);
    for (int i = 0; i < 10; i++)
    {
        const auto div = new Div();
        div->style.color = utils::hexToVec4("#9212a9ff");
        divs.push_back(div);
    }
    rootDiv.append(divs);

    rootDiv.showTree();

    /* Trigger initial resize event to "stabilize" the scene */
    componentManager.resizeEvent(initialWidth, initialHeight);
}

void App::update()
{
    // TODO: This needs to be carefully thought when it should be called and on what
    componentManager.updateLayout();
    componentManager.render();
}

void App::mouseClickEvent(MouseButton button, KeyAction action, ActiveModifiersBits mods)
{
    componentManager.mouseClickEvent(button, action, mods);
}

void App::mouseMoveEvent(double mouseX, double mouseY)
{
    componentManager.mouseMoveEvent(mouseX, mouseY);
}

void App::resizeEvent(int newWidth, int newHeight)
{
    componentManager.resizeEvent(newWidth, newHeight);
}