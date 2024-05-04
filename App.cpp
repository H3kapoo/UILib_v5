#include "App.hpp"
#include "GLFW/glfw3.h"
#include "src/UIState.hpp"
#include "src/Utility.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"
#include <cstdint>

App::~App()
{
    for (auto&& o : divs)
        delete o;
}

void App::start(int initialWidth, int initialHeight)
{
    rootDiv.style.color = utils::hexToVec4("#125ea9ff");
    rootDiv.style.borderColor = utils::hexToVec4("#7210bcff");
    rootDiv.layout.borderSize = glm::vec4(6, 12, 18, 50);
    // rootDiv.layout.borderSize = glm::vec4(6);
    rootDiv.layout.orientation = LdOrientation::Vertical;
    rootDiv.layout.fillPolicy = LdFillPolicy::Tightly;
    rootDiv.layout.align = LdAlignment{LdAlign::Left, LdAlign::Top};
    rootDiv.layout.internalAlign = LdAlign::Top;
    rootDiv.layout.wrap = LdWrapMode::NoWrap;

    div3.style.borderColor = utils::hexToVec4("#0bb636ff");
    // div3.layout.borderSize = glm::vec4(36, 12, 18, 50);
    // div3.layout.marginSize = glm::vec4(6.0f);
    div3.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 100}, {LdScalePolicy::Absolute, 200}};
    div4.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 100}, {LdScalePolicy::Absolute, 100}};
    div5.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 200}, {LdScalePolicy::Absolute, 100}};

    div2.style.color = utils::hexToVec4("#6de025ff");
    div3.style.color = utils::hexToVec4("#bc115bff");
    div4.style.color = utils::hexToVec4("#b33970ff");
    div5.style.color = utils::hexToVec4("#d5317bff");

    componentManager.setRoot(&rootDiv);

    // div3.append(&div2);
    rootDiv.append({&div3, &div4, &div5});
    // div3.append({&div4, &div5});

    rootDiv.showTree();

    /* Trigger initial resize event to "stabilize" the scene */
    componentManager.resizeEvent(initialWidth, initialHeight);

    utils::printlni("size of {}B", sizeof(Div));
    utils::printlni("size of {}Kb", sizeof(Div) / 1024.0f);

    // div5.addOnEnterListener([this]() { div5.style.color = utils::hexToVec4("#336b55ff"); });
    // div5.addOnExitListener([this]() { div5.style.color = utils::hexToVec4("#3b1526ff"); });
    rootDiv.addClickListener(
        [this](auto, auto, MouseButton btn)
        {
            if (btn == MouseButton::Left)
            {
                uint8_t x = (uint8_t)rootDiv.layout.orientation + 1;
                rootDiv.layout.orientation = LdOrientation(x % (uint8_t)LdOrientation::COUNT);
            }
            if (btn == MouseButton::Right)
            {
                uint8_t x = (uint8_t)rootDiv.layout.fillPolicy + 1;
                rootDiv.layout.fillPolicy = LdFillPolicy(x % (uint8_t)LdFillPolicy::COUNT);
            }
            // rootDiv.layout.childPos = LayoutData::ChildPos::TopLeft;
            rootDiv.updateLayout();
        });

    rootDiv.addOnKeyListener(
        [this](const HIDAction* keyAction)
        {
            if (keyAction[GLFW_KEY_A] == HIDAction::Pressed)
            {
                utils::printlni("Pressed A key");
                uint8_t x = (uint8_t)rootDiv.layout.fillPolicy + 1;
                rootDiv.layout.fillPolicy = LdFillPolicy(x % (uint8_t)LdFillPolicy::COUNT);
                rootDiv.updateLayout();
            }
            else if (keyAction[GLFW_KEY_A] == HIDAction::Released) { utils::printlni("Released A key"); }
        });
}

void App::update()
{
    // TODO: This needs to be carefully thought when it should be called and on what
    componentManager.render();
}

void App::mouseClickEvent(MouseButton button, HIDAction action, ActiveModifiersBits mods)
{
    if (button == MouseButton::Left && action == HIDAction::Pressed)
    {
        // rootDiv.isComponentRenderable() ? rootDiv.setRenderable(false) : rootDiv.setRenderable(true);
    }
    componentManager.mouseClickEvent(button, action, mods);
}

void App::mouseMoveEvent(double mouseX, double mouseY)
{
    componentManager.mouseMoveEvent(mouseX, mouseY);
}

void App::keyEvent(int key, HIDAction action, int mods)
{
    componentManager.keyEvent(key, action, mods);
}

void App::resizeEvent(int newWidth, int newHeight)
{
    componentManager.resizeEvent(newWidth, newHeight);
}