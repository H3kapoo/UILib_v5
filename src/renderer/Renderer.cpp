#include "Renderer.hpp"

#include "../Utility.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

namespace renderer
{
Renderer::Renderer()
    : shaderLoader{shaderManagement::ShaderLoader::get()}
{
    // vec4s.reserve(batchSize);
    // transformMat4s.reserve(batchSize);
}

void Renderer::setProjectionMatrix(const glm::mat4& projMatrix)
{
    projectionMatrix = projMatrix;
}

// void Renderer::pushToBatch(components::AbstractComponent& comp)
// {
//     ZoneScoped;

//     // utils::printlni("Size! {} {}", glfwGetTime(), transformMat4s.size());
//     if (currentBatchSize == batchSize)
//     // if (transformMat4s.size() == batchSize)
//     {
//         // todo: issue render call
//         batchRenderCall();
//         // transformMat4s.clear();
//         vec4s.clear();
//         currentBatchSize = 0;
//     }
//     batchShaderId = comp.getShaderId();
//     batchVao = comp.getVaoId();
//     comp.onPrepareToRender(vec4s);
//     // transformMat4s.push_back(comp.getBoxModelRead().getTransform());
//     {
//         ZoneNamedN(Zone1, "getTransform", true);
//         transformMat4s[currentBatchSize] = comp.getBoxModelRead().getTransform();
//     }
//     currentBatchSize++;
// }

// void Renderer::beginBatch(components::AbstractComponent& comp)
// {
//     ZoneScoped;
//     batchShaderId = comp.getShaderId();
//     batchVao = comp.getVaoId();
//     shaderLoader.setActiveShaderId(batchShaderId);
//     glBindVertexArray(batchVao);
// }

// void Renderer::endBatch()
// {
//     ZoneScoped;
//     if (currentBatchSize <= 0) { return; }
//     // utils::printlni("Size! {} {}", glfwGetTime(), currentBatchSize);
//     // if (transformMat4s.size() <= 0) { return; }
//     batchRenderCall();
//     // transformMat4s.clear();
//     vec4s.clear();
//     currentBatchSize = 0;
// }

// void Renderer::batchRenderCall()
// {
//     ZoneScoped;
//     /* Set very common uniforms and activate shader id*/
//     shaderLoader.setMatrix4("uProjMatrix", projectionMatrix);
//     shaderLoader.setMatrix4Vec("uModelMatrix", batchSize, glm::value_ptr(transformMat4s[0]));

//     for (const auto& [uniformName, uniformValue] : vec4s)
//     {
//         shaderLoader.setVec4fVec(uniformName.c_str(), batchSize, glm::value_ptr(uniformValue[0]));
//     }

//     glBindVertexArray(batchVao);
//     glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, batchSize);
// }

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
    // transformMat4s.clear();
    // vec4s.clear();
    // currentBatchSize = 0;
    glClearColor(.3f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace renderer