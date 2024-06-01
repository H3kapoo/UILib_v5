#include "PinchDiv.hpp"

#include "../Utility.hpp"
#include "AbstractComponent.hpp"
#include <cstdint>
#include <string_view>

namespace components
{
PinchDiv::PinchDiv()
    : AbstractComponent({.type = CompType::PinchDiv,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl"})
// , imageDummy("/home/hekapoo/newTryAtUI/src/assets/shaders/baseTextured.glsl")
// , textureLoader(TextureLoader::get())

{
    /* This object should not be renderable. It should serve only as a skeleton */
    // setRenderable(false);
}

PinchDiv::~PinchDiv()
{
    for (const auto& sep : separators)
    {
        if (sep) { delete sep; }
    }
}

void PinchDiv::append(AbstractComponent* comp)
{
    if (!getNodes().empty())
    {
        // prev comp
        AbstractComponent* it = *(getNodes().end() - 1);

        // append separator
        Button* btn = new Button();
        separators.push_back(btn);
        btn->style.color = utils::hexToVec4("#b000c4ff");
        btn->layout.scaling = LdScaling{{LdScalePolicy::Absolute, 5}, {LdScalePolicy::Relative, 1.0f}};
        AbstractComponent::append(btn);
        AbstractComponent::append(comp);

        pinchPairs.emplace_back(it, comp);
        int16_t index = pinchPairs.size() - 1;

        btn->addClickListener(
            [this](auto x, auto, MouseButton b)
            {
                if (b == MouseButton::Left)
                {
                    deltaX = 0;
                    prevX = x;
                    for (const auto& comp : getNodes())
                    {
                        if (comp->getType() == CompType::Button) { continue; }
                        comp->layout.scaling.horizontal.policy = LdScalePolicy::Absolute;
                        comp->layout.scaling.horizontal.value = comp->getTransformRead().scale.x;
                    }
                }
            });

        btn->addReleaseListener(
            [this](auto x, auto, MouseButton b)
            {
                if (b == MouseButton::Left)
                {
                    deltaX = 0;
                    prevX = x;
                    float decreaseBy = 0;
                    for (const auto& comp : getNodes())
                    {
                        if (comp->getType() != CompType::Button) { continue; }
                        decreaseBy += comp->layout.scaling.horizontal.value;
                    }
                    for (const auto& comp : getNodes())
                    {
                        if (comp->getType() == CompType::Button) { continue; }
                        comp->layout.scaling.horizontal.policy = LdScalePolicy::Relative;
                        comp->layout.scaling.horizontal.value = comp->getTransformRead().scale.x /
                                                                (getTransformRead().scale.x - decreaseBy);
                        refreshLayout();
                    }
                }
            });

        btn->addMouseMoveListener(
            [this, index](auto x, auto y)
            {
                deltaX = x - prevX;
                // utils::printlne("Diff X {}", x - prevX);
                prevX = x;
                prevY = y;

                const float diff = deltaX; // * 1.0f / getTransformRead().scale.x;
                // utils::printlne("Diff X {} {}", deltaX, deltaX * 1.0f / getTransformRead().scale.x);

                PinchPair& pp = pinchPairs[index];
                pp.first->layout.scaling.horizontal.value += diff;
                pp.second->layout.scaling.horizontal.value -= diff;

                refreshLayout();
            });
        return;
    }

    AbstractComponent::append(comp);
}

void PinchDiv::onPrepareToRender()
{
    auto border = glm::vec4(layout.border.top, layout.border.bottom, layout.border.left, layout.border.right);

    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uInnerColor", style.color);
    getShader().setVec4f("uBorderColor", style.borderColor);
    getShader().setVec4f("uBorderSize", border);
    getShader().setVec2f("uResolution", getTransformRead().scale);
}

void PinchDiv::onStart()
{
    // imageDummy.transform.layer = getDepth() + 1;
    // utils::printlni("[INF] I am node {} and onStart() called {}", getId(), imageDummy.transform.layer);
}

void PinchDiv::onRenderDone()
{
    // if (textureData == nullptr) { return; }
    // lwr.render(getState()->projectionMatrix, imageDummy);
}

void PinchDiv::onClickEvent() {
    // println("Button element id {} has been clicked!", getId());

    // const auto& s = getState();
    // if (s->mouseAction == HIDAction::Pressed)
    // {
    //     if (mouseClickCb) mouseClickCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
    // }
};

void PinchDiv::onMouseEnterEvent()
{
    // utils::printlni("Button element id {} has been entered!", getId());
    style.color = utils::hexToVec4("#606160ff");
    // const auto& s = getState();
    // println("For Button {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void PinchDiv::onMouseExitEvent()
{
    // utils::printlni("Button element id {} has been exited!", getId());
    style.color = utils::hexToVec4("#404140ff");
    // const auto& s = getState();
    // println("For Button {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
}

void PinchDiv::onMoveEvent()
{
    // getBoxModelRW().pos.z = getDepth(); // TODO: Shall not exist
    // utils::printlni("[INF] I am node {} and onStart() called", getId());
}

bool PinchDiv::onLayoutUpdate()
{
    // imageDummy.transform.pos = getTransformRead().pos + glm::vec2{10, 10};
    // auto scale = getTransformRead().scale.y - 20;
    // imageDummy.transform.scale = {scale, scale};
    // imageDummy.transform.markDirty();
    layoutCalc.calculate();
    return false;
}

} // namespace components