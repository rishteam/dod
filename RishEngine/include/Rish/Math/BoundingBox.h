#pragma once

#include <Rish/rlpch.h>

namespace rl {


struct BoundingBox2D
{
    float x, y, w, h;

    BoundingBox2D() = default;
    BoundingBox2D(const glm::vec2 &pos, const glm::vec2 &scale)
    {
        x = pos.x;
        y = pos.y;
        w = scale.x;
        h = scale.y;
    }
    BoundingBox2D(float X, float Y, float W, float H)
    {
        x = X;
        y = Y;
        w = W;
        h = H;
    }

    glm::vec2 getPosition() const { return glm::vec2{x, y}; }
    glm::vec2 getScale() const { return glm::vec2{w, h}; }
};

BoundingBox2D CalculateBoundingBox2D(const glm::vec2 &pos, const glm::vec2 &scale, float rotate);

} // end of namespace rl