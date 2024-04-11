#include "Renderer.hpp"

#include "../Utility.hpp"

namespace renderer
{

// Renderer& Renderer::get()
// {
//     static Renderer instance;
//     return instance;
// }

Renderer::Renderer()
    : shaderLoader{shaderManagement::ShaderLoader::get()}
{}

void Renderer::setProjectionMatrix(const glm::mat4& projMatrix)
{
    projectionMatrix = projMatrix;
}

void Renderer::renderComponent(components::AbstractComponent& comp)
{
    /* Set very common uniforms and activate shader id*/
    shaderLoader.setMatrix4("uProjMatrix", projectionMatrix);
    shaderLoader.setMatrix4("uModelMatrix", comp.getBoxModelRead().getTransform());

    const auto compVao = comp.getVaoId();
    if (compVao != boundVao) { glBindVertexArray(compVao); }
    boundVao = compVao;

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // unbind texture array
    // glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Renderer::clearScreen()
{
    glClearColor(.3f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace renderer