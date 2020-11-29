#include "Rish/Collider/Circle.h"

namespace rl {

Circle::Circle(float x_, float y_, float radius_)
{
    type = Shape::Type::Circle;
    position.x = x_;
    position.y = y_;
    radius = radius_;

    setMatrix(0.0f);
    angle = 0.0f;
}


}