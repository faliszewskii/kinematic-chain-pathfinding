//
// Created by faliszewskii on 19.02.24.
//

#ifndef OPENGL_SANDBOX_GRIDMODULE_H
#define OPENGL_SANDBOX_GRIDMODULE_H

#include <vector>
#include "../../../opengl/shader/Shader.h"
#include "imgui.h"
#include "../../../opengl/mesh/PositionVertex.h"
#include "../../../opengl/mesh/Mesh.h"

struct AppContext;

class Grid {
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Mesh<PositionVertex>> mesh;
public:
    explicit Grid();
    void render(AppContext &appContext) const;
    void addQuad(std::vector<unsigned int> &indices, int i0, int i1, int i2, int i3);
};

#endif //OPENGL_SANDBOX_GRIDMODULE_H
