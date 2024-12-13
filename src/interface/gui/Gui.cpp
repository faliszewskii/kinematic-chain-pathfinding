//
// Created by faliszewskii on 16.06.24.
//

#include <glm/gtc/type_ptr.hpp>
#include "Gui.h"
#include "imgui.h"

Gui::Gui(AppContext &appContext) : appContext(appContext) {}

void Gui::render() {
    ImGui::Begin("Kinematic chain pathfinding");
    ImGui::DragFloat("l1", &appContext.kinematicChain->l1, 0.01, 0.01, 3);
    ImGui::DragFloat("l2", &appContext.kinematicChain->l2, 0.01, 0.01, 3);


    const char* items[] = {
            "Choosing start position",
            "Choosing end position",
            "Drawing obstacles"
    };
    ImGui::Combo("##mode", reinterpret_cast<int*>(&appContext.mode), items, 3);

    if(ImGui::Button("Calculate path")) {
        appContext.kinematicChain->calculatePath();
        if(appContext.kinematicChain->maxGradient != 0) {
            std::array<std::array<glm::vec4, KinematicChain::resX>, KinematicChain::resY> tex{};
            for (int x = 0; x < KinematicChain::resX; x++) {
                for (int y = 0; y < KinematicChain::resY; y++) {
                    if (appContext.kinematicChain->getParameters()[x][y] == 1)
                        tex[x][y] = glm::vec4(1, 0, 0, 1);
                    else
                        tex[x][y] = glm::vec4(0, ((float)appContext.kinematicChain->gradient[x][y] /
                                                 appContext.kinematicChain->maxGradient), 0, 1);
                }
            }
            std::vector<float> flattenedArray;
            flattenedArray.reserve(KinematicChain::resX * KinematicChain::resY * 4);
            for (const auto &row: tex) {
                for (const auto &vec: row) {
                    flattenedArray.push_back(vec.x);
                    flattenedArray.push_back(vec.y);
                    flattenedArray.push_back(vec.z);
                    flattenedArray.push_back(1);
                }
            }
            appContext.parameterTexture->update2D(flattenedArray.data());
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

    auto size = ImVec2(KinematicChain::resX, KinematicChain::resY);
    ImGui::Image((void*)(intptr_t)appContext.parameterTexture->id, size,  ImVec2(0, 0),  ImVec2(1, 1));

    ImGui::End();
}
