#include <Rish/rlpch.h>
#include "Rish/Physics/PhysicsWorld.h"
#include "Rish/Physics/RigidBody2D.h"
#include "Rish/Physics/vector_math.h"

#ifndef BOX_H
#define BOX_H

namespace rl {
    class RL_API Box
    {
        public:
            Box(float x_, float y_, float w_, float h_);
            Box(RigidBody2D &body);
            ~Box() = default;
            void _setVertices();
            void _findSAT();
            float get_x();
            float get_y();
            float get_width();
            float get_height();

            bool isCollide(Box &b);

            bool simulate = false;
            std::deque<Vec2> corner;
            std::deque<Vec2> Vertices;
            std::deque<Vec2> SAT;

        private:
            float x, y, w, h;
            float rotation = 0.0f;
    };
}

#endif
