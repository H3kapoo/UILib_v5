#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <unordered_map>

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

    // Experimental attempt at batch rendering. It looked like for simple quads it didnt do anything
    // Better luck with text i hope
    void batchRenderCall();
    void beginBatch(components::AbstractComponent& comp);
    void endBatch();
    void pushToBatch(components::AbstractComponent& comp);

    /**
     * @brief Clear currently bound window depth and color bit.
     *
     */
    void clearScreen();

    Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    std::unordered_map<std::string, std::vector<glm::vec4>> vec4s;
    // std::vector<glm::mat4> transformMat4s;
    glm::mat4 transformMat4s[100];
    int batchShaderId{0};
    int batchVao{0};
    unsigned int batchSize{100};
    unsigned int currentBatchSize{0};

private:
    shaderManagement::ShaderLoader& shaderLoader; /* Justified use of ref& */

    unsigned int boundVao{0};
    glm::mat4 projectionMatrix;
};
} // namespace renderer