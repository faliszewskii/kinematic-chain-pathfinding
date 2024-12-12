//
// Created by faliszewskii on 11.12.24.
//

#include <glm/geometric.hpp>
#include <algorithm>
#include <stack>
#include <queue>
#include "KinematicChain.h"

KinematicChain::KinematicChain() {
    l1 = l2 = 1;

    for(int i = 0; i < resX; i++) {
        for(int j = 0; j < resY; j++) {
            parametricMap[i][j] = 0;
        }
    }
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

    std::vector<float> correct1;
    std::vector<float> correct2;
    for(int i = 0; i < angles1.size(); i++) {
        float a1 = angles1[i];
        float a2 = angles2[i];
        if(!checkCollision(a1, a2)) {
            correct1.push_back(a1);
            correct2.push_back(a2);
        }
    }

    return {correct1, correct2};
}

bool KinematicChain::checkCollision(float angle1, float angle2) const {
    for(auto obstacle : obstacles) {
        auto p0 = glm::vec2(0);
        glm::vec2 p1 = p0 + l1 * glm::vec2(std::cos(angle1), std::sin(angle1));
        glm::vec2 p2 = p1 + l2 * glm::vec2(std::cos(angle1+angle2), std::sin(angle1+angle2));
        if(lineRectIntersection(p0, p1, obstacle.leftBottom, obstacle.size))
            return true;
        if(lineRectIntersection(p1, p2, obstacle.leftBottom, obstacle.size))
            return true;
    }
    return false;
}


std::optional<std::vector<glm::vec2>> KinematicChain::lineRectIntersection(glm::vec2 p, glm::vec2 q, glm::vec2 rectMin, glm::vec2 rectSize) const {
    glm::vec2 rectMax = rectMin + rectSize;
    glm::vec2 dirs[] = {glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(-1, 0), glm::vec2(0, -1)};
    glm::vec2 edges[] = {rectMin, {rectMax.x, rectMin.y}, rectMax, {rectMin.x, rectMax.y}};

    auto intersect = [](glm::vec2 p1, glm::vec2 p2, glm::vec2 e1, glm::vec2 e2) -> std::optional<glm::vec2> {
        glm::vec2 r = p2 - p1, s = e2 - e1;
        float det = r.x * s.y - r.y * s.x;
        if (abs(det) < 1e-10) return {}; // Parallel
        float t = ((e1.x - p1.x) * s.y - (e1.y - p1.y) * s.x) / det;
        float u = ((e1.x - p1.x) * r.y - (e1.y - p1.y) * r.x) / det;
        if (t >= 0 && t <= 1 && u >= 0 && u <= 1) return p1 + t * r;
        return {};
    };

    std::vector<glm::vec2> intersections;
    for (int i = 0; i < 4; ++i) {
        if (auto pt = intersect(p, q, edges[i], edges[(i + 1) % 4])) intersections.push_back(*pt);
    }
    return intersections.empty() ? std::optional<std::vector<glm::vec2>>{} : intersections;
}

void KinematicChain::addObstacle(Obstacle obstacle) {
    obstacles.push_back(obstacle);

    for(int i = 0; i < resX; i++) {
        for(int j = 0; j < resY; j++) {
            float a1 = i / 360.f * 2 * std::numbers::pi;
            float a2 = j / 360.f * 2 * std::numbers::pi;
            parametricMap[i][j] = checkCollision(a1, a2)? 1.f: 0;
        }
    }
}

std::vector<Obstacle> &KinematicChain::getObstacles() {
    return obstacles;
}


std::array<std::array<float, KinematicChain::resX>, KinematicChain::resY> &KinematicChain::getParameters() {
    return parametricMap;
}

void KinematicChain::calculatePath() {
    if(!startCoords || !endCoords) return;
    maxGradient = 1;
    gradient = std::array<std::array<int, resX>, resY>{};

    std::array<std::tuple<int, int>, 4> dirs = {
            {{0, 1},
            {1, 0},
            {0, -1},
            {-1, 0}},
    };

    auto [angles1, angles2] = findAngles(startCoords.value());
    int a1 = angles1[0] / 2 / std::numbers::pi * 360 + 180;
    int a2 = angles2[0] / 2 / std::numbers::pi * 360 + 180;

    if(parametricMap[a1][a2] != 0) return;
    gradient[a1][a2] = 1;

    std::queue<std::tuple<int, int>> queue;
    queue.emplace(a1, a2);

    while(!queue.empty()) {
        auto [x, y] = queue.front();
        queue.pop();
        for(auto &dir : dirs) {
            auto [dx, dy] = dir;
            auto nextX = (x + dx + resX)%resX;
            auto nextY = (y + dy + resY)%resY;
            if(gradient[nextX][nextY] != 0)
                continue;
            if(parametricMap[nextX][nextY] < 0.5f) {
                gradient[nextX][nextY] = gradient[x][y] + 1;
                if(maxGradient < gradient[nextX][nextY]) maxGradient = gradient[nextX][nextY];
                queue.emplace(nextX, nextY);
            } else {
                maxGradient = maxGradient;
            }
        }
    }
}
