#pragma once
#include <Rish/Collider/Shape.h>

namespace rl {

class RL_API Polygon : public Shape
{
public:
    Polygon(Vec2 pt[], Vec2 pos, int pointSize , float angle_);
    ~Polygon() = default;

};

}