#include "TabSwitcher.hpp"

#include "../Utility.hpp"
#include "layoutCalc/LayoutData.hpp"

namespace components
{
TabSwitcher::TabSwitcher()
    : AbstractComponent({.type = CompType::TabSwitcher,
          .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/bordered.glsl"})

{
    layout.orientation = LdOrientation::Vertical;
    layout.fillPolicy = LdFillPolicy::Tightly;

    tabsSelector.style.color = utils::hexToVec4("#041d99ff");
    tabView.style.color = utils::hexToVec4("#c40068ff");

    // layout.grid.config = LdGridConfig{2, 3};
    tabsSelector.style.enableHScroll = false;
    tabsSelector.style.enableVScroll = false;
    tabsSelector.layout.fillPolicy = LdFillPolicy::Grid;
    tabsSelector.layout.grid.config = LdGridConfig{1, 3};
    tabsSelector.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Absolute, 60}};

    tabView.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};

    append(&tabsSelector);
    append(&tabView);
}

TabSwitcher::~TabSwitcher()
{
    for (const auto& td : tabData)
    {
        delete td.tabBtn;
    }
}

void TabSwitcher::onPrepareToRender()
{
    auto border = glm::vec4(layout.border.top, layout.border.bottom, layout.border.left, layout.border.right);

    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uInnerColor", style.color);
    getShader().setVec4f("uBorderColor", style.borderColor);
    getShader().setVec4f("uBorderSize", border);
    getShader().setVec2f("uResolution", getTransformRead().scale);
}

void TabSwitcher::onStart()
{
    utils::printlni("TabSwitcher {} started", getId());
}

void TabSwitcher::addTab(const std::string& name, uint16_t pos, Div* root)
{
    root->layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};

    TabData data{new Button(), root, name, pos};
    data.tabBtn->style.color = utils::hexToVec4("#0db312ff");
    data.tabBtn->layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    data.tabBtn->layout.grid.pos = {0, pos};
    data.tabBtn->layout.margin = {2, 2, 2, 2};
    data.tabBtn->sideImage.path = "/home/hekapoo/newTryAtUI/src/assets/textures/arrow_left.png";

    data.tabBtn->addClickListener(
        [this, root](auto, auto, MouseButton btn)
        {
            if (btn == MouseButton::Left)
            {
                const auto& nodes = tabView.getNodes();
                if (nodes.size() > 0)
                {
                    tabView.remove(*nodes.begin());
                    // utils::printlne("Removed previous root");
                }

                tabView.append(root);
            }
        });
    tabsSelector.append(data.tabBtn);
    tabData.push_back(std::move(data));
}

bool TabSwitcher::append(AbstractComponent* comp)
{
    return AbstractComponent::append(comp);
}

// void TabSwitcher::onRenderDone()
// {
// }

// void TabSwitcher::addClickListener(std::function<void(int, int, MouseTabSwitcher)> func)
// {
//     mouseClickCb = func;
// }

// void TabSwitcher::onClickEvent()
// {
//     // println("TabSwitcher element id {} has been clicked!", getId());

//     const auto& s = getState();
//     if (mouseClickCb) mouseClickCb(s->mouseX, s->mouseY, (MouseTabSwitcher)s->clickedTabSwitcher);
// };

// void TabSwitcher::onMouseEnterEvent()
// {
//     utils::printlni("TabSwitcher element id {} has been entered!", getId());
//     style.color = utils::hexToVec4("#606160ff");
//     // const auto& s = getState();
//     // println("For TabSwitcher {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
// }

// void TabSwitcher::onMouseExitEvent()
// {
//     utils::printlni("TabSwitcher element id {} has been exited!", getId());
//     style.color = utils::hexToVec4("#404140ff");
//     // const auto& s = getState();
//     // println("For TabSwitcher {} mouse position is {}-{}", getId(), s->mouseX, s->mouseY);
// }

// void TabSwitcher::onMoveEvent()
// {
//     // getBoxModelRW().pos.z = getDepth(); // TODO: Shall not exist
//     // utils::printlni("[INF] I am node {} and onStart() called", getId());
// }

bool TabSwitcher::onLayoutUpdate()
{
    layoutCalc.calculate();

    return false;
}

} // namespace components