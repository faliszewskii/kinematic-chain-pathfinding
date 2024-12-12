//
// Created by faliszewskii on 10.01.24.
//

#include "InputHandler.h"
#include "imgui.h"
#include "../camera/CameraAnchor.h"

void InputHandler::setupCallbacks(GLFWwindow *window) {
    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(window));
        inputHandler->keyCallback(window, key, scancode, action, mods);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
        auto *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(window));
        inputHandler->mouseCallback(window, xpos, ypos);
    });
    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset) {
        auto *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(window));
        inputHandler->scrollCallback(window, xoffset, yoffset);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
        auto *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(window));
        inputHandler->mouseButtonCallback(window, button, action, mods);
    });
    glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        auto *inputHandler = static_cast<InputHandler *>(glfwGetWindowUserPointer(window));
        inputHandler->windowResize(window, width, height);
    });
}


void InputHandler::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) const {
    static auto deltaTime = static_cast<float>(glfwGetTime());
    deltaTime = static_cast<float>(glfwGetTime()) - deltaTime;

    // TODO: Reimplement handling of input to use imGUI
    //if (ImGui::GetIO().WantCaptureMouse) return;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        appContext.camera->processKeyboard(FORWARD, deltaTime);
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        appContext.camera->processKeyboard(BACKWARD, deltaTime);
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        appContext.camera->processKeyboard(LEFT, deltaTime);
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        appContext.camera->processKeyboard(RIGHT, deltaTime);
}

void InputHandler::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    double xpos, ypos;
    auto &io = ImGui::GetIO();
    float side = io.DisplaySize.x / io.DisplaySize.y * 5;
    glfwGetCursorPos(window , &xpos, &ypos);
    xpos = (xpos) / (io.DisplaySize.x);
    xpos = xpos * 2 - 1;
    ypos = ypos / io.DisplaySize.y;
    ypos = -2 * ypos + 1;
    glm::vec2 p = glm::vec2(xpos*side, ypos*5);

    if(dynamic_cast<CameraAnchor*>(appContext.camera.get())) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
            guiFocus = false;
        else {
            guiFocus = true;
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            appContext.window = window;
            appContext.draggingMouse = true;
            appContext.startDragging = glm::vec2(p.x, p.y);
        } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            appContext.draggingMouse = false;
            if(appContext.mode == AppContext::DrawingObstacles) {
                float xMin = std::min(appContext.startDragging.x, (float)p.x);
                float yMin = std::min(appContext.startDragging.y, (float)p.y);
                auto distX = (float)std::abs(appContext.startDragging.x - p.x);
                auto distY = (float)std::abs(appContext.startDragging.y - p.y);
                if(distX < 0.01 || distY < 0.01) return;
                appContext.kinematicChain->addObstacle({glm::vec2(xMin, yMin), glm::vec2(distX, distY)});

                std::array<std::array<glm::vec4, KinematicChain::resX>, KinematicChain::resY> tex{};
                for(int x = 0; x < KinematicChain::resX; x++) {
                    for(int y = 0; y < KinematicChain::resY; y++) {
                        if(appContext.kinematicChain->getParameters()[x][y] == 1)
                            tex[x][y] = glm::vec4(1, 0, 0, 1);
                        else
                            tex[x][y] = glm::vec4(0, 0, 0, 1);
                    }
                }
                std::vector<float> flattenedArray;
                flattenedArray.reserve(KinematicChain::resX * KinematicChain::resY * 4);
                for (const auto& row : tex) {
                    for (const auto& vec : row) {
                        flattenedArray.push_back(vec.x);
                        flattenedArray.push_back(vec.y);
                        flattenedArray.push_back(vec.z);
                        flattenedArray.push_back(1);
                    }
                }

                appContext.parameterTexture->update2D(flattenedArray.data());
            }
        }
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void InputHandler::mouseCallback(GLFWwindow *window, double xposIn, double yposIn) {
    if(dynamic_cast<CameraAnchor*>(appContext.camera.get()))
    {
        auto xpos = static_cast<float>(xposIn);
        auto ypos = static_cast<float>(yposIn);

        static float lastX = xpos;
        static float lastY = ypos;
        static bool firstMouse = true;

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        if (!guiFocus) {
            appContext.camera->processMouseMovement(xoffset, yoffset);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void InputHandler::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    // TODO: Reimplement handling of input to use imGUI
    //if (ImGui::GetIO().WantCaptureMouse) return;
    if(dynamic_cast<CameraAnchor*>(appContext.camera.get()))
    {
        appContext.camera->processMouseScroll(static_cast<float>(yoffset));
    }
}

void InputHandler::windowResize (GLFWwindow *window, int width, int height) {
    appContext.camera->resize(width, height); // NOLINT(*-narrowing-conversions)
    appContext.frameBufferManager->create_buffers(appContext.camera->screenWidth, appContext.camera->screenHeight);
}
