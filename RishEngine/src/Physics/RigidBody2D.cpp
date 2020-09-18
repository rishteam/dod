#include "Rish/Physics/RigidBody2D.h"

rl::RigidBody2D::RigidBody2D(){
    mass = 0;
    velocity = Vec2(1.0, -1.0);
    angularVelocity = 0;
    force = Vec2(0.0, 0.0);
    torque = 0;
}

std::pair<Vec2, float> rl::RigidBody2D::getPhysicsData()
{
    return std::make_pair(position, angle);
}

float rl::RigidBody2D::getMass()
{
    return mass;
}

Vec2 rl::RigidBody2D::getwh()
{
    return wh;
}

float rl::RigidBody2D::getfriction()
{
    return friction;
}


void rl::RigidBody2D::AddForce(const Vec2& f)
{
    force += f;
}


void rl::RigidBody2D::ComputeForce(float delta_t, Vec2 gravity)
{
    if (this->invMass == 0.0f)
        return;
    else
    {
        this->velocity += delta_t * (gravity + this->invMass * this->force);
        this->angularVelocity += delta_t * this->invI * this->torque;
    }
}

void rl::RigidBody2D::IntegrateVelocities(float delta_t)
{
    auto tmp = delta_t * velocity;
    position += tmp;
    auto tmp2 = delta_t * angularVelocity;
    angle += tmp2;

    this->force.Set(0.0f, 0.0f);
    this->torque = 0.0f;
}

