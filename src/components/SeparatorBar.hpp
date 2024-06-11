#pragma once

#include <cstdint>

#include "src/assetLoaders/TextureLoader.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/compUtils/LightWeightDummy.hpp"
#include "src/components/layoutCalc/LayoutCalculator.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"
#include "src/renderer/LightWeightRenderer.hpp"

namespace components
{

using namespace layoutcalc;
using namespace assetloaders;

class SeparatorBar : public AbstractComponent
{

public:
    /* General style parameters associated with this component in particular */
    struct Style
    {
        glm::vec4 color{0.0f};
        glm::vec4 hoveredColor{0.0f};
        glm::vec4 borderColor{1.0f};
    };

    /* General options for the InternalImage that can be present in the separator */
    struct InternalImage
    {
        AssignReloadable<const char*> path{""};
        LdAlign align{LdAlign::Left};
    };

    SeparatorBar();
    ~SeparatorBar();

    /* General event listeners */
    void addClickListener(std::function<void(int, int, MouseButton)> cb);
    void addMoveClickedListener(std::function<void(int16_t, int16_t)> cb);
    void addMoveListener(std::function<void(int16_t, int16_t)> cb);
    void addOnExitListener(std::function<void()> cb);
    void addReleaseListener(std::function<void(int, int, MouseButton)> cb);

    Style style;
    InternalImage internalImg;

    /* Events this component listens to. Public because they need to be called from ResizeDiv */
    void onClickEvent() override;
    void onMouseEnterEvent() override;
    void onMouseExitEvent() override;
    void onMoveEvent() override;

private:
    /**
     * @brief Trigger runtime reloading of associated internal image data.
     *
     */
    void reloadImage();

    /* User shall not be able to add or remove children to button */
    bool append();
    bool remove();
    bool appendAux();
    bool removeAux();

    /* Other events but these don't need to be public */
    bool onLayoutUpdate() override;
    void onPrepareToRender() override;
    void onRenderDone() override;
    void onStart() override;

    /* Internal image related */
    computils::LightWeightDummy imgHolder;
    renderer::LightWeightRenderer lwr;
    TextureLoader::TextureDataPtr textureData{nullptr};
    TextureLoader& textureLoader;

    /* Callback holders */
    std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
    std::function<void(int, int, MouseButton)> mouseReleaseCb{nullptr};
    std::function<void(int16_t, int16_t)> mouseMoveClickedCb{nullptr};
    std::function<void(int16_t, int16_t)> mouseMoveCb{nullptr};
    std::function<void()> mouseExitCb{nullptr};

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this};
};
} // namespace components