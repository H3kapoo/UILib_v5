#include "SeparatorBar.hpp"

#include "../Utility.hpp"
#include <cstdint>
#include <string_view>

namespace components
{
SeparatorBar::SeparatorBar()
    : AbstractComponent({.type = CompType::SeparatorBar,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl"})
    , imageDummy("/home/hekapoo/newTryAtUI/src/assets/shaders/baseTextured.glsl")
    , textureLoader(TextureLoader::get())

{
    style.color = utils::hexToVec4("#404140ff");
    sideImage.path.onReload = std::bind(&SeparatorBar::reloadImage, this);
    imageDummy.options.isForTextures = true;
}

SeparatorBar::~SeparatorBar() {}

void SeparatorBar::onPrepareToRender()
{
    auto border = glm::vec4(layout.border.top, layout.border.bottom, layout.border.left, layout.border.right);

    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uInnerColor", style.color);
    getShader().setVec4f("uBorderColor", style.borderColor);
    getShader().setVec4f("uBorderSize", border);
    getShader().setVec2f("uResolution", getTransformRead().scale);
}
void SeparatorBar::onStart()
{
    imageDummy.transform.layer = getDepth() + 1;
    utils::printlni("[INF] I am node {} and onStart() called {}", getId(), imageDummy.transform.layer);
}

void SeparatorBar::onRenderDone()
{
    if (textureData == nullptr) { return; }
    lwr.render(getState()->projectionMatrix, imageDummy);
}

void SeparatorBar::reloadImage()
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

void SeparatorBar::addClickListener(std::function<void(int, int, MouseButton)> func)
{
    mouseClickCb = func;
}

void SeparatorBar::addReleaseListener(std::function<void(int, int, MouseButton)> func)
{
    mouseReleaseCb = func;
}

void SeparatorBar::addMoveClickedListener(std::function<void(int16_t, int16_t)> func)
{
    mouseMoveClickedCb = func;
}

void SeparatorBar::addMoveListener(std::function<void(int16_t, int16_t)> func)
{
    mouseMoveCb = func;
}

void SeparatorBar::addOnExitListener(std::function<void()> func)
{
    mouseExitCb = func;
}

void SeparatorBar::onClickEvent()
{
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

void SeparatorBar::onMouseEnterEvent()
{
    style.color = utils::hexToVec4("#606160ff");
}

void SeparatorBar::onMouseExitEvent()
{
    style.color = utils::hexToVec4("#404140ff");
    if (mouseExitCb) { mouseExitCb(); }
}

void SeparatorBar::onMoveEvent()
{
    // getBoxModelRW().pos.z = getDepth(); // TODO: Shall not exist
    // utils::printlni("[INF] I am node {} and onStart() called", getId());

    const auto& s = getState();
    if (mouseMoveCb) mouseMoveCb(s->mouseX, s->mouseY);

    if (s->mouseAction == HIDAction::Pressed && s->clickedButton == MouseButton::Left)
    {
        if (mouseMoveClickedCb) mouseMoveClickedCb(s->mouseX, s->mouseY);
    }
}

bool SeparatorBar::onLayoutUpdate()
{
    imageDummy.transform.pos = getTransformRead().pos + glm::vec2{10, 10};
    auto scale = getTransformRead().scale.y - 20;
    imageDummy.transform.scale = {scale, scale};
    imageDummy.transform.markDirty();
    return false;
}

} // namespace components