#include "Input.hpp"

#include <assert.h>

namespace inputManagement
{

InputHelper& inputManagement::InputHelper::get()
{
    static InputHelper instance;
    return instance;
}

void InputHelper::observe(GLFWwindow* window)
{
    gObservedWindow = window;

    /* Set key callback */
    glfwSetKeyCallback(window, keyCallback);

    /* Window resize callback */
    glfwSetFramebufferSizeCallback(window, windowResizeCallback);

    /* Window mouse button callback */
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    /* Mouse move callback */
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    /* Mouse dropped items callback */
    glfwSetDropCallback(window, dropCallback);

    /* Mouse scroll callback */
    glfwSetScrollCallback(window, scrollCallback);

    /* Window maximized/minimized callback */
    glfwSetWindowMaximizeCallback(window, windowMaximizedCallback);
}

// ------ STATIC CALLBACKS ------
void InputHelper::keyCallback(GLFWwindow*, int key, int scanCode, int action, int mods)
{
    get().invokeOnKeyAction(key, scanCode, action, mods);
}

void InputHelper::windowResizeCallback(GLFWwindow*, int width, int height)
{
    get().invokeOnWindowResizeAction(width, height);
}

void InputHelper::mouseButtonCallback(GLFWwindow*, int button, int action, int mods)
{
    get().invokeOnMouseButtonAction(button, action, mods);
}

void InputHelper::mouseMoveCallback(GLFWwindow*, double xPos, double yPos)
{
    get().invokeOnMouseMoveAction(xPos, yPos);
}

void InputHelper::dropCallback(GLFWwindow*, int dropCount, const char** paths)
{
    get().invokeOnMouseDropAction(dropCount, paths);
}

void InputHelper::scrollCallback(GLFWwindow*, double xOffset, double yOffset)
{
    get().invokeOnScrollAction(xOffset, yOffset);
}

void InputHelper::windowMaximizedCallback(GLFWwindow*, int maximized)
{
    get().invokeOnWindowMaximizedAction(maximized);
}

// ------ INVOKE ----------
void InputHelper::invokeOnKeyAction(int key, int scanCode, int action, int mods)
{
    if (gOnKeyActionCallback) gOnKeyActionCallback(key, scanCode, action, mods);
}

void InputHelper::invokeOnWindowResizeAction(int width, int height)
{
    if (gOnWindowResizeActionCallback) gOnWindowResizeActionCallback(width, height);
}

void InputHelper::invokeOnMouseButtonAction(int button, int action, int mods)
{
    if (gOnMouseButtonActionCallback) gOnMouseButtonActionCallback(button, action, mods);
}

void InputHelper::invokeOnMouseMoveAction(double xPos, double yPos)
{
    if (gOnMouseMoveActionCallback) gOnMouseMoveActionCallback(xPos, yPos);
}

void InputHelper::invokeOnMouseDropAction(int dropCount, const char** paths)
{
    if (gOnMouseDropActionCallback) gOnMouseDropActionCallback(dropCount, paths);
}

void InputHelper::invokeOnScrollAction(double xOffset, double yOffset)
{
    if (gOnMouseScrollActionCallback) gOnMouseScrollActionCallback(xOffset, yOffset);
}

void InputHelper::invokeOnWindowMaximizedAction(int maximized)
{
    if (gOnWindowMaximizedActionCallback) gOnWindowMaximizedActionCallback(maximized);
}

// ------ REGISTER ----------
void InputHelper::registerOnKeyAction(const std::function<void(int key, int scanCode, int action, int mods)> callback)
{
    gOnKeyActionCallback = callback;
}

void InputHelper::registerOnWindowResizeAction(const std::function<void(int width, int height)> callback)
{
    gOnWindowResizeActionCallback = callback;
}

void InputHelper::registerOnMouseButtonAction(const std::function<void(int button, int action, int mods)> callback)
{
    gOnMouseButtonActionCallback = callback;
}

void InputHelper::registerOnMouseMoveAction(const std::function<void(double xPos, double yPos)> callback)
{
    gOnMouseMoveActionCallback = callback;
}

void InputHelper::registerOnMouseDropAction(const std::function<void(double dropCount, const char** paths)> callback)
{
    gOnMouseDropActionCallback = callback;
}

void InputHelper::registerOnMouseScrollAction(const std::function<void(double xOffset, double yOffset)> callback)
{
    gOnMouseScrollActionCallback = callback;
}

void InputHelper::registerOnWindowMaximizedAction(const std::function<void(int maximized)> callback)
{
    gOnWindowMaximizedActionCallback = callback;
}

} // namespace inputManagement