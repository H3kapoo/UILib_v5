
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <sys/types.h>

namespace components::layoutcalc
{

/**
 * @brief Stateless class responsible for positioning elements with respect to their parent and user supplied options
 *
 */
struct LayoutData
{
    enum class Orientation : uint8_t
    {
        Horizontal,
        Vartical
    };

    enum class ChildPolicy : uint8_t
    {
        Tight,
        TightReverse,
        TightCenter,
        EvenlySpaced
    };

    enum class SizingPolicy : uint8_t
    {
        Absolute,
        Relative,
        Fill
    };

    Orientation orientation{Orientation::Horizontal};
    ChildPolicy childPolicy{ChildPolicy::Tight};
    SizingPolicy sizingPolicy{SizingPolicy::Absolute};

    bool childWrap{false};

    glm::vec4 borderSize{0};
    glm::vec4 marginSize{0};
    glm::vec4 paddingSize{0};
    glm::vec2 size{10, 10};
};

} // namespace components::layoutcalc