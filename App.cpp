#include "App.hpp"
#include "src/UIState.hpp"
#include "src/Utility.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/Div.hpp"
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
    // rootDiv.style.enableHScroll = false;
    // rootDiv.style.enableVScroll = false;
    // rootDiv.style.imagePath = NewValue("/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg");
    div6.style.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";

    // rootDiv.layout.borderSize = {.left = 6, .right = 50, .top = 20, .bottom = 14};
    // rootDiv.layout.borderSize = glm::vec4(6);
    rootDiv.layout.orientation = LdOrientation::Horizontal;
    rootDiv.layout.fillPolicy = LdFillPolicy::Tightly;
    // rootDiv.layout.align = LdAlignment{LdAlign::Center, LdAlign::Center};
    // rootDiv.layout.internalAlign = LdAlign::Center; // check this later after margins are implemented
    rootDiv.layout.wrap = LdWrapMode::NoWrap;
    // rootDiv.layout.scrollBarSize = 11;

    div3.style.borderColor = utils::hexToVec4("#13b613ff");
    // div3.layout.borderSize = {.left = 4, .right = 4, .top = 4, .bottom = 4};

    div3.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 600}, {LdScalePolicy::Absolute, 500}};
    div4.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 110}, {LdScalePolicy::Absolute, 100}};
    div5.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 400}, {LdScalePolicy::Absolute, 250}};
    div6.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 300}, {LdScalePolicy::Absolute, 290}};

    div2.style.color = utils::hexToVec4("#6de025ff");
    div3.style.color = utils::hexToVec4("#bc115bff");
    // div4.style.color = glm::vec4(0.4f, 0.2f, 0.9f, 1.0f); // utils::hexToVec4("#932758ff");
    div4.style.color = utils::hexToVec4("#932758ff");
    div5.style.color = utils::hexToVec4("#d5317bff");
    div6.style.color = utils::hexToVec4("#0608a5ff");

    componentManager.setRoot(&rootDiv);

    div3.layout.orientation = LdOrientation::Horizontal;
    div3.layout.fillPolicy = LdFillPolicy::Tightly;
    div3.layout.internalAlign = LdAlign::Top;
    div3.layout.wrap = LdWrapMode::NoWrap;
    // div3.layout.align = LdAlignment{LdAlign::Right, LdAlign::Center};

    rootDiv.append(&div3);
    div3.append({&div4, &div5, &div6});
    rootDiv.showTree();

    div3.layout.marginSize = {50, 5, 0, 0};
    // div5.layout.marginSize = {50, 50, 50, 50};
    div5.layout.marginSize = {0, 0, 5, 5};
    // div6.layout.marginSize = {0, 2, 4, 5};

    /* Trigger initial resize event to "stabilize" the scene */

    utils::printlni("size of {}B", sizeof(Div));
    utils::printlni("size of {}Kb", sizeof(Div) / 1024.0f);

    // utils::printlni("size of {}", sizeof(Div::AssignReloadable<bool>));
    // rootDiv.refreshOptions();

    // div5.addOnEnterListener([this]() { div5.style.color = utils::hexToVec4("#336b55ff"); });
    // div5.addOnExitListener([this]() { div5.style.color = utils::hexToVec4("#3b1526ff"); });

    // Div* newDiv = new Div();
    // newDiv->style.color = utils::hexToVec4("#a10b88ff");
    // newDiv->layout.scaling = LdScaling{{LdScalePolicy::Absolute, 300}, {LdScalePolicy::Absolute, 200}};
    // newDiv->style.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";

    // divs.push_back(newDiv);

    // div6.append(newDiv);

    rootDiv.addClickListener(
        [this](auto, auto, MouseButton btn)
        {
            // if (btn == MouseButton::Left)
            // {
            //     if (rootDiv.remove(&div3)) { utils::printlnw("Removed"); }
            //     else { utils::printlnw("NOT removed"); }
            // }
            // else if (btn == MouseButton::Right) { rootDiv.append(&div3); }
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
    componentManager.resizeEvent(initialWidth, initialHeight);
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

void App::mouseScrollEvent(double offsetX, double offsetY)
{
    componentManager.mouseScrollEvent(offsetX, offsetY);
}

void App::windowMaximizedEvent(int maximized)
{
    componentManager.windowMaximizedEvent(maximized);
}
