//
// Created by USER on 14/12/2024.
//

#ifndef PARAMETRICMAP_H
#define PARAMETRICMAP_H
#include <memory>
#include <glm/vec4.hpp>
#include "../../opengl/texture/Texture.h"
#include "../kinematicChain/KinematicChain.h"


class ParametricMap {
public:
    ParametricMap();

    std::unique_ptr<Texture> parameterTexture;

    void updateTexture(const std::array<std::array<glm::vec4, KinematicChain::resX>, KinematicChain::resY> &tex) const;

    unsigned int getId() const;
};


#endif //PARAMETRICMAP_H
