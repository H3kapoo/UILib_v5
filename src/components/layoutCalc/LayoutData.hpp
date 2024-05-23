
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <sys/types.h>

#include "../../Utility.hpp"

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
        Top    = 0b000,
        Left   = 0b000,
        Center = 0b001,
        Bot    = 0b010,
        Right  = 0b010,
        COUNT  = 0b011
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
        float val;
    };

    struct Scaling
    {
        ScalePair horizontal;
        ScalePair vertical;
    };

    struct BorderSize
    {
        uint16_t left{0};
        uint16_t right{0};
        uint16_t top{0};
        uint16_t bottom{0};
    };

    ReloadableValue<Orientation> orientation{Orientation::Horizontal};
    ReloadableValue<FillPolicy> fillPolicy{FillPolicy::Tightly};
    ReloadableValue<Alignment> align{Alignment{Align::Left, Align::Top}};
    ReloadableValue<Align> internalAlign{Align::Top};
    ReloadableValue<Scaling> scaling{{ScalePair{ScalePolicy::Absolute, 10}, ScalePair{ScalePolicy::Absolute, 10}}};
    ReloadableValue<WrapMode> wrap{WrapMode::NoWrap};

    int16_t scrollBarSize{21};

    BorderSize borderSize{0};
    // glm::u16vec4 borderSize{0};
    // glm::u16vec4 marginSize{0};
    // glm::u16vec4 paddingSize{0};

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