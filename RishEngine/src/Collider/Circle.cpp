#include "Rish/Collider/Circle.h"

namespace rl {

Circle::Circle(float x_, float y_, float radius_, float angle_)
{
    type = Shape::Type::Circle;
    position.x = x_;
    position.y = y_;
    radius = radius_;
    angle = angle_;
    setMatrix(angle_);
}


}