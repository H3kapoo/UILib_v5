#include "ComponentManager.hpp"

#include <algorithm>
#include <ranges>

#include "../Utility.hpp"

namespace components
{
ComponentManager::ComponentManager()
{
    state.triggerTreeUpdate = std::bind(&ComponentManager::updateInternalStructure, this, std::placeholders::_1);
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
    updateInternalStructure("RootAdition");
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
        if (childNode->getNodes().empty()) { return; }
        childNode->onLayoutUpdate();
    }
}

void ComponentManager::render()
{
    renderer.clearScreen();
    for (const auto& childNode : flattenedNodes)
    {
        childNode->onPrepareToRender();
        renderer.renderComponent(*childNode);
        childNode->onRenderDone();
    }
}

void ComponentManager::mouseClickEvent(MouseButton button, KeyAction action, ActiveModifiersBits mods)
{
    /* Will trigger the event top to bottom */
    /* TODO: decide who actually got clicked */
    for (const auto& childNode : flattenedNodes)
    {
        state.clickedButton = button;
        state.keyAction = action;
        state.activeMods = mods;
        childNode->onClickEvent();
    }
}

void ComponentManager::mouseMoveEvent(double mouseX, double mouseY)
{
    /* Will trigger the event top to bottom */
    /* TODO: decide who actually got clicked */
    for (const auto& childNode : flattenedNodes)
    {
        /* Conversion double -> int done here since in 99% of the cases we dont have fractional mouse position.
           Might be different on MacOS, but I dont have one. */
        state.mouseX = (int)mouseX;
        state.mouseY = (int)mouseY;
        childNode->onMoveEvent();
    }
}

void ComponentManager::resizeEvent(int newWidth, int newHeight)
{
    // utils::printlni("Resizing...");
    state.windowWidth = newWidth;
    state.windowHeight = newHeight;
    glm::mat4 projMatrix = glm::ortho(0.0f, (float)newWidth, (float)newHeight, 0.0f, renderer::Renderer::MAX_LAYERS,
        0.0f);

    renderer.setProjectionMatrix(projMatrix);

    /* glViewport is needed after changing the ortho matrix or else
       the NDC coordinates will not be mapped correctly to screen coordinates. */
    glViewport(0, 0, newWidth, newHeight);

    /* Keeps root the same size as the window */
    root->getBoxModelRW().scale = {newWidth, newHeight};
    updateLayout();
}

void ComponentManager::updateInternalStructure(const std::string& action)
{
    state.lastActionOnTree = action;
    root->updateNodeStructure();

    /* Tree can be dirty due to additions or removals */
    flattenRootIfNeeded();
};

void ComponentManager::flattenRootIfNeeded()
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