#pragma once
#include <Rish/Collider/Shape.h>

namespace rl {

class RL_API Circle : public Shape
{
public:
    Circle(float x_, float y_, float radius_, float angle_);
    ~Circle() = default;

    virtual bool isCollide(Ref<Shape> s) override
    {
        return s->isCollide(static_cast<Ref <Circle>>(this));
    }
    virtual bool isCollide(Ref<Box> b) override;
    virtual bool isCollide(Ref<Polygon> p) override;
    virtual bool isCollide(Ref<Circle> c) override;

};


}
