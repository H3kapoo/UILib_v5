#pragma once

#include <format>

template <typename... Args> void print(std::format_string<Args...> s, Args&&... args)
{
    printf("%s", std::format(s, std::forward<Args>(args)...).c_str());
}

template <typename... Args> void println(std::format_string<Args...> s, Args&&... args)
{
    printf("%s\n", std::format(s, std::forward<Args>(args)...).c_str());
}
