//
// Created by faliszewskii on 11.12.24.
//

#include <glm/geometric.hpp>
#include "KinematicChain.h"

KinematicChain::KinematicChain() {
    l1 = l2 = 1;
}

std::tuple<std::vector<float>, std::vector<float>> KinematicChain::findAngles(glm::vec2 position) const {
    std::vector<float> angles1;
    std::vector<float> angles2;

    float x = position.x;
    float y = position.y;
    float dist = glm::length(position);
    float beta = std::atan2(y / dist, x / dist);

    float arg = 1/(2*l1*l2) * (x*x + y*y - l1*l1 - l2*l2);
    if(std::abs(arg) > 1) return {angles1, angles2};

    float gamma = std::acos(arg);
    float q2 = -gamma;
    float alpha = std::asin(std::sin(gamma) * l2 / dist);
    float q1 = beta + alpha;
    angles1.push_back(q1);
    angles2.push_back(q2);

    if(std::abs(arg) > std::numeric_limits<float>::epsilon() * 1000) {
        float gamma = std::acos(arg);
        float q2 = gamma;
        float alpha = std::asin(std::sin(gamma) * l2 / dist);
        float q1 = beta - alpha;
        angles1.push_back(q1);
        angles2.push_back(q2);
    }

    return {angles1, angles2};
}
