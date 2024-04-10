#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../components/AbstractComponent.hpp"
#include "../shaderManagement/ShaderLoader.hpp"

namespace renderer
{

/**
 * @brief Class for rendering basic meshes.
 *
 */
class Renderer
{
public:
    static constexpr float MAX_LAYERS{-100};

    /**
     * @brief Sets the projection matrix that will be used on next draw call
     *
     * @param projMatrix Projection Matrix to be used.
     */
    void setProjectionMatrix(const glm::mat4& projMatrix);

    /**
     * @brief Render node of type **components::AbstractComponent** .
     *
     * Quickly render **meshHelpers::RectNodeABC** type. This enables it's
     * stored shader to be the active one while also uploads uniforms such as
     * projection/model matrix and scale of the object to the shader.
     *
     * @param comp - Rectangle component to be rendered.
     */
    void renderComponent(components::AbstractComponent& comp);

    /**
     * @brief Clear currently bound window depth and color bit.
     *
     */
    void clearScreen();

    Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

private:
    shaderManagement::ShaderLoader& shaderLoader; /* Justified use of ref& */

    glm::mat4 projectionMatrix;
};
} // namespace renderer