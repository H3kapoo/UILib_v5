#include "Renderer.hpp"

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
    shaderLoader.setActiveShaderId(comp.getShaderId());

    shaderLoader.setMatrix4("uProjMatrix", projectionMatrix);
    shaderLoader.setMatrix4("uModelMatrix", comp.getBoxModel().getTransform());

    /* IMPORTANT: Keep in mind that if the value referenced happens to not exist
       anymore, we will get a segfault, so be careful. */
    const auto& uniKeeper = comp.getKeeper();
    for (const auto& uNameValPair : uniKeeper.vec3Ptrs)
    {
        shaderLoader.setVec3f(uNameValPair.first, *uNameValPair.second);
    }

    for (const auto& uNameValPair : uniKeeper.vec4Ptrs)
    {
        shaderLoader.setVec4f(uNameValPair.first, *uNameValPair.second);
    }

    for (const auto& uNameValPair : uniKeeper.intPtrs)
    {
        shaderLoader.setInt(uNameValPair.first, *uNameValPair.second);
    }

    for (const auto& uNameValPair : uniKeeper.texturePtrs)
    {
        glActiveTexture(GL_TEXTURE0);
        shaderLoader.setInt(uNameValPair.first, GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, uNameValPair.second);
    }

    glBindVertexArray(comp.getVaoId());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // unbind texture array
    // glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    /* Notify node and derived class (if impl) that rendering is done. Use it to
       render post-node objects */
    // node.emitEvent(inputHelpers::Event::RenderDone);
}

void Renderer::clearScreen()
{
    glClearColor(.3f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

} // namespace renderer