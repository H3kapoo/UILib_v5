#include "ComponentManager.hpp"

#include <algorithm>
#include <ranges>

#include "../Utility.hpp"
#include "layoutCalc/LayoutCalculator.hpp"

namespace components
{
ComponentManager::ComponentManager()
{
    state.triggerTreeUpdate = std::bind(&ComponentManager::updateInternalTreeStructure, this, std::placeholders::_1);
    state.triggerLayoutUpdate = std::bind(&ComponentManager::updateLayout, this);
}

ComponentManager::~ComponentManager()
{
    utils::printlni("Deleting ComponentManager..");
}

void ComponentManager::setRoot(AbstractComponent* newRoot)
{
    if (!newRoot) { utils::printlne("Provided root is null"); }
    if (root) { utils::printlnw("Root node already set to {}. Use removeRoot() first.", root->getId()); }
    root = newRoot;
    root->isParented = true; /* Parented to invisible object higher than root */
    root->setState(&state);
    updateInternalTreeStructure("RootAdition");
}

void ComponentManager::removeRoot()
{
    if (!root) { utils::printlnw("Root of component manager not set."); }

    utils::printlne("removeRoot() not implemented yet.");
}

void ComponentManager::updateLayout()
{
    for (const auto& childNode : flattenedNodes | std::views::reverse)
    {
        if (childNode->getNodes().empty()) { continue; }

        childNode->onLayoutUpdate();
    }
}

void ComponentManager::render()
{
    renderer.clearScreen();
    for (const auto& childNode : flattenedNodes)
    {
        if (!childNode->isComponentRenderable()) { continue; }

        childNode->onPrepareToRender();
        renderer.renderComponent(*childNode);
        childNode->onRenderDone();
    }
}

void ComponentManager::mouseClickEvent(MouseButton button, HIDAction action, ActiveModifiersBits mods)
{
    state.clickedButton = button;
    state.mouseAction = action;
    state.activeMods |= mods;

    /* Will trigger the event top to bottom */
    for (const auto& childNode : flattenedNodes)
    {
        if (state.hoveredId == childNode->getId())
        {
            state.selectedId = childNode->getId();
            childNode->onClickEvent();
            break;
        }
    }
}

void ComponentManager::mouseMoveEvent(double mouseX, double mouseY)
{
    /* Conversion double -> int done here since in 99% of the cases we dont have fractional mouse position.
       Might be different on MacOS, but I dont have one. */
    state.mouseX = (int)mouseX;
    state.mouseY = (int)mouseY;

    /* Bellow two passes should in very worst case be O(2N) but in 95% of cases is much less than that */
    bool shouldNotify = false;
    for (const auto& childNode : flattenedNodes)
    {
        const auto& childBox = childNode->getTransformRead();
        bool xConstraint = state.mouseX >= childBox.pos.x && state.mouseX <= childBox.pos.x + childBox.scale.x;
        bool yConstraint = state.mouseY >= childBox.pos.y && state.mouseY <= childBox.pos.y + childBox.scale.y;
        if (xConstraint && yConstraint)
        {
            /* This means we are still the currently hoeverd node, no need to do anything */
            if (state.hoveredId == childNode->getId()) { break; }

            /* Hovered node is now another one, set and notify */
            state.prevHoveredId = state.hoveredId;
            state.hoveredId = childNode->getId();
            shouldNotify = true;
            break;
        }
    }

    if (!shouldNotify) { return; }

    bool enterNotified = false, exitNotified = false;
    for (const auto& childNode : flattenedNodes)
    {
        if (childNode->getId() == state.prevHoveredId)
        {
            exitNotified = true;
            childNode->onMouseExitEvent();
        }
        if (childNode->getId() == state.hoveredId)
        {
            enterNotified = true;
            childNode->onMouseEnterEvent();
        }

        if (enterNotified && exitNotified) { break; }
    }
}

void ComponentManager::keyEvent(int key, HIDAction action, int mods)
{
    // TODO: Logic for clearing MODs bits needs to be introduced
    state.keyboardActionState[key] = action;
    state.activeMods |= mods;

    /* Will trigger the event top to bottom */
    for (const auto& childNode : flattenedNodes)
    {
        if (state.selectedId == childNode->getId())
        {
            childNode->onKeyEvent();
            break;
        }
    }

    // utils::printlne("key: {} act: {} mods: {}", key, (uint8_t)action, mods);
}

void ComponentManager::resizeEvent(int newWidth, int newHeight)
{
    // utils::printlni("Resizing...");
    state.windowWidth = newWidth;
    state.windowHeight = newHeight;
    glm::mat4 projMatrix = glm::ortho(0.0f, (float)newWidth, (float)newHeight, 0.0f, renderer::Renderer::MAX_LAYERS,
        0.0f);

    state.projectionMatrix = projMatrix;
    renderer.setProjectionMatrix(projMatrix);

    /* glViewport is needed after changing the ortho matrix or else
       the NDC coordinates will not be mapped correctly to screen coordinates. */
    glViewport(0, 0, newWidth, newHeight);

    /* Keeps root the same size as the window */
    root->getTransformRW().scale = {newWidth, newHeight};

    /* Layout also needs to be recalculated */
    updateLayout();
}

void ComponentManager::updateInternalTreeStructure(const std::string& action)
{
    state.lastActionOnTree = action;
    root->updateNodeStructure();

    /* Tree can be dirty due to additions or removals */
    flattenRoot();

    /* Layout also needs to be recalculated due to possible addition/removal */
    updateLayout();
};

void ComponentManager::flattenRoot()
{
    utils::printlni("Flattening tree due to {}", state.lastActionOnTree);
    flattenedNodes.clear();
    recursivelyPushNodes(root);

    std::ranges::sort(flattenedNodes,
        [](const AbstractComponent* a, const AbstractComponent* b) { return a->getDepth() > b->getDepth(); });
}

void ComponentManager::recursivelyPushNodes(AbstractComponent* comp)
{
    flattenedNodes.push_back(comp);
    for (const auto& ch : comp->getNodes())
    {
        recursivelyPushNodes(ch);
    }
}
} // namespace components