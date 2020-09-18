#pragma once
#include <Rish/rlpch.h>

#include "Rish/Collider/Box.h"
#include "Rish/Physics/RigidBody2D.h"
#include "Rish/Physics/Arbiter.h"
#include "Rish/Physics/Joint.h"
#include "Rish/Physics/vector_math.h"

class ArbiterKey;
class Arbiter;

namespace rl {
    class RL_API PhysicsWorld {
    public:
        PhysicsWorld(Vec2 gravity_, float width_, float height_);

        void Add(RigidBody2D *body);

        void AddJoints(Joint *joint);

        void Clear();

        void Step(float delta_t);

        void BoardPhase();

        static Vec2 ChangeToPhysicsWorld(const Vec2 &ps);

        static Vec2 ConvertWorldToScreen(const Vec2 &pw);

        Vec2 gravity;
        static bool accumulateImpulses;
        static bool warmStarting;
        static bool positionCorrection;
        static float width;
        static float height;
        static Vec2 m_center;
        float timeStep = 1.0 / 60.0f;
        int iterations = 10;

        std::vector<RigidBody2D*> bodies;
        std::vector<Joint*> joints;
        std::map<ArbiterKey, Arbiter> arbiters;

    };
}
