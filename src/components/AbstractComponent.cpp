#include "AbstractComponent.hpp"

#include "../Utility.hpp"
#include "../mesh/QuadMesh.hpp"

namespace components
{

// TODO: In the future, ctor could take a configuration object
AbstractComponent::AbstractComponent(const std::string& compType)
{
    id = generateNextId();
    type = compType;
    mesh = new mesh::QuadMesh();
}

AbstractComponent::~AbstractComponent()
{
    delete mesh;
}

bool AbstractComponent::append(AbstractComponent* comp)
{
    if (appendAux(comp))
    {
        state->triggerTreeUpdate("Append");
        return true;
    }
    return false;
}

bool AbstractComponent::append(std::vector<AbstractComponent*>&& comps)
{
    bool didAppendSomething = false;
    for (const auto& comp : comps)
    {
        didAppendSomething = appendAux(comp);
    }
    state->triggerTreeUpdate("AppendMany");

    return didAppendSomething;
}

bool AbstractComponent::remove(AbstractComponent* comp)
{
    if (removeAux(comp))
    {
        state->triggerTreeUpdate("Remove");
        return true;
    }
    return false;
}

bool AbstractComponent::remove(std::vector<AbstractComponent*>&& comps)
{
    bool didRemoveSomething = false;
    for (const auto& comp : comps)
    {
        didRemoveSomething = remove(comp);
    }
    state->triggerTreeUpdate("RemoveMany");

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

void AbstractComponent::details()
{
    const auto pid = parent ? std::to_string(parent->getId()) : "N/A";
    const auto dep = depth ? std::to_string(depth) : "Root(0)";
    println("{{id: {}, pid={}, depth={}, type: {}}}", id, pid, dep, type);
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

const std::string& AbstractComponent::getType() const
{
    return type;
}

bool AbstractComponent::appendAux(AbstractComponent* node)
{
    if (!node)
    {
        println("[ERR] Tried to append an uninitialized node");
        return false;
    }

    if (node->isParented)
    {
        println("[ERR] Tried to append already parented nId {} to {}", node->id, id);
        return false;
    }

    if (!state)
    {
        println("[ERR] Tried to append without state being set in nId: {} (maybe orphan node)", id);
        return false;
    }

    node->setState(state);
    node->parent = this;
    node->depth = depth + 1;
    node->isParented = true;
    children.push_back(node);

    return true;
}

bool AbstractComponent::removeAux(AbstractComponent* node)
{
    if (children.empty()) { return false; }
    if (!node)
    {
        println("[ERR] Passed nullptr node for removal");
        return false;
    }

    // on remove, children shall be accounted for as well (e.g remove parent
    // pointer and level depth)
    const auto it = std::find_if(children.begin(), children.end(),
        [&node](AbstractComponent* n) { return n->getId() == node->getId(); });

    if (it != children.end())
    {
        /* Reset values */
        (*it)->isParented = false;
        (*it)->parent = nullptr;
        (*it)->depth = 0;
        (*it)->state = nullptr;
        children.erase(it);
        return true;
    }

    return false;
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
        print("    ");
    }
    print("\\---");
    details();

    for (const auto& node : children)
    {
        node->showTree(currentDepth + 1);
    }
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
        node->updateNodeStructure();
    }
}

void AbstractComponent::setState(UIState* newState)
{
    state = newState;
}

void AbstractComponent::onClickEvent() {}
void AbstractComponent::onMoveEvent() {}
void AbstractComponent::onStart() {}
} // namespace components
