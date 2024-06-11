#include "SeparatorBar.hpp"

#include <string_view>

#include "src/Utility.hpp"

namespace components
{
SeparatorBar::SeparatorBar()
    : AbstractComponent({.type = CompType::SeparatorBar,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl"})
    , imgHolder("/home/hekapoo/newTryAtUI/src/assets/shaders/baseTextured.glsl")
    , textureLoader(TextureLoader::get())

{
    style.color = utils::hexToVec4("#404140ff");
    internalImg.path.onReload = std::bind(&SeparatorBar::reloadImage, this);
    imgHolder.options.isForTextures = true;
}

SeparatorBar::~SeparatorBar() {}

void SeparatorBar::reloadImage()
{
    if (std::string_view{internalImg.path.value}.empty())
    {
        // Empty
        textureData = nullptr;
    }
    else
    {
        textureData = textureLoader.loadTexture(internalImg.path.value);
        imgHolder.options.textureId = textureData->id;
        // utils::printlni("Button {} reloaded image to {}", getId(), sideImage.path.value);
    }
}

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
    imgHolder.transform.layer = getDepth() + 1;
    // utils::printlni("[INF] I am node {} and onStart() called {}", getId(), imgHolder.transform.layer);
}

void SeparatorBar::onRenderDone()
{
    /* No need to try to render internal image if we don't have one */
    if (textureData == nullptr) { return; }
    lwr.render(getState()->projectionMatrix, imgHolder);
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
    const auto& s = getState();
    if (mouseMoveCb) mouseMoveCb(s->mouseX, s->mouseY);

    if (s->mouseAction == HIDAction::Pressed && s->clickedButton == MouseButton::Left)
    {
        if (mouseMoveClickedCb) mouseMoveClickedCb(s->mouseX, s->mouseY);
    }
}

bool SeparatorBar::onLayoutUpdate()
{
    imgHolder.transform.pos = getTransformRead().pos + glm::vec2{10, 10};
    auto scale = getTransformRead().scale.y - 20;
    imgHolder.transform.scale = {scale, scale};
    imgHolder.transform.markDirty();
    return false;
}

void SeparatorBar::addClickListener(std::function<void(int, int, MouseButton)> cb)
{
    mouseClickCb = cb;
}

void SeparatorBar::addReleaseListener(std::function<void(int, int, MouseButton)> cb)
{
    mouseReleaseCb = cb;
}

void SeparatorBar::addMoveClickedListener(std::function<void(int16_t, int16_t)> cb)
{
    mouseMoveClickedCb = cb;
}

void SeparatorBar::addMoveListener(std::function<void(int16_t, int16_t)> cb)
{
    mouseMoveCb = cb;
}

void SeparatorBar::addOnExitListener(std::function<void()> cb)
{
    mouseExitCb = cb;
}

} // namespace components