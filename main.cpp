#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "App.hpp"
#include "src/UIState.hpp"
#include "src/Utility.hpp"
#include "src/inputManagement/Input.hpp"

int frameCount = 0;
double previousTime = 0;
void frameCounter(GLFWwindow* window)
{
    ZoneScoped;
    double currentTime = glfwGetTime();
    double delta = currentTime - previousTime;
    frameCount++;
    if (delta >= 1.0)
    { // If last cout was more than 1 sec ago

        double fps = double(frameCount) / delta;

        std::string title = "FPS: " + std::to_string(fps);
        glfwSetWindowTitle(window, title.c_str());

        frameCount = 0;
        previousTime = currentTime;
    }
}

int main()
{
    const auto startWindowWidth = 1280;
    const auto startWindowHeight = 720;
    const auto appName = "Another App";

    /* Init glfw */
    if (glfwInit() == GLFW_FALSE)
    {
        utils::printlne("Something happened while trying to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create mock window just to succeed initializing glew*/
    GLFWwindow* window = glfwCreateWindow(startWindowWidth, startWindowHeight, appName, NULL, NULL);
    if (window == NULL)
    {
        utils::printlne("Failed to create glew initializing window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // zero to disable Vsync

    /* Init glew after glfw (we need to have a valid context bound first) */
    if (glewInit() != GLEW_OK)
    {
        utils::printlne("GLEW failed to initialize");
        return -1;
    }

    App app;
    inputManagement::InputHelper::get().observe(window);
    inputManagement::InputHelper::get().registerOnMouseButtonAction(
        [&app](int btn, int act, int)
        {
            MouseButton b = MouseButton::None;
            HIDAction a = HIDAction::Released;
            switch (btn)
            {
                case 0:
                    b = MouseButton::Left;
                    break;
                case 1:
                    b = MouseButton::Right;
                    break;
                case 2:
                    b = MouseButton::Middle;
                    break;
                default:
                    b = MouseButton::None;
            }

            switch (act)
            {
                case 0:
                    a = HIDAction::Released;
                    break;
                case 1:
                    a = HIDAction::Pressed;
                    break;
                default:
                    a = HIDAction::Repeated;
            }
            app.mouseClickEvent(b, a, 0);
        });

    inputManagement::InputHelper::get().registerOnMouseMoveAction(std::bind(&App::mouseMoveEvent, &app,
        std::placeholders::_1, std::placeholders::_2));
    inputManagement::InputHelper::get().registerOnWindowResizeAction(std::bind(&App::resizeEvent, &app,
        std::placeholders::_1, std::placeholders::_2));
    inputManagement::InputHelper::get().registerOnKeyAction(
        [&app](int key, int, int action, int mods)
        {
            HIDAction a = HIDAction::Released;
            switch (action)
            {
                case 0:
                    a = HIDAction::Released;
                    break;
                case 1:
                    a = HIDAction::Pressed;
                    break;
                default:
                    a = HIDAction::Repeated;
            }
            app.keyEvent(key, a, mods);
        });
    app.start(startWindowWidth, startWindowHeight);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window))
    {
        // ZoneScoped;
        frameCounter(window);

        glfwWaitEvents();
        // glfwPollEvents();
        app.update();
        glfwSwapBuffers(window);
    }

    /* Free no longer needed init window. User must make sure now there's a context bound
       by him before using any glew/glfw API calls
    */
    glfwDestroyWindow(window);
    glfwTerminate();

    // deallocateFast({div, div2, div3, div4, div5, div6});

    return 0;
}