#include "ComponentManager.hpp"

#include <algorithm>
#include <cstdint>
#include <glm/fwd.hpp>
#include <ranges>

#include "../Utility.hpp"
#include "AbstractComponent.hpp"

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
    /* Note: due to the fact that child.onLayoutUpdate can add/remove new nodes while this function is running, it can
       invalidate "flattenedNodes | std::views::reverse" statement pointers. Basically addition/removal while iterating.
       Solution for now is to cache "flattenedNodes" into a temp vector and iterate over the temp vector. */
    std::vector<AbstractComponent*> tmpFlattenedNodes;
    tmpFlattenedNodes.reserve(flattenedNodes.size());
    for (const auto& childNode : flattenedNodes | std::views::reverse)
    {
        tmpFlattenedNodes.push_back(childNode);
    }

    for (const auto& childNode : tmpFlattenedNodes)
    {
        if (childNode->getNodes().empty()) { continue; }

        childNode->onLayoutUpdate();
    }

    /* Compute viewableArea for each element */
    computeViewableArea();
}

void ComponentManager::render()
{
    // TODO: take into account viewable area. GL_SCISSORS. No rounded corners cutting is supported (might need to
    // experiment with stencil buffer in the future)

    renderer.clearScreen();

    /* Note: glScissor normally has start coords in lower left corner. This UI has start coords in top left corner,
     * so calculations on where Scissors start on Y axis are reversed.*/
    glEnable(GL_SCISSOR_TEST);

    /* Note: For alpha blending to work, we unfortunatelly have to render objects back to front and disable depth
       testing. This introduces a bit of overdraw sadly. If it's know there will be no alpha blending, 'reverse' can be
       removed. */
    for (const auto& childNode : flattenedNodes | std::views::reverse)
    {
        // TODO: Render only if: comp is renderable and viewable area is not zero (basically scale on X or Y should be
        // greater than zero
        if (!childNode->isComponentRenderable()) { continue; }

        if (childNode->getId() == root->getId())
        {
            const auto pTransform = childNode->getTransformRead();
            glScissor(pTransform.pos.x, state.windowHeight - (pTransform.pos.y + pTransform.scale.y),
                pTransform.scale.x, pTransform.scale.y);
        }
        else
        {
            const auto& pViewableArea = childNode->viewArea;
            glScissor(pViewableArea.start.x, state.windowHeight - pViewableArea.end.y, pViewableArea.end.x,
                pViewableArea.end.y - pViewableArea.start.y);
        }

        childNode->onPrepareToRender();
        renderer.renderComponent(*childNode);
        childNode->onRenderDone();
    }

    glDisable(GL_SCISSOR_TEST);
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

    // TODO: Notify only the hovered comp
    for (const auto& childNode : flattenedNodes)
    {
        if (state.hoveredId == childNode->getId())
        {
            childNode->onMoveEvent();
            break;
        }
    }

    /* Bellow two passes should in very worst case be O(2N) but in 95% of cases is much less than that */
    bool shouldNotify = false;
    for (const auto& childNode : flattenedNodes)
    {
        /* If mouse is still pressed, we cannot switch who's the hovered element just yet. */
        if (state.mouseAction == HIDAction::Pressed) { return; }

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

void ComponentManager::mouseScrollEvent(double offsetX, double offsetY)
{
    (void)offsetX;

    state.scrollDirection = offsetY;

    for (const auto& childNode : flattenedNodes)
    {
        if (childNode->getId() == state.hoveredId) { childNode->onScroll(); }
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
    root->getTransformRW().pos = {0, 0};

    /* Layout also needs to be recalculated */
    updateLayout();
}

void ComponentManager::updateInternalTreeStructure(const std::string& action)
{
    state.lastActionOnTree = action;
    root->updateNodeStructure();

    /* Tree can be dirty due to additions or removals */
    flattenRoot();

    /* Keeps root the same size as the window */
    root->getTransformRW().scale = {state.windowWidth, state.windowHeight};
    root->getTransformRW().pos = {0, 0};

    /* Layout also needs to be recalculated due to possible addition/removal */
    updateLayout();
};

void ComponentManager::computeViewableArea()
{
    for (const auto& childNode : flattenedNodes | std::views::reverse)
    {
        /* If we are root, viewable area is ourself */
        if (childNode->getId() == root->getId())
        {
            childNode->viewArea.start = childNode->getTransformRead().pos;
            childNode->viewArea.end = childNode->getTransformRead().scale + childNode->getTransformRead().pos;
        }
        /* Else we are a child, our viewable area depends on the viewable area of the parent */
        else
        {
            // TODO: This is too simple, it depends on which side the 2 elements are on.

            const auto& pViewableArea = childNode->getParent()->viewArea;
            const auto& childPos = childNode->getTransformRead().pos;
            const auto& childScale = childNode->getTransformRead().scale;

            const auto pViewAreaEndX = (int16_t)(0 + pViewableArea.end.x);
            const auto pViewAreaEndY = (int16_t)(0 + pViewableArea.end.y);

            const auto childEndX = (int16_t)(childPos.x + childScale.x);
            const auto childEndY = (int16_t)(childPos.y + childScale.y);

            // Logic: maxStart(parentXY,childXY) , minEnd(parentXY,childXY);
            childNode->viewArea.start.x = std::max(pViewableArea.start.x, (int16_t)childPos.x);
            childNode->viewArea.end.y = std::min(pViewAreaEndY, childEndY);
            childNode->viewArea.start.y = std::max(pViewableArea.start.y, (int16_t)childPos.y);
            childNode->viewArea.end.x = std::min(pViewAreaEndX, childEndX);
        }
    }
}

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