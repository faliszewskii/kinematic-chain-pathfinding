//
// Created by faliszewskii on 11.12.24.
//

#ifndef KINEMATIC_CHAIN_PATHFINDING_KINEMATICCHAIN_H
#define KINEMATIC_CHAIN_PATHFINDING_KINEMATICCHAIN_H


#include <vector>
#include <glm/vec2.hpp>

class KinematicChain {

public:
    KinematicChain();

    float l1, l2;

    std::tuple<std::vector<float>, std::vector<float>> findAngles(glm::vec2 position) const;
};


#endif //KINEMATIC_CHAIN_PATHFINDING_KINEMATICCHAIN_H
