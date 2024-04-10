#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "src/Utility.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/ComponentManager.hpp"
#include "src/components/Div.hpp"
#include "src/inputManagement/Input.hpp"

using namespace components;
using namespace inputManagement;

void deallocateFast(std::vector<AbstractComponent*>&& obj)
{
    for (auto&& o : obj)
        delete o;
}

int main()
{
    const auto startWindowWidth = 1280;
    const auto startWindowHeight = 720;
    const auto appName = "Another App";

    /* Init glfw */
    if (glfwInit() == GLFW_FALSE)
    {
        println("[ERR] Something happened while trying to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create mock window just to succeed initializing glew*/
    GLFWwindow* window = glfwCreateWindow(startWindowWidth, startWindowHeight, appName, NULL, NULL);
    if (window == NULL)
    {
        println("[ERR] Failed to create glew initializing window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // zero to disable Vsync

    /* Init glew after glfw (we need to have a valid context bound first) */
    if (glewInit() != GLEW_OK)
    {
        println("[ERR] GLEW failed to initialize");
        return -1;
    }

    ComponentManager* cm = new ComponentManager();

    Div* div = new Div();
    Div* div2 = new Div();
    Div* div3 = new Div();
    Div* div4 = new Div();
    Div* div5 = new Div();
    Div* div6 = new Div();

    cm->setRoot(div);

    // div->append({div3, div2});

    // div3->append(div4);
    // div4->append(div5);

    // div->onClickListener(
    //     [&div5, &div6](int x, int y, MouseButton z)
    //     {
    //         // div5->append(div6);
    //         println("EE aaa {} {} {}", x, y, (int)z);
    //     });

    div->showTree();

    inputManagement::InputHelper::get().observe(window);
    inputManagement::InputHelper::get().registerOnMouseButtonAction(
        [&cm](int btn, int act, int)
        {
            MouseButton b = MouseButton::None;
            KeyAction a = KeyAction::Released;
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
                a = KeyAction::Released;
                break;
            case 1:
                a = KeyAction::Pressed;
                break;
            default:
                a = KeyAction::Released;
            }

            // println("Pressed int {} action {} mod {}", btn, act, mod);
            cm->mouseClickEvent(b, a, 0);
        });

    inputManagement::InputHelper::get().registerOnMouseMoveAction(std::bind(&ComponentManager::mouseMoveEvent, cm,
        std::placeholders::_1, std::placeholders::_2));
    inputManagement::InputHelper::get().registerOnWindowResizeAction(std::bind(&ComponentManager::resizeEvent, cm,
        std::placeholders::_1, std::placeholders::_2));

    cm->resizeEvent(startWindowWidth, startWindowHeight);

    printf("Looping..\n");
    glEnable(GL_DEPTH_TEST);

    int frameCount = 0;
    double previousTime = 0;
    while (!glfwWindowShouldClose(window))
    {
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

        cm->render();
        glfwSwapBuffers(window);
        glfwWaitEvents();
        // glfwPollEvents();
    }

    /* Free no longer needed init window. User must make sure now there's a context bound
       by him before using any glew/glfw API calls
    */
    glfwDestroyWindow(window);
    glfwTerminate();

    // deallocateFast(std::move(divs));
    deallocateFast({div, div2, div3, div4, div5, div6});
    delete cm;

    return 0;
}