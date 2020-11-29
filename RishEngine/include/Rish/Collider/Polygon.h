#pragma once
#include <Rish/Collider/Shape.h>

namespace rl {

class RL_API Polygon : public Shape
{
public:
    Polygon(std::deque<Vec2> &pt, Vec2 pos);
    ~Polygon() = default;

};

}