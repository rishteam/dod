#pragma once

#include <Rish/Collider/Box.h>
#include <Rish/Physics/vector_math.h>
#include <Rish/Physics/RigidBody2D.h>
#include <Rish/Physics/PhysicsWorld.h>

namespace rl{

class RL_API Joint{
public:
    Joint();
    ~Joint() = default;
    void Set(Ref<RigidBody2D> &b1_, Ref<RigidBody2D> &b2_, const Vec2 &anchor);
    void PreStep(float inv_dt);
    void ApplyImpulse();

    Mat22 M;
    Vec2 localAnchor1, localAnchor2;
    Vec2 r1, r2;
    Vec2 bias;
    Vec2 P;		// accumulated impulse
    Ref<RigidBody2D> b1;
    Ref<RigidBody2D> b2;
    float biasFactor;
    float softness;
};

}
