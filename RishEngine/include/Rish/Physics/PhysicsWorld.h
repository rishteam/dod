#pragma once
#include <Rish/rlpch.h>

#include "Rish/Collider/Shape.h"
#include "Rish/Collider/Box.h"
#include "Rish/Collider/Circle.h"
#include "Rish/Collider/Polygon.h"
#include "Rish/Physics/vector_math.h"
#include "Rish/Physics/Arbiter.h"
#include "Rish/Physics/Joint.h"

class ArbiterKey;
class Arbiter;

enum class COLLISION {
    SAT,
    GJK
};

class World
{
public:
    World(Vec2 gravity_, float width_, float height_);

    void Add(Shape* body);

    void AddJoints(Joint* joint);

    void Clear();

    void Step(float delta_t);

    void BoardPhase();

    static Vec2 ChangeToPhysicsWorld(const Vec2& ps);

    static Vec2 ConvertWorldToScreen(const Vec2& pw);

    Vec2 gravity;
    static COLLISION collision_type;
    static bool accumulateImpulses;
    static bool warmStarting;
    static bool positionCorrection;
    static float width;
    static float height;
    static Vec2 m_center;
    float timeStep = 1.0 / 60.0f;
    int iterations = 10;

//    QuadTree QT;
    std::vector<Shape*> bodies;
    std::vector<Joint*> joints;
    std::map<ArbiterKey, Arbiter> arbiters;

};
