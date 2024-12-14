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
            obstacleMap[i][j] = 0;
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
            while(a1 < -std::numbers::pi) a1 += 2 * std::numbers::pi;
            while(a2 < -std::numbers::pi) a2 += 2 * std::numbers::pi;
            while(a1 >= std::numbers::pi) a1 -= 2 * std::numbers::pi;
            while(a2 >= std::numbers::pi) a2 -= 2 * std::numbers::pi;
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
        if(doSegmentIntersectRectangle(p0, p1, obstacle.leftBottom, obstacle.size))
            return true;
        if(doSegmentIntersectRectangle(p1, p2, obstacle.leftBottom, obstacle.size))
            return true;
    }
    return false;
}

bool KinematicChain::doSegmentIntersectRectangle(glm::vec2 p, glm::vec2 q, glm::vec2 rectMin, glm::vec2 rectSize) const {
    glm::vec2 rectMax = rectMin + rectSize;
    glm::vec2 edges[] = {rectMin, {rectMax.x, rectMin.y}, rectMax, {rectMin.x, rectMax.y}};

    if(p.x > rectMin.x && p.x < rectMax.x && p.y > rectMin.y && p.y < rectMax.y)
        return true;
    if(q.x > rectMin.x && q.x < rectMax.x && q.y > rectMin.y && q.y < rectMax.y)
        return true;

    for(int i = 0; i < 4; i++) {
        if(doSegmentsIntersect(p, q, edges[i], edges[(i+1)%4]))
            return true;
    }
    return false;
}

bool KinematicChain::doSegmentsIntersect(const glm::vec2& a1, const glm::vec2& a2, const glm::vec2& b1, const glm::vec2& b2) const {

    glm::vec2 p = a1;
    glm::vec2 q = b1;
    glm::vec2 r = a2 - a1;
    glm::vec2 s = b2 - b1;

    auto cp = [](glm::vec2 v, glm::vec2 w) { return v.x * w.y - v.y * w.x; };

    float t = cp(q - p, s) / cp(r, s);
    float u = cp(q - p, r) / cp(r, s);

    return t >= 0 && t <= 1 && u >= 0 && u <= 1;
}


void KinematicChain::updateObstacles() {
    for(int i = 0; i < resX; i++) {
        for(int j = 0; j < resY; j++) {
            float a1 = (i - 180) / 360.f * 2 * std::numbers::pi;
            float a2 = (j - 180) / 360.f * 2 * std::numbers::pi;
            obstacleMap[i][j] = checkCollision(a1, a2) ? 1.f : 0;
        }
    }
}

void KinematicChain::addObstacle(Obstacle obstacle) {
    obstacles.push_back(obstacle);
    updateObstacles();
}

std::vector<Obstacle> &KinematicChain::getObstacles() {
    return obstacles;
}


std::array<std::array<float, KinematicChain::resX>, KinematicChain::resY> &KinematicChain::getParameters() {
    return obstacleMap;
}

void KinematicChain::calculatePath() {
    if(!startCoords || !endCoords) return;

    // Handle all cases
    auto [angles1, angles2] = findAngles(startCoords.value());
    if(angles1.empty() || angles2.empty()) return;

    auto [vecTarget1, vecTarget2] = findAngles(endCoords.value());
    if(vecTarget1.empty() || vecTarget2.empty()) return;

    if(testConfiguration(
            getIndexFromAngle(angles1[0]), getIndexFromAngle(angles2[0]),
            getIndexFromAngle(vecTarget1[0]), getIndexFromAngle(vecTarget2[0]))) {
        startOption = 0;
        endOption = 0;
        return;
    }

    if(vecTarget2.size()>1 && testConfiguration(
            getIndexFromAngle(angles1[0]), getIndexFromAngle(angles2[0]),
            getIndexFromAngle(vecTarget1[1]), getIndexFromAngle(vecTarget2[1]))) {
        startOption = 0;
        endOption = 1;
        return;
    }

    if(angles1.size()>1 && testConfiguration(
            getIndexFromAngle(angles1[1]), getIndexFromAngle(angles2[1]),
            getIndexFromAngle(vecTarget1[0]), getIndexFromAngle(vecTarget2[0]))) {
        startOption = 1;
        endOption = 0;
        return;
    }

    if(angles1.size()>1 && vecTarget2.size()>1 && testConfiguration(
            getIndexFromAngle(angles1[1]), getIndexFromAngle(angles2[1]),
            getIndexFromAngle(vecTarget1[1]), getIndexFromAngle(vecTarget2[1]))) {
        startOption = 1;
        endOption = 1;
        return;
    }
}

bool KinematicChain::testConfiguration(int a1, int a2, int target1, int target2) {
    if(obstacleMap[a1][a2] != 0 || obstacleMap[target1][target2] != 0) return false;

    std::array<std::tuple<int, int>, 4> dirs = {
            {{0, 1},
             {1, 0},
             {0, -1},
             {-1, 0}},
    };
    maxGradient = 1;
    gradient = std::array<std::array<int, resX>, resY>{};
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
            if(gradient[nextX][nextY] != 0 || obstacleMap[nextX][nextY] == 1.f) {
                continue;
            }
            gradient[nextX][nextY] = gradient[x][y] + 1;
            if(maxGradient < gradient[nextX][nextY]) maxGradient = gradient[nextX][nextY];
            queue.emplace(nextX, nextY);
        }
    }

    if(gradient[target1][target2] == 0) {
        return false;
    }

    targetGradient = std::array<std::array<int, resX>, resY>{};
    targetGradient[target1][target2] = 1;
    maxTargetGradient = 1;
    queue.emplace(target1, target2);
    while(!queue.empty()) {
        auto [x, y] = queue.front();
        queue.pop();
        for(auto &dir : dirs) {
            auto [dx, dy] = dir;
            auto nextX = (x + dx + resX)%resX;
            auto nextY = (y + dy + resY)%resY;
            if(targetGradient[nextX][nextY] != 0 || obstacleMap[nextX][nextY] == 1.f) {
                continue;
            }
            targetGradient[nextX][nextY] = targetGradient[x][y] + 1;
            if(maxTargetGradient < targetGradient[nextX][nextY]) maxTargetGradient = targetGradient[nextX][nextY];
            queue.emplace(nextX, nextY);
        }
    }

    foundPath.clear();
    int currentGradient = gradient[target1][target2];
    int x = target1;
    int y = target2;
    foundPath.emplace_back((x - 180) / 360.f * 2 * std::numbers::pi, (y - 180) / 360.f * 2 * std::numbers::pi);
    while(currentGradient != 1) {
        for(auto &dir : dirs) {
            auto [dx, dy] = dir;
            auto nextX = (x + dx + resX)%resX;
            auto nextY = (y + dy + resY)%resY;
            if(gradient[nextX][nextY] < currentGradient && obstacleMap[nextX][nextY] != 1.f) {
                currentGradient = gradient[nextX][nextY];
                x = nextX;
                y = nextY;
                foundPath.emplace_back((x - 180) / 360.f * 2 * std::numbers::pi, (y - 180) / 360.f * 2 * std::numbers::pi);
                break;
            }
        }
    }
    return true;
}

glm::vec2 KinematicChain::getTip(float angle1, float angle2) {
    auto p0 = glm::vec2(0);
    glm::vec2 p1 = p0 + l1 * glm::vec2(std::cos(angle1), std::sin(angle1));
    glm::vec2 p2 = p1 + l2 * glm::vec2(std::cos(angle1+angle2), std::sin(angle1+angle2));
    return p2;
}

int KinematicChain::getIndexFromAngle(float a) const {
    return a / 2 / (float)std::numbers::pi * 360 + 180;
}
