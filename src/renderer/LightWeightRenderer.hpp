#pragma once

#include "src/Utility.hpp"
#include "src/components/compUtils/LightWeightDummy.hpp"
#include "src/shaderManagement/ShaderLoader.hpp"

namespace renderer
{
using namespace components::computils;

/**
 * @brief Simple class for quick render of LIghWeightDummy objects
 *
 */
class LightWeightRenderer
{
public:
    LightWeightRenderer()
        : shaderLoader{shaderManagement::ShaderLoader::get()}
    {}

    void render(const glm::mat4& projMatrix, LightWeightDummy& dummy)
    {
        // TODO: Check for NULL or make loading functions return 0 on error
        if (dummy.shaderId == 0) { dummy.shaderId = *shaderLoader.loadFromPath(dummy.options.shaderPath); }
        shaderLoader.setActiveShaderId(dummy.shaderId);
        shaderLoader.setMatrix4("uProjMatrix", projMatrix);
        shaderLoader.setMatrix4("uModelMatrix", dummy.transform.getTransform());

        // TODO: Quick for testing only. Refactor later.
        if (dummy.options.isForTextures)
        {
            shaderLoader.set2DTextureUnit("uTexture", dummy.options.textureId, GL_TEXTURE0);
        }
        else { shaderLoader.setVec4f("uColor", dummy.options.color); }

        glBindVertexArray(dummy.vaoId);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

private:
    shaderManagement::ShaderLoader& shaderLoader; /* Justified use of ref& */
};
} // namespace renderer