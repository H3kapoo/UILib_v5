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
    componentManager.resizeEvent(initialWidth, initialHeight);

    rootDiv.style.color = utils::hexToVec4("#125ea9ff");
    rootDiv.layout.padding = {5, 5, 5, 5};

    ts.style.color = utils::hexToVec4("#c85c0fff");
    ts.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    ts2.style.color = utils::hexToVec4("#c85c0fff");
    // ts2.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 400}, {LdScalePolicy::Absolute, 400}};

    div2.style.color = utils::hexToVec4("#c85c0fff");
    div3.style.color = utils::hexToVec4("#1c0995ff");
    div4.style.color = utils::hexToVec4("#058a3cff");
    pdiv.style.color = utils::hexToVec4("#ab0ea6ff");

    div5.style.color = utils::hexToVec4("#151061ff");
    div6.style.color = utils::hexToVec4("#5e0667ff");
    div6.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 800}, {LdScalePolicy::Absolute, 600}};
    // div6.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";
    div5.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";

    pdiv.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    pdiv.layout.orientation = LdOrientation::Horizontal;

    pdiv2.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    pdiv2.layout.orientation = LdOrientation::Horizontal;

    pdiv3.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    pdiv3.layout.orientation = LdOrientation::Horizontal;

    // div7.style.color = utils::hexToVec4("#401e05ff");
    div8.style.color = utils::hexToVec4("#1c0995ff");
    div9.style.color = utils::hexToVec4("#1b021aff");

    div7.imagePath = "/home/hekapoo/newTryAtUI/src/assets/textures/container.jpg";
    div7.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f}, {LdScalePolicy::Relative, .5f}};
    // div7.layout.scaling = LdScaling{{LdScalePolicy::Absolute, 300}, {LdScalePolicy::Absolute, 400}};
    rootDiv.append(&pdiv);

    div4.layout.orientation = LdOrientation::Vertical;
    // div9.append(&div7);
    div9.layout.align = LdAlignment{LdAlign::Center, LdAlign::Center};
    // div9.append(&pdiv2);

    div2.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f}, {LdScalePolicy::Relative, .5f}};
    div4.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f}, {LdScalePolicy::Relative, .5f}};
    div5.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f}, {LdScalePolicy::Relative, .5f}};
    // div6.layout.scaling = LdScaling{
    //     {LdScalePolicy::Relative, .5f}, {LdScalePolicy::Relative, .5f}};

    // div2.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f, 1, 400}, {LdScalePolicy::Relative, .5f, 50,
    // 1000}}; div4.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f, 1, 300}, {LdScalePolicy::Relative, .5f,
    // 50, 1000}}; div5.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f, 1, 200}, {LdScalePolicy::Relative,
    // .5f, 50, 1000}}; div6.layout.scaling = LdScaling{{LdScalePolicy::Relative, .5f, 1, 500},
    // {LdScalePolicy::Relative, .5f, 50, 1000}};

    // pdiv.append({&div2, &div4, &div5, &div6});
    // pdiv.append({&div2, &div4, &div5});
    pdiv.append({&div2, &div3, &div9});
    // pdiv2.append({&div3, &div9});
    // pdiv2.append({&div9, &div2});
    // pdiv3.append({&div2, &div3});

    rootDiv.showTree();

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
