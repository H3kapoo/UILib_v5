#include "TabSwitcher.hpp"

#include "src/Utility.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"

namespace components
{
TabSwitcher::TabSwitcher()
    : AbstractComponent({.type = CompType::TabSwitcher, .shaderPath = "src/assets/shaders/bordered.glsl"})
{
    /* Default startup config */
    layout.orientation = LdOrientation::Vertical;
    layout.fillPolicy = LdFillPolicy::Tightly;

    tabsSelector.style.color = utils::hexToVec4("#041d99ff");
    tabView.style.color = utils::hexToVec4("#c40068ff");

    tabsSelector.style.enableHScroll = false;
    tabsSelector.style.enableVScroll = false;
    tabsSelector.layout.fillPolicy = LdFillPolicy::Grid;
    tabsSelector.layout.grid.config = LdGridConfig{1, 3};
    tabsSelector.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Absolute, 60}};

    tabView.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    tabView.layout.padding = {2, 2, 2, 2};

    AbstractComponent::append(&tabsSelector);
    AbstractComponent::append(&tabView);
}

TabSwitcher::~TabSwitcher()
{
    /* Deallocate tabs related data */
    for (const auto& td : tabData)
    {
        delete td.tabBtn;
    }
}

void TabSwitcher::addTab(const std::string& name, uint16_t pos, AbstractComponent* root)
{
    root->layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};

    tabData.emplace_back();

    auto& it = tabData[tabData.size() - 1];

    /* Fill tab data*/
    it.tabBtn = new Button();
    it.root = root;
    it.name = name;
    it.pos = pos;

    /* Fill default button options */
    it.tabBtn->style.color = utils::hexToVec4("#0db312ff");
    it.tabBtn->layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    it.tabBtn->layout.grid.pos = {0, pos};
    it.tabBtn->layout.margin = {2, 2, 2, 2};
    it.tabBtn->sideImage.path = "/home/hekapoo/newTryAtUI/src/assets/textures/arrow_left.png";

    /* On click, the currently viewed tab will be replaced with the tab root we clicked on. Root swapping kind of.
       Elements of previous root will be uninitialized. Their onStart will be called again. There can only be one root
       at most at any given time. */
    it.tabBtn->addClickListener(
        [this, root](auto, auto, MouseButton btn)
        {
            if (btn == MouseButton::Left)
            {
                if (!root)
                {
                    utils::printlne("Tab switching to new root failed. Null ptr.");
                    return;
                }
                const auto& nodes = tabView.getNodes();
                if (nodes.size() > 0) { tabView.remove(*nodes.begin()); }

                tabView.append(root);
            }
        });

    /* This makes sure the tab button is visible in the header (tab selector) */
    tabsSelector.append(it.tabBtn);

    /* If no tab is currently active (no root), make this first 'root' the active one. */
    if (tabView.getNodes().empty()) { tabView.append(root); }
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
    // utils::printlni("TabSwitcher {} started", getId());
}

bool TabSwitcher::onLayoutUpdate()
{
    /* No overflow will be resolved in this component */
    layoutCalc.calculate();
    return false;
}

} // namespace components