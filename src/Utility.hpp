#pragma once

#include <cstdio>
#include <format>
#include <glm/glm.hpp>

// Just for now
#include "/home/hekapoo/Downloads/tracy/public/tracy/Tracy.hpp"

namespace utils
{

#define ENABLE_ALL_LOGGING 1

/**
 * @brief Get the GLFW time since application start. For obvious reasons, we first need to have a valid glfw context.
 *
 * @return double - time in seconds
 */
double getTime();

/**
   @brief Simple functions for outputting to stdout. Later this can be used to output to a specific file for debugging
*/
template <typename... Args> void print(std::format_string<Args...> s, Args&&... args)
{
#if ENABLE_ALL_LOGGING == 1
    printf("%s", std::format(s, std::forward<Args>(args)...).c_str());
#endif
}

template <typename... Args> void println(std::format_string<Args...> s, Args&&... args)
{
#if ENABLE_ALL_LOGGING == 1
    printf("%s\n", std::format(s, std::forward<Args>(args)...).c_str());
#endif
}

template <typename... Args> void printlni(std::format_string<Args...> s, Args&&... args)
{
#if ENABLE_ALL_LOGGING == 1
    printf("\033[38;5;15m[%f][INF] %s\033[0m\n", getTime(), std::format(s, std::forward<Args>(args)...).c_str());
#endif
}

template <typename... Args> void printlnw(std::format_string<Args...> s, Args&&... args)
{
#if ENABLE_ALL_LOGGING == 1
    printf("\033[38;5;159m[%f][WRN] %s\033[0m\n", getTime(), std::format(s, std::forward<Args>(args)...).c_str());
#endif
}

template <typename... Args> void printlne(std::format_string<Args...> s, Args&&... args)
{
#if ENABLE_ALL_LOGGING == 1
    printf("\033[38;5;196m[%f][ERR] %s\033[0m\n", getTime(), std::format(s, std::forward<Args>(args)...).c_str());
#endif
}

inline glm::vec4 hexToVec4(const std::string& hexColor)
{
    /* A bit restrictive but good enough for now */
    if ((hexColor.size() != 7 && hexColor.size() != 9) || hexColor[0] != '#')
    {
        fprintf(stderr, "Invalid hex color format!\n");
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }

    uint32_t r, g, b, a;
    sscanf(hexColor.c_str(), "#%02x%02x%02x%02x", &r, &g, &b, &a);

    // Normalize the color values to the range [0, 1]
    glm::vec4 normalizedColor;
    normalizedColor.r = static_cast<float>(r) / 255.0f;
    normalizedColor.g = static_cast<float>(g) / 255.0f;
    normalizedColor.b = static_cast<float>(b) / 255.0f;
    normalizedColor.a = static_cast<float>(a) / 255.0f;

    return normalizedColor;
}

inline float remap(float value, const float startA, const float endA, const float startB, const float endB)
{
    /*
        [a,b]
        [c,d]
        x from [a,b]
        t1 = (x-a)/(b-a)
        y = (1-t1)*c + t1*d
    */
    if (value > endA) { return endB; }
    if (value < startA) { return startB; }

    const float t = (value - startA) / (endA - startA);
    return (1.0f - t) * startB + t * endB;
}

} // namespace utils

// Not really an utility but we need it outside of a scope for clean access
/* Utility needed so that we know when we refresh components options what options really need reloading and actions to
   be taken. */
template <typename T> struct NewValue
{
    NewValue(const T& _value)
        : value{_value}
    {}

    T value;
    bool isNew{true};
};

// Deduction guide
NewValue(const char*) -> NewValue<const char*>;
template <typename T> NewValue(const T&) -> NewValue<T>;
