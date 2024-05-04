
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <sys/types.h>

namespace components::layoutcalc
{
/**
 * @brief Each component has data telling the layout calculator how to place the comp and it's children.
 *
 */
struct LayoutData
{
    enum class Orientation : uint8_t
    {
        Horizontal,
        Vertical,
        COUNT
    };

    // clang-format off
    enum class Align : uint8_t
    {
        Top    = 0b00,
        Left   = 0b00,
        Bot    = 0b01,
        Right  = 0b01,
        Center = 0b10
    };
    // clang-format on

    enum class WrapMode
    {
        NoWrap,
        WrapAround
    };

    enum class FillPolicy : uint8_t
    {
        Tightly,
        EvenlySpaced,
        SpaceBetween,
        COUNT
    };

    enum class ScalePolicy : uint8_t
    {
        Absolute,
        Relative
    };

    struct Alignment
    {
        Align horizontal;
        Align vertical;
    };

    struct ScalePair
    {
        ScalePolicy policy;
        float value;
    };

    struct Scaling
    {
        ScalePair horizontal;
        ScalePair vertical;
    };

    Orientation orientation{Orientation::Horizontal};
    FillPolicy fillPolicy{FillPolicy::Tightly};
    Alignment align{Alignment{Align::Left, Align::Top}};
    Align internalAlign{Align::Top};
    Scaling scaling{ScalePair{ScalePolicy::Absolute, 10}, ScalePair{ScalePolicy::Absolute, 10}};
    WrapMode wrap{WrapMode::NoWrap};

    // glm::u16vec4 borderSize{0};
    // glm::u16vec4 marginSize{0};
    // glm::u16vec4 paddingSize{0};

    glm::vec4 borderSize{0};
    glm::vec4 marginSize{0};
    glm::vec4 paddingSize{0};
};

using LdOrientation = LayoutData::Orientation;
using LdFillPolicy = LayoutData::FillPolicy;
using LdAlignment = LayoutData::Alignment;
using LdAlign = LayoutData::Align;
using LdWrapMode = LayoutData::WrapMode;
using LdScaling = LayoutData::Scaling;
using LdScalePolicy = LayoutData::ScalePolicy;
using LdScalePair = LayoutData::ScalePair;

} // namespace components::layoutcalc