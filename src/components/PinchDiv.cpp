#include "PinchDiv.hpp"

#include "../Utility.hpp"
#include "AbstractComponent.hpp"
#include "PinchBar.hpp"
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
            PinchBar* bar = new PinchBar();
            separators.push_back(bar);
            // bar->style.color = utils::hexToVec4("#b000c4ff");
            if (layout.orientation == LdOrientation::Horizontal)
            {
                bar->layout.scaling = LdScaling{
                    {LdScalePolicy::Absolute, (float)separatorSize}, {LdScalePolicy::Relative, 1.0f}};
            }
            else
            {
                bar->layout.scaling = LdScaling{
                    {LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Absolute, (float)separatorSize}};
            }

            AbstractComponent::append(bar);
            AbstractComponent::append(c);

            pinchPairs.emplace_back(it, c);
            int16_t index = pinchPairs.size() - 1;

            bar->addClickListener(std::bind(&PinchDiv::separatorClick, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3, bar));

            bar->addReleaseListener(std::bind(&PinchDiv::separatorRelease, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3, bar));

            bar->addMoveClickedListener(std::bind(&PinchDiv::separatorClickedMove, this, std::placeholders::_1,
                std::placeholders::_2, index));

            bar->addMoveListener(std::bind(&PinchDiv::separatorMove, this, std::placeholders::_1, std::placeholders::_2,
                bar));
            continue;
        }

        AbstractComponent::append(c);
    }
}

void PinchDiv::separatorClick(int16_t x, int16_t y, MouseButton b, PinchBar* thisBar)
{
    if (b == MouseButton::Left)
    {
        //
        // utils::printlne("id is {}", thisBar->getId());
        for (const auto& comp : getNodes())
        {
            if (comp->getType() == CompType::PinchDiv)
            {
                const auto& pDivNodes = comp->getNodes();
                for (const auto& pDivChild : pDivNodes)
                {
                    if (pDivChild->getType() == CompType::PinchBar)
                    {
                        float d = utils::dist(glm::vec2(x, y), pDivChild->getTransformRead().pos);
                        float d2 = utils::dist(glm::vec2(x, y),
                            pDivChild->getTransformRead().pos + pDivChild->getTransformRead().scale);
                        if (d <= separatorSize || d2 <= separatorSize)
                        {
                            if (pinchedId2 == 0 && pinchedId != 0) { pinchedId2 = pDivChild->getId(); }
                            if (pinchedId == 0 && pinchedId2 == 0) { pinchedId = pDivChild->getId(); }
                            dynamic_cast<PinchBar*>(pDivChild)->mouseClickCb(x, y, b);
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
            if (comp->getType() == CompType::PinchBar) { continue; }
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

void PinchDiv::separatorRelease(int16_t x, int16_t y, MouseButton b, PinchBar* thisBar)
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
                    if (pDivChild->getType() == CompType::PinchBar)
                    {
                        if (pDivChild->getId() == pinchedId)
                        {
                            dynamic_cast<PinchBar*>(pDivChild)->mouseReleaseCb(x, y, b);
                        }
                        if (pDivChild->getId() == pinchedId2)
                        {
                            dynamic_cast<PinchBar*>(pDivChild)->mouseReleaseCb(x, y, b);
                        }
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
            if (comp->getType() != CompType::PinchBar) { continue; }
            if (layout.orientation == LdOrientation::Horizontal)
            {
                decreaseBy += comp->layout.scaling.horizontal.value;
            }
            else { decreaseBy += comp->layout.scaling.vertical.value; }
        }
        for (const auto& comp : getNodes())
        {
            if (comp->getType() == CompType::PinchBar) { continue; }
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

void PinchDiv::separatorClickedMove(int16_t x, int16_t y, int16_t index)
{
    //
    for (const auto& comp : getNodes())
    {
        if (comp->getType() == CompType::PinchDiv)
        {
            const auto& pDivNodes = comp->getNodes();
            for (const auto& pDivChild : pDivNodes)
            {
                if (pDivChild->getType() == CompType::PinchBar)
                {
                    if (pDivChild->getId() == pinchedId)
                    {
                        dynamic_cast<PinchBar*>(pDivChild)->mouseMoveClickedCb(x, y);
                    }
                    if (pDivChild->getId() == pinchedId2)
                    {
                        dynamic_cast<PinchBar*>(pDivChild)->mouseMoveClickedCb(x, y);
                    }
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

void PinchDiv::separatorMove(int16_t x, int16_t y, PinchBar* thisBar)
{
    for (const auto& comp : getNodes())
    {
        if (comp->getType() == CompType::PinchDiv)
        {
            const auto& pDivNodes = comp->getNodes();
            for (const auto& pDivChild : pDivNodes)
            {
                if (pDivChild->getType() == CompType::PinchBar)
                {
                    float d = utils::dist(glm::vec2(x, y), pDivChild->getTransformRead().pos);
                    float d2 = utils::dist(glm::vec2(x, y),
                        pDivChild->getTransformRead().pos + pDivChild->getTransformRead().scale);
                    if (d <= separatorSize || d2 <= separatorSize)
                    {
                        dynamic_cast<PinchBar*>(pDivChild)->onMouseEnterEvent();
                    }
                    else
                    {
                        // ee
                        dynamic_cast<PinchBar*>(pDivChild)->onMouseExitEvent();
                    }
                }
            }
        }
    }
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

void PinchDiv::onStart() {}

void PinchDiv::onRenderDone() {}

void PinchDiv::onClickEvent() {}

void PinchDiv::onMouseEnterEvent() {}

void PinchDiv::onMouseExitEvent() {}

void PinchDiv::onMoveEvent() {}

bool PinchDiv::onLayoutUpdate()
{
    layoutCalc.calculate();
    return false;
}

} // namespace components