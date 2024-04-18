#pragma once

#include "../AbstractComponent.hpp"
#include "LayoutUtils.hpp"

namespace components::layoutcalc
{

/**
 * @brief Stateless class responsible for positioning elements with respect to their parent and user supplied options
 *
 */
class LayoutCalculator
{
public:
    LayoutCalculator(AbstractComponent* comp);
    void calculate(const int ceva);

private:
    /* TODO: Maybe in the future this root can be removed and class can be made static so we are fully stateless */
    AbstractComponent* root;
};
} // namespace components::layoutcalc