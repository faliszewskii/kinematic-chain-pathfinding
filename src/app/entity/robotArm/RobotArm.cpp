//
// Created by faliszewskii on 11.12.24.
//

#include "RobotArm.h"

void RobotArm::render(Shader &shader, float l1, float l2, float angle1, float angle2) {
    glm::mat4 m1 = calcArmMat(0, angle1) ;
    glm::mat4 m2 = calcArmMat(l1, angle2);
    m2 = m1 * m2;
    shader.setUniform("model", m1 * restFrame(l1));
    arm1.render();
    shader.setUniform("model", m2 * restFrame(l2));
    arm2.render();
}



glm::mat4 RobotArm::calcArmMat(float l1, float angle1) const {
    auto m1 = glm::identity<glm::mat4>();
    m1 = glm::translate(m1, glm::vec3(l1, 0, 0));
    m1 = glm::rotate(m1, angle1, glm::vec3(0, 1, 0));
    return m1;
}

glm::mat4 RobotArm::restFrame(float l) {
    auto m = glm::identity<glm::mat4>();
    m = glm::scale(m, glm::vec3(l, 1, 1));
    m = glm::rotate(m, -float(std::numbers::pi / 2), glm::vec3(0, 1, 0));
    m = glm::translate(m, glm::vec3(0, 0, -0.5f));
    m = glm::rotate(m, float(std::numbers::pi / 2), glm::vec3(1, 0, 0));
    return m;
}
