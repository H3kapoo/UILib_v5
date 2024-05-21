#pragma once

#include "AbstractComponent.hpp"
#include "ScrollBar.hpp"
#include "layoutCalc/LayoutCalculator.hpp"

#include "../assetLoaders/TextureLoader.hpp"
#include <functional>

namespace components
{

using namespace assetloaders;

class Div : public AbstractComponent
{

public:
    struct Style
    {
        glm::vec4 borderColor{0.0f};
        glm::vec4 color{0.0f};
        glm::vec4 hOut{0.0f};

        /* Background texture path */
        // std::optional<const char*> imagePath{""};
        NewValue<const char*> imagePath{""};

        /* Scrollbar related */
        NewValue<bool> enableHScroll{true};
        NewValue<bool> enableVScroll{true};
        int16_t knobInset{2};
        float scrollSensitivity{10.0f};
    };

    Div();
    ~Div();

    void refreshOptions() override;

    void addClickListener(std::function<void(int, int, MouseButton)>&& func);
    void addOnEnterListener(std::function<void()>&& func);
    void addOnExitListener(std::function<void()>&& func);
    void addOnKeyListener(std::function<void(const HIDAction*)>&& func);

    Style style;

private:
    void onPrepareToRender() override;
    void onRenderDone() override;
    void onClickEvent() override;
    void onKeyEvent() override;
    void onMouseEnterEvent() override;
    void onMouseExitEvent() override;
    void onMoveEvent() override;
    void onStart() override;
    bool onLayoutUpdate() override;

    TextureLoader::TextureDataPtr textureData{nullptr};
    // TextureLoader::TextureDataPtr textureData2{nullptr};
    TextureLoader& textureLoader;

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this}; // TODO: Singleton

    /*Note: Pointers to due memory consumption reasons. Create/delete on demand. */
    computils::ScrollBar* hsb{nullptr};
    computils::ScrollBar* vsb{nullptr};

    /* Callbacks related*/
    std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
    std::function<void(const HIDAction*)> keyEventCb{nullptr};
    std::function<void()> mouseEnterCb{nullptr};
    std::function<void()> mouseExitCb{nullptr};
};
} // namespace components