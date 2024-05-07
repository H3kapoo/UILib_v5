// #include "ScrollBar.hpp"
// #include "layoutCalc/LayoutData.hpp"

// namespace components
// {

// ScrollBar::ScrollBar(const computils::Transform& root)
//     : AbstractComponent({.type = CompType::ScrollBar,
//           .shaderPath = "/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl"})
//     // , dummy("/home/hekapoo/newTryAtUI/src/assets/shaders/base.glsl")
//     , rootTransform{root}
// {}

// void ScrollBar::onPrepareToRender()
// {
//     if (options.orientation == layoutcalc::LdOrientation::Horizontal)
//     {
//         getTransformRW().pos = {rootTransform.pos.x, rootTransform.pos.y + rootTransform.scale.y - options.barSize};
//         getTransformRW().scale = {rootTransform.scale.x, options.barSize};
//     }
//     else
//     {
//         getTransformRW().pos = {rootTransform.pos.x + rootTransform.scale.x - options.barSize, rootTransform.pos.y};
//         getTransformRW().scale = {options.barSize, rootTransform.scale.y};
//     }

//     auto border = glm::vec4(layout.borderSize.top, layout.borderSize.bottom, layout.borderSize.left,
//         layout.borderSize.right);

//     glm::vec4 col = glm::vec4(1, 1, 1, 1);
//     getShader().setActiveShaderId(getShaderId());
//     getShader().setVec4f("uColor", col);
// }

// void ScrollBar::onStart()
// {
//     utils::printlni("[INF] I am node {} and onStart() called", getId());
// }

// void ScrollBar::onLayoutUpdate() {}
// } // namespace components