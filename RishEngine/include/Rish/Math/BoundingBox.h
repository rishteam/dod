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

    bool IsInside(const  BoundingBox2D entbound){
        if( entbound.x+entbound.w/2 > x+w/2 )
            return false;
        if( entbound.x-entbound.w/2 < x-w/2 )
            return false;
        if( entbound.y+entbound.h/2 > y+h/2 )
            return false;
        if( entbound.y-entbound.h/2 < y-h/2 )
            return false;
        return true;
    }

};

BoundingBox2D CalculateBoundingBox2D(const glm::vec2 &pos, const glm::vec2 &scale, float rotate);
BoundingBox2D CombinaBoundingBox2D(const BoundingBox2D lhs, const BoundingBox2D rhs);
BoundingBox2D CalculateBounding2Point(const glm::vec2 p1, const glm::vec2 p2);


} // end of namespace rl