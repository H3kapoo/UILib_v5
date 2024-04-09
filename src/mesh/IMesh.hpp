#pragma once

namespace mesh
{
/**
 * @brief Trivial interface for any type of mesh.
 *
 */
class IMesh
{
public:
    /**
     * @brief Generates mesh based on provided data by derived class and returns the VAO id generated
     *
     * @return unsigned int - generated VAO.
     */
    virtual unsigned int generateMesh() = 0;

    /**
     * @brief Get the Vao id
     *
     * @return unsigned int - vao id
     */
    virtual unsigned int getVaoID() = 0;
};
} // namespace mesh