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
                std::placeholders::_2, std::placeholders::_3));

            bar->addReleaseListener(std::bind(&PinchDiv::separatorRelease, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

            bar->addMoveClickedListener(std::bind(&PinchDiv::separatorClickedMove, this, std::placeholders::_1,
                std::placeholders::_2, index));

            bar->addMoveListener(std::bind(&PinchDiv::separatorGeneralMove, this, std::placeholders::_1,
                std::placeholders::_2));
            continue;
        }

        AbstractComponent::append(c);
    }
}

void PinchDiv::separatorClick(int16_t x, int16_t y, MouseButton b)
{
    if (b != MouseButton::Left) { return; }
    currentlyDragging = true;

    /* Simulate mouse click on other PinchBars first due to click on this PinchBar */
    if (firstBar) firstBar->mouseClickCb(x, y, b);
    if (secondBar) secondBar->mouseClickCb(x, y, b);

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

void PinchDiv::separatorRelease(int16_t x, int16_t y, MouseButton b)
{
    if (b != MouseButton::Left) { return; }
    currentlyDragging = false;

    /* Simulate mouse release on other PinchBars first due to click on this PinchBar */
    if (firstBar) firstBar->mouseReleaseCb(x, y, b);
    if (secondBar) secondBar->mouseReleaseCb(x, y, b);

    delta = 0;
    prevX = x;
    prevY = y;
    float decreaseBy = 0;
    for (const auto& comp : getNodes())
    {
        if (comp->getType() != CompType::PinchBar) { continue; }
        if (layout.orientation == LdOrientation::Horizontal) { decreaseBy += comp->layout.scaling.horizontal.value; }
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

void PinchDiv::separatorClickedMove(int16_t x, int16_t y, int16_t index)
{
    /* Simulate movement on other PinchBars first due to click on this PinchBar */
    if (firstBar) firstBar->mouseMoveClickedCb(x, y);
    if (secondBar) secondBar->mouseMoveClickedCb(x, y);

    /* Add size to the PaneId-1 and subtract it from the PaneId+1, effectively pinching and resizing the panes */
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

void PinchDiv::separatorGeneralMove(int16_t x, int16_t y)
{
    /* If we are currently dragging, all side bars we need have been already set. No reason to recompute. */
    if (currentlyDragging) { return; }

    /* Simulate on mouse exit for the closest PinchBars from before. Since we are not dragging anymore, these need to be
       reset & recalculated */
    if (firstBar) { firstBar->onMouseExitEvent(); }
    if (secondBar) { secondBar->onMouseExitEvent(); }

    firstBar = nullptr;
    secondBar = nullptr;

    /* Find inside the children of this PinchDiv other PinchDivs and calculate which PinchBars of those are in
       "separatorSize" distance from the clicked PinchBar. These new PinchBars will be used for 3-way and 4-way pane
       pinch. */
    bool putInFirst = true;
    glm::vec2 start, end;
    for (const auto& comp : getNodes())
    {
        if (comp->getType() == CompType::PinchDiv)
        {
            const auto& pDivNodes = comp->getNodes();
            for (const auto& pDivChild : pDivNodes)
            {
                if (pDivChild->getType() == CompType::PinchBar)
                {
                    if (layout.orientation == LdOrientation::Horizontal)
                    {
                        start = pDivChild->getTransformRead().pos + pDivChild->getTransformRead().scale;
                        start.y -= separatorSize;
                        end = start + glm::vec2(separatorSize, separatorSize);
                    }
                    else
                    {
                        start = pDivChild->getTransformRead().pos + pDivChild->getTransformRead().scale;
                        start.x -= separatorSize;
                        end = start + glm::vec2(separatorSize, separatorSize);
                    }

                    bool xCEnd = x >= start.x && x <= end.x;
                    bool yCEnd = y >= start.y && y <= end.y;

                    if (layout.orientation == LdOrientation::Horizontal)
                    {
                        start = pDivChild->getTransformRead().pos;
                        start.x -= separatorSize;
                        end = start + glm::vec2(separatorSize, separatorSize);
                    }
                    else
                    {
                        start = pDivChild->getTransformRead().pos;
                        start.y -= separatorSize;
                        end = start + glm::vec2(separatorSize, separatorSize);
                    }

                    bool xCStart = x >= start.x && x <= end.x;
                    bool yCStart = y >= start.y && y <= end.y;

                    if ((xCEnd && yCEnd) || (xCStart && yCStart))
                    {
                        const auto pb = dynamic_cast<PinchBar*>(pDivChild);
                        if (!pb)
                        {
                            utils::printlne("FATAL dynamic_cast error for {}", pDivChild->getId());
                            return;
                        }

                        /* Simulate on mouse enter for the closest PinchBar */
                        pb->onMouseEnterEvent();

                        /* Populate the 2 possible additional PinchBars */
                        if (putInFirst) { firstBar = pb; }
                        else { secondBar = pb; }
                        putInFirst = !putInFirst;
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