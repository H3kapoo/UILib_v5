#include "PinchDiv.hpp"

#include "../Utility.hpp"
#include "AbstractComponent.hpp"
#include "layoutCalc/LayoutData.hpp"
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
    setRenderable(false);
}

PinchDiv::~PinchDiv()
{
    for (const auto& sep : separators)
    {
        if (sep) { delete sep; }
    }
}

void PinchDiv::append(std::vector<AbstractComponent*>&& comps)
{
    if (comps.size() < 2)
    {
        utils::printlnw("PinchDiv not really useful with less than 2 elements");
        return;
    }

    const auto& nodes = getNodes();
    const float scalingFactor = 1.0f / comps.size();
    for (const auto& c : comps)
    {
        if (layout.orientation == LdOrientation::Horizontal)
        {
            c->layout.scaling = LdScaling{{LdScalePolicy::Relative, scalingFactor}, {LdScalePolicy::Relative, 1.0f}};
        }
        else
        {
            c->layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, scalingFactor}};
        }

        if (!nodes.empty())
        {
            // prev comp
            AbstractComponent* it = *(nodes.end() - 1);

            // append separator
            Button* btn = new Button();
            separators.push_back(btn);
            btn->style.color = utils::hexToVec4("#b000c4ff");
            if (layout.orientation == LdOrientation::Horizontal)
            {
                btn->layout.scaling = LdScaling{{LdScalePolicy::Absolute, 5}, {LdScalePolicy::Relative, 1.0f}};
            }
            else { btn->layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Absolute, 5}}; }

            AbstractComponent::append(btn);
            AbstractComponent::append(c);

            pinchPairs.emplace_back(it, c);
            int16_t index = pinchPairs.size() - 1;

            btn->addClickListener(std::bind(&PinchDiv::separatorClick, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

            btn->addReleaseListener(std::bind(&PinchDiv::separatorRelease, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

            btn->addMouseMoveListener(std::bind(&PinchDiv::separatorMove, this, std::placeholders::_1,
                std::placeholders::_2, index));
            continue;
        }

        AbstractComponent::append(c);
    }
}

void PinchDiv::separatorClick(int16_t x, int16_t y, MouseButton b)
{
    if (b == MouseButton::Left)
    {
        //
        for (const auto& comp : getNodes())
        {
            if (comp->getType() == CompType::PinchDiv)
            {
                const auto& pDivNodes = comp->getNodes();
                for (const auto& pDivChild : pDivNodes)
                {
                    if (pDivChild->getType() == CompType::Button)
                    {
                        float d = utils::dist(glm::vec2(x, y), pDivChild->getTransformRead().pos);
                        float d2 = utils::dist(glm::vec2(x, y),
                            pDivChild->getTransformRead().pos + pDivChild->getTransformRead().scale);
                        if (d <= 5 || d2 <= 5)
                        {
                            if (pinchedId2 == 0 && pinchedId != 0) { pinchedId2 = pDivChild->getId(); }
                            if (pinchedId == 0 && pinchedId2 == 0) { pinchedId = pDivChild->getId(); }
                            dynamic_cast<Button*>(pDivChild)->mouseClickCb(x, y, b);
                        }
                    }
                }
            }
        }
        //
        delta = 0;
        prevX = x;
        prevY = y;
        for (const auto& comp : getNodes())
        {
            if (comp->getType() == CompType::Button) { continue; }
            if (layout.orientation == LdOrientation::Horizontal)
            {
                comp->layout.scaling.horizontal.policy = LdScalePolicy::Absolute;
                comp->layout.scaling.horizontal.value = comp->getTransformRead().scale.x;
            }
            else
            {
                comp->layout.scaling.vertical.policy = LdScalePolicy::Absolute;
                comp->layout.scaling.vertical.value = comp->getTransformRead().scale.y;
            }
        }
    }
}

void PinchDiv::separatorRelease(int16_t x, int16_t y, MouseButton b)
{
    if (b == MouseButton::Left)
    {
        //
        for (const auto& comp : getNodes())
        {
            if (comp->getType() == CompType::PinchDiv)
            {
                const auto& pDivNodes = comp->getNodes();
                for (const auto& pDivChild : pDivNodes)
                {
                    if (pDivChild->getType() == CompType::Button)
                    {
                        if (pDivChild->getId() == pinchedId) dynamic_cast<Button*>(pDivChild)->mouseReleaseCb(x, y, b);
                        if (pDivChild->getId() == pinchedId2) dynamic_cast<Button*>(pDivChild)->mouseReleaseCb(x, y, b);
                    }
                }
            }
        }
        pinchedId = 0;
        pinchedId2 = 0;
        //
        delta = 0;
        prevX = x;
        prevY = y;
        float decreaseBy = 0;
        for (const auto& comp : getNodes())
        {
            if (comp->getType() != CompType::Button) { continue; }
            if (layout.orientation == LdOrientation::Horizontal)
            {
                decreaseBy += comp->layout.scaling.horizontal.value;
            }
            else { decreaseBy += comp->layout.scaling.vertical.value; }
        }
        for (const auto& comp : getNodes())
        {
            if (comp->getType() == CompType::Button) { continue; }
            if (layout.orientation == LdOrientation::Horizontal)
            {

                comp->layout.scaling.horizontal.policy = LdScalePolicy::Relative;
                comp->layout.scaling.horizontal.value = comp->getTransformRead().scale.x /
                                                        (getTransformRead().scale.x - decreaseBy);
            }
            else
            {
                comp->layout.scaling.vertical.policy = LdScalePolicy::Relative;
                comp->layout.scaling.vertical.value = comp->getTransformRead().scale.y /
                                                      (getTransformRead().scale.y - decreaseBy);
            }
        }
    }
}

void PinchDiv::separatorMove(int16_t x, int16_t y, int16_t index)
{
    //
    for (const auto& comp : getNodes())
    {
        if (comp->getType() == CompType::PinchDiv)
        {
            const auto& pDivNodes = comp->getNodes();
            for (const auto& pDivChild : pDivNodes)
            {
                if (pDivChild->getType() == CompType::Button)
                {
                    if (pDivChild->getId() == pinchedId) dynamic_cast<Button*>(pDivChild)->mouseMoveCb(x, y);
                    if (pDivChild->getId() == pinchedId2) dynamic_cast<Button*>(pDivChild)->mouseMoveCb(x, y);
                }
            }
        }
    }
    //

    PinchPair& pp = pinchPairs[index];
    if (layout.orientation == LdOrientation::Horizontal)
    {

        delta = x - prevX;
        pp.first->layout.scaling.horizontal.value += delta;
        pp.second->layout.scaling.horizontal.value -= delta;
    }
    else
    {
        delta = y - prevY;
        pp.first->layout.scaling.vertical.value += delta;
        pp.second->layout.scaling.vertical.value -= delta;
    }

    prevX = x;
    prevY = y;
    refreshLayout();
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