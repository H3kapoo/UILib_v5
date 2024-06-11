#pragma once

#include <glm/glm.hpp>
#include <string>

#include "src/components/compUtils/Transform.hpp"
#include "src/mesh/QuadMesh.hpp"

namespace components::computils
{
/**
 * @brief Simple class used to hold a dummy object for non-component quick rendering.
 *
 */
class LightWeightDummy
{
public:
    LightWeightDummy(const std::string& shaderPath)
    {
        options.shaderPath = shaderPath;
        vaoId = mesh::QuadMesh::get().generateMesh();
    }

    struct Options
    {
        glm::vec4 color{1.0f};
        int64_t textureId{0};
        bool isForTextures{false};
        std::string shaderPath;
    };

    unsigned int vaoId{0};
    unsigned int shaderId{0};
    Options options;
    Transform transform;
};
} // namespace components::computils
