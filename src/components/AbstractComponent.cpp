#include "AbstractComponent.hpp"

#include "src/Utility.hpp"
#include "src/components/layoutCalc/LayoutData.hpp"
#include "src/mesh/QuadMesh.hpp"

namespace components
{

// TODO: In the future, ctor could take a configuration object
AbstractComponent::AbstractComponent(const Options& opts)
{
    id = generateNextId();
    options = opts;
    type = opts.type;
    meshVao = mesh::QuadMesh::get().generateMesh();
    compShaderPtr = shaderLoaderRef.loadFromPath(opts.shaderPath);
    // compShaderPtr = shaderLoaderRef.loadFromPath("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl");
    // compShaderPtr = shaderLoaderRef.loadFromPath("/home/hekapoo/newTryAtUI/src/assets/shaders/baseTextured.glsl");
    // compShaderPtr = shaderLoaderRef.loadFromPath("/home/hekapoo/newTryAtUI/src/assets/shaders/baseInstanced.glsl");
}

AbstractComponent::~AbstractComponent()
{
    // printlni("Virtual Dtor called for {}", getId());
}

bool AbstractComponent::append(AbstractComponent* comp)
{
    if (appendAux(comp))
    {
        triggerTreeChangedAction("Append");
        return true;
    }
    return false;
}

bool AbstractComponent::appendAt(AbstractComponent* comp, const int32_t index)
{
    if (appendAux(comp, index))
    {
        triggerTreeChangedAction("Append");
        return true;
    }
    return false;
}

bool AbstractComponent::append(const std::vector<AbstractComponent*>& comps)
{
    bool didAppendSomething = false;
    for (const auto& comp : comps)
    {
        didAppendSomething = appendAux(comp);
    }
    triggerTreeChangedAction("AppendMany");

    return didAppendSomething;
}

bool AbstractComponent::append(std::vector<AbstractComponent*>&& comps)
{
    bool didAppendSomething = false;
    for (const auto& comp : comps)
    {
        didAppendSomething = appendAux(comp);
    }
    triggerTreeChangedAction("AppendMany");

    return didAppendSomething;
}

bool AbstractComponent::remove(AbstractComponent* comp)
{
    if (removeAux(comp))
    {
        triggerTreeChangedAction("Remove");
        return true;
    }
    return false;
}

bool AbstractComponent::remove(const std::vector<AbstractComponent*>& comps)
{
    bool didRemoveSomething = false;
    for (const auto& comp : comps)
    {
        didRemoveSomething = remove(comp);
    }
    triggerTreeChangedAction("RemoveMany");

    return didRemoveSomething;
}

bool AbstractComponent::remove(std::vector<AbstractComponent*>&& comps)
{
    bool didRemoveSomething = false;
    for (const auto& comp : comps)
    {
        didRemoveSomething = remove(comp);
    }
    triggerTreeChangedAction("RemoveMany");

    return didRemoveSomething;
}

bool AbstractComponent::removeDeep(AbstractComponent* comp)
{
    /* If this node contains the toRm node, remove it, exit*/
    if (remove(comp)) { return true; }

    /* Else recurse my children and see if one of them has the node to remove */
    for (const auto& childComp : children)
    {
        if (childComp->removeDeep(comp)) { return true; }
    }

    return false;
}

void AbstractComponent::refreshLayout()
{
    /* If a node is added into a subtree that is not yet part of the big stateful tree, we will not
       have a "state" object to update layout on, so we just don't do it now. Update for the whole new
       subtree will be done when the root of this subtree is added to the main tree. */
    state ? state->triggerLayoutUpdate() : void();
}

void AbstractComponent::changeShaderTo(const std::string& newShaderPath)
{
    compShaderPtr = shaderLoaderRef.loadFromPath(newShaderPath);
}

void AbstractComponent::showTree()
{
    showTree(0);
}

std::vector<AbstractComponent*>& AbstractComponent::getNodes()
{
    return children;
}

AbstractComponent* AbstractComponent::getParent()
{
    return parent;
}

UIState* AbstractComponent::getState()
{
    return state;
}

int AbstractComponent::getDepth() const
{
    return depth;
}

int AbstractComponent::getId() const
{
    return id;
}

const AbstractComponent::CompType& AbstractComponent::getType() const
{
    return type;
}

unsigned int AbstractComponent::getVaoId() const
{
    return meshVao;
}

shaderManagement::ShaderLoader& AbstractComponent::getShader()
{
    return shaderLoaderRef;
}

shaderManagement::shaderId AbstractComponent::getShaderId() const
{
    return *compShaderPtr;
}

computils::Transform& AbstractComponent::getTransformRW()
{
    /* Note: Marking it dirty because 99% of the time we will modify something on it, so better to mark it dirty as soon
       as we get the object.
       In case it's 100% known we only want to read it and not modify it, explicitly call getBoxModelRead(). */
    transform.markDirty();
    return transform;
}

computils::Transform& AbstractComponent::getTransformRead()
{
    return transform;
}

bool AbstractComponent::isComponentRenderable() const
{
    return isRenderable;
}

bool AbstractComponent::isComponentParented() const
{
    return isParented;
}

bool AbstractComponent::isComponentIgnoringScroll() const
{
    return ignoreScrollEvent;
}

void AbstractComponent::setRenderable(const bool canBeRendered)
{
    isRenderable = canBeRendered;
}

void AbstractComponent::setDetailChildrenOnPrint(const bool detailChildren)
{
    detailMyChildren = detailChildren;
}

void AbstractComponent::setIgnoreScrollEvent(const bool ignore)
{
    ignoreScrollEvent = ignore;
}

bool AbstractComponent::appendAux(AbstractComponent* node, const int32_t index)
{
    if (!node)
    {
        utils::printlne("Tried to append an uninitialized node");
        return false;
    }

    if (node->isParented)
    {
        utils::printlne("Tried to append already parented nId {} to {}", node->getId(), getId());
        return false;
    }

    node->setState(state);
    node->depth = depth + 1;
    node->parent = this;
    node->isParented = true;

    /* Since we are using a vector container, inserting at specific index positions is costly. Use with this in mind.
       For small number of elements it might not matter, but for large lists consider changing container to list. */
    if (index >= 0) { children.insert(children.begin() + index, node); }
    else { children.push_back(node); }

    return true;
}

bool AbstractComponent::removeAux(AbstractComponent* node)
{
    if (children.empty()) { return false; }
    if (!node)
    {
        utils::printlne("Passed nullptr node for removal");
        return false;
    }

    // TODO: on remove X, we shall remove and reset state of all nodes underneath X
    // For now, it's just a shallow remove
    // Later edit: At least set 'isRuntimeInitialized' to false for them.
    const auto it = std::find_if(children.begin(), children.end(),
        [&node](AbstractComponent* n) { return n->getId() == node->getId(); });

    if (it != children.end())
    {
        /* Reset values */
        // (*it)->invalidateSubtree();
        (*it)->isParented = false;
        (*it)->isRuntimeInitialized = false;
        (*it)->parent = nullptr;
        (*it)->depth = 0;
        (*it)->state = nullptr;
        children.erase(it);
        return true;
    }

    return false;
}

void AbstractComponent::invalidateSubtree()
{
    for (const auto& child : children)
    {
        child->isRuntimeInitialized = false;
        child->depth = 0;
        child->state = nullptr;
        child->invalidateSubtree();
    }
}

int AbstractComponent::generateNextId()
{
    static int nextId = 1;
    return nextId++;
}

void AbstractComponent::showTree(int currentDepth)
{
    for (int i = 0; i < currentDepth; i++)
    {
        utils::print("    ");
    }

    utils::print("\\---");
    details();

    /* User doesn't want this node to print data about it's children */
    if (!detailMyChildren) { return; }

    for (const auto& node : children)
    {
        node->showTree(currentDepth + 1);
    }
}

void AbstractComponent::triggerTreeChangedAction(const std::string&& action)
{
    /* If a node is added into a subtree that is not yet part of the big stateful tree, we will not
       have a "state" object to update tree on, so we just don't do it now. Update for the whole new
       subtree will be done when the root of this subtree is added to the main tree. */
    state ? state->triggerTreeUpdate(action) : void();
}

void AbstractComponent::updateNodeStructure()
{
    /* Do stuff necessiting update */
    if (!isRuntimeInitialized)
    {
        /* Init needs to be set first thing to avoid infinite loops in case nodes spawn children inside them */
        isRuntimeInitialized = true;

        onStart();
    };

    /* Update the children aswell */
    for (const auto& node : children)
    {
        /* When tree structure gets modified, update internal state and depth of nodes.
           For now it's a bit redundant to do this because maybe most of the tree already has state/depth
           set correctly. However this is a very fast operation anyway. We can still check that if state is set
           means the node is already ok. */
        if (!node->state)
        {
            utils::printlnw("Node nId {} came from unparented at that point subtree {}!", node->getId(), getId());
            node->setState(state);
            node->depth = getDepth() + 1;
            node->getTransformRW().layer = getDepth();
        }
        node->updateNodeStructure();
    }
}

void AbstractComponent::setState(UIState* newState)
{
    state = newState;
}

void AbstractComponent::manuallyAdjustDepthTo(const int newDepth)
{
    depth = newDepth;
    getTransformRW().layer = getDepth();
}

void AbstractComponent::details()
{
#define WRITE_TYPE(x)                                                                                                  \
    case CompType::x:                                                                                                  \
        stringType = #x;                                                                                               \
        break;

#define WRITE_POLICY(x)                                                                                                \
    case components::layoutcalc::LdFillPolicy::x:                                                                      \
        layoutType = #x;                                                                                               \
        break;

    const auto pid = parent ? std::to_string(parent->getId()) : "N/A";
    const auto dep = depth ? std::to_string(depth) : "Root(0)";
    std::string stringType, layoutType;

    switch (type)
    {
        WRITE_TYPE(Unknown)
        WRITE_TYPE(Div)
        WRITE_TYPE(Button)
        WRITE_TYPE(ScrollBar)
        WRITE_TYPE(TabSwitcher)
        WRITE_TYPE(ResizeDiv)
        WRITE_TYPE(SeparatorBar)
        WRITE_TYPE(CheckBox)
        WRITE_TYPE(List)
        default:
            stringType = "Unknown";
    }

    switch (layout.fillPolicy)
    {
        WRITE_POLICY(Tightly);
        WRITE_POLICY(SpaceBetween);
        WRITE_POLICY(EvenlySpaced);
        WRITE_POLICY(Grid);
        case components::layoutcalc::LdFillPolicy::COUNT:
            layoutType = "Unknown";
    }

    utils::println("{{id: {}, pid={}, depth={}, type: {}, layout: {}}}", id, pid, dep, stringType, layoutType);

#undef WRITE_TYPE
#undef WRITE_POLICY
}

void AbstractComponent::onClickEvent() {}
void AbstractComponent::onMoveEvent() {}
void AbstractComponent::onScroll() {}
void AbstractComponent::onKeyEvent() {}
void AbstractComponent::onMouseEnterEvent() {}
void AbstractComponent::onMouseExitEvent() {}
void AbstractComponent::onPrepareToRender() {}
void AbstractComponent::onRenderDone() {}
void AbstractComponent::onStart() {}
bool AbstractComponent::onLayoutUpdate()
{
    return false;
}
} // namespace components
