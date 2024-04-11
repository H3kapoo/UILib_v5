#pragma once

#include <functional>
#include <string>

typedef int ActiveModifiersBits;

// clang-format off
enum ActiveModifiers
{
    SHIFT     = 0b000001,
    CTRL      = 0b000010,
    ALT       = 0b000100,
    SUPER     = 0b001000,
    CAPS_LOCK = 0b010000,
    NUM_LOCK  = 0b100000
};
// clang-format on

enum KeyAction
{
    Pressed = 0,
    Released,
    Repeated
};

enum MouseButton
{
    None = 0,
    Left,
    Middle,
    Right
};
// TODO: ^^^^ Shall move those away from here ^^^^^^

struct UIState
{
    MouseButton clickedButton{MouseButton::None};
    int mouseX{0};
    int mouseY{0};
    int selectedId{0};

    KeyAction keyAction{KeyAction::Released};
    ActiveModifiersBits activeMods{0};

    int windowWidth{0};
    int windowHeight{0};

    /* Child nodes will call this so that root gets updated */
    std::function<void(const std::string&)> triggerTreeUpdate{nullptr};
    std::string lastActionOnTree{"Nothing"};
};
