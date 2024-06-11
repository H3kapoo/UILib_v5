#pragma once

#include <vector>

#include "src/mesh/IMesh.hpp"

namespace mesh
{
/**
 * @brief Class representing a quad mesh and all it's OpenGL dependencies like vertex data and layout data.
 *
 */
class QuadMesh : public IMesh
{
public:
    static QuadMesh& get();
    virtual ~QuadMesh() = default;
    unsigned int generateMesh() override;
    unsigned int getVaoID() override;

private:
    QuadMesh() = default;
    QuadMesh(const QuadMesh&) = delete;
    QuadMesh& operator=(const QuadMesh&) = delete;

    /* Being static, it will be reused whenever QuadMesh is used */
    static unsigned int vaoID;

    // clang-format off
    /* Quad mesh data */
    std::vector<float> vertices = {
        // POS 3F         TEX 2F
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    /* Index buffer, 2 triangles, 1 quad */
    std::vector<unsigned int> indices = {
        0, 1, 3,
        1, 2, 3
    };

    /* 3 floats for position, 2 floats for texture position */
    std::vector<unsigned int> layout = { 3, 2 };
    // clang-format on
};
} // namespace mesh