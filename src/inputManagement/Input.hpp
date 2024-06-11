#pragma once

#include <functional>

#include <GLFW/glfw3.h>

namespace inputManagement
{
/**
 * @brief Singleton class dealing with basic user input on a given window
 */
class InputHelper
{
public:
    static InputHelper& get();

    /**
     * @brief Observe window and fire events accordingly.
     *
     * It sets internal callback listeners for events such as mouse movement/window resize/etc
     * that appear on the observed window. User has to **register** callbacks for when
     * those events fire.
     *
     * @note Multi window support non existent yet.
     *
     * @param window GLFW window to be observed.
     */
    void observe(GLFWwindow* window);

    /**
     * @brief Register callback to be called on window key action.
     *
     * @param callback Callback to be called.
     */
    void registerOnKeyAction(const std::function<void(int key, int scanCode, int action, int mods)> callback);

    /**
     * @brief Register callback to be called on window resize action.
     *
     * @param callback Callback to be called.
     */
    void registerOnWindowResizeAction(const std::function<void(int width, int height)> callback);

    /**
     * @brief Register callback to be called on mouse button action.
     *
     * @param callback Callback to be called.
     */
    void registerOnMouseButtonAction(const std::function<void(int button, int action, int mods)> callback);

    /**
     * @brief Register callback to be called on mouse move action.
     *
     * @param callback Callback to be called.
     */
    void registerOnMouseMoveAction(const std::function<void(double xPos, double yPos)> callback);

    /**
     * @brief Register callback to be called on mouse drop action.
     *
     * @param callback Callback to be called.
     */
    void registerOnMouseDropAction(const std::function<void(double dropCount, const char** paths)> callback);

    /**
     * @brief Register callback to be called on mouse scroll action.
     *
     * @param callback Callback to be called.
     */
    void registerOnMouseScrollAction(const std::function<void(double xOffset, double yOffset)> callback);

    /**
     * @brief Register callback to be called on window minimized/maximized action.
     *
     * @param callback Callback to be called.
     */
    void registerOnWindowMaximizedAction(const std::function<void(int maximized)> callback);

private:
    InputHelper() = default;
    InputHelper(const InputHelper&) = delete;
    InputHelper& operator=(const InputHelper&) = delete;

    /**
     * @brief Static gateway needed to access key input data from the OS.
     */
    static void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);

    /**
     * @brief Static gateway needed to access window resize data from the OS.
     */
    static void windowResizeCallback(GLFWwindow* window, int width, int height);

    /**
     * @brief Static gateway needed to access window mouse button data from the OS.
     */
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    /**
     * @brief Static gateway needed to access window mouse position data from the OS.
     */
    static void mouseMoveCallback(GLFWwindow* window, double xPos, double yPos);

    /**
     * @brief Static gateway needed to access window mouse dropped items from the OS.
     */
    static void dropCallback(GLFWwindow* window, int dropCount, const char** paths);

    /**
     * @brief Static gateway needed to access window mouse scroll data from the OS.
     */
    static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

    /**
     * @brief Static gateway needed to access window maximized/minimized data from the OS.
     */
    static void windowMaximizedCallback(GLFWwindow* window, int maximied);

    /**
     * @brief Invoke the key action event.
     *
     * This will be called when any key events happen on the observed window.
     *
     * @param key      Pressed key itself.
     * @param scanCode Scan code of the pressed key.
     * @param action   Action such as GLFW_PRESS/RELEASE/REPEAT.
     * @param mods     Mods such as if capslock is pressed, or shift, etc.
     */
    void invokeOnKeyAction(int key, int scanCode, int action, int mods);

    /**
     * @brief Invoke the window resize action event.
     *
     * @param width  New window width.
     * @param height New window height.
     */
    void invokeOnWindowResizeAction(int width, int height);

    /**
     * @brief Invoke the window resize action event.
     *
     * @param width  New window width.
     * @param height New window height.
     */
    void invokeOnMouseButtonAction(int button, int action, int mods);

    /**
     * @brief Invoke the mouse move action event.
     *
     * @param xPos  New cursor x position.
     * @param yPos  New cursor y position.
     */
    void invokeOnMouseMoveAction(double xPos, double yPos);

    /**
     * @brief Invoke the mouse drop action event.
     *
     * @param dropCount  Number of paths dropped.
     * @param paths      Vector of string paths.
     */
    void invokeOnMouseDropAction(int dropCount, const char** paths);

    /**
     * @brief Invoke the mouse scroll action event.
     *
     * @param xOffset  -1 or 1 offset of scroll wheel X.
     * @param yOffset  -1 or 1 offset of scroll wheel Y.
     */
    void invokeOnScrollAction(double xOffset, double yOffset);

    /**
     * @brief Invoke the window minimized/maximized action event.
     *
     * @param maximized  Int bool denoting if window is now maximized or not.
     */
    void invokeOnWindowMaximizedAction(int maximized);

    /* Only one window can be observed at the time for now */
    GLFWwindow* gObservedWindow{nullptr};

    std::function<void(int key, int scanCode, int action, int mods)> gOnKeyActionCallback;
    std::function<void(int width, int height)> gOnWindowResizeActionCallback;
    std::function<void(int button, int action, int mods)> gOnMouseButtonActionCallback;
    std::function<void(double xPos, double yPos)> gOnMouseMoveActionCallback;
    std::function<void(int dropCount, const char** paths)> gOnMouseDropActionCallback;
    std::function<void(double xOffset, double yOffset)> gOnMouseScrollActionCallback;
    std::function<void(int maximized)> gOnWindowMaximizedActionCallback;
};
} // namespace inputManagement