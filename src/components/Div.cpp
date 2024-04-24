#include "Div.hpp"

#include "../Utility.hpp"

namespace components
{
Div::Div()
    // : AbstractComponent({.type = "Div", .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl"})
    : AbstractComponent({.type = "Div", .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl"})
    , textureLoader(assetloaders::TextureLoader::get())
{
    // style.hIn = utils::hexToVec4("#932222ff");
}

Div::~Div()
{
    // printlni("Deleting {} id {} ..", getType(), getId());
}

void Div::onPrepareToRender()
{
    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uInnerColor", style.color);
    getShader().setVec4f("uBorderColor", style.borderColor);
    getShader().setVec4f("uBorderSize", layout.borderSize);
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

void Div::onClickEvent()
{

    const auto& s = getState();
    if (s->clickedButton == MouseButton::Left && s->keyAction == KeyAction::Pressed)
    {
        utils::printlni("Div element id {} has been clicked!", getId());
        //     if (getId() == 1)
        //     {
        //         utils::printlni("Switching texture..");
        //         textureLoader.reloadFromPath("src/assets/textures/container.jpg");
        //     }
    }
    else { return; }

    if (mouseClickCb) mouseClickCb(s->mouseX, s->mouseY, s->clickedButton);
};

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
    layoutCalc.calculate(style.color.x);
}

} // namespace components