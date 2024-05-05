#pragma once

#include "AbstractComponent.hpp"
#include "compUtils/LightWeightDummy.hpp"
#include "layoutCalc/LayoutCalculator.hpp"

#include "../assetLoaders/TextureLoader.hpp"
#include "../renderer/LightWeightRenderer.hpp"

namespace components
{

using namespace assetloaders;

class Div : public AbstractComponent
{

public:
    struct Style
    {
        // struct BorderColor
        // {
        //     glm::vec4 left{0};
        //     glm::vec4 right{0};
        //     glm::vec4 top{0};
        //     glm::vec4 bottom{0};
        // };

        glm::vec4 borderColor;
        glm::vec4 color{0.0f};
        glm::vec4 hOut{0.0f};
    };

    Div();
    ~Div();

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
    void onLayoutUpdate() override;

    TextureLoader::TextureDataPtr textureData{nullptr};
    TextureLoader::TextureDataPtr textureData2{nullptr};
    TextureLoader& textureLoader;

    // computils::LightWeightDummy dummy;
    // renderer::LightWeightRenderer lwr;

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this}; // TODO: Singleton
    float overflow{0};

    /* Callbacks related*/
    std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
    std::function<void(const HIDAction*)> keyEventCb{nullptr};
    std::function<void()> mouseEnterCb{nullptr};
    std::function<void()> mouseExitCb{nullptr};
};
} // namespace components