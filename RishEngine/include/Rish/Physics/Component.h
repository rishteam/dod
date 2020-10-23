#pragma once

#include <Rish/rlpch.h>
#include <Rish/Physics/vector_math.h>
namespace rl {

/**
 * @brief RigidBody2D
 */
struct RigidBody2DComponent
{

    enum class BodyType
    {
        Static = 0,
        Dynamic
    };

    // 角速度
    float angularVelocity = 0.0f;
    // 角度
    float angle = 0.0f;
    // 速度
    Vec2 velocity = Vec2(0.0f, 0.0f);
    // 受力接觸點
    Vec2 attachPoint = Vec2(0.0f, 0.0f);
    // 受的力
    Vec2 force = Vec2(0.0f, 0.0f);
    // 力矩
    float torque = 0.0f;
    // 摩擦力
    float friction = 0.2f;
    // 質量，質量倒數
    float mass = 10.0f;

    BodyType BodyTypeState;

    RigidBody2DComponent() = default;

private:
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(
            CEREAL_NVP(angularVelocity),
            CEREAL_NVP(angle),
            CEREAL_NVP(velocity),
            CEREAL_NVP(force),
            CEREAL_NVP(torque),
            CEREAL_NVP(friction),
            CEREAL_NVP(mass)
        );
    }
};

struct BoxCollider2DComponent
{
    float x = 0.0f;
    float y = 0.0f;
    float w = 1.0f;
    float h = 1.0f;

    BoxCollider2DComponent() = default;
private:
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(
                CEREAL_NVP(x),
                CEREAL_NVP(y),
                CEREAL_NVP(w),
                CEREAL_NVP(h)
        );
    }
};

struct Joint2DComponent
{
    //can change
    UUID rigidBody1;
    UUID rigidBody2;
    Vec2 anchor = Vec2(0.0f, 0.0f);

    //fixed value
    Vec2 localAnchor1, localAnchor2;
    Vec2 r1, r2;
    Vec2 P = Vec2(0.0f, 0.0f);
    Vec2 bias = Vec2(0.0f, 0.0f);
    float biasFactor = 0.2f;
    float softness = 0.0f;

    Joint2DComponent() = default;
private:
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(
                CEREAL_NVP(rigidBody1),
                CEREAL_NVP(rigidBody2),
                CEREAL_NVP(anchor),
                CEREAL_NVP(localAnchor1),
                CEREAL_NVP(localAnchor2),
                CEREAL_NVP(r1),
                CEREAL_NVP(r2),
                CEREAL_NVP(P),
                CEREAL_NVP(bias),
                CEREAL_NVP(biasFactor),
                CEREAL_NVP(softness)
        );
    }
};



}