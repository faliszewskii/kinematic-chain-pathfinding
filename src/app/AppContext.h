//
// Created by faliszewskii on 16.06.24.
//

#ifndef OPENGL_TEMPLATE_APPCONTEXT_H
#define OPENGL_TEMPLATE_APPCONTEXT_H

#include <memory>
#include "../opengl/framebuffer/FrameBufferManager.h"
#include "../interface/camera/BaseCamera.h"
#include "entity/quad/Quad.h"
#include "../opengl/shader/Shader.h"
#include "entity/grid/Grid.h"
#include "entity/robotArm/RobotArm.h"
#include "kinematicChain/KinematicChain.h"
#include "entity/line/Line.h"
#include "parametricMap/ParametricMap.h"

struct AppContext {
    AppContext() = default;

    std::unique_ptr<BaseCamera> camera;
    std::unique_ptr<FrameBufferManager> frameBufferManager;

    // Shaders
    std::unique_ptr<Shader> pointShader;
    std::unique_ptr<Shader> colorShader;

    std::unique_ptr<Grid> grid;
    std::unique_ptr<RobotArm> robotArm;
    std::unique_ptr<KinematicChain> kinematicChain;
    std::unique_ptr<Line> pathLine;

    std::unique_ptr<ParametricMap> parametricMap;


    bool draggingMouse;
    glm::vec2 startDragging;
    GLFWwindow *window;

    std::unique_ptr<Cylinder> unreachableSpace;

    enum {
        ChoosingStart,
        ChoosingEnd,
        DrawingObstacles
    } mode;
    Quad obstacleQuad;
    glm::vec2 currentMouse;
};

#endif //OPENGL_TEMPLATE_APPCONTEXT_H
