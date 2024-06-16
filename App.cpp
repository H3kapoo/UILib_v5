#include "App.hpp"
#include "src/UIState.hpp"
#include "src/Utility.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/CheckBox.hpp"
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
    componentManager.resizeEvent(initialWidth, initialHeight);

    rootDiv.style.color = utils::hexToVec4("#125ea9ff");
    rootDiv.style.borderColor = utils::hexToVec4("#4e025bff");
    rootDiv.layout.padding = {5, 5, 5, 5};
    rootDiv.layout.border = {5, 5, 5, 5};
    rootDiv.layout.orientation = LdOrientation::Vertical;
    // rootDiv.layout.align = LdAlignment{LdAlign::Center, LdAlign::Center};

    list.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 200}, {LdScalePolicy::Relative, 1.0f}};

    rootDiv.append(&list);

    // uint32_t entryId = list.appendEntry("TextOfEntry");
    // std::vec<uint32_t> entryIds = list.appendEntries({"TextOfEntry", "TextOfEntry2"});
    // std::string text = list.getEntryContent(entryId);
    // list.updateEntry(entryId, "NewText");
    // list.removeEntry(entryId);
    // list.removeEntry(entryIds);
    // list.onEntryClick([](uint32_t entryId, MouseButton btn){});

    // cb.style.color = utils::hexToVec4("#ffffffff");
    // // cb.layout.border = {5, 5, 5, 5};
    // // cb.style.borderColor = utils::hexToVec4("#9b9b9bff");

    // cb.disabled = true;

    // for (int i = 0; i < 10; i++)
    // {
    //     auto c = new CheckBox();
    //     c->layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.5f}, {LdScalePolicy::Absolute, 30}};
    //     c->layout.margin = {0, 0, 0, 5};
    //     c->style.color = utils::hexToVec4("#ffffffff");
    //     c->addCheckedListener(
    //         [this](bool checked)
    //         {
    //             utils::printlne("Check box is checked: {}", checked);
    //             // Empty
    //         });

    //     divs.emplace_back(c);
    // }
    // cb.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 200}, {LdScalePolicy::Absolute, 60}};

    // rootDiv.append(divs);
    // rootDiv.append(&cb);

    // rootDiv.addClickListener(
    //     [this](int, int, MouseButton)
    //     {
    //         cb.disabled = !cb.disabled.value;
    //         utils::printlne("Check box is disabled: {}", cb.disabled.value);
    //         // Empty
    //     });
    // ts.style.color = utils::hexToVec4("#c85c0fff");
    // // ts.addTab("First", 0, &div8);
    // // ts.addTab("Second", 1, &div9);
    // // ts.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    // // ts2.style.color = utils::hexToVec4("#c85c0fff");
    // // ts2.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 400}, {LdScalePolicy::Absolute, 400}};

    // div2.style.color = utils::hexToVec4("#c85c0fff");
    // div3.style.color = utils::hexToVec4("#1c0995ff");
    // div4.style.color = utils::hexToVec4("#058a3cff");
    // pdiv.style.color = utils::hexToVec4("#ab0ea6ff");

    // div5.style.color = utils::hexToVec4("#151061ff");
    // div6.style.color = utils::hexToVec4("#5e0667ff");
    // div6.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 800}, {LdScalePolicy::Absolute, 600}};
    // // div6.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";
    // // div6.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";

    // pdiv.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    // pdiv.layout.orientation = LdOrientation::Vertical;

    // pdiv2.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    // pdiv2.layout.orientation = LdOrientation::Horizontal;

    // pdiv3.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    // pdiv3.layout.orientation = LdOrientation::Horizontal;

    // // div7.style.color = utils::hexToVec4("#401e05ff");
    // div8.style.color = utils::hexToVec4("#1c0995ff");
    // div9.style.color = utils::hexToVec4("#1b021aff");

    // div7.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";
    // // div7.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f}, {LdScalePolicy::Relative, .5f}};
    // div7.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 300}, {LdScalePolicy::Absolute, 400}};
    // rootDiv.append(&pdiv);

    // div4.layout.orientation = LdOrientation::Vertical;
    // // div9.append(&div7);
    // div9.layout.align = LdAlignment{LdAlign::Center, LdAlign::Center};
    // // div9.append(&pdiv2);

    // float xs = 1.0f / 2.0f;
    // // div2.layout.scaling = LdScaling{{LdScalePolicy::Relative, xs, 150}, {LdScalePolicy::Relative, .5f}};
    // // div4.layout.scaling = LdScaling{{LdScalePolicy::Relative, xs, 100}, {LdScalePolicy::Relative, .5f}};
    // div5.layout.scaling = LdScaling{{LdScalePolicy::Relative, xs, 400}, {LdScalePolicy::Relative, .5f}};
    // div6.layout.scaling = LdScaling{{LdScalePolicy::Relative, xs, 200}, {LdScalePolicy::Relative, .5f}};

    // pdiv2.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f, 400}, {LdScalePolicy::Relative, .5f}};
    // div3.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f, 200}, {LdScalePolicy::Relative, .5f}};
    // // div9.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f, 200}, {LdScalePolicy::Relative, .5f}};

    // div2.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f}, {LdScalePolicy::Relative, xs, 200}};
    // pdiv2.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.5f}, {LdScalePolicy::Relative, xs, 200}};

    // // div3.layout.scaling = LdScaling{{LdScalePolicy::Relative, xs, 200}, {LdScalePolicy::Relative, .5f}};
    // ts.layout.scaling = LdScaling{{LdScalePolicy::Relative, xs, 200}, {LdScalePolicy::Relative, .5f}};
    // div4.layout.scaling = LdScaling{{LdScalePolicy::Relative, xs, 200}, {LdScalePolicy::Relative, .5f}};

    // pdiv3.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.5f}, {LdScalePolicy::Relative, xs, 200}};
    // div6.append(&div7);

    // pdiv.append({&pdiv3, &pdiv2});
    // pdiv2.append({&ts, &div4});
    // pdiv3.append({&div5, &div6});

    rootDiv.showTree();
    // utils::printlne("size of {}", sizeof(Div));
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
    // utils::printlne("new width {}", newWidth);
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
