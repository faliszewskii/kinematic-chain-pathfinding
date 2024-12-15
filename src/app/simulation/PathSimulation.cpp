//
// Created by USER on 15/12/2024.
//

#include "PathSimulation.h"

#include <GLFW/glfw3.h>

PathSimulation::PathSimulation() {
    speedMpS = 1;
    running = false;
}

void PathSimulation::startSimulation(std::vector<std::tuple<float, float>> newPath) {
    t = 0;
    path = std::move(newPath);
    auto [angle1, angle2] = path[0];
    a1 = angle1;
    a2 = angle2;
    previousTimeS = glfwGetTime();
}

void PathSimulation::nextStep() {
    float deltaTimeS =  glfwGetTime() - previousTimeS;
    float fullPath = 1;
    float tSegments = fullPath / (path.size() - 1);
    t += deltaTimeS * speedMpS / fullPath;
    if(t > 1) {
        running = false;
        t = 1;
    }
    int previous = int(t / tSegments);
    int current = previous + 1;
    float microT = t / tSegments - previous;

    auto [pA1, pA2] = path[current];
    auto [nA1, nA2] = path[current];
    a1 = pA1 + microT * (nA1 - pA1);
    a2 = pA2 + microT * (nA2 - pA2);
    previousTimeS += deltaTimeS;
}
