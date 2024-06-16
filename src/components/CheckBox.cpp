#include "CheckBox.hpp"

#include "src/UIState.hpp"
#include "src/Utility.hpp"

namespace components
{
CheckBox::CheckBox()
    : AbstractComponent({.type = CompType::CheckBox, .shaderPath = "src/assets/shaders/bordered.glsl"})
    , imgHolder("src/assets/shaders/baseTextured.glsl")
    , textureLoader(TextureLoader::get())

{
    /* Don't show children of this component. Set to True if you want to see them displayed in the prints. */
    setDetailChildrenOnPrint(false);

    btn.style.color = utils::hexToVec4("#d7d7d7ff");
    btn.style.borderColor = utils::hexToVec4("#cd4444ff");

    btn.layout.scaling = LdScaling{{LdScalePolicy::Relative, 0.3f}, {LdScalePolicy::Relative, 1.0f}};
    btn.layout.margin = {2, 2, 2, 2};
    btn.layout.border = {2, 2, 2, 2};

    btn.addClickListener(
        [this](int, int, MouseButton mButton)
        {
            if (mButton == MouseButton::Left)
            {
                if (disabled.value) { return; }

                checked = !checked;
                if (checked) { btn.style.color = utils::hexToVec4("#332e2eff"); }
                else { btn.style.color = utils::hexToVec4("#ffffffff"); }
                if (checkedCb) checkedCb(checked);
            }
        });

    /* Toggle colors of disabled/enabled checkbox at runtime*/
    disabled.onReload = [this]()
    {
        if (disabled.value) { btn.style.color = utils::hexToVec4("#727272ff"); }
        else { btn.style.color = utils::hexToVec4("#ffffffff"); }
        checked = false;
    };

    AbstractComponent::append(&btn);
}

CheckBox::~CheckBox() {}

void CheckBox::onPrepareToRender()
{
    auto border = glm::vec4(layout.border.top, layout.border.bottom, layout.border.left, layout.border.right);

    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uInnerColor", style.color);
    getShader().setVec4f("uBorderColor", style.borderColor);
    getShader().setVec4f("uBorderSize", border);
    getShader().setVec2f("uResolution", getTransformRead().scale);
}

void CheckBox::onStart()
{
    // imgHolder.transform.layer = getDepth() + 1;
    // utils::printlni("[INF] I am node {} and onStart() called {}", getId(), imgHolder.transform.layer);
}

void CheckBox::onRenderDone()
{
    /* No need to try to render internal image if we don't have one */
    // if (textureData == nullptr) { return; }
    // lwr.render(getState()->projectionMatrix, imgHolder);
}

void CheckBox::onClickEvent() {
    // const auto& s = getState();
    // if (s->mouseAction == HIDAction::Pressed)
    // {
    //     if (mouseClickCb) mouseClickCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
    // }
    // else if (s->mouseAction == HIDAction::Released)
    // {
    //     if (mouseReleaseCb) mouseReleaseCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
    // }
};

void CheckBox::onMouseEnterEvent() {}

void CheckBox::onMouseExitEvent() {}

void CheckBox::onMoveEvent()
{
    // const auto& s = getState();
    // if (s->mouseAction == HIDAction::Pressed && s->clickedButton == MouseButton::Left)
    // {
    //     if (mouseMoveCb) mouseMoveCb(s->mouseX, s->mouseY);
    // }
}

bool CheckBox::onLayoutUpdate()
{
    layoutCalc.calculate();
    btn.getTransformRW().scale.x = btn.getTransformRW().scale.y;
    return false;
}

void CheckBox::addCheckedListener(std::function<void(bool)> cb)
{
    checkedCb = cb;
}

// void CheckBox::addClickListener(std::function<void(int, int, MouseButton)> func)
// {
//     mouseClickCb = func;
// }

// void CheckBox::addReleaseListener(std::function<void(int, int, MouseButton)> func)
// {
//     mouseReleaseCb = func;
// }

// void CheckBox::addMouseMoveListener(std::function<void(int16_t, int16_t)> func)
// {
//     mouseMoveCb = func;
// }

} // namespace components