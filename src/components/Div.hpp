#pragma once

#include "AbstractComponent.hpp"

namespace components
{

class Div : public AbstractComponent
{
public:
    Div();
    ~Div();

    void onClickListener(std::function<void(int, int, MouseButton)> func);

private:
    void onClickEvent() override;
    void onMoveEvent() override;
    void onStart() override;

    std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
};
} // namespace components