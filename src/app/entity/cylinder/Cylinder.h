//
// Created by faliszewskii on 04.12.24.
//

#ifndef MOVEMENT_INTERPOLATION_CYLINDER_H
#define MOVEMENT_INTERPOLATION_CYLINDER_H

#include <memory>
#include "../../../opengl/mesh/PosNorTexVertex.h"
#include "../../../opengl/mesh/Mesh.h"
#include "../../../opengl/shader/Shader.h"

class Cylinder {

    Mesh<PosNorTexVertex> generateCylinder(float radius, float height, int slices);
public:
    std::unique_ptr<Mesh<PosNorTexVertex>> mesh;
    std::vector<PosNorTexVertex> vertices;
    std::vector<unsigned int> indices;

    Cylinder();

    void render();
};

#endif //MOVEMENT_INTERPOLATION_CYLINDER_H
