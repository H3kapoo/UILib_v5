#include "Utility.hpp"

#include "GLFW/glfw3.h"

namespace utils
{

double getTime()
{
    return glfwGetTime();
}

} // namespace utils