#include "Div.hpp"

#include "GLFW/glfw3.h"

#include "../Utility.hpp"
#include "compUtils/LightWeightDummy.hpp"

namespace components
{
Div::Div()
    // : AbstractComponent({.type = "Div", .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl"})
    : AbstractComponent({.type = "Div", .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl"})
    , textureLoader(assetloaders::TextureLoader::get())
// , dummy("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
{
    // style.hIn = utils::hexToVec4("#932222ff");
    // dummy.options.color = utils::hexToVec4("#ffaaffff");
    // dummy.transform.layer = 3;
}

Div::~Div()
{
    // printlni("Deleting {} id {} ..", getType(), getId());
}

void Div::onRenderDone()
{
    // lwr.render(getState()->projectionMatrix, dummy);
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
    if (mouseClickCb && s->mouseAction == HIDAction::Pressed) { mouseClickCb(s->mouseX, s->mouseY, s->clickedButton); }
}

void Div::onKeyEvent()
{
    const auto& s = getState();
    // if (keyEventCb) { keyEventCb(s->keyboardActionState); }
    if (s->keyboardActionState[GLFW_KEY_A] == HIDAction::Pressed) { overflow -= 5; }
    if (s->keyboardActionState[GLFW_KEY_D] == HIDAction::Pressed) { overflow += 5; }
    s->triggerLayoutUpdate();
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
    // const auto& s = getState();
    // println("For div {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void Div::onStart()
{
    style.hOut = style.color;
    utils::printlni("[INF] I am node {} and onStart() called", getId());
}

void Div::onLayoutUpdate()
{
    layoutCalc.calculate(overflow);
}

} // namespace components