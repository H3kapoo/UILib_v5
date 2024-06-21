#pragma once

#include "src/Utility.hpp"
#include "src/assetLoaders/TextureLoader.hpp"
#include "src/components/AbstractComponent.hpp"
#include "src/components/Button.hpp"
#include "src/components/Div.hpp"
#include "src/components/compUtils/LightWeightDummy.hpp"
#include "src/components/layoutCalc/LayoutCalculator.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"
#include "src/renderer/LightWeightRenderer.hpp"
#include <cstdint>

namespace components
{

using namespace layoutcalc;
using namespace assetloaders;

class List : public AbstractComponent
{

public:
    /* General style parameters associated with this component in particular */
    struct Style
    {
        glm::vec4 color{0.0f};
        glm::vec4 hoveredColor{0.0f};
        glm::vec4 borderColor{1.0f};
        int32_t entrySize{15};
        int32_t entrySpacing{5};
    };

    struct Entry
    {
        std::string entry;
        uint32_t id{0};
        // uint32_t referencedLabelId{0};
    };

    List();
    ~List();

    /* Event listeners */
    // void addCheckedListener(std::function<void(bool)> cb);

    Style style;

    bool checked{false};
    AssignReloadable<bool> disabled{false};

private:
    bool maxEntriesReached();

    bool appendOrRemove();
    void recycleLastEntry(const float push);

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
    void onScroll() override;

    /*
        Allocate at most the max number of entries the list permits vertically to be kept in memory
            -> screenHeight/entrySizeY => max nr of Labels to allocate

        Practice view recyclying:
        -- Ent 1  --
        -- Ent 2  --
        -- Ent 3  --
        -- Ent 4  --
        - View End -
        -- Ent 5  --  <- not shown, should be recycled
    */

    /* Holder */
    std::vector<Entry> entries;

    computils::ScrollBar* vsb{nullptr};

    const float sensitivity{2.0f};

    float currentScrollDownOffset{0.0f};
    float maxAllowedScrollOffset{0.0f};
    float scrollOffset{0.0f};
    uint32_t maxEntries{40};

    /* Internal image related */
    computils::LightWeightDummy imgHolder;
    renderer::LightWeightRenderer lwr;
    TextureLoader::TextureDataPtr textureData{nullptr};
    TextureLoader& textureLoader;

    /* Callback holders */
    std::function<void(bool)> checkedCb{nullptr};

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this};
};
} // namespace components