#pragma once

#include "../renderer/Renderer.hpp"
#include "AbstractComponent.hpp"

namespace components
{
/**
 * @brief Class responsible for managing the UI Components.
 *
 * Class sends inputs from the user to the components, updates internal structure on addition/removal of components,
 * keeps track of UI state, etc
 */
class ComponentManager
{
public:
    /* Constructors */
    ComponentManager();
    ~ComponentManager();

    /* Tree Management */
    /**
     * @brief Set the Root object.
     *
     * @param newRoot - trivial
     */
    void setRoot(AbstractComponent* newRoot);

    /**
     * @brief Remove currently set root.
     * TODO: Not implemented.
     */
    void removeRoot();

    void render();

    /* Events handlers */
    /**
     * @brief Function to be called by Input management when mouse click happens.
     *
     * @param button - LMB-0 RMB-1 MMB-2
     * @param action - Released/0 Pressed/1
     * @param mods   - modifier bits regarding CTRL/ALT/SHIFT being pressed or not
     */
    void mouseClickEvent(MouseButton button, KeyAction action, ActiveModifiersBits mods);

    /**
     * @brief Function to be called by Input management when mouse move happens.
     *
     * @param mouseX - trivial
     * @param mouseY - trivial
     */
    void mouseMoveEvent(double mouseX, double mouseY);
    /**
     * @brief Function to be called by Input management when window is resized.
     *
     * @param newWidth - trivial
     * @param newHeight - trivial
     */
    void resizeEvent(int newWidth, int newHeight);

private:
    /* Internal helpers */
    /**
     * @brief Updates node structure by calling the update function on every node and child node of the tree. Also
     * flattens the nodes in a depth descending way suited for fast click detection.
     *
     * @param action - action that was performed to trigger this function.
     */
    void updateInternalStructure(const std::string& action);

    /**
     * @brief Flattens the nodes in a depth descending way suited for fast click detection.
     * No reason to traverse all the tree nodes from root to leaf nodes and checking if click/hover happened. 99% of the
     * time it's a leaf node we click on and in this project leafs have increasing depth values. By sorting nodes in a
     * vector in a depth descending order, click/hover detection can happen without traversing the tree at all.
     */
    void flattenRootIfNeeded();
    void recursivelyPushNodes(AbstractComponent* comp);

    renderer::Renderer renderer;

    /* UI State storage */
    UIState state;

    /* These will be deallocated trivially*/
    AbstractComponent* root;
    std::vector<AbstractComponent*> flattenedNodes;
};
} // namespace components