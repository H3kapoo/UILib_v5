#include "App.hpp"
#include "src/UIState.hpp"
#include "src/Utility.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"

App::~App()
{
    for (auto&& o : divs)
        delete o;
}

void App::start(int initialWidth, int initialHeight)
{
    rootDiv.style.color = utils::hexToVec4("#125ea9ff");
    rootDiv.style.borderColor = utils::hexToVec4("#c85c0fff");
    // rootDiv.layout.borderSize = {.left = 6, .right = 50, .top = 20, .bottom = 14};
    // rootDiv.layout.borderSize = glm::vec4(6);
    rootDiv.layout.orientation = LdOrientation::Vertical;
    rootDiv.layout.fillPolicy = LdFillPolicy::Tightly;
    rootDiv.layout.align = LdAlignment{LdAlign::Left, LdAlign::Top};
    rootDiv.layout.internalAlign = LdAlign::Top;
    rootDiv.layout.wrap = LdWrapMode::NoWrap;

    div3.style.borderColor = utils::hexToVec4("#13b613ff");
    // div3.layout.borderSize = {.left = 4, .right = 4, .top = 4, .bottom = 4};

    // div3.layout.marginSize = glm::vec4(6.0f);
    // div3.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 200}, {LdScalePolicy::Absolute, 200}};
    // div4.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 550}, {LdScalePolicy::Absolute, 100}};
    // div5.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 700}, {LdScalePolicy::Absolute, 250}};

    div3.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 200}, {LdScalePolicy::Absolute, 200}};
    div4.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 300}, {LdScalePolicy::Absolute, 100}};
    div5.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 200}, {LdScalePolicy::Absolute, 250}};

    div2.style.color = utils::hexToVec4("#6de025ff");
    div3.style.color = utils::hexToVec4("#bc115bff");
    // div4.style.color = glm::vec4(0.4f, 0.2f, 0.9f, 1.0f); // utils::hexToVec4("#932758ff");
    div4.style.color = utils::hexToVec4("#932758ff");
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
        [this](auto, auto, MouseButton)
        {
            // if (btn == MouseButton::Left)
            // {
            //     uint8_t x = (uint8_t)rootDiv.layout.orientation + 1;
            //     rootDiv.layout.orientation = LdOrientation(x % (uint8_t)LdOrientation::COUNT);
            // }
            // if (btn == MouseButton::Right)
            // {
            //     uint8_t x = (uint8_t)rootDiv.layout.fillPolicy + 1;
            //     rootDiv.layout.fillPolicy = LdFillPolicy(x % (uint8_t)LdFillPolicy::COUNT);
            // }
            // rootDiv.updateLayout();
        });

    // rootDiv.addOnKeyListener(
    //     [this](const HIDAction* keyAction)
    //     {
    //         if (keyAction[GLFW_KEY_D] == HIDAction::Pressed)
    //         {
    //             uint8_t x = (uint8_t)rootDiv.layout.align.horizontal + 1;
    //             rootDiv.layout.align.horizontal = LdAlign(x % (uint8_t)LdAlign::COUNT);
    //         }
    //         if (keyAction[GLFW_KEY_W] == HIDAction::Pressed)
    //         {
    //             uint8_t x = (uint8_t)rootDiv.layout.align.vertical + 1;
    //             rootDiv.layout.align.vertical = LdAlign(x % (uint8_t)LdAlign::COUNT);
    //         }
    //         rootDiv.updateLayout();
    //     });
}

void App::update()
{
    // rootDiv.updateLayout();

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