#include "Div.hpp"

#include "AbstractComponent.hpp"
#include "GLFW/glfw3.h"

#include "../Utility.hpp"
#include "layoutCalc/LayoutData.hpp"
#include <algorithm>
#include <glm/fwd.hpp>

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

void Div::onRenderDone() {}

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

void Div::onMoveEvent() {}

void Div::onStart()
{
    style.hOut = style.color;
    utils::printlni("[INF] I am node {} and onStart() called", getId());
}

void Div::onLayoutUpdate()
{
    // TODO: Do not parent anything at first. Make it that somehow the next call to update is the valid one to do so
    // Otherwise we will have to append a SB to each Div for nothing
    const int scrollValueH = hsb.getScrollValue();
    const int scrollValueV = vsb.getScrollValue();
    const int scrollBarSize = hsb.options.barSize;
    const auto deducedOverflow = layoutCalc.calculate(scrollValueH, scrollValueV, hsb.isBarActive(), vsb.isBarActive(),
        scrollBarSize);

    if (deducedOverflow.x > 0 && !hsb.isComponentParented())
    {
        hsb.setActive();
        vsb.setOppositeScrollBarActive();
        append(&hsb);
    }
    else if (deducedOverflow.x <= 0 && hsb.isComponentParented())
    {
        hsb.setInactive();
        vsb.setOppositeScrollBarInactive();
        remove(&hsb);
    }

    if (deducedOverflow.y > 0 && !vsb.isComponentParented())
    {
        vsb.setActive();
        hsb.setOppositeScrollBarActive();
        append(&vsb);
    }
    else if (deducedOverflow.y <= 0 && vsb.isComponentParented())
    {
        vsb.setInactive();
        hsb.setOppositeScrollBarInactive();
        remove(&vsb);
    }

    if (hsb.isComponentParented()) { hsb.updateOverflow(deducedOverflow.x); }
    if (vsb.isComponentParented()) { vsb.updateOverflow(deducedOverflow.y); }
}

} // namespace components