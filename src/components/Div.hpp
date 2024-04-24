#pragma once

#include "AbstractComponent.hpp"
#include "layoutCalc/LayoutCalculator.hpp"

#include "../assetLoaders/TextureLoader.hpp"

namespace components
{

using namespace assetloaders;

class Div : public AbstractComponent
{

public:
    struct Style
    {
        glm::vec4 color{0.0f};
        glm::vec4 borderColor{0.0f};
        glm::vec4 hOut{0.0f};
    };

    Div();
    ~Div();

    void addClickListener(std::function<void(int, int, MouseButton)>&& func);
    void addOnEnterListener(std::function<void()>&& func);
    void addOnExitListener(std::function<void()>&& func);

    Style style;

private:
    void onPrepareToRender() override;
    void onClickEvent() override;
    void onMouseEnterEvent() override;
    void onMouseExitEvent() override;
    void onMoveEvent() override;
    void onStart() override;
    void onLayoutUpdate() override;

    TextureLoader::TextureDataPtr textureData{nullptr};
    TextureLoader::TextureDataPtr textureData2{nullptr};
    TextureLoader& textureLoader;

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this}; // TODO: Singleton

    /* Callbacks related*/
    std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
    std::function<void()> mouseEnterCb{nullptr};
    std::function<void()> mouseExitCb{nullptr};
};
} // namespace components