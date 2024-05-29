#include "Div.hpp"

#include "AbstractComponent.hpp"

#include "../Utility.hpp"
#include "ScrollBar.hpp"
#include "layoutCalc/LayoutData.hpp"
#include <string_view>

namespace components
{
Div::Div()
    : AbstractComponent({.type = CompType::Div,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl"})
    //   .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/baseTextured.glsl"})

    , textureLoader(assetloaders::TextureLoader::get())
{
    imagePath.onReload = std::bind(&Div::reloadImage, this);
}

Div::~Div()
{
    if (hsb) { delete hsb; }
    if (vsb) { delete vsb; }
}

void Div::reloadImage()
{
    /* Note: User shall ensure valid path. */
    if (!std::string_view(imagePath.value).empty())
    {
        textureData = textureLoader.loadTexture(imagePath.value);
        changeShaderTo("/home/hekapoo/newTryAtUI/src/assets/shaders/baseTextured.glsl");
        utils::printlni("Div {} reloaded image to {}", getId(), imagePath.value);
    }
    else
    {
        changeShaderTo("/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl");
        textureData = nullptr;
    }
}

void Div::onPrepareToRender()
{
    auto border = glm::vec4(layout.border.top, layout.border.bottom, layout.border.left, layout.border.right);

    getShader().setActiveShaderId(getShaderId());

    if (textureData == nullptr)
    {
        getShader().setVec4f("uInnerColor", style.color);
        getShader().setVec4f("uBorderColor", style.borderColor);
        getShader().setVec4f("uBorderSize", border);
        getShader().setVec2f("uResolution", getTransformRead().scale);
    }
    else { getShader().set2DTextureUnit("uTexture", textureData->id, GL_TEXTURE0); }
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
    // utils::printlni("[INF] I am node {} and onClick() called", getId());

    const auto& s = getState();
    if (mouseClickCb && s->mouseAction == HIDAction::Pressed) { mouseClickCb(s->mouseX, s->mouseY, s->clickedButton); }
}

void Div::onKeyEvent() {}

void Div::onMouseEnterEvent()
{

    if (mouseEnterCb) { mouseEnterCb(); }
}

void Div::onMouseExitEvent()
{

    if (mouseExitCb) { mouseExitCb(); }
}

void Div::onMoveEvent() {}

void Div::onStart()
{
    utils::printlni("[INF] I am node {} and onStart() called", getId());
}

bool Div::onLayoutUpdate()
{
    const int scrollValueH = hsb ? hsb->getScrollValue() : 0;
    const int scrollValueV = vsb ? vsb->getScrollValue() : 0;
    const bool isHBarActive = hsb ? true : false;
    const bool isVBarActive = vsb ? true : false;
    const auto deducedOverflow = layoutCalc.calculate(scrollValueH, scrollValueV, isHBarActive, isVBarActive);

    /* Following IFs add or remove scrollbars on demand */
    bool needsUpdate = false;
    if (style.enableHScroll && deducedOverflow.x > 0 && hsb == nullptr)
    {
        hsb = new computils::ScrollBar();
        hsb->setActive();

        appendAux(hsb);
        needsUpdate = true;
    }
    else if ((deducedOverflow.x <= 0 || !style.enableHScroll) && hsb != nullptr)
    {
        hsb->setInactive();
        if (vsb) { vsb->setOppositeScrollBarInactive(); }
        removeAux(hsb);
        delete hsb;
        hsb = nullptr;
        needsUpdate = true;
    }

    if (style.enableVScroll && deducedOverflow.y > 0 && vsb == nullptr)
    {
        vsb = new computils::ScrollBar();
        vsb->setActive();

        if (hsb) { hsb->setOppositeScrollBarActive(); }
        appendAux(vsb);
        needsUpdate = true;
    }
    else if ((deducedOverflow.y <= 0 || !style.enableVScroll) && vsb != nullptr)
    {
        vsb->setInactive();
        removeAux(vsb);
        delete vsb;
        vsb = nullptr;
        needsUpdate = true;
    }

    /* Options can change at runtime. This updates those and the scrollbars overflow calculations. */
    if (hsb)
    {
        hsb->options.knobInset = style.knobInset;
        hsb->options.scrollSensitivity = style.scrollSensitivity;
        hsb->options.barSize = layout.scrollBarSize;
        hsb->options.orientation = layoutcalc::LdOrientation::Horizontal;

        if (vsb) { vsb->setOppositeScrollBarActive(); }
        hsb->updateOverflow(deducedOverflow.x);
    }

    if (vsb)
    {
        vsb->options.knobInset = style.knobInset;
        vsb->options.scrollSensitivity = style.scrollSensitivity;
        vsb->options.barSize = layout.scrollBarSize;
        vsb->options.orientation = layoutcalc::LdOrientation::Vertical;

        if (hsb) { hsb->setOppositeScrollBarInactive(); }
        vsb->updateOverflow(deducedOverflow.y);
    }
    return needsUpdate;
}

} // namespace components