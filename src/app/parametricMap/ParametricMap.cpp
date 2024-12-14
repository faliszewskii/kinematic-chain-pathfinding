//
// Created by USER on 14/12/2024.
//

#include "ParametricMap.h"

#include "../../opengl/shader/Shader.h"
#include <vector>
#include <glm/vec4.hpp>

ParametricMap::ParametricMap() {
    parameterTexture = std::make_unique<Texture>(360, 360, 4, GL_SRGB_ALPHA,
                                                            GL_RGBA, GL_FLOAT, GL_TEXTURE_2D,
                                                            nullptr);
}

void ParametricMap::updateTexture(const std::array<std::array<glm::vec4, KinematicChain::resX>, KinematicChain::resY> &tex) const {
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
    parameterTexture->update2D(flattenedArray.data());
}

unsigned int ParametricMap::getId() const {
    return parameterTexture->id;
}
