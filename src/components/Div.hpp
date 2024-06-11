#pragma once

#include <functional>

#include "src/assetLoaders/TextureLoader.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/ScrollBar.hpp"
#include "src/components/layoutCalc/LayoutCalculator.hpp"

namespace components
{
using namespace assetloaders;

class Div : public AbstractComponent
{

public:
    /* General style parameters associated with this component in particular */
    struct Style
    {
        glm::vec4 borderColor{0.0f};
        glm::vec4 color{0.0f};

        /* Scrollbar related */
        bool enableHScroll{true};
        bool enableVScroll{true};
        int16_t knobInset{2};
        float scrollSensitivity{10.0f};
    };

    Div();
    ~Div();

    void addClickListener(std::function<void(int, int, MouseButton)>&& cb);
    void addOnEnterListener(std::function<void()>&& cb);
    void addOnExitListener(std::function<void()>&& cb);
    void addOnKeyListener(std::function<void(const HIDAction*)>&& cb);

    Style style;

    /* Background texture path */
    AssignReloadable<const char*> imagePath{""};

private:
    /**
     * @brief Trigger runtime reloading of associated internal image data.
     *
     */
    void reloadImage();

    /* Events */
    bool onLayoutUpdate() override;
    void onClickEvent() override;
    void onKeyEvent() override;
    void onMouseEnterEvent() override;
    void onMouseExitEvent() override;
    void onMoveEvent() override;
    void onPrepareToRender() override;
    void onRenderDone() override;
    void onStart() override;

    /* This Div can display an image. This are helpers for that. */
    TextureLoader::TextureDataPtr textureData{nullptr};
    TextureLoader& textureLoader;

    /* Callbacks related*/
    std::function<void()> mouseEnterCb{nullptr};
    std::function<void()> mouseExitCb{nullptr};
    std::function<void(const HIDAction*)> keyEventCb{nullptr};
    std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};

    /* Note: Pointers to due memory consumption reasons. Create/delete on demand. */
    computils::ScrollBar* hsb{nullptr};
    computils::ScrollBar* vsb{nullptr};

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this};
};
} // namespace components