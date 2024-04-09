#include "QuadMesh.hpp"

#include <GL/glew.h>
#include <numeric>

namespace mesh
{
unsigned int QuadMesh::vaoID = 0;

QuadMesh& QuadMesh::get()
{
    static QuadMesh instance;
    return instance;
}

unsigned int QuadMesh::generateMesh()
{
    if (vaoID) { return vaoID; }

    unsigned int vboId, eboId;
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboId);
    glGenBuffers(1, &eboId);

    glBindVertexArray(vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);

    /* Buffer data about the vertices to the GPU */
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    /* Buffer data about the indices to the GPU */
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), &indices[0], GL_STATIC_DRAW);

    uint32_t totalVertexLen = std::accumulate(layout.begin(), layout.end(), 0);
    int32_t currentOffset = 0;
    for (uint32_t i = 0; i < layout.size(); i++)
    {
        glVertexAttribPointer(i,                     // attribId
            layout[i],                               // attrLenBytes
            GL_FLOAT,                                // attrType
            GL_FALSE,                                // bNorm
            totalVertexLen * sizeof(float),          // vertexLen
            reinterpret_cast<void*>(currentOffset)); // vertexAttrOffset
        glEnableVertexAttribArray(i);
        currentOffset += layout[i] * sizeof(float);
    }

    return vaoID;
}

unsigned int QuadMesh::getVaoID()
{
    return vaoID;
}

} // namespace mesh