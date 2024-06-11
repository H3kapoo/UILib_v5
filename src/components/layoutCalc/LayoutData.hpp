
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
        Grid,
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
        float value{10};
        float min{1.0f};
        // float max{10'000.0f};
    };

    struct Scaling
    {
        ScalePair horizontal;
        ScalePair vertical;
    };

    struct SpacialSize
    {
        uint16_t left{0};
        uint16_t right{0};
        uint16_t top{0};
        uint16_t bottom{0};
    };

    struct GridConfig
    {
        uint16_t rows{1};
        uint16_t cols{1};
    };

    struct GridPos
    {
        uint16_t row{0};
        uint16_t col{0};
    };

    struct GridSpan
    {
        uint16_t rowSpan{1};
        uint16_t colSpan{1};
    };

    struct Grid
    {
        GridConfig config;
        GridPos pos;
        GridSpan span;
    };

    Orientation orientation{Orientation::Horizontal};
    FillPolicy fillPolicy{FillPolicy::Tightly};
    Alignment align{Alignment{Align::Left, Align::Top}};
    Align internalAlign{Align::Top};
    Scaling scaling{ScalePair{ScalePolicy::Absolute, 10}, ScalePair{ScalePolicy::Absolute, 10}};
    WrapMode wrap{WrapMode::NoWrap};
    Grid grid{GridConfig{1, 1}, GridPos{0, 0}, GridSpan{1, 1}};

    SpacialSize border{0};
    SpacialSize margin{0};
    SpacialSize padding{0};

    int16_t scrollBarSize{21};
};

using LdOrientation = LayoutData::Orientation;
using LdGrid = LayoutData::Grid;
using LdGridPos = LayoutData::GridPos;
using LdGridSpan = LayoutData::GridSpan;
using LdGridConfig = LayoutData::GridConfig;
using LdFillPolicy = LayoutData::FillPolicy;
using LdAlignment = LayoutData::Alignment;
using LdAlign = LayoutData::Align;
using LdWrapMode = LayoutData::WrapMode;
using LdScaling = LayoutData::Scaling;
using LdScalePolicy = LayoutData::ScalePolicy;
using LdScalePair = LayoutData::ScalePair;

} // namespace components::layoutcalc