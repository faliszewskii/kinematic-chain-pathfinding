//
// Created by faliszewskii on 16.06.24.
//

#include "Scene.h"
#include "../interface/camera/CameraAnchor.h"

Scene::Scene(AppContext &appContext) : appContext(appContext) {
    appContext.camera = std::make_unique<CameraAnchor>(1920, 1080, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.f), glm::vec3(-M_PI / 2, 0, 0));
    appContext.frameBufferManager = std::make_unique<FrameBufferManager>();
    appContext.frameBufferManager->create_buffers(appContext.camera->screenWidth, appContext.camera->screenHeight);

    appContext.pointShader = std::make_unique<Shader>(Shader::createTraditionalShader(
            "../res/shaders/point/point.vert", "../res/shaders/point/point.frag"));
    appContext.colorShader = std::make_unique<Shader>(Shader::createTraditionalShader(
            "../res/shaders/basic/position.vert", "../res/shaders/basic/color.frag"));

    appContext.grid = std::make_unique<Grid>();
    appContext.robotArm = std::make_unique<RobotArm>();
    appContext.kinematicChain = std::make_unique<KinematicChain>();
    appContext.kinematicChain->l1 = 3;
    appContext.kinematicChain->l2 = 2;
    appContext.unreachableSpace = std::make_unique<Cylinder>();
    appContext.mode = AppContext::ChoosingStart;
    appContext.parameterTexture = std::make_unique<Texture>(360, 360, 4, GL_SRGB_ALPHA,
                                                            GL_RGBA, GL_FLOAT, GL_TEXTURE_2D,
                                                            nullptr);
    appContext.pathLine = std::make_unique<Line>();
}

void Scene::update() {

    if(appContext.draggingMouse) {
        double xpos, ypos;
        auto &io = ImGui::GetIO();
        float side = io.DisplaySize.x / io.DisplaySize.y * 5;
        glfwGetCursorPos(appContext.window , &xpos, &ypos);
        xpos = (xpos) / (io.DisplaySize.x);
        xpos = xpos * 2 - 1;
        ypos = ypos / io.DisplaySize.y;
        ypos = -2 * ypos + 1;
        glm::vec2 p = glm::vec2(xpos*side, ypos*5);
        appContext.currentMouse = p;

        switch(appContext.mode) {

            case AppContext::ChoosingStart:
                if (!get<0>(appContext.kinematicChain->findAngles(p)).empty())
                {
                    appContext.kinematicChain->startCoords = p;
                    // TODO Add real time path calculation
                }

                break;
            case AppContext::ChoosingEnd:
                if (!get<0>(appContext.kinematicChain->findAngles(p)).empty()) {
                    appContext.kinematicChain->endCoords = p;
                    // TODO Add real time path calculation
                }
                break;
            case AppContext::DrawingObstacles:

                break;
        }
    }
}

void Scene::render() {
    appContext.frameBufferManager->bind();

    appContext.colorShader->use();
    appContext.colorShader->setUniform("view", appContext.camera->getViewMatrix());
    appContext.colorShader->setUniform("projection", appContext.camera->getProjectionMatrix());
    appContext.colorShader->setUniform("color", glm::vec4(1,0,1,0.0));
    auto m = glm::identity<glm::mat4>();
    m = glm::scale(m, glm::vec3(2*10 * (appContext.kinematicChain->l1+appContext.kinematicChain->l2)));
    m = glm::translate(m, glm::vec3(0,-0.51,0));
    appContext.colorShader->setUniform("model", m);
    appContext.unreachableSpace->render();

    appContext.colorShader->setUniform("color", glm::vec4(1,0,0,0.1));
    m = glm::identity<glm::mat4>();
    m = glm::scale(m, glm::vec3(2*200));
    m = glm::translate(m, glm::vec3(0,-0.51,0));
    appContext.colorShader->setUniform("model", m);
    appContext.unreachableSpace->render();

    if(appContext.kinematicChain->l1-appContext.kinematicChain->l2 > 0) {
        appContext.colorShader->setUniform("color", glm::vec4(1,0,0,0.1));
        m = glm::identity<glm::mat4>();
        m = glm::scale(m, glm::vec3(2*10 * (appContext.kinematicChain->l1-appContext.kinematicChain->l2)));
        m = glm::translate(m, glm::vec3(0,-0.51,0));
        appContext.colorShader->setUniform("model", m);
        appContext.unreachableSpace->render();
    }

    if(appContext.kinematicChain->startCoords.has_value()) {
        auto [q1, q2] = appContext.kinematicChain->findAngles(appContext.kinematicChain->startCoords.value());
        if (!q1.empty()) {
            appContext.colorShader->setUniform("color", glm::vec4(0.5, 0.2, 0.3, 1));
            appContext.robotArm->render(*appContext.colorShader, appContext.kinematicChain->l1,
                                        appContext.kinematicChain->l2, q1[0], q2[0]);
        }
        if (q1.size() > 1) {
            appContext.colorShader->setUniform("color", glm::vec4(0.5, 0.2, 0.3, 0.2));
            appContext.robotArm->render(*appContext.colorShader, appContext.kinematicChain->l1,
                                        appContext.kinematicChain->l2, q1[1], q2[1]);
        }
    }


    if(appContext.kinematicChain->endCoords.has_value()){
        auto [q1b, q2b] = appContext.kinematicChain->findAngles(appContext.kinematicChain->endCoords.value());
        if (!q1b.empty()) {
            appContext.colorShader->setUniform("color", glm::vec4(0.2, 0.5, 0.3, 1));
            appContext.robotArm->render(*appContext.colorShader, appContext.kinematicChain->l1,
                                        appContext.kinematicChain->l2, q1b[0], q2b[0]);
        }
        if (q1b.size() > 1) {
            appContext.colorShader->setUniform("color", glm::vec4(0.2, 0.5, 0.3, 0.2));
            appContext.robotArm->render(*appContext.colorShader, appContext.kinematicChain->l1,
                                        appContext.kinematicChain->l2, q1b[1], q2b[1]);
        }
    }

    appContext.colorShader->setUniform("model", glm::identity<glm::mat4>());
    appContext.colorShader->setUniform("color", glm::vec4{1, 1, 1, 1});
    appContext.pathLine->render();

    for(auto &obstacle : appContext.kinematicChain->getObstacles()) {
        appContext.colorShader->setUniform("color", glm::vec4(0.2,0.2,0.2,1.f));
        m = glm::identity<glm::mat4>();
        glm::vec2 d = obstacle.size;
        m = glm::translate(m, glm::vec3(obstacle.leftBottom.x, 0 ,-obstacle.leftBottom.y));
        m = glm::scale(m, glm::vec3(d.x, 1, d.y));
        m = glm::rotate(m, float(std::numbers::pi/2), glm::vec3(0,1,0));
        m = glm::translate(m, glm::vec3(0.5f, 0, 0.5f));
        m = glm::rotate(m, float(-std::numbers::pi/2), glm::vec3(1,0,0));
        appContext.colorShader->setUniform("model", m);
        appContext.obstacleQuad.render();
    }

    glDisable(GL_CULL_FACE);
    if(appContext.mode == AppContext::DrawingObstacles && appContext.draggingMouse) {
        appContext.colorShader->setUniform("color", glm::vec4(0.2,0.2,0.2,1.f));
        m = glm::identity<glm::mat4>();
        glm::vec2 d = appContext.startDragging - appContext.currentMouse;
        m = glm::translate(m, glm::vec3(appContext.startDragging.x,0,-appContext.startDragging.y));
        m = glm::scale(m, glm::vec3(-d.x, 1, -d.y));
        m = glm::rotate(m, float(std::numbers::pi/2), glm::vec3(0,1,0));
        m = glm::translate(m, glm::vec3(0.5f, 0, 0.5f));
        m = glm::rotate(m, float(-std::numbers::pi/2), glm::vec3(1,0,0));
        appContext.colorShader->setUniform("model", m);
        appContext.obstacleQuad.render();
    }

    appContext.grid->render(appContext);
    glEnable(GL_CULL_FACE);

    appContext.frameBufferManager->unbind();
}
