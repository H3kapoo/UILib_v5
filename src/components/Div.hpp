#pragma once

#include "AbstractComponent.hpp"

namespace components
{

class Div : public AbstractComponent
{
public:
    Div();
    ~Div();

    void addClickListener(std::function<void(int, int, MouseButton)> func);

private:
    void onPrepareToRender() override;
    void onClickEvent() override;
    void onMoveEvent() override;
    void onStart() override;

    glm::vec4 color{1.0f, 0.3f, 0.3f, 1.0f};
    std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
};
} // namespace components