#include "List.hpp"

#include "src/UIState.hpp"
#include "src/Utility.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/Button.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"
#include <cstdint>
#include <string>
#include <utility>

namespace components
{
List::List()
    : AbstractComponent({.type = CompType::List, .shaderPath = "src/assets/shaders/bordered.glsl"})
    , imgHolder("src/assets/shaders/baseTextured.glsl")
    , textureLoader(TextureLoader::get())

{
    /* Don't show children of this component. Set to True if you want to see them displayed in the prints. */
    // setDetailChildrenOnPrint(false);

    /* This acts just as a holder. Don't render */
    // setRenderable(false);

    // holder.layout.scaling = LdScaling{{LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Relative, 1.0f}};
    style.color = utils::hexToVec4("#919293ff");
    layout.orientation = LdOrientation::Vertical;
    layout.padding = {0, 0, 0, 5};
    layout.scrollBarSize = 11;

    setDetailChildrenOnPrint(false);

    /* Append the holder */
    // AbstractComponent::append(&holder);

    const int entriesToSpawn = 40;
    for (int i = 0; i < entriesToSpawn; i++)
    {
        entries.emplace_back("Entry" + std::to_string(i), i);
    }
}

List::~List()
{
    /* Deallocate scrollbars if needed */
    if (vsb) { delete vsb; }
}

bool List::maxEntriesReached()
{
    const float entryTotalSize = style.entrySize + style.entrySpacing;
    const int currentEntriesFill = getNodes().size() * entryTotalSize;
    const int maxEntriesFill = entries.size() * entryTotalSize;

    return currentEntriesFill >= maxEntriesFill;
}

bool List::appendOrRemove()
{
    // This reacts only on parent size changed. Keep the in memory entries to a low.

    static int entryId = 0;
    const auto& allNodes = getNodes();

    const float entryTotalSize = style.entrySize + style.entrySpacing;
    const float parentScale = getTransformRead().scale.y - 5;
    const int32_t noEntries = std::round(parentScale / entryTotalSize);
    const int32_t idealNoOfEntries = noEntries + 1 + (vsb ? 1 : 0);

    // utils::printlni("Ideal nr of entries: {}", idealNoOfEntries);

    bool needsUpdate = false;
    if ((int32_t)allNodes.size() != idealNoOfEntries)
    {
        const int32_t diff = allNodes.size() - idealNoOfEntries;
        // utils::printlni("Away from ideal: {}", diff);
        // too few elements, add more
        if (diff < 0)
        {
            for (int i = 0; i < std::abs(diff); i++)
            {
                // Stop adding if there's nothing to add anymore
                // if (maxEntriesReached()) { return false; }

                auto entry = new Button();
                entry->layout.scaling = LdScaling{
                    {LdScalePolicy::Relative, 1.0f}, {LdScalePolicy::Absolute, (float)style.entrySize}};
                entry->layout.margin = {5, 5, (uint16_t)style.entrySpacing, 0};
                entry->style.color = ++entryId % 2 == 0 ? utils::hexToVec4("#383838ff") : utils::hexToVec4("#595959ff");

                if (entryId == 20) entry->style.color = utils::hexToVec4("#be0c0cff");

                entry->setIgnoreScrollEvent(true);
                entry->addClickListener(
                    [i](int, int, MouseButton mBtn)
                    {
                        if (mBtn != MouseButton::Left) { return; }

                        utils::printlne("Clicked on entry id {}", i);
                        // Empty
                    });

                AbstractComponent::appendAux(entry);
                // AbstractComponent::append(entry);
                needsUpdate = true;
            }
        }
        // too many elements, remove some
        else
        {
            for (int i = 0; i < diff; i++)
            {
                auto lastNodeIt = allNodes.end() - 1;
                if ((*lastNodeIt)->getType() == AbstractComponent::CompType::ScrollBar)
                {
                    lastNodeIt = allNodes.end() - 2;
                }
                AbstractComponent::removeAux(*lastNodeIt);
                needsUpdate = true;
            }
        }
    }

    const int currentEntriesFill = getNodes().size() * entryTotalSize;
    const int maxEntriesFill = entries.size() * entryTotalSize;
    maxAllowedScrollOffset = maxEntriesFill - currentEntriesFill;
    // utils::printlnw("Max allowed scroll offset from top: {}", maxAllowedScrollOffset);

    return needsUpdate;
}

void List::onPrepareToRender()
{
    auto border = glm::vec4(layout.border.top, layout.border.bottom, layout.border.left, layout.border.right);

    getShader().setActiveShaderId(getShaderId());
    getShader().setVec4f("uInnerColor", style.color);
    getShader().setVec4f("uBorderColor", style.borderColor);
    getShader().setVec4f("uBorderSize", border);
    getShader().setVec2f("uResolution", getTransformRead().scale);
}

void List::onStart() {}

void List::recycleLastEntry(const float push)
{
    // return;
    const auto& allNodes = getNodes();

    const auto delta = vsb ? vsb->getDeltaScrollValue() : 0;
    utils::printlne("delta {}", delta);
    scrollOffset += delta;

    const float entryTopMargin = style.entrySpacing;

    for (auto& child : allNodes)
    {
        if (child->getType() == AbstractComponent::CompType::ScrollBar) { continue; }
        const auto childExitTop = (child->getTransformRead().pos.y + child->getTransformRead().scale.y -
                                      entryTopMargin) < getTransformRead().pos.y;
        const auto childExitBot = child->getTransformRead().pos.y + entryTopMargin >
                                  getTransformRead().pos.y + getTransformRead().scale.y;
        if (childExitBot)
        {
            AbstractComponent* lastNode = allNodes.back();
            AbstractComponent::removeAux(lastNode);
            AbstractComponent::appendAux(lastNode, 0);
            scrollOffset += style.entrySize + style.entrySpacing; // put back entry size + its margin
            break;
        }
        else if (childExitTop)
        {
            AbstractComponent* firstNode = allNodes.front();
            AbstractComponent::removeAux(firstNode);
            AbstractComponent::appendAux(firstNode);
            scrollOffset -= style.entrySize + style.entrySpacing; // put back entry size + its margin
            break;
        }
    }
}

void List::onScroll()
{
    // const auto delta = vsb ? vsb->getDeltaScrollValue() : 0;
    // utils::printlne("delta {}", delta);
    // float prev = scrollOffset;
    // scrollOffset += delta;
}

void List::onRenderDone() {}

void List::onClickEvent()
{

    // const auto& s = getState();
    // if (s->mouseAction == HIDAction::Pressed)
    // {
    //     if (mouseClickCb) mouseClickCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
    // }
    // else if (s->mouseAction == HIDAction::Released)
    // {
    //     if (mouseReleaseCb) mouseReleaseCb(s->mouseX, s->mouseY, (MouseButton)s->clickedButton);
    // }
}

void List::onMouseEnterEvent() {}

void List::onMouseExitEvent() {}

void List::onMoveEvent() {}

bool List::onLayoutUpdate()
{
    bool needsUpdate = false;
    bool appended = false;
    // bool appended = appendOrRemove();
    // recycleLastEntry(0);

    const bool isVBarActive = vsb ? true : false;
    layoutCalc.calculate(0, scrollOffset, false, isVBarActive);

    // recycleLastEntry(vsb->getScrollValue());
    if (vsb == nullptr)
    {
        vsb = new computils::ScrollBar();
        vsb->setActive();

        AbstractComponent::appendAux(vsb);
        needsUpdate = true;
    }

    if (vsb)
    {
        vsb->options.barSize = layout.scrollBarSize;
        vsb->options.orientation = layoutcalc::LdOrientation::Vertical;
    }

    if (vsb) vsb->updateOverflow(100);

    utils::printlne("Delta {}", vsb->getDeltaScrollValue());
    return needsUpdate || appended;
    // return false;
}

} // namespace components