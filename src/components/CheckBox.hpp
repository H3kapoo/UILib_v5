#pragma once

#include "src/Utility.hpp"
#include "src/assetLoaders/TextureLoader.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/Button.hpp"
#include "src/components/compUtils/LightWeightDummy.hpp"
#include "src/components/layoutCalc/LayoutCalculator.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"
#include "src/renderer/LightWeightRenderer.hpp"

namespace components
{

using namespace layoutcalc;
using namespace assetloaders;

class CheckBox : public AbstractComponent
{

public:
    /* General style parameters associated with this component in particular */
    struct Style
    {
        glm::vec4 color{0.0f};
        glm::vec4 hoveredColor{0.0f};
        glm::vec4 borderColor{1.0f};
    };

    /* General options for the SideImage that can be present in the separator */
    // struct SideImage
    // {
    //     AssignReloadable<const char*> path{""};
    //     LdAlign align{LdAlign::Left};
    // };

    CheckBox();
    ~CheckBox();

    /* Event listeners */
    void addCheckedListener(std::function<void(bool)> cb);
    // void addClickListener(std::function<void(int, int, MouseButton)> cb);
    // void addMouseMoveListener(std::function<void(int16_t, int16_t)> cb);
    // void addReleaseListener(std::function<void(int, int, MouseButton)> cb);

    Style style;
    std::string text;
    // SideImage sideImage;

    bool checked{false};
    AssignReloadable<bool> disabled{false};

private:
    /**
     * @brief Trigger runtime reloading of associated internal image data.
     *
     */
    // void reloadImage();

    /* User shall not be able to add or remove children to button */
    bool append();
    bool remove();
    bool appendAux();
    bool removeAux();

    bool onLayoutUpdate() override;
    void onClickEvent() override;
    void onMouseEnterEvent() override;
    void onMouseExitEvent() override;
    void onMoveEvent() override;
    void onPrepareToRender() override;
    void onRenderDone() override;
    void onStart() override;

    /* Internal Button and future Label */
    Button btn;
    // Label label;

    /* Internal image related */
    computils::LightWeightDummy imgHolder;
    renderer::LightWeightRenderer lwr;
    TextureLoader::TextureDataPtr textureData{nullptr};
    TextureLoader& textureLoader;

    /* Callback holders */
    // std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
    // std::function<void(int, int, MouseButton)> mouseReleaseCb{nullptr};
    // std::function<void(int16_t, int16_t)> mouseMoveCb{nullptr};
    std::function<void(bool)> checkedCb{nullptr};

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this};
};
} // namespace components