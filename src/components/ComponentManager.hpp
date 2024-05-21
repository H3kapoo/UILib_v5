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

    /**
     * @brief Render the components associated with the currently bound root.
     *
     * There are 3 passes for each component: preRender, actual render and post render pass.
     * PreRender pass gives compoenent time to setup associated shader uniforms, for example.
     * Render pass actually sends data to the gpu and draw calls.
     * PostRender pass can be used by the component to render additional assets on top of it, for example.
     */
    void render();

    /* Events handlers */
    /**
     * @brief Function to be called by Input management when mouse click happens.
     *
     * @param button - LMB-0 RMB-1 MMB-2
     * @param action - Released/0 Pressed/1
     * @param mods   - modifier bits regarding CTRL/ALT/SHIFT being pressed or not.
     */
    void mouseClickEvent(MouseButton button, HIDAction action, ActiveModifiersBits mods);

    /**
     * @brief Function to be called by Input management when mouse move happens.
     *
     * @param mouseX - trivial
     * @param mouseY - trivial
     */
    void mouseMoveEvent(double mouseX, double mouseY);

    /**
     * @brief Function to be called by Input management when mouse is scrolled.
     *
     * @param offsetX - trivial
     * @param offsetY - trivial
     */
    void mouseScrollEvent(double offsetX, double offsetY);

    /**
     * @brief Function to be called by Input management when a key event happens.
     *
     * @param key - Key that triggered the event.
     * @param action - trivial
     * @param mods   - modifier bits regarding CTRL/ALT/SHIFT being pressed or not.
     */
    void keyEvent(int key, HIDAction action, int mods);

    /**
     * @brief Function to be called by Input management when window is resized.
     *
     * @param newWidth - trivial
     * @param newHeight - trivial
     */
    void resizeEvent(int newWidth, int newHeight);

    /**
     * @brief Function to be called by Input management when window is minimized/maximized.
     *
     * @param maximized - Is window maximized now or not
     */
    void windowMaximizedEvent(int maximized);

private:
    /* Internal helpers */

    /**
     * @brief Calls to each component to reposition itself and it's children to the desired layout
     *
     */
    void updateLayout();

    /**
     * @brief Updates node structure by calling the update function on every node and child node of the tree. Also
     * flattens the nodes in a depth descending way suited for fast click detection.
     *
     * @param action - action that was performed to trigger this function.
     */
    void updateInternalTreeStructure(const std::string& action);

    /**
     * @brief Function responsible for calculating, based on parent viewable area, what the viewable area of the
     * children is. This is useful in conjunction with scrollbars and with eliminating from the rendering process
     * elements that are fully not visible.
     *
     */
    void computeViewableArea();

    /**
     * @brief Flattens the nodes in a depth descending way suited for fast click detection.
     * No reason to traverse all the tree nodes from root to leaf nodes and checking if click/hover happened. 99% of the
     * time it's a leaf node we click on and in this project leafs have increasing depth values. By sorting nodes in a
     * vector in a depth descending order, click/hover detection can happen without traversing the tree at all.
     */
    void flattenRoot();
    void recursivelyPushNodes(AbstractComponent* comp);

    renderer::Renderer renderer;

    /* UI State storage */
    UIState state;

    /* These will be deallocated trivially*/
    AbstractComponent* root{nullptr};
    std::vector<AbstractComponent*> flattenedNodes; /* Nodes sorted in depth descending order */
};
} // namespace components