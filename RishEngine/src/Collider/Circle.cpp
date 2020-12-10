#include <Rish/Collider/Circle.h>
#include <Rish/Collider/ColliderCollision.h>

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

bool Circle::isCollide(Ref<Box> b)
{
    return ColliderCircle2Box(static_cast<Ref <Shape>>(this), b);
}

bool Circle::isCollide(Ref<Polygon> p)
{
    return ColliderCircle2Polygon(static_cast<Ref <Shape>>(this), p);
}

bool Circle::isCollide(Ref<Circle> c)
{
    return ColliderCircle2Circle(static_cast<Ref <Shape>>(this), c);
}

}