#pragma once

#include <format>
#include <glm/glm.hpp>

namespace utils
{

#define ENABLE_ALL_LOGGING 1
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
    printf("[INF] %s\n", std::format(s, std::forward<Args>(args)...).c_str());
#endif
}

template <typename... Args> void printlnw(std::format_string<Args...> s, Args&&... args)
{
#if ENABLE_ALL_LOGGING == 1
    printf("[WRN] %s\n", std::format(s, std::forward<Args>(args)...).c_str());
#endif
}

template <typename... Args> void printlne(std::format_string<Args...> s, Args&&... args)
{
#if ENABLE_ALL_LOGGING == 1
    printf("[ERR] %s\n", std::format(s, std::forward<Args>(args)...).c_str());
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
} // namespace utils
