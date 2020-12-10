#pragma once
#include <Rish/Collider/Shape.h>

namespace rl {

class RL_API Box : public Shape
{
public:
    Box(float x_, float y_, float w_, float h_, float angle_);
    ~Box() = default;

    virtual bool isCollide(Ref<Shape> s) override
    {
        return s->isCollide(static_cast<Ref <Box>>(this));
    }
    virtual bool isCollide(Ref<Box> b) override;
    virtual bool isCollide(Ref<Polygon> p) override;
    virtual bool isCollide(Ref<Circle> c) override;

};

}

