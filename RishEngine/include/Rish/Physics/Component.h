#pragma once

#include <Rish/rlpch.h>

namespace rl {

/**
 * @brief RigidBody2D
 */
struct RigidBody2DComponent{
    RigidBody2DComponent() = default;
    //角速度
    float angularVelocity = 0.0f;
    //角度
    float angle = 0.0f;
    //速度
    Vec2 velocity = Vec2(0.0f, 0.0f);
    //力
    Vec2 force = Vec2(0.0f, 0.0f);
    //力矩
    float torque = 0.0f;
    //摩擦力
    float friction = 0.0f;
    //質量，質量倒數
    float mass = 10.0f;
    bool isCollide = false;


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
            CEREAL_NVP(mass),
            CEREAL_NVP(isCollide)
        );
    }
};

struct BoxCollider2DComponent{
    BoxCollider2DComponent() = default;
    float x;
    float y;
    float w;
    float h;

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

}