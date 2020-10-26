#pragma once

#include <Rish/rlpch.h>

#include <Rish/Physics/PhysicsWorld.h>
#include <Rish/Physics/RigidBody2D.h>
#include <Rish/Physics/vector_math.h>

namespace rl {

class RL_API Box
{
public:
    Box(float x_, float y_, float w_, float h_);
    Box(Ref<RigidBody2D> &body);
    ~Box() = default;
    //
    void _setVertices();
    void _findSAT();
    //
    float getX();
    float getY();
    float getWidth();
    float getHeight();

    bool isCollide(Ref<Box> &b);

    std::deque<Vec2> m_corner;
    std::deque<Vec2> m_vertices;
    std::deque<Vec2> m_SAT;

    float x = 0.f, y = 0.f, w = 0.f, h = 0.f;
    float rotation = 0.0f;
};

}

