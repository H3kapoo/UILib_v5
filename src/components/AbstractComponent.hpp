#pragma once

#include <vector>

#include "../UIState.hpp"
#include "../shaderManagement/ShaderLoader.hpp"
#include "utils/BoxModel.hpp"

namespace components
{
class AbstractComponent
{
    /* Needed so that ComponentManager can access the some functions of AbstractComponent that shall only be used by
     * CM (e.g. setRoot/onEventX)*/
    friend class ComponentManager;

public:
    /* Constructors */
    /**
     * @brief Construct a new AbstractComponent object of type.
     *
     * @param compType - component type. Useful for creating custom components.
     */
    AbstractComponent(const std::string& compType);
    virtual ~AbstractComponent();

    /* Funcs acting on node */
    /**
     * @brief Append single component to this component.
     *
     * @param comp - trivial
     *
     * @return True if comp got appended successfully. False otherwise.
     */
    bool append(AbstractComponent* comp);

    /**
     * @brief Append multiple components to this component.
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
    /* Can be used to modify the global state */
    UIState* getState();
    int getDepth() const;
    int getId() const;
    const std::string& getType() const;
    unsigned int getVaoId() const;
    shaderManagement::ShaderLoader& getShader();
    shaderManagement::shaderId getShaderId() const;
    utils::BoxModel& getBoxModelRW();
    utils::BoxModel& getBoxModelRead();

private:
    /* Internal helpers */
    bool appendAux(AbstractComponent* node);
    /* TODO: If we remove this node, remove the data of all children nodes as well */
    bool removeAux(AbstractComponent* node);
    int generateNextId();
    void showTree(int currentDepth);

    /* Update tree from root down to bottom */
    void updateNodeStructure();

    /* Should only be used for the tree Root node. Accessible only via CM */
    void setState(UIState* newState);

    /* Rendering related */
    utils::BoxModel boxModel;
    unsigned int meshVao{0};
    shaderManagement::shaderIdPtr compShaderPtr{nullptr}; /* Pointer due to hot-reload */
    shaderManagement::ShaderLoader& shaderLoaderRef{shaderManagement::ShaderLoader::get()};

    /* Tree component structure related */
    UIState* state{nullptr};
    int depth{0}; /* 0 depth means it's root node of tree */
    int id{1};
    bool isRuntimeInitialized{false};
    bool isParented{false};
    std::string type{"None"};
    AbstractComponent* parent{nullptr};
    std::vector<AbstractComponent*> children;

protected:
    /* Virtuals called by CM */
    virtual void onClickEvent();
    virtual void onMoveEvent();
    virtual void onPrepareToRender();
    virtual void onRenderDone();
    virtual void onStart();
};
} // namespace components
