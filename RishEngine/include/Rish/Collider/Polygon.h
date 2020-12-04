#pragma once
#include <Rish/Collider/Shape.h>

namespace rl {

class RL_API Polygon : public Shape
{
public:
    Polygon(Vec2 pt_[], Vec2 pos_, int pointSize_, float angle_);
    ~Polygon() = default;

};

}