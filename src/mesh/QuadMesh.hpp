#pragma once

#include "IMesh.hpp"

namespace mesh
{
class QuadMesh : public IMesh
{
public:
    virtual ~QuadMesh() = default;
};
} // namespace mesh