#include <Rish/Physics/Joint.h>
#include <Rish/Physics/PhysicsWorld.h>

namespace rl {

Joint::Joint() {
    P.Set(0.0f, 0.0f);
    softness = 0.0f;
    biasFactor = 0.2f;
}


void Joint::SetAnchor(Vec2 &anchor)
{
    Anchor = anchor;
}

void Joint::SetBodies(Ref<RigidBody2D> &b1_, Ref<RigidBody2D> &b2_) {
    b1 = b1_;
    b2 = b2_;

    Mat22 Rot1(b1->angle);
    Mat22 Rot2(b2->angle);
    Mat22 Rot1T = Rot1.Transpose();
    Mat22 Rot2T = Rot2.Transpose();

    localAnchor1 = Rot1T * (Anchor - b1->position);
    localAnchor2 = Rot2T * (Anchor - b2->position);

    P.Set(0.0f, 0.0f);

    softness = 0.0f;
    biasFactor = 0.2f;
}

void Joint::PreStep(float inv_dt) {
    // Pre-compute anchors, mass matrix, and bias.
    Mat22 Rot1(b1->angle);
    Mat22 Rot2(b2->angle);

    r1 = Rot1 * localAnchor1;
    r2 = Rot2 * localAnchor2;

    // deltaV = deltaV0 + K * impulse
    // invM = [(1/m1 + 1/m2) * eye(2) - skew(r1) * invI1 * skew(r1) - skew(r2) * invI2 * skew(r2)]
    //      = [1/m1+1/m2     0    ] + invI1 * [r1.y*r1.y -r1.x*r1.y] + invI2 * [r1.y*r1.y -r1.x*r1.y]
    //        [    0     1/m1+1/m2]           [-r1.x*r1.y r1.x*r1.x]           [-r1.x*r1.y r1.x*r1.x]
    Mat22 K1;
    K1.col1.x = b1->invMass + b2->invMass;
    K1.col2.x = 0.0f;
    K1.col1.y = 0.0f;
    K1.col2.y = b1->invMass + b2->invMass;

    Mat22 K2;
    K2.col1.x = b1->invI * r1.y * r1.y;
    K2.col2.x = -b1->invI * r1.x * r1.y;
    K2.col1.y = -b1->invI * r1.x * r1.y;
    K2.col2.y = b1->invI * r1.x * r1.x;

    Mat22 K3;
    K3.col1.x = b2->invI * r2.y * r2.y;
    K3.col2.x = -b2->invI * r2.x * r2.y;
    K3.col1.y = -b2->invI * r2.x * r2.y;
    K3.col2.y = b2->invI * r2.x * r2.x;

    Mat22 K = K1 + K2 + K3;
    K.col1.x += softness;
    K.col2.y += softness;

    M = K.Invert();

    Vec2 p1 = b1->position + r1;
    Vec2 p2 = b2->position + r2;
    Vec2 dp = p2 - p1;

    if (PhysicsWorld::positionCorrection) {
        bias = -biasFactor * inv_dt * dp;
    } else {
        bias.Set(0.0f, 0.0f);
    }

    if (PhysicsWorld::warmStarting)
    {
        // Apply accumulated impulse.
        b1->velocity -= b1->invMass * P;
        b1->angularVelocity -= b1->invI * Cross(r1, P);

        b2->velocity += b2->invMass * P;
        b2->angularVelocity += b2->invI * Cross(r2, P);
    }
    else
    {
        P.Set(0.0f, 0.0f);
    }
}

void Joint::ApplyImpulse() {
    Vec2 dv = b2->velocity + Cross(b2->angularVelocity, r2) - b1->velocity - Cross(b1->angularVelocity, r1);

    Vec2 impulse(0.0f, 0.0f);

    impulse = M * (bias - dv - softness * P);

    b1->velocity -= b1->invMass * impulse;
    b1->angularVelocity -= b1->invI * Cross(r1, impulse);

    b2->velocity += b2->invMass * impulse;
    b2->angularVelocity += b2->invI * Cross(r2, impulse);

    P += impulse;
}

}
