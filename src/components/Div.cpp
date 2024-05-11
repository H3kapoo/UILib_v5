#include "Div.hpp"

#include "AbstractComponent.hpp"
#include "GLFW/glfw3.h"

#include "../Utility.hpp"
#include "layoutCalc/LayoutData.hpp"
#include <algorithm>

namespace components
{
Div::Div()
    // : AbstractComponent({.type = "Div", .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl"})
    : AbstractComponent({.type = CompType::Div,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl"})
// , textureLoader(assetloaders::TextureLoader::get())
{

    hsb.options.orientation = layoutcalc::LdOrientation::Horizontal;
    vsb.options.orientation = layoutcalc::LdOrientation::Vertical;
}

Div::~Div()
{
    // printlni("Deleting {} id {} ..", getType(), getId());
}

void Div::onPrepareToRender()
{

    auto border = glm::vec4(layout.borderSize.top, layout.borderSize.bottom, layout.borderSize.left,
        layout.borderSize.right);

    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uInnerColor", style.color);
    getShader().setVec4f("uBorderColor", style.borderColor);
    getShader().setVec4f("uBorderSize", border);
    getShader().setVec2f("uResolution", getTransformRead().scale);
    // getShader().set2DTextureUnit("uTexture", textureData->id, GL_TEXTURE0);
}

void Div::onRenderDone()
{
    hsb.show(getState()->projectionMatrix);
    vsb.show(getState()->projectionMatrix);
}

void Div::addClickListener(std::function<void(int, int, MouseButton)>&& func)
{
    mouseClickCb = func;
}

void Div::addOnEnterListener(std::function<void()>&& func)
{
    mouseEnterCb = func;
}

void Div::addOnExitListener(std::function<void()>&& func)
{
    mouseExitCb = func;
}

void Div::addOnKeyListener(std::function<void(const HIDAction*)>&& func)
{
    keyEventCb = func;
}

void Div::onClickEvent()
{
    const auto& s = getState();
    if (s->mouseAction == HIDAction::Pressed && s->clickedButton == MouseButton::Left)
    {
        if (hsb.onMouseClick(s->mouseX, s->mouseY)) { return; }
        if (vsb.onMouseClick(s->mouseX, s->mouseY)) { return; }
    }

    if (s->mouseAction == HIDAction::Released && s->clickedButton == MouseButton::Left)
    {
        hsb.onMouseRelease();
        vsb.onMouseRelease();
    }

    if (mouseClickCb && s->mouseAction == HIDAction::Pressed) { mouseClickCb(s->mouseX, s->mouseY, s->clickedButton); }
}

void Div::onKeyEvent()
{
    // const auto& s = getState();
    // if (keyEventCb) { keyEventCb(s->keyboardActionState); }
    // if (s->keyboardActionState[GLFW_KEY_A] == HIDAction::Pressed)
    // {
    //     overflow -= 15;
    //     overflow = std::clamp(overflow, 0, deducedOverflow);
    // }
    // if (s->keyboardActionState[GLFW_KEY_D] == HIDAction::Pressed)
    // {
    //     overflow += 15;
    //     overflow = std::clamp(overflow, 0, deducedOverflow);
    // }
    // s->triggerLayoutUpdate();
}

void Div::onMouseEnterEvent()
{
    // utils::printlni("[H] div element id {} has entered hover!", getId());
    // style.color = style.hIn;
    if (mouseEnterCb) { mouseEnterCb(); }
}

void Div::onMouseExitEvent()
{
    // utils::printlni("[H] div element id {} has exited hover!", getId());
    // style.color = style.hOut;
    if (mouseExitCb) { mouseExitCb(); }
}

void Div::onMoveEvent()
{
    const auto& s = getState();
    if (hsb.onMouseMove(s->mouseX, s->mouseY)) { return; }
    if (vsb.onMouseMove(s->mouseX, s->mouseY)) { return; }
}

void Div::onStart()
{
    style.hOut = style.color;
    utils::printlni("[INF] I am node {} and onStart() called", getId());
}

void Div::onLayoutUpdate()
{
    deducedOverflow = layoutCalc.calculate();
    hsb.updateOverflow(deducedOverflow.x);
    vsb.updateOverflow(deducedOverflow.y);
}

} // namespace components