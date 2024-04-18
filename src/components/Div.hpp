#pragma once

#include "AbstractComponent.hpp"
#include "layoutCalc/LayoutCalculator.hpp"

#include "Button.hpp"

namespace components
{

class Div : public AbstractComponent
{
public:
    struct Style
    {
        glm::vec4 color{0.0f};
        int someOption{false};
    };

    Div();
    ~Div();

    void addClickListener(std::function<void(int, int, MouseButton)> func);

    Style style;

private:
    void onPrepareToRender() override;
    void onClickEvent() override;
    void onMoveEvent() override;
    void onStart() override;
    void onLayoutUpdate() override;

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this}; // TODO: Singleton
    std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
};
} // namespace components