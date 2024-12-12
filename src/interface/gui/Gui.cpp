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

    auto size = ImVec2(KinematicChain::resX, KinematicChain::resY);
    ImGui::Image((void*)(intptr_t)appContext.parameterTexture->id, size,  ImVec2(0, 0),  ImVec2(1, 1));

    ImGui::End();
}
