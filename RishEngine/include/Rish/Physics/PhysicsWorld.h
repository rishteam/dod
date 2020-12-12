#pragma once

#include <Rish/rlpch.h>

#include <Rish/Collider/Box.h>
#include <Rish/Physics/RigidBody2D.h>
#include <Rish/Physics/Arbiter.h>
#include <Rish/Physics/Joint.h>

#include <Rish/Physics/vector_math.h>


class Arbiter;

namespace rl {

class RigidBody2D;
class Box;
class Joint;

class RL_API PhysicsWorld
{
public:
    // TODO: object constructor

    PhysicsWorld(Vec2 gravity_);
    PhysicsWorld(Vec2 gravity_, float width_, float height_);

    void setGravity(Vec2 gravity_);

    void Add(const Ref<RigidBody2D> &body);
    void AddJoints(const Ref<Joint> &joint);

    void Clear();

    void Step(float delta_t);

    void BoardPhase();

    static Vec2 ChangeToPhysicsWorld(const Vec2 &ps);


    Vec2 gravity;
    float timeStep = 0.0f;
    static bool accumulateImpulses;
    static bool warmStarting;
    static bool positionCorrection;
    static float width;
    static float height;
    static Vec2 m_center;
    bool restrictGravity = false;
    int iterations = 10;

    std::vector<Ref<RigidBody2D>> bodies;
    std::vector<Ref<Joint>> joints;
    std::vector<Arbiter> arbList;

};
}
