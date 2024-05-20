#include "Div.hpp"

#include "AbstractComponent.hpp"
#include "GLFW/glfw3.h"

#include "../Utility.hpp"
#include "ScrollBar.hpp"
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
    // hsb = new computils::ScrollBar();
    // vsb = new computils::ScrollBar();
}

Div::~Div()
{
    // delete hsb;
    // delete vsb;
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

bool Div::onLayoutUpdate()
{
    const int scrollValueH = hsb ? hsb->getScrollValue() : 0;
    const int scrollValueV = vsb ? vsb->getScrollValue() : 0;
    const bool isHBarActive = hsb ? true : false;
    const bool isVBarActive = vsb ? true : false;
    const auto deducedOverflow = layoutCalc.calculate(scrollValueH, scrollValueV, isHBarActive, isVBarActive);

    bool needsUpdate = false;
    if (deducedOverflow.x > 0 && hsb == nullptr)
    {
        hsb = new computils::ScrollBar();
        hsb->setActive();
        hsb->options.barSize = layout.scrollBarSize;
        hsb->options.orientation = layoutcalc::LdOrientation::Horizontal;
        if (vsb) { vsb->setOppositeScrollBarActive(); }
        appendAux(hsb);
        needsUpdate = true;
    }
    else if (deducedOverflow.x <= 0 && hsb != nullptr)
    {
        hsb->setInactive();
        if (vsb) { vsb->setOppositeScrollBarInactive(); }
        removeAux(hsb);
        delete hsb;
        hsb = nullptr;
        needsUpdate = true;
    }

    if (deducedOverflow.y > 0 && vsb == nullptr)
    {
        vsb = new computils::ScrollBar();
        vsb->setActive();
        vsb->options.barSize = layout.scrollBarSize;
        vsb->options.orientation = layoutcalc::LdOrientation::Vertical;
        if (hsb) { hsb->setOppositeScrollBarActive(); }
        appendAux(vsb);
        needsUpdate = true;
    }
    else if (deducedOverflow.y <= 0 && vsb != nullptr)
    {
        vsb->setInactive();
        if (hsb) { hsb->setOppositeScrollBarInactive(); }
        removeAux(vsb);
        delete vsb;
        vsb = nullptr;
        needsUpdate = true;
    }

    if (hsb) { hsb->updateOverflow(deducedOverflow.x); }
    if (vsb) { vsb->updateOverflow(deducedOverflow.y); }
    return needsUpdate;
}

} // namespace components