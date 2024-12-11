//
// Created by faliszewskii on 04.12.24.
//
#include "Grid.h"
#include "../../AppContext.h"

Grid::Grid() {
    shader =  std::make_unique<Shader>(Shader::createTraditionalShader(
                "../res/shaders/grid/grid.vert", "../res/shaders/grid/grid.frag"));
    std::vector<PositionVertex> vertices;
    std::vector<unsigned int> indices;
    auto normal = glm::vec3(0,1,0);

    vertices.push_back(PositionVertex(glm::vec3(1,1,0)));
    vertices.push_back(PositionVertex(glm::vec3(1,-1,0)));
    vertices.push_back(PositionVertex(glm::vec3(-1,-1,0)));
    vertices.push_back(PositionVertex(glm::vec3(-1,1,0)));

    addQuad(indices,0, 1, 2, 3);

    mesh = std::make_unique<Mesh<PositionVertex>>(Mesh<PositionVertex>(vertices, indices, GL_TRIANGLES));
}

void Grid::render(AppContext &appContext) const {
    BaseCamera &camera = *appContext.camera;

    auto view = camera.getViewMatrix();
    auto projection = camera.getProjectionMatrix();

    shader->use();
    shader->setUniform("near", camera.getNearPlane());
    shader->setUniform("far", camera.getFarPlane());
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);
    shader->setUniform("model", glm::identity<glm::mat4>());
    mesh->render();
}

void Grid::addQuad(vector<unsigned int> &indices, int i0, int i1, int i2, int i3) {
    indices.push_back(i0);
    indices.push_back(i1);
    indices.push_back(i2);

    indices.push_back(i3);
    indices.push_back(i0);
    indices.push_back(i2);
}

