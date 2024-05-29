#include "Button.hpp"

#include "../Utility.hpp"
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
    imageDummy.transform.layer = getDepth() + 10;
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
        utils::printlni("Button {} reloaded image to {}", getId(), sideImage.path.value);
    }
}

void Button::addClickListener(std::function<void(int, int, MouseButton)> func)
{
    mouseClickCb = func;
}

void Button::onClickEvent()
{
    // println("Button element id {} has been clicked!", getId());

    const auto& s = getState();
    if (mouseClickCb) mouseClickCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
};

void Button::onMouseEnterEvent()
{
    utils::printlni("Button element id {} has been entered!", getId());
    style.color = utils::hexToVec4("#606160ff");
    // const auto& s = getState();
    // println("For Button {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void Button::onMouseExitEvent()
{
    utils::printlni("Button element id {} has been exited!", getId());
    style.color = utils::hexToVec4("#404140ff");
    // const auto& s = getState();
    // println("For Button {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void Button::onMoveEvent()
{
    // getBoxModelRW().pos.z = getDepth(); // TODO: Shall not exist
    // utils::printlni("[INF] I am node {} and onStart() called", getId());
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