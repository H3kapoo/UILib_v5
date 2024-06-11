#pragma once

#include <cstdint>

#include "src/assetLoaders/TextureLoader.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/Button.hpp"
#include "src/components/Div.hpp"
#include "src/components/compUtils/LightWeightDummy.hpp"
#include "src/components/layoutCalc/LayoutCalculator.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"
#include "src/renderer/LightWeightRenderer.hpp"

namespace components
{

using namespace layoutcalc;
using namespace assetloaders;

class TabSwitcher : public AbstractComponent
{
public:
    /* General style parameters associated with this component in particular */
    struct Style
    {
        glm::vec4 color{0.0f};
        glm::vec4 hoveredColor{0.0f};
        glm::vec4 borderColor{1.0f};
    };

    TabSwitcher();
    ~TabSwitcher();

    void addTab(const std::string& name, uint16_t pos, AbstractComponent* root);

    Style style;

private:
    /* General tab related infomation */
    struct TabData
    {
        Button* tabBtn;
        AbstractComponent* root{nullptr};
        std::string name{"?"};
        uint16_t pos{0};
    };

    std::vector<TabData> tabData;
    Div tabsSelector;      /* Header*/
    Div tabView;           /* Body */
    Button* currentTabBtn; /* Currently selected Tab button */

    /* User shall not be able to add or remove children to TabSwitcher */
    bool append(AbstractComponent* comp);
    bool remove();
    bool appendAux();
    bool removeAux();

    /* Events */
    void onPrepareToRender() override;
    void onStart() override;
    bool onLayoutUpdate() override;

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this};
};
} // namespace components