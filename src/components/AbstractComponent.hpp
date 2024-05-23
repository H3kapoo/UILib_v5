#pragma once

#include <glm/fwd.hpp>
#include <string>
#include <vector>

#include "../UIState.hpp"
#include "../shaderManagement/ShaderLoader.hpp"
#include "compUtils/Transform.hpp"
#include "layoutCalc/LayoutData.hpp"

namespace components
{
class AbstractComponent
{
    /* Needed so that ComponentManager can access the some functions of AbstractComponent that shall only be used by
     * CM (e.g. setRoot/onEventX)*/
    friend class ComponentManager;

public:
    enum class CompType
    {
        Unknown,
        Button,
        Div,
        ScrollBar,
    };

    struct Options
    {
        CompType type{CompType::Unknown};
        std::string shaderPath{"SHADER_PATH_NON_EXISTENT"};
    };

    struct ViewableArea
    {
        glm::i16vec2 start;
        glm::i16vec2 scale;
    };

    /* Constructors */
    /**
     * @brief Construct a new AbstractComponent object of type.
     *
     * @param compType - component type. Useful for creating custom components.
     */
    AbstractComponent(const Options& opts);
    virtual ~AbstractComponent();

    /* Funcs acting on node */
    /**
     * @brief Append single component to this component.
     *
     * @note DO NOT APPEND if parent isnt part of the tree yet. This will set depth incorrectly.
     *
     * @param comp - trivial
     *
     * @return True if comp got appended successfully. False otherwise.
     */
    bool append(AbstractComponent* comp);

    /**
     * @brief Append multiple components to this component.
     *
     * @note DO NOT APPEND if parent isnt part of the tree yet. This will set depth incorrectly.
     *
     * @param comps - trivial
     *
     * @return True if at least one comp got appended successfully. False otherwise.
     */
    bool append(const std::vector<AbstractComponent*>& comps);

    /**
     * @brief Append multiple components to this component.
     *
     * @param comps - trivial
     *
     * @return True if at least one comp got appended successfully. False otherwise.
     */
    bool append(std::vector<AbstractComponent*>&& comps);

    /**
     * @brief Remove single component from this component.
     *
     * @param comp - trivial
     *
     * @return True if comp exists and got removed. False otherwise.
     */
    bool remove(AbstractComponent* comp);

    /**
     * @brief Remove multiple components from this component.
     *
     * @param comp - trivial
     *
     * @return True if at least one comp exists and got removed, False otherwise.
     */
    bool remove(const std::vector<AbstractComponent*>& nodes);

    /**
     * @brief Remove multiple components from this component.
     *
     * @param comp - trivial
     *
     * @return True if at least one comp exists and got removed, False otherwise.
     */
    bool remove(std::vector<AbstractComponent*>&& nodes);

    /**
     * @brief Deep remove single component from this component's tree.
     *
     * @param comp - trivial
     *
     * @return True if comp exists and got removed. False otherwise.
     */
    bool removeDeep(AbstractComponent* node);

    /**
     * @brief Append multiple components to this component. This doesn't call layout update. Use with care.
     *
     * @param comps - trivial
     *
     * @return True if at least one comp got appended successfully. False otherwise.
     **/
    bool appendAux(AbstractComponent* node);

    /**
     * @brief Remove single component from this component. This doesn't call layout update. Use with care.
     *
     * @param comp - trivial
     *
     * @return True if comp exists and got removed. False otherwise.
     */
    /* TODO: If we remove this node, remove the data of all children nodes as well */
    bool removeAux(AbstractComponent* node);

    // /**
    //  * @brief Triggers instant recalculation of the layout starting from root.
    //  *
    //  * Function needs to be used in cases when user updates layout settings at runtime in order for those settings to
    //  * get applied right now.
    //  *
    //  */
    // void updateLayout();

    /**
     * @brief Some options need direct user involvement to be refreshed (e.g. texture reloading) and this function
     * provides a way to tell the component to refresh any dependencies it might have.
     *
     * @return True if some update was deduced and needed to be done.
     */
    virtual void refreshOptions();

    /**
     * @brief Show details about this component.
     *
     */
    void details();

    /**
     * @brief Show tree view of this component and it's children.
     *
     */
    void showTree();

    /* Trivial getters */
    std::vector<AbstractComponent*>& getNodes();
    AbstractComponent* getParent();
    int getDepth() const;
    int getId() const;
    const CompType& getType() const;
    unsigned int getVaoId() const;
    shaderManagement::ShaderLoader& getShader();
    shaderManagement::shaderId getShaderId() const;
    computils::Transform& getTransformRW();
    computils::Transform& getTransformRead();
    bool isComponentRenderable() const;
    bool isComponentParented() const;

    /* Trivial setters */
    void setRenderable(const bool canBeRendered);

    /* Direct getters */
    layoutcalc::LayoutData layout;

private:
    /* Internal helpers */
    int generateNextId();
    void showTree(int currentDepth);

    /* Self explanatory */
    void triggerTreeChangedAction(const std::string&& action);

    /* Update tree from root down to bottom */
    void updateNodeStructure();

    /* Should only be used for the tree Root node. Accessible only via CM */
    void setState(UIState* newState);

    /* Ctor options */
    Options options;

    /* Rendering related */
    ViewableArea viewArea;
    bool isRenderable{true};
    unsigned int meshVao{0};
    computils::Transform transform;
    shaderManagement::shaderIdPtr compShaderPtr{nullptr}; /* Pointer due to hot-reload */
    shaderManagement::ShaderLoader& shaderLoaderRef{shaderManagement::ShaderLoader::get()};

    /* Tree component structure related */
    UIState* state{nullptr};
    int depth{1}; /* 1 depth means it's root node of tree */
    int id{1};
    bool isRuntimeInitialized{false};
    bool isParented{false};
    CompType type{CompType::Unknown};
    AbstractComponent* parent{nullptr};
    std::vector<AbstractComponent*> children;

protected:
    /* Can be used to modify the global state */
    UIState* getState();

    void manuallyAdjustDepthTo(const int newDepth);

    /**
     * @brief Change current component shader to another shader from path.
     *
     * @param newShaderPath - trivial
     */
    void changeShaderTo(const std::string& newShaderPath);

    /* Virtuals called by CM */
    virtual void onClickEvent();
    virtual void onMoveEvent();
    virtual void onScroll();
    virtual void onKeyEvent();
    virtual void onMouseEnterEvent();
    virtual void onMouseExitEvent();
    virtual void onPrepareToRender();
    virtual void onRenderDone();
    virtual void onStart();
    virtual bool onLayoutUpdate();
};
} // namespace components
