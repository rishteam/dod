#pragma once
#include <Rish/Collider/Shape.h>

namespace rl {

class RL_API Circle : public Shape
{
public:
    Circle(float x_, float y_, float radius_);
    ~Circle() = default;
};


}
