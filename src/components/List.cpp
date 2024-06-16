#include "List.hpp"

#include "src/UIState.hpp"
#include "src/Utility.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/Button.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"
#include <utility>

namespace components
{
List::List()
    : AbstractComponent({.type = CompType::List, .shaderPath = "src/assets/shaders/bordered.glsl"})
    , imgHolder("src/assets/shaders/baseTextured.glsl")
    , textureLoader(TextureLoader::get())

{
    /* Don't show children of this component. Set to True if you want to see them displayed in the prints. */
    // setDetailChildrenOnPrint(false);

    /* This acts just as a holder. Don't render */
    // setRenderable(false);

    // holder.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    style.color = utils::hexToVec4("#919293ff");
    layout.orientation = LdOrientation::Vertical;

    setDetailChildrenOnPrint(false);

    /* Append the holder */
    // AbstractComponent::append(&holder);
}

List::~List()
{
    /* Free remaining entries on delete */
    for (auto&& entry : entries)
    {
        delete entry;
    }
}

void List::onPrepareToRender()
{
    auto border = glm::vec4(layout.border.top, layout.border.bottom, layout.border.left, layout.border.right);

    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uInnerColor", style.color);
    getShader().setVec4f("uBorderColor", style.borderColor);
    getShader().setVec4f("uBorderSize", border);
    getShader().setVec2f("uResolution", getTransformRead().scale);
}

void List::onStart()
{
    for (int i = 0; i < 30; i++)
    {
        auto entry = new Button();
        entry->layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Absolute, 15}};
        entry->layout.margin = {5, 5, 5, 0};
        entry->style.color = i % 2 == 0 ? utils::hexToVec4("#383838ff") : utils::hexToVec4("#595959ff");

        if (i == 29) { entry->style.color = utils::hexToVec4("#29b440ff"); }

        entry->addClickListener(
            [i](int, int, MouseButton mBtn)
            {
                if (mBtn != MouseButton::Left) { return; }

                utils::printlne("Clicked on entry id {}", i);
                // Empty
            });
        entry->setIgnoreScrollEvent(true);

        entries.emplace_back(std::move(entry));
    }

    AbstractComponent::append(entries);
    // imgHolder.transform.layer = getDepth() + 1;
    // utils::printlni("[INF] I am node {} and onStart() called {}", getId(), imgHolder.transform.layer);
}

void List::recycleLastEntry()
{
    const auto& allNodes = getNodes();
    AbstractComponent* lastNode = allNodes.back();
    // auto lastNodeIt = allNodes.end() - 1;

    AbstractComponent::remove(lastNode);
    AbstractComponent::appendAt(lastNode, 0);
    // std::swap(firstNode, lastNode);
    // if (lastNode) { utils::printlni("All good {}", lastNode->getId()); }
}

void List::onScroll()
{
    utils::printlni("Scroll {}", getState()->scrollDirection == 1 ? "Up" : "Down");
    recycleLastEntry();
}

void List::onRenderDone()
{
    /* No need to try to render internal image if we don't have one */
    // if (textureData == nullptr) { return; }
    // lwr.render(getState()->projectionMatrix, imgHolder);
}

void List::onClickEvent() {
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

void List::onMouseEnterEvent() {}

void List::onMouseExitEvent() {}

void List::onMoveEvent()
{
    // const auto& s = getState();
    // if (s->mouseAction == HIDAction::Pressed && s->clickedButton == MouseButton::Left)
    // {
    //     if (mouseMoveCb) mouseMoveCb(s->mouseX, s->mouseY);
    // }
}

bool List::onLayoutUpdate()
{
    layoutCalc.calculate();
    // btn.getTransformRW().scale.x = btn.getTransformRW().scale.y;
    return false;
}

// void List::addCheckedListener(std::function<void(bool)> cb)
// {
//     checkedCb = cb;
// }
} // namespace components