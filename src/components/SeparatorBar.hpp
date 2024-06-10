#pragma once

#include "../assetLoaders/TextureLoader.hpp"
#include "../renderer/LightWeightRenderer.hpp"
#include "AbstractComponent.hpp"
#include "compUtils/LightWeightDummy.hpp"
#include "layoutCalc/LayoutCalculator.hpp"
#include "layoutCalc/LayoutData.hpp"
#include <cstdint>

namespace components
{

using namespace layoutcalc;
using namespace assetloaders;
class SeparatorBar : public AbstractComponent
{

public:
    struct Style
    {
        glm::vec4 color{0.0f};
        glm::vec4 hoveredColor{0.0f};
        glm::vec4 borderColor{1.0f};
    };

    struct SideImage
    {
        AssignReloadable<const char*> path{""};
        LdAlign align{LdAlign::Left};
    };

    SeparatorBar();
    ~SeparatorBar();

    void addClickListener(std::function<void(int, int, MouseButton)> func);
    void addReleaseListener(std::function<void(int, int, MouseButton)> func);
    void addMoveClickedListener(std::function<void(int16_t, int16_t)> func);
    void addMoveListener(std::function<void(int16_t, int16_t)> func);
    void addOnExitListener(std::function<void()> func);

    Style style;
    std::string text;
    SideImage sideImage;

    // Direct invoke, rm later
    std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
    std::function<void(int, int, MouseButton)> mouseReleaseCb{nullptr};
    std::function<void(int16_t, int16_t)> mouseMoveClickedCb{nullptr};
    std::function<void(int16_t, int16_t)> mouseMoveCb{nullptr};
    std::function<void()> mouseExitCb{nullptr};

    void onClickEvent() override;
    void onMoveEvent() override;
    void onMouseExitEvent() override;
    void onMouseEnterEvent() override;

private:
    void reloadImage();

    /* User shall not be able to add or remove children to button */
    bool append();
    bool remove();
    bool appendAux();
    bool removeAux();

    void onPrepareToRender() override;
    void onRenderDone() override;
    // void onClickEvent() override;
    // void onMoveEvent() override;
    // void onMouseExitEvent() override;
    // void onMouseEnterEvent() override;
    void onStart() override;
    bool onLayoutUpdate() override;

    computils::LightWeightDummy imageDummy;
    renderer::LightWeightRenderer lwr;

    TextureLoader::TextureDataPtr textureData{nullptr};
    TextureLoader& textureLoader;

    SeparatorBar* sideBar{nullptr};

    // std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
    // std::function<void(int, int, MouseButton)> mouseReleaseCb{nullptr};
    // std::function<void(int16_t, int16_t)> mouseMoveCb{nullptr};

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this};
};
} // namespace components