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
    rootDiv.style.borderColor = utils::hexToVec4("#7210bcff");
    rootDiv.layout.borderSize = glm::vec4(6, 12, 18, 50);
    // rootDiv.layout.borderSize = glm::vec4(6);
    rootDiv.layout.orientation = LdOrientation::Horizontal;
    rootDiv.layout.fillPolicy = LdFillPolicy::EvenlySpaced;
    rootDiv.layout.align = LdAlignment{LdAlign::Left, LdAlign::Top};
    rootDiv.layout.internalAlign = LdAlign::Top;
    rootDiv.layout.wrap = LdWrapMode::NoWrap;

    div3.style.borderColor = utils::hexToVec4("#0a1520ff");
    div3.layout.borderSize = glm::vec4(36, 12, 18, 50);
    // div3.layout.marginSize = glm::vec4(6.0f);
    div3.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.5f}, {LdScalePolicy::Relative, 0.6f}};
    div4.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.5f}, {LdScalePolicy::Relative, 0.5f}};
    div5.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.5f}, {LdScalePolicy::Relative, 0.5f}};

    div2.style.color = utils::hexToVec4("#6de025ff");
    div3.style.color = utils::hexToVec4("#bc115bff");
    div4.style.color = utils::hexToVec4("#371a27ff");
    div5.style.color = utils::hexToVec4("#820d42ff");

    componentManager.setRoot(&rootDiv);

    // div3.append(&div2);
    // rootDiv.append({&div3, &div4, &div5});
    rootDiv.append(&div3);
    div3.append({&div4, &div5});

    rootDiv.showTree();

    /* Trigger initial resize event to "stabilize" the scene */
    componentManager.resizeEvent(initialWidth, initialHeight);

    utils::printlni("size of {}B", sizeof(Div));
    utils::printlni("size of {}Kb", sizeof(Div) / 1024.0f);

    div5.addOnEnterListener([this]() { div5.style.color = utils::hexToVec4("#336b55ff"); });
    div5.addOnExitListener([this]() { div5.style.color = utils::hexToVec4("#3b1526ff"); });
    div5.addClickListener(
        [this](auto, auto, auto)
        {
            // rootDiv.layout.childPos = LayoutData::ChildPos::TopLeft;
            rootDiv.updateLayout();
        });
}

void App::update()
{
    // TODO: This needs to be carefully thought when it should be called and on what
    componentManager.render();
}

void App::mouseClickEvent(MouseButton button, KeyAction action, ActiveModifiersBits mods)
{
    if (button == MouseButton::Left && action == KeyAction::Pressed)
    {
        // rootDiv.isComponentRenderable() ? rootDiv.setRenderable(false) : rootDiv.setRenderable(true);
    }
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