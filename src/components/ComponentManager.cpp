#include "ComponentManager.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
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
    /* Note: 'onLayoutUpdate' can add/remove nodes while iterating 'flattenedNodes', fact designated by 'onLayoutUpdate'
       returning TRUE. In such cases, we need to retrigger a layout update again to make sure that new node or the now
       removed node action is correctly performed. We also need to stop each time we detect an update is needed as to
       not go further an access now invalid iterators. */
    bool needsUpdate = false;
    for (const auto& childNode : flattenedNodes | std::views::reverse)
    {
        if (childNode->getNodes().empty()) { continue; }

        if (childNode->onLayoutUpdate())
        {
            needsUpdate = true;
            break;
        }
    }

    if (needsUpdate) { updateInternalTreeStructure("AdditionOrRemovalInLayoutUpdate"); }

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
       removed and depth testing SHALL be enabled. */
    // for (const auto& childNode : flattenedNodes | std::views::reverse)
    for (const auto& childNode : flattenedNodes)
    {
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
            glScissor(pViewableArea.start.x, state.windowHeight - (pViewableArea.start.y + pViewableArea.scale.y),
                pViewableArea.scale.x, pViewableArea.scale.y);
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

        const auto& childVA = childNode->viewArea;
        bool xConstraint = state.mouseX >= childVA.start.x && state.mouseX <= childVA.start.x + childVA.scale.x;
        bool yConstraint = state.mouseY >= childVA.start.y && state.mouseY <= childVA.start.y + childVA.scale.y;
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
}

void ComponentManager::resizeEvent(int newWidth, int newHeight)
{
    // utils::printlni("Resizing to {} {}...", newWidth, newHeight);

    const int16_t WINDOW_FULLSCREEN_SENSE_VALUE = 200;
    bool sensedWindowFullscreen = std::abs(newWidth - state.windowWidth) > WINDOW_FULLSCREEN_SENSE_VALUE;

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

    /* Note: Due to (1) 'windowMaximizedEvent' being called before 'resizeEvent', update layout from (1) is useless due
       to it having to deal with previous window size values. Due to previous values, scrollbars knob size will not be
       displayed correctly. A trick to solving this is to somehow "sense" that the window might of been resized to
       fullscreen or restored to previous size, in which case, trigger a layout updated again. This is not related only
       to scrollbars, but this is where the bug was found.*/
    if (sensedWindowFullscreen)
    {
        utils::printlni("Sensed window going fullscreen or reverse.");
        updateLayout();
    }
}

void ComponentManager::windowMaximizedEvent(int maximized)
{
    (void)maximized;
    /* Note: On linux, this is called before 'resizeEvent' */
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
    /* Compute viewable area from backmost element to frontmost element. Back to front. */
    for (const auto& childNode : flattenedNodes | std::views::reverse)
    {
        /* If we are root, viewable area is ourself */
        if (childNode->getId() == root->getId())
        {
            childNode->viewArea.start = childNode->getTransformRead().pos;
            childNode->viewArea.scale = childNode->getTransformRead().scale;
        }
        /* Else we are a child, our viewable area depends on the viewable area of the parent */
        else
        {
            const auto& pViewableArea = childNode->getParent()->viewArea;
            const auto& childPos = childNode->getTransformRead().pos;
            const auto& childScale = childNode->getTransformRead().scale;

            const auto pViewAreaEndX = (int16_t)(pViewableArea.start.x + pViewableArea.scale.x);
            const auto pViewAreaEndY = (int16_t)(pViewableArea.start.y + pViewableArea.scale.y);

            const auto childEndX = (int16_t)(childPos.x + childScale.x);
            const auto childEndY = (int16_t)(childPos.y + childScale.y);

            // Logic: maxStart(parentXY,childXY) , minEnd(parentXY,childXY);
            childNode->viewArea.start.x = std::max(pViewableArea.start.x, (int16_t)childPos.x);
            childNode->viewArea.start.y = std::max(pViewableArea.start.y, (int16_t)childPos.y);
            childNode->viewArea.scale.x = std::min(pViewAreaEndX, childEndX) - childNode->viewArea.start.x;
            childNode->viewArea.scale.y = std::min(pViewAreaEndY, childEndY) - childNode->viewArea.start.y;
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