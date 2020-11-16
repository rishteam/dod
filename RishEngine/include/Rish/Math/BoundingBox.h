#pragma once

#include <Rish/rlpch.h>

namespace rl {

struct BoundingBox2D
{
    float x{0.f}, y{0.f}, w{0.f}, h{0.f};

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

    bool isInside(const BoundingBox2D &bound)
    {
        float halfW = w / 2.f, halfH = h / 2.f;
        float bHalfW = bound.w / 2.f, bHalfH = bound.h / 2.f;
        //
        if(bound.x + bHalfW > x + halfW ||
           bound.x - bHalfW < x - halfW ||
           bound.y + bHalfH > y + halfH ||
           bound.y - bHalfH < y - halfH)
            return false;
        else
            return true;
    }

    static BoundingBox2D CalculateBoundingBox2D(const glm::vec2 &pos, const glm::vec2 &scale, float rotate);
    static BoundingBox2D CombineBoundingBox2D(const BoundingBox2D &lhs, const BoundingBox2D &rhs);
    static BoundingBox2D CalculateBoundingFromPoint(const glm::vec2 &p1, const glm::vec2 &p2);
};


} // end of namespace rl