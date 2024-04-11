#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace components::utils
{
class BoxModel
{
public:
    glm::mat4 getTransform()
    {
        if (!isDirty) { return modelMat; }

        /* (1) and (2) are for rotation so that we rotate about the center of the quad*/
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, pos);
        modelMat = glm::translate(modelMat, glm::vec3(scale.x * 0.5f, scale.y * 0.5f, 0)); // (1)
        modelMat = glm::rotate(modelMat, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMat = glm::translate(modelMat, glm::vec3(-scale.x * 0.5f, -scale.y * 0.5f, 0)); // (2)
        modelMat = glm::scale(modelMat, scale);
        isDirty = false;
        return modelMat;
    }

    void markDirty()
    {
        isDirty = true;
    }

    void markClean()
    {
        isDirty = false;
    }

    glm::vec3 pos{0, 0, 1};
    glm::vec3 scale{10, 10, 1};
    glm::vec3 rot{0};

private:
    bool isDirty{true};
    glm::mat4 modelMat;
};
} // namespace components::utils