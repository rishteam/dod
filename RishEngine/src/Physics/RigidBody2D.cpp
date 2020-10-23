#include <Rish/Physics/RigidBody2D.h>

rl::RigidBody2D::RigidBody2D(Vec2 position_, Vec2 wh_, float m_){
    mass = m_;
    position = position_;
    wh = wh_;
    velocity.Set(0.0f, 0.0f);
    angularVelocity = 0.0f;
    force.Set(0.0f, 0.0f);
    torque = 0.0f;
    friction = 0.2f;
    angle = 0.0f;
    mass = m_;

    if (mass < MAX_float)
    {
        invMass = 1.0f / mass;
        I = mass * (wh.x * wh.x + wh.y * wh.y) / 12.0f;
        invI = 1.0f / I;
    }
    else
    {
        invMass = 0.0f;
        I = MAX_float;
        invI = 0.0f;
    }
}


void rl::RigidBody2D::setAngle(float rad)
{
    angle = rad;
}

void rl::RigidBody2D::setisCollide(bool state)
{
    isCollider = state;
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

void rl::RigidBody2D::AddForce(const Vec2& f, const Vec2 &attachPoint)
{
    // Update the Linear Force
    force += f;

    // Update the Torque
    Vec2 delta = position - attachPoint;
    torque += force.x * delta.y + force.y * delta.x;
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

    // TODO: Single Time Update or Keeping Update
    this->force.Set(0.0f, 0.0f);
    this->torque = 0.0f;
}

