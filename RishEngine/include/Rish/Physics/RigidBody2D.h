#pragma once

#include <Rish/rlpch.h>

#include <Rish/Physics/vector_math.h>

#define MaxVertexCount 64

namespace rl {

class Shape;
class Box;
class Circle;
class Polygon;

class RL_API RigidBody2D
{
public:
    enum class Type{
        Circle,
        Box,
        Polygon
    } RigidShapeType;

    RigidBody2D(RigidBody2D::Type type_);
    ~RigidBody2D() = default;


    void setBox(Vec2 pos, Vec2 wh, float angle);
    void setCircle(Vec2 pos, float radius_, float angle);
    void setPolygon(Vec2 pt[], Vec2 pos, int pointSize, float angle);


    void IntegrateVelocities(float delta_t);
    void ComputeForce(float delta_t, Vec2 gravity);
    void AddForce(const Vec2& f, const Vec2 &attachPoint);

    // Type
    Ref<Box> box;
    Ref<Circle> circle;
    Ref<Polygon> polygon;

    /// decide shape
    Ref<Shape> shape;

    /// 物理世界座標
    Vec2 position;
    /// 寬高
    Vec2 wh;
    /// 角速度
    float angularVelocity;
    /// 角度
    float angle;
    /// 速度
    Vec2 velocity;
    /// 受的力
    Vec2 force;
    /// 力矩
    float torque;
    /// 摩擦力
    float friction;
    /// 恢復係數
    float restitution = 0.2f;

    /// 質量，質量倒數
    float mass, invMass;
    /// 慣性矩，慣性矩倒數
    float I, invI;
    /// 是否進行碰撞偵測
    bool isCollider = false;
    /// 持續受力
    bool keepingForce = false;

    /// 受力點
    Vec2 attachPoint;
    /// 限制重力
    bool RestrictGravity = false;

};

}
