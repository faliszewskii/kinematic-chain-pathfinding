//
// Created by faliszewskii on 16.06.24.
//

#include <glm/gtc/type_ptr.hpp>
#include "Gui.h"
#include "imgui.h"

Gui::Gui(AppContext &appContext) : appContext(appContext) {}

void Gui::render() {
    bool modified = false;
    ImGui::Begin("Kinematic chain pathfinding");
    modified |= ImGui::DragFloat("l1", &appContext.kinematicChain->l1, 0.01, 0.01, 3);
    modified |= ImGui::DragFloat("l2", &appContext.kinematicChain->l2, 0.01, 0.01, 3);
    if(modified) appContext.kinematicChain->updateObstacles();

    const char* items[] = {
            "Choose start position",
            "Choose end position",
            "Draw obstacles",
            "Simulate path"
    };
    ImGui::Combo("##mode", reinterpret_cast<int*>(&appContext.mode), items, 4);

    if(appContext.mode == AppContext::SimulatingPath && !appContext.kinematicChain->foundPath.empty()) {
        ImGui::DragFloat("Speed", &appContext.pathSimulation->speedMpS, 0.01, 0.01, 3);
        if(ImGui::Button(appContext.pathSimulation->running? "Stop": "Start")) {
            if(!appContext.pathSimulation->running) {
                appContext.pathSimulation->startSimulation(appContext.kinematicChain->foundPath);
            }
            appContext.pathSimulation->running = !appContext.pathSimulation->running;
        }
    } else {
        if(ImGui::Button("Calculate path")) {
            appContext.kinematicChain->calculatePath();
            if(appContext.kinematicChain->maxGradient != 0) {
                std::array<std::array<glm::vec4, KinematicChain::resX>, KinematicChain::resY> tex{};
                for (int x = 0; x < KinematicChain::resX; x++) {
                    for (int y = 0; y < KinematicChain::resY; y++) {
                        if(appContext.pathSimulation->running
                             && int(appContext.pathSimulation->a1/ 2 / (float)std::numbers::pi * 360 + 180)==x
                              && int(appContext.pathSimulation->a2/ 2 / (float)std::numbers::pi * 360 + 180)==y) {
                            tex[x][y] = glm::vec4(1,0,0,1);
                        } else if(appContext.kinematicChain->gradient[x][y] == 1)
                            tex[x][y] = glm::vec4(1, 0, 1, 1);
                        else if(appContext.kinematicChain->targetGradient[x][y] == 1)
                            tex[x][y] = glm::vec4(1, 1, 0, 1);
                        else if (appContext.kinematicChain->getParameters()[x][y] == 1)
                            tex[x][y] = glm::vec4(1, 0, 0, 1);
                        else if(appContext.kinematicChain->gradient[x][y] == 0 && (float)appContext.kinematicChain->targetGradient[x][y] == 0)
                            tex[x][y] = glm::vec4(0, 0, 0, 1);
                        else
                            tex[x][y] = glm::vec4(
                                0,
                                1-((float)appContext.kinematicChain->gradient[x][y] /
                                                    appContext.kinematicChain->maxGradient),
                                1-((float)appContext.kinematicChain->targetGradient[x][y] /
                                                    appContext.kinematicChain->maxTargetGradient),
                                                     1);
                    }
                }
                if(appContext.kinematicChain->foundPath.size() > 2) {
                    for(auto p = appContext.kinematicChain->foundPath.begin()+1; p != appContext.kinematicChain->foundPath.end()-1; ++p) {
                        auto [a1, a2] = *p;
                        int x = a1 / 2 / (float)std::numbers::pi * 360 + 180;
                        int y = a2 / 2 / (float)std::numbers::pi * 360 + 180;
                        tex[x][y] = glm::vec4(1);
                    }
                }
                appContext.parametricMap->updateTexture(tex);
                std::vector<glm::vec3> vertices;
                auto path = appContext.kinematicChain->foundPath;
                std::transform(path.begin(), path.end(), std::back_inserter(vertices), [&](auto p){
                    const auto& [a1, a2] = p;
                    glm::vec2 tip = appContext.kinematicChain->getTip(a1, a2);
                    return glm::vec3(tip.x, 1, -tip.y);
                });
                appContext.pathLine->updatePoints(vertices);
            }
        }
    }

    auto size = ImVec2(KinematicChain::resX, KinematicChain::resY);
    ImGui::Image((void*)(intptr_t)appContext.parametricMap->getId(), size,  ImVec2(0, 0),  ImVec2(1, 1));

    ImGui::End();
}
