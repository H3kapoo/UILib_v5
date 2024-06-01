#include "Button.hpp"

#include "../Utility.hpp"
#include <cstdint>
#include <string_view>

namespace components
{
Button::Button()
    : AbstractComponent({.type = CompType::Button,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl"})
    , imageDummy("/home/hekapoo/newTryAtUI/src/assets/shaders/baseTextured.glsl")
    , textureLoader(TextureLoader::get())

{
    sideImage.path.onReload = std::bind(&Button::reloadImage, this);
    imageDummy.options.isForTextures = true;
}

Button::~Button() {}

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
    imageDummy.transform.layer = getDepth() + 1;
    utils::printlni("[INF] I am node {} and onStart() called {}", getId(), imageDummy.transform.layer);
}

void Button::onRenderDone()
{
    if (textureData == nullptr) { return; }
    lwr.render(getState()->projectionMatrix, imageDummy);
}

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
        imageDummy.options.textureId = textureData->id;
        // utils::printlni("Button {} reloaded image to {}", getId(), sideImage.path.value);
    }
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
    // utils::printlni("Button element id {} has been entered!", getId());
    style.color = utils::hexToVec4("#606160ff");
    // const auto& s = getState();
    // println("For Button {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void Button::onMouseExitEvent()
{
    // utils::printlni("Button element id {} has been exited!", getId());
    style.color = utils::hexToVec4("#404140ff");
    // const auto& s = getState();
    // println("For Button {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void Button::onMoveEvent()
{
    // getBoxModelRW().pos.z = getDepth(); // TODO: Shall not exist
    // utils::printlni("[INF] I am node {} and onStart() called", getId());
    const auto& s = getState();
    if (s->mouseAction == HIDAction::Pressed && s->clickedButton == MouseButton::Left)
    {
        if (mouseMoveCb) mouseMoveCb(s->mouseX, s->mouseY);
    }
}

bool Button::onLayoutUpdate()
{
    imageDummy.transform.pos = getTransformRead().pos + glm::vec2{10, 10};
    auto scale = getTransformRead().scale.y - 20;
    imageDummy.transform.scale = {scale, scale};
    imageDummy.transform.markDirty();
    return false;
}

} // namespace components