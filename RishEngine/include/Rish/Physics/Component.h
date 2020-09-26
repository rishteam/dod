#pragma once

#include <Rish/rlpch.h>

namespace rl {

/**
 * @brief RigidBody2D
 */
struct RigidBody2DComponent{
    RigidBody2DComponent() = default;
    //角速度
    float angularVelocity;
    //角度
    float angle;
    //速度
    Vec2 velocity;
    //力
    Vec2 force;
    //力矩
    float torque;
    //摩擦力
    float friction;
    //質量，質量倒數
    float mass, invMass;
    //慣性矩，慣性矩倒數
    float I, invI;
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
            CEREAL_NVP(invMass),
            CEREAL_NVP(I),
            CEREAL_NVP(invI),
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
    bool isSimulate = false;
    bool isTrigger = true;

private:
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(
                CEREAL_NVP(x),
                CEREAL_NVP(y),
                CEREAL_NVP(w),
                CEREAL_NVP(h),
                CEREAL_NVP(isSimulate),
                CEREAL_NVP(isTrigger)
        );
    }

};

}