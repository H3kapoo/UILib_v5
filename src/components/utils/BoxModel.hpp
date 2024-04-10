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
        /* (1) and (2) are for rotation so that we rotate about the center of the quad*/
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, pos);
        modelMat = glm::translate(modelMat, glm::vec3(scale.x * 0.5f, scale.y * 0.5f, 0)); // (1)
        modelMat = glm::rotate(modelMat, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMat = glm::translate(modelMat, glm::vec3(-scale.x * 0.5f, -scale.y * 0.5f, 0)); // (2)
        modelMat = glm::scale(modelMat, scale);
        return modelMat;
    }

    glm::vec3 pos{0, 0, 1};
    glm::vec3 scale{50, 100, 1};
    glm::vec3 rot{0};

private:
    glm::mat4 modelMat;
};
} // namespace components::utils