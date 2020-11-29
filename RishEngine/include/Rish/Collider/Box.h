#pragma once
#include <Rish/Collider/Shape.h>

namespace rl {

class RL_API Box : public Shape
{
public:
    Box(float x_, float y_, float w_, float h_);
    ~Box() = default;


};

}

