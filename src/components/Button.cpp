#include "Button.hpp"

#include <cstdint>
#include <string_view>

#include "src/Utility.hpp"

namespace components
{
Button::Button()
    : AbstractComponent({.type = CompType::Button, .shaderPath = "src/assets/shaders/bordered.glsl"})
    , imgHolder("src/assets/shaders/baseTextured.glsl")
    , textureLoader(TextureLoader::get())

{
    sideImage.path.onReload = std::bind(&Button::reloadImage, this);
    imgHolder.options.isForTextures = true;
}

Button::~Button() {}

void Button::reloadImage()
{
    if (std::string_view{sideImage.path.value}.empty())
    {
        // Empty
        textureData = nullptr;
    }
    else
    {
        textureData = textureLoader.loadTexture(sideImage.path.value);
        imgHolder.options.textureId = textureData->id;
        // utils::printlni("Button {} reloaded image to {}", getId(), sideImage.path.value);
    }
}

void Button::onPrepareToRender()
{
    auto border = glm::vec4(layout.border.top, layout.border.bottom, layout.border.left, layout.border.right);

    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uInnerColor", style.color);
    getShader().setVec4f("uBorderColor", style.borderColor);
    getShader().setVec4f("uBorderSize", border);
    getShader().setVec2f("uResolution", getTransformRead().scale);
}

void Button::onStart()
{
    imgHolder.transform.layer = getDepth() + 1;
    // utils::printlni("[INF] I am node {} and onStart() called {}", getId(), imgHolder.transform.layer);
}

void Button::onRenderDone()
{
    /* No need to try to render internal image if we don't have one */
    if (textureData == nullptr) { return; }
    lwr.render(getState()->projectionMatrix, imgHolder);
}

void Button::onClickEvent()
{
    // println("Button element id {} has been clicked!", getId());

    const auto& s = getState();
    if (s->mouseAction == HIDAction::Pressed)
    {
        if (mouseClickCb) mouseClickCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
    }
    else if (s->mouseAction == HIDAction::Released)
    {
        if (mouseReleaseCb) mouseReleaseCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
    }
};

void Button::onMouseEnterEvent()
{
    style.color = utils::hexToVec4("#606160ff");
}

void Button::onMouseExitEvent()
{
    style.color = utils::hexToVec4("#404140ff");
}

void Button::onMoveEvent()
{
    const auto& s = getState();
    if (s->mouseAction == HIDAction::Pressed && s->clickedButton == MouseButton::Left)
    {
        if (mouseMoveCb) mouseMoveCb(s->mouseX, s->mouseY);
    }
}

bool Button::onLayoutUpdate()
{
    imgHolder.transform.pos = getTransformRead().pos + glm::vec2{10, 10};
    auto scale = getTransformRead().scale.y - 20;
    imgHolder.transform.scale = {scale, scale};
    imgHolder.transform.markDirty();
    return false;
}

void Button::addClickListener(std::function<void(int, int, MouseButton)> func)
{
    mouseClickCb = func;
}

void Button::addReleaseListener(std::function<void(int, int, MouseButton)> func)
{
    mouseReleaseCb = func;
}

void Button::addMouseMoveListener(std::function<void(int16_t, int16_t)> func)
{
    mouseMoveCb = func;
}

} // namespace components