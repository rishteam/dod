#pragma once
#include <Rish/Collider/Shape.h>
#include <Rish/Physics/RigidBody2D.h>
#include <Rish/Physics/vector_math.h>
#include <Rish/Physics/PhysicsWorld.h>



//被撞的body2, 撞人的body1
namespace rl {

struct Contact
{
    Contact() = default;
    // 接觸點座標
    Vec2 position;

    // 修正之衝量(lambda)
    float Pn = 0.0f;
    // 切線方向的衝量(lambda tangent)
    float Pt = 0.0f;
};


class Arbiter
{
public:
    Arbiter(Ref<RigidBody2D> &b1_, Ref<RigidBody2D> &b2_);
    ~Arbiter() = default;

    bool operator == (const Arbiter& a1)
    {
        if (a1.b1 == b1 && a1.b2 == b2)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void Solve();
    void PreStep(float inv_dt, Vec2 gravity);
    void ApplyImpulse();
    void PositionalCorrection();
    void Update();

    // 接觸點
    Contact contacts[2];
    // 分離法向量
    Vec2 normal;
    // 接觸點總數
    int contactCounter = 0;
    // 穿透值
    float penetration = 0.0f;

    // 恢復係數
    float e;
    // 物體合併摩擦力
    float f;

    Ref<RigidBody2D> b1;
    Ref<RigidBody2D> b2;
};

}
