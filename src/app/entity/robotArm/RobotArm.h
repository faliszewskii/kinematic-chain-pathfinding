//
// Created by faliszewskii on 11.12.24.
//

#ifndef KINEMATIC_CHAIN_PATHFINDING_ROBOTARM_H
#define KINEMATIC_CHAIN_PATHFINDING_ROBOTARM_H


#include "../cylinder/Cylinder.h"

class RobotArm {
    Cylinder arm1;
    Cylinder arm2;
public:

    void render(Shader &shader, float l1, float l2, float angle1, float angle2);

    glm::mat4 calcArmMat(float l1, float angle1) const;
    static glm::mat4 restFrame(float l);
};


#endif //KINEMATIC_CHAIN_PATHFINDING_ROBOTARM_H
