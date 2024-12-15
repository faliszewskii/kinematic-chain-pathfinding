//
// Created by USER on 15/12/2024.
//

#ifndef PATHSIMULATION_H
#define PATHSIMULATION_H

#include <tuple>
#include <vector>

class PathSimulation {
    float previousTimeS;
    std::vector<std::tuple<float, float>> path;

public:
    PathSimulation();

    float t;
    float speedMpS;
    bool running;

    float a1;
    float a2;

    void startSimulation(std::vector<std::tuple<float, float>> newPath);
    void nextStep();
};



#endif //PATHSIMULATION_H
