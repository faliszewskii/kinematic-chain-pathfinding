//
// Created by faliszewskii on 11.12.24.
//

#ifndef KINEMATIC_CHAIN_PATHFINDING_KINEMATICCHAIN_H
#define KINEMATIC_CHAIN_PATHFINDING_KINEMATICCHAIN_H


#include <vector>
#include <glm/vec2.hpp>
#include <array>
#include "../obstacle/Obstacle.h"

class KinematicChain {
public:
    static const int resX = 360;
    static const int resY = 360;
    std::array<std::array<float, resX>, resY> parametricMap; // Change to obstacle map
    std::array<std::array<int, resX>, resY> gradient;
    int maxGradient;
private:
    std::vector<Obstacle> obstacles;
    /* TODO
     * Add path point list
     * Add path gradient map
     * In visualisation show union of both maps.
     * Fix collisions
     * Add animation.
     * Done
     * */

public:
    KinematicChain();

    float l1, l2;
    std::optional<glm::vec2> startCoords;
    std::optional<glm::vec2> endCoords;

    void addObstacle(Obstacle obstacle);
    std::vector<Obstacle>& getObstacles();

    [[nodiscard]] std::tuple<std::vector<float>, std::vector<float>> findAngles(glm::vec2 position) const;
    [[nodiscard]] bool checkCollision(float angle1, float angle2) const;
    [[nodiscard]] std::optional<std::vector<glm::vec2>> lineRectIntersection(glm::vec2 p, glm::vec2 q, glm::vec2 rectMin, glm::vec2 rectSize) const;

    std::array<std::array<float, resX>, resY> &getParameters();
    void calculatePath();
};


#endif //KINEMATIC_CHAIN_PATHFINDING_KINEMATICCHAIN_H
