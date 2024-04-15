#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace components::computils
{
class BoxModel
{
public:
    glm::mat4 getTransform()
    {
        if (!isDirty) { return modelMat; }

        /* (1) and (2) are for rotation so that we rotate about the center of the quad*/
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, {pos, layer});
        modelMat = glm::translate(modelMat, glm::vec3(scale.x * 0.5f, scale.y * 0.5f, 0)); // (1)
        modelMat = glm::rotate(modelMat, glm::radians(rot), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMat = glm::translate(modelMat, glm::vec3(-scale.x * 0.5f, -scale.y * 0.5f, 0)); // (2)
        modelMat = glm::scale(modelMat, {scale, 1.0f});
        isDirty = false;
        return modelMat;
    }

    // BoxModel& setPos(const glm::vec2& newPos)
    // {
    //     pos.x = newPos.x;
    //     pos.y = newPos.y;
    //     return *this;
    // }

    // BoxModel& setScale(const glm::vec2& newScale)
    // {
    //     scale.x = newScale.x;
    //     scale.y = newScale.y;
    //     return *this;
    // }

    void markDirty()
    {
        isDirty = true;
    }

    void markClean()
    {
        isDirty = false;
    }

    int layer{1};
    glm::vec2 pos{0, 0};
    glm::vec2 scale{10, 10};
    float rot{0};

private:
    bool isDirty{true};
    glm::mat4 modelMat;
};
} // namespace components::computils