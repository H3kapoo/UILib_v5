#include "ResizeDiv.hpp"

#include "src/Utility.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"

namespace components
{
ResizeDiv::ResizeDiv()
    : AbstractComponent({.type = CompType::ResizeDiv, .shaderPath = "src/assets/shaders/bordered.glsl"})
// , imageDummy("/home/hekapoo/newTryAtUI/src/assets/shaders/baseTextured.glsl")
// , textureLoader(TextureLoader::get())

{
    /* This object should not be renderable. It should serve only as a skeleton. */
    setRenderable(false);
}

ResizeDiv::~ResizeDiv()
{
    /* Free separators occupied memory. */
    for (const auto& sep : separators)
    {
        if (sep) { delete sep; }
    }
}

void ResizeDiv::append(std::vector<AbstractComponent*>&& panes)
{
    if (panes.size() < 2)
    {
        utils::printlnw("ResizeDiv not really useful with less than 2 elements :(");
        return;
    }

    const auto& nodes = getNodes();
    for (const auto& pane : panes)
    {
        /* If ResizeDiv is horizontal, fill the vertical part of the pane and vice versa if vertical. */
        if (layout.orientation == LdOrientation::Horizontal)
        {
            pane->layout.scaling.horizontal.policy = LdScalePolicy::Relative;
            pane->layout.scaling.vertical.policy = LdScalePolicy::Relative;
            pane->layout.scaling.vertical.value = 1.0f;
        }
        else
        {
            pane->layout.scaling.vertical.policy = LdScalePolicy::Relative;
            pane->layout.scaling.horizontal.policy = LdScalePolicy::Relative;
            pane->layout.scaling.horizontal.value = 1.0f;
        }

        /* If we have no other children, simply push the first Pane in. */
        if (nodes.empty())
        {
            AbstractComponent::append(pane);
            continue;
        }

        /* If ResizeDiv is not empty, means there's already at least one pane as a child. When adding the next pane
           child, put a SeparatorBar between them.*/
        SeparatorBar* separatorBar = new SeparatorBar();
        separators.push_back(separatorBar);

        /* Get the last pane element. It's safe to dereference. */
        AbstractComponent* lastPane = *(nodes.end() - 1);

        /* Save the current pane and the previous one in a pair for later. Save it's index to pass to the
           SeparatorBar so the bar knows what elements to scale on event. */
        panePairs.emplace_back(lastPane, pane);
        int16_t index = panePairs.size() - 1;

        /* Depending on orientation, scale the SeparatorBar accordingly. */
        const auto scaling = layout.orientation == LdOrientation::Horizontal // clang-format off
                                 ? LdScaling{
                                    {LdScalePolicy::Absolute, (float)style.separatorSize},
                                    {LdScalePolicy::Relative, 1.0f}}
                                 : LdScaling{
                                    {LdScalePolicy::Relative, 1.0f},
                                    {LdScalePolicy::Absolute, (float)style.separatorSize}}; // clang-format on
        separatorBar->layout.scaling = scaling;

        /* Add the new SeparatorBar and Pane */
        AbstractComponent::append({separatorBar, pane});

        /* Bind all necessary callbacks for the bar */
        separatorBar->addClickListener(std::bind(&ResizeDiv::onSeparatorClick, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3));

        separatorBar->addReleaseListener(std::bind(&ResizeDiv::onSeparatorRelease, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3));

        separatorBar->addMoveClickedListener(std::bind(&ResizeDiv::onSeparatorDrag, this, std::placeholders::_1,
            std::placeholders::_2, index));

        separatorBar->addMoveListener(std::bind(&ResizeDiv::onMouseMoveOverSeparator, this, std::placeholders::_1,
            std::placeholders::_2));

        separatorBar->addOnExitListener(std::bind(&ResizeDiv::onSeparatorExit, this));
    }

    firstUpdateAfterAppend = true;
    refreshLayout();
}

void ResizeDiv::onSeparatorClick(const int16_t x, const int16_t y, const MouseButton b)
{
    /* Resizing is only possible on left mouse click */
    if (b != MouseButton::Left) { return; }

    currentlyDragging = true;

    /* Simulate mouse click on other SeparatorBars first due to click on this SeparatorBar. This introduces the
      posibility to manipulate multiple pinch areas at once. */
    if (suppBarOne) suppBarOne->onClickEvent();
    if (suppBarTwo) suppBarTwo->onClickEvent();

    prevX = x;
    prevY = y;
}

void ResizeDiv::onSeparatorRelease(const int16_t x, const int16_t y, const MouseButton b)
{
    /* We do not care if other buttons have been released, only the left one. */
    if (b != MouseButton::Left) { return; }
    currentlyDragging = false;

    /* Simulate mouse release on other SeparatorBars first due to release on this SeparatorBar. Even if the name is
       onClick, this is a generic event that knows how to handle click/release of click. If uses the current UIState. */
    if (suppBarOne) suppBarOne->onClickEvent();
    if (suppBarTwo) suppBarTwo->onClickEvent();

    prevX = x;
    prevY = y;
}

void ResizeDiv::onSeparatorDrag(int16_t x, int16_t y, int16_t index)
{
    /* Simulate mouse move on supporting SeparatorBars of this SeparatorBar first due to dragging. */
    if (suppBarOne) suppBarOne->onMoveEvent();
    if (suppBarTwo) suppBarTwo->onMoveEvent();

    /* Add size to the first Pane and subtract it from the second Pane, effectively resizing and resizing the panes.
       Also do min bounds checking. */
    PanePair& pp = panePairs[index];
    if (layout.orientation == LdOrientation::Horizontal)
    {
        int16_t delta = x - prevX;
        if (pp.first->layout.scaling.horizontal.value + delta >= pp.first->layout.scaling.horizontal.min &&
            pp.second->layout.scaling.horizontal.value - delta >= pp.second->layout.scaling.horizontal.min)
        {
            pp.first->layout.scaling.horizontal.value += delta;
            pp.second->layout.scaling.horizontal.value -= delta;
        }
    }
    else
    {
        int16_t delta = y - prevY;
        if (pp.first->layout.scaling.vertical.value + delta >= pp.first->layout.scaling.vertical.min &&
            pp.second->layout.scaling.vertical.value - delta >= pp.second->layout.scaling.vertical.min)
        {
            pp.first->layout.scaling.vertical.value += delta;
            pp.second->layout.scaling.vertical.value -= delta;
        }
    }

    prevX = x;
    prevY = y;
    refreshLayout();
}

void ResizeDiv::onSeparatorExit()
{
    /* If we exit the currently highlighed separator and it has some 3/4 way bar also highlighed, we need to call exit
       on them too. Otherwise they will stay highlighed even if we exit them. */
    if (suppBarOne && suppBarOne->getId() != getState()->hoveredId) { suppBarOne->onMouseExitEvent(); }
    if (suppBarTwo && suppBarTwo->getId() != getState()->hoveredId) { suppBarTwo->onMouseExitEvent(); }
    suppBarOne = nullptr;
    suppBarTwo = nullptr;
}

void ResizeDiv::onMouseMoveOverSeparator(int16_t x, int16_t y)
{
    /* If we are currently dragging, all support separator we need have been already set. No reason to recompute. */
    if (currentlyDragging) { return; }

    /* Find inside the children of this PinchDiv other PinchDivs and calculate which SeparatorBars of those are in
       "style.separatorSize" distance from the clicked SeparatorBar. These new SeparatorBars will be used for 3-way and
       4-way pane pinch. */
    bool putInFirstSupportingSlot = true;
    for (const auto& comp : getNodes())
    {
        if (comp->getType() != CompType::ResizeDiv) { continue; }

        const auto& pDivNodes = comp->getNodes();
        for (const auto& pDivChild : pDivNodes)
        {
            if (pDivChild->getType() != CompType::SeparatorBar || !isSeparatorClose(pDivChild, x, y)) { continue; }

            const auto sepBar = dynamic_cast<SeparatorBar*>(pDivChild);
            if (!sepBar)
            {
                utils::printlne("FATAL dynamic_cast error for {}", pDivChild->getId());
                return;
            }

            /* Simulate on mouse enter for the closest SeparatorBar */
            sepBar->onMouseEnterEvent();

            /* Populate the 2 possible additional SeparatorBars */
            if (putInFirstSupportingSlot) { suppBarOne = sepBar; }
            else { suppBarTwo = sepBar; }
            putInFirstSupportingSlot = false;
        }
    }
}

bool ResizeDiv::isSeparatorClose(AbstractComponent* sep, const int16_t x, const int16_t y)
{
    glm::vec2 start, end;
    auto& sepTrans = sep->getTransformRead();
    if (layout.orientation == LdOrientation::Horizontal)
    {
        start = sepTrans.pos + sepTrans.scale;
        start.y -= style.separatorSize;
        end = start + glm::vec2(style.separatorSize, style.separatorSize);
    }
    else
    {
        start = sepTrans.pos + sepTrans.scale;
        start.x -= style.separatorSize;
        end = start + glm::vec2(style.separatorSize, style.separatorSize);
    }

    bool xCEnd = x >= start.x && x <= end.x;
    bool yCEnd = y >= start.y && y <= end.y;

    if (layout.orientation == LdOrientation::Horizontal)
    {
        start = sepTrans.pos;
        start.x -= style.separatorSize;
        end = start + glm::vec2(style.separatorSize, style.separatorSize);
    }
    else
    {
        start = sepTrans.pos;
        start.y -= style.separatorSize;
        end = start + glm::vec2(style.separatorSize, style.separatorSize);
    }

    bool xCStart = x >= start.x && x <= end.x;
    bool yCStart = y >= start.y && y <= end.y;

    return (xCEnd && yCEnd) || (xCStart && yCStart);
}

void ResizeDiv::onPrepareToRender()
{
    auto border = glm::vec4(layout.border.top, layout.border.bottom, layout.border.left, layout.border.right);

    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uInnerColor", style.color);
    getShader().setVec4f("uBorderColor", style.borderColor);
    getShader().setVec4f("uBorderSize", border);
    getShader().setVec2f("uResolution", getTransformRead().scale);
}

void ResizeDiv::onStart() {}

void ResizeDiv::onRenderDone() {}

void ResizeDiv::onClickEvent() {}

void ResizeDiv::onMouseEnterEvent() {}

void ResizeDiv::onMouseExitEvent() {}

void ResizeDiv::onMoveEvent() {}

bool ResizeDiv::onLayoutUpdate()
{
    layoutCalc.calculate(firstUpdateAfterAppend);
    firstUpdateAfterAppend = false;
    return false;
}

} // namespace components