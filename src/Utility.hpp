#pragma once

#include <format>

/**
   @brief Simple functions for outputting to stdout. Later this can be used to output to a specific file for debugging
*/
template <typename... Args> void print(std::format_string<Args...> s, Args&&... args)
{
    printf("%s", std::format(s, std::forward<Args>(args)...).c_str());
}

template <typename... Args> void println(std::format_string<Args...> s, Args&&... args)
{
    printf("%s\n", std::format(s, std::forward<Args>(args)...).c_str());
}

template <typename... Args> void printlni(std::format_string<Args...> s, Args&&... args)
{
    printf("[INF] %s\n", std::format(s, std::forward<Args>(args)...).c_str());
}

template <typename... Args> void printlnw(std::format_string<Args...> s, Args&&... args)
{
    printf("[WRN] %s\n", std::format(s, std::forward<Args>(args)...).c_str());
}

template <typename... Args> void printlne(std::format_string<Args...> s, Args&&... args)
{
    printf("[ERR] %s\n", std::format(s, std::forward<Args>(args)...).c_str());
}