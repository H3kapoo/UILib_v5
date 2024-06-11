#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <unordered_map>

#include "src/components/AbstractComponent.hpp"
#include "src/shaderManagement/ShaderLoader.hpp"

namespace renderer
{

/**
 * @brief Class for rendering basic meshes.
 *
 */
class Renderer
{
public:
    static constexpr float MAX_LAYERS{-1000};

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

    // // Experimental attempt at batch rendering. It looked like for simple quads it didnt do anything
    // // Better luck with text i hope
    // void batchRenderCall();
    // void beginBatch(components::AbstractComponent& comp);
    // void endBatch();
    // void pushToBatch(components::AbstractComponent& comp);

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

    unsigned int boundVao{0};
    glm::mat4 projectionMatrix;
};
} // namespace renderer