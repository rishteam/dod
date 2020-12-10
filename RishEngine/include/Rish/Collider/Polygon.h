#pragma once
#include <Rish/Collider/Shape.h>


namespace rl {

class RL_API Polygon : public Shape
{
public:
    Polygon(Vec2 pt_[], Vec2 pos_, int pointSize_, float angle_);
    ~Polygon() = default;

    virtual bool isCollide(Ref<Shape> s) override
    {
        return s->isCollide(static_cast<Ref <Polygon>>(this));
    }
    virtual bool isCollide(Ref<Box> b) override;
    virtual bool isCollide(Ref<Polygon> p) override;
    virtual bool isCollide(Ref<Circle> c) override;

};

}