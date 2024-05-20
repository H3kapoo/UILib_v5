#pragma once

#include "AbstractComponent.hpp"
#include "layoutCalc/LayoutCalculator.hpp"

namespace components
{

class Button : public AbstractComponent
{
public:
    struct Style
    {
        glm::vec4 color{0.0f};
        int someOption{0};
    };

    Button();
    ~Button();

    void addClickListener(std::function<void(int, int, MouseButton)> func);

    Style style;

private:
    void onPrepareToRender() override;
    void onClickEvent() override;
    void onMoveEvent() override;
    void onStart() override;
    bool onLayoutUpdate() override;

    /* Layout related */
    layoutcalc::LayoutCalculator layoutCalc{this};
    std::function<void(int, int, MouseButton)> mouseClickCb{nullptr};
};
} // namespace components