#pragma once

#include <Rish/rlpch.h>
#include <Rish/Physics/vector_math.h>

namespace rl {

class RL_API RigidBody2D
{
public:
    RigidBody2D(Vec2 position_, Vec2 wh_, float m_);
    ~RigidBody2D() = default;
    void setAngle(float rad);
    void setisCollide(bool state);
    std::pair<Vec2, float> getPhysicsData();
    Vec2 getwh();
    float getMass();
    float getfriction();
    void IntegrateVelocities(float delta_t);
    void ComputeForce(float delta_t, Vec2 gravity);
    void AddForce(const Vec2& f, const Vec2 &attachPoint);

    // 物理世界座標
    Vec2 position;
    // 寬高
    Vec2 wh;
    // 角速度
    float angularVelocity;
    // 角度
    float angle;
    // 速度
    Vec2 velocity;
    // 力矩
    float torque;
    // 摩擦力
    float friction;
    // 質量，質量倒數
    float mass, invMass;
    // 慣性矩，慣性矩倒數
    float I, invI;
    // 是否進行碰撞偵測
    bool isCollider = false;
    // 持續受力
    bool keepingForce = false;
    // 受力點
    Vec2 attachPoint;
    // 受的力
    Vec2 force;


};

}