#include <Rish/Collider/Box.h>
#include <Rish/Physics/vector_math.h>
#include <Rish/Physics/RigidBody2D.h>
#include <Rish/Physics/PhysicsWorld.h>

#ifndef Joint_H
#define Joint_H

namespace rl {
class Joint
{
public:
    Joint();

    ~Joint() = default;

    void SetAnchor(Vec2 &anchor);

    void SetBodies(Ref <RigidBody2D> &b1_, Ref<RigidBody2D> &b2_);

    void PreStep(float inv_dt);

    void ApplyImpulse();

    Mat22 M;
    // TODO: Anchor Update
    Vec2 Anchor;
    Vec2 localAnchor1, localAnchor2;
    Vec2 r1, r2;
    Vec2 bias;
    Vec2 P;        // accumulated impulse
    Ref <RigidBody2D> b1;
    Ref <RigidBody2D> b2;
    float biasFactor;
    float softness;
};
}

#endif