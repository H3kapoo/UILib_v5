#pragma once

#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
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

enum HIDAction
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
    int8_t scrollDirection{0};

    int selectedId{0};
    int prevHoveredId{0}, hoveredId{0};

    HIDAction mouseAction{HIDAction::Released};
    HIDAction keyboardActionState[348]; // GLFW_KEY_LAST
    ActiveModifiersBits activeMods{0};

    int windowWidth{0};
    int windowHeight{0};
    glm::mat4 projectionMatrix{1.0f};

    bool isSomeLayoutDirty{false}; // TODO: Will be used in the future

    /* Child nodes will call this so that root gets updated */
    std::function<void(const std::string&)> triggerTreeUpdate{nullptr};
    std::string lastActionOnTree{"Nothing"};

    /* Child nodes will call this to trigger a layout update at any time*/
    std::function<void()> triggerLayoutUpdate{nullptr};
};
