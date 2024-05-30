#pragma once

#include "../assetLoaders/TextureLoader.hpp"
#include "../renderer/LightWeightRenderer.hpp"
#include "AbstractComponent.hpp"
#include "Button.hpp"
#include "Div.hpp"
#include "compUtils/LightWeightDummy.hpp"
#include "layoutCalc/LayoutCalculator.hpp"
#include "layoutCalc/LayoutData.hpp"
#include <cstdint>

namespace components
{

using namespace layoutcalc;
using namespace assetloaders;
class TabSwitcher : public AbstractComponent
{

public:
    struct Style
    {
        glm::vec4 color{0.0f};
        glm::vec4 hoveredColor{0.0f};
        glm::vec4 borderColor{1.0f};
    };

    struct TabData
    {
        Button* tabBtn;
        AbstractComponent* root{nullptr};
        std::string name{"?"};
        uint16_t pos{0};
    };

    TabSwitcher();
    ~TabSwitcher();

    void addTab(const std::string& name, uint16_t pos, AbstractComponent* root);

    Style style;

private:
    std::vector<TabData> tabData;
    Div tabsSelector;
    Div tabView;
    Button* currentTabBtn;
    // std::array<TabData, 4> tabData;

    /* User shall not be able to add or remove children to TabSwitcher */
    bool append(AbstractComponent* comp);
    // bool remove();
    // bool appendAux();
    // bool removeAux();

    void onPrepareToRender() override;
    // void onRenderDone() override;
    // void onClickEvent() override;
    // void onMoveEvent() override;
    // void onMouseExitEvent() override;
    // void onMouseEnterEvent() override;
    void onStart() override;
    bool onLayoutUpdate() override;

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this};
};
} // namespace components