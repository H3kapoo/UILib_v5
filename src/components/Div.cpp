#include "Div.hpp"

#include "AbstractComponent.hpp"
#include "GLFW/glfw3.h"

#include "../Utility.hpp"
#include "compUtils/LightWeightDummy.hpp"
#include "layoutCalc/LayoutData.hpp"
#include <algorithm>

namespace components
{
Div::Div()
    // : AbstractComponent({.type = "Div", .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl"})
    : AbstractComponent({.type = CompType::Div,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl"})
// , textureLoader(assetloaders::TextureLoader::get())
// , dummy("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
{
    // style.hIn = utils::hexToVec4("#932222ff");
    // dummy.options.color = utils::hexToVec4("#ffaaffff");
    // dummy.transform.layer = 3;

    hsb.options.orientation = layoutcalc::LdOrientation::Horizontal;
    vsb.options.orientation = layoutcalc::LdOrientation::Vertical;
    // TODO: This is a workaround for now
    // append(&hsb);
    // append(&vsb);
    // TODO: Make scrollbars an utility again. Not a component in itself.
    //  use onCLick/Release/Move and make them return values such that we know if they should receive
    //  the input or the Div underneath
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
        if (const bool evConsumedByThis = hsb.onMouseClick(s->mouseX, s->mouseY))
        {
            utils::printlne("CLiecked update");
            updateLayout();
            return;
        }
    }

    if (s->mouseAction == HIDAction::Released && s->clickedButton == MouseButton::Left) { hsb.onMouseRelease(); }

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
    if (const bool shouldUpdateLayout = hsb.onMouseMove(s->mouseX, s->mouseY))
    {
        updateLayout();
        return;
    }
    // println("For div {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void Div::onStart()
{
    style.hOut = style.color;
    utils::printlni("[INF] I am node {} and onStart() called", getId());
    // append(&hsb);
    // hsb.onLayoutUpdate();
}

void Div::onLayoutUpdate()
{
    // TODO: There's a problem with appending nodes at runtime in this function specifically
    //       No ideea why tbh.
    const int hsbScrollValue = hsb.isBarActive() ? hsb.getScrollValue() : 0;
    const int vsbScrollValue = 0; // vsb.isBarActive() ? vsb.getScrollValue() : 0;
    deducedOverflow = layoutCalc.calculate(hsbScrollValue, vsbScrollValue);

    utils::printlne("overflow {}  {}", deducedOverflow.x, hsbScrollValue);
    if (deducedOverflow.x > 0)
    {
        hsb.setActive();
        hsb.setOverflow(deducedOverflow.x);
    }
    else
    {
        hsb.setInactive();
        hsb.setOverflow(0);
    }

    hsb.onLayoutUpdate();
    // if (deducedOverflow.x <= 0 && hsb.isBarActive()) { hsb.setOverflow(0); }

    // if (deducedOverflow.y > 0 && !vsb.isBarActive()) { vsb.setOverflow(deducedOverflow.y); }

    // if (deducedOverflow.y <= 0 && vsb.isBarActive()) { vsb.setOverflow(0); }

    // utils::printlne("Max: {} Scroll value: {}", deducedOverflow.x, hsb.getScrollValue());
}

} // namespace components