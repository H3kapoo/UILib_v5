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
    componentManager.setRoot(&rootDiv);

    rootDiv.style.color = utils::hexToVec4("#125ea9ff");
    rootDiv.layout.padding = {5, 5, 5, 5};

    ts.style.color = utils::hexToVec4("#c85c0fff");
    ts.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.8f}, {LdScalePolicy::Relative, 0.8f}};

    // ts.addTab("LeftTab", 0, &div2);
    // ts.addTab("RightTab", 1, &div3);

    rootDiv.append(&ts);
    // rootDiv.append({&div2, &div3});

    // div2.layout.orientation = LdOrientation::Vertical;
    // div2.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.2f}, {LdScalePolicy::Relative, 1.0f}};
    // div2.style.color = utils::hexToVec4("#c85c0fff");

    // div3.layout.fillPolicy = LdFillPolicy::Grid;
    // div3.layout.grid.config = LdGridConfig{2, 2};
    // div3.layout.border = {10, 10, 10, 10};
    // div3.style.borderColor = utils::hexToVec4("#090406ff");
    // div3.style.color = utils::hexToVec4("#bc115bff");
    // div3.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.8f}, {LdScalePolicy::Relative, 1.0f}};

    // div3.append(&btn);

    // btn.style.color = utils::hexToVec4("#404140ff");
    // btn.style.borderColor = utils::hexToVec4("#4171a1ff");
    // btn.sideImage.path = "/home/hekapoo/newTryAtUI/src/assets/textures/arrow_left.png";
    // btn.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Absolute, 50}};
    // btn.layout.border = {4, 4, 4, 4};
    // btn.addClickListener(
    //     [](int x, int y, MouseButton mButton)
    //     {
    //         if (mButton == MouseButton::Left) { utils::printlne("{} {}", x, y); }
    //     });

    // div4.style.color = utils::hexToVec4("#c85c0fff");
    // div4.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.5f}, {LdScalePolicy::Relative, 0.5f}};
    // div4.layout.align = LdAlignment{LdAlign::Center, LdAlign::Center};

    // div5.style.color = utils::hexToVec4("#820437ff");
    // div5.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.5f}, {LdScalePolicy::Relative, 0.5f}};
    // div4.append(&div5);

    // rootDiv.append({&div4});

    // rootDiv.addClickListener(
    //     [this](auto, auto, MouseButton btn)
    //     {
    //         if (btn == MouseButton::Left)
    //         {
    //             rootDiv.remove(&div3);
    //             rootDiv.append(&div4);
    //         }
    //         else if (btn == MouseButton::Right)
    //         {
    //             rootDiv.remove(&div4);
    //             rootDiv.append(&div3);
    //         }
    //     });
    // div2.append({&div4, &div5});

    // div4.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Absolute, 200}};
    // div4.style.color = utils::hexToVec4("#492736ff");

    // div5.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Absolute, 300}};
    // div5.style.color = utils::hexToVec4("#381b28ff");

    // div6.style.color = utils::hexToVec4("#24a40dff");
    // div6.style.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";
    // div6.layout.margin = {5, 5, 5, 0};
    // div6.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};

    // div7.style.color = utils::hexToVec4("#24a42dff");
    // div7.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};

    // div8.style.color = utils::hexToVec4("#24a44dff");
    // div8.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};

    // div9.style.color = utils::hexToVec4("#24a46dff");
    // div9.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};

    // div6.layout.grid.pos = {0, 0};
    // div7.layout.grid.pos = {0, 1};
    // div8.layout.grid.pos = {1, 0};
    // div9.layout.grid.pos = {1, 1};
    // div3.append({&div6, &div7, &div8, &div9});
    // rootDiv.style.color = utils::hexToVec4("#125ea9ff");
    // rootDiv.style.borderColor = utils::hexToVec4("#c85c0fff");
    // // rootDiv.style.enableHScroll = false;
    // // rootDiv.style.enableVScroll = false;
    // // rootDiv.style.imagePath = NewValue("/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg");

    // // rootDiv.layout.borderSize = {.left = 6, .right = 50, .top = 20, .bottom = 14};
    // // rootDiv.layout.borderSize = glm::vec4(6);
    // rootDiv.layout.orientation = LdOrientation::Horizontal;
    // rootDiv.layout.fillPolicy = LdFillPolicy::Tightly;

    // // rootDiv.layout.grid.row = 1;
    // // rootDiv.layout.grid.rowSpan = 2;
    // // rootDiv.layout.grid.col = 1;
    // // rootDiv.layout.grid.colSpan = 1;
    // // rootDiv.layout.align = LdAlignment{LdAlign::Center, LdAlign::Center};
    // // rootDiv.layout.internalAlign = LdAlign::Center; // check this later after margins are implemented
    // // rootDiv.layout.wrap = LdWrapMode::NoWrap;
    // // rootDiv.layout.wrap = LdWrapMode::WrapAround;
    // // rootDiv.layout.scrollBarSize = 11;

    // // div3.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 320}, {LdScalePolicy::Absolute, 300}};
    // // div4.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 110}, {LdScalePolicy::Absolute, 100}};
    // // div6.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 300}, {LdScalePolicy::Absolute, 290}};

    // // div3.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 50}, {LdScalePolicy::Absolute, 50}};
    // // div4.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 50}, {LdScalePolicy::Absolute, 50}};
    // // div6.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 50}, {LdScalePolicy::Absolute, 50}};
    // div5.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 400}, {LdScalePolicy::Absolute, 550}};

    // div3.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    // div4.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.f}};
    // div6.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.f}};
    // div5.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};

    // div2.style.color = utils::hexToVec4("#6de025ff");
    // div3.style.color = utils::hexToVec4("#bc115bff");
    // // div4.style.color = glm::vec4(0.4f, 0.2f, 0.9f, 1.0f); // utils::hexToVec4("#932758ff");
    // div4.style.color = utils::hexToVec4("#0a038cff");
    // div5.style.color = utils::hexToVec4("#d5317bff");
    // div6.style.color = utils::hexToVec4("#0608a5ff");

    // componentManager.setRoot(&rootDiv);

    // div3.layout.orientation = LdOrientation::Horizontal;
    // div3.layout.fillPolicy = LdFillPolicy::Tightly;
    // div3.layout.internalAlign = LdAlign::Top;
    // div3.layout.wrap = LdWrapMode::NoWrap;
    // // div3.layout.align = LdAlignment{LdAlign::Right, LdAlign::Center};

    // // rootDiv.append(&div3);
    // // rootDiv.append({&div3, &div4, &div5, &div6});
    // rootDiv.layout.paddingSize = {5, 5, 0, 0};
    // rootDiv.append(&div5);

    // div5.append({&div4, &div6, &div3});
    // div3.layout.grid.pos = {0, 0};
    // div3.layout.grid.span = {1, 1};

    // div4.layout.grid.pos = {0, 1};
    // div4.layout.grid.span = {1, 1};

    // div6.layout.grid.pos = {1, 0};
    // div6.layout.grid.span = {1, 1};

    // // div3.layout.internalAlign = LdAlign::Center; // check this later after margins are implemented

    // div5.layout.fillPolicy = LdFillPolicy::Grid;
    // div5.layout.grid.config = LdGridConfig{.rows{2}, .cols{2}};
    // // div6.append(&div7);
    // div5.layout.marginSize = {5, 5, 5, 5};
    // // div6.layout.marginSize = {5, 25, 5, 30};
    // // div3.layout.paddingSize = {5, 5, 5, 5};
    // // div3.layout.marginSize = {10, 5, 5, 0};
    // // div3.layout.wrap = LdWrapMode::WrapAround;

    // div3.layout.borderSize = {.left = 6, .right = 6, .top = 6, .bottom = 6};
    // div3.style.borderColor = utils::hexToVec4("#cacacaff");

    // div7.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 800}, {LdScalePolicy::Absolute, 100}};
    // div7.style.color = utils::hexToVec4("#810011ff");
    // div7.style.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";

    // div6.append(&div7);
    // // div4.layout.marginSize = {5, 5, 5, 5};
    // // div5.layout.marginSize = {0, 0, 5, 5};
    // div6.layout.marginSize = {10, 2, 4, 5};

    // /* Trigger initial resize event to "stabilize" the scene */

    // utils::printlni("size of {}B", sizeof(Div));
    // utils::printlni("size of {}Kb", sizeof(Div) / 1024.0f);

    // // utils::printlni("size of {}", sizeof(Div::AssignReloadable<bool>));
    // // rootDiv.refreshOptions();

    // // div5.addOnEnterListener([this]() { div5.style.color = utils::hexToVec4("#336b55ff"); });
    // // div5.addOnExitListener([this]() { div5.style.color = utils::hexToVec4("#3b1526ff"); });

    // // Div* newDiv = new Div();
    // // newDiv->style.color = utils::hexToVec4("#a10b88ff");
    // // newDiv->layout.scaling = LdScaling{{LdScalePolicy::Absolute, 300}, {LdScalePolicy::Absolute, 200}};
    // // newDiv->style.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";

    // // divs.push_back(newDiv);

    // // div6.append(newDiv);

    // rootDiv.addClickListener(
    //     [this](auto, auto, MouseButton btn)
    //     {
    //         if (btn == MouseButton::Left)
    //         {
    //             //     if (rootDiv.remove(&div3)) { utils::printlnw("Removed"); }
    //             //     else { utils::printlnw("NOT removed"); }
    //         }
    //         // else if (btn == MouseButton::Right) { rootDiv.append(&div3); }
    //     });

    // // rootDiv.addOnKeyListener(
    // //     [this](const HIDAction* keyAction)
    // //     {
    // //         if (keyAction[GLFW_KEY_D] == HIDAction::Pressed)
    // //         {
    // //             uint8_t x = (uint8_t)rootDiv.layout.align.horizontal + 1;
    // //             rootDiv.layout.align.horizontal = LdAlign(x % (uint8_t)LdAlign::COUNT);
    // //         }
    // //         if (keyAction[GLFW_KEY_W] == HIDAction::Pressed)
    // //         {
    // //             uint8_t x = (uint8_t)rootDiv.layout.align.vertical + 1;
    // //             rootDiv.layout.align.vertical = LdAlign(x % (uint8_t)LdAlign::COUNT);
    // //         }
    // //         rootDiv.updateLayout();
    // //     });
    componentManager.resizeEvent(initialWidth, initialHeight);
    rootDiv.showTree();
    // utils::printlne("X1 {} X2 {}", div2.getTransformRead().scale.x, div3.getTransformRead().scale.x);
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

void App::mouseScrollEvent(double offsetX, double offsetY)
{
    componentManager.mouseScrollEvent(offsetX, offsetY);
}

void App::windowMaximizedEvent(int maximized)
{
    componentManager.windowMaximizedEvent(maximized);
}
