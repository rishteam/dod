#include <Rish/Physics/RigidBody2D.h>
#include <Rish/Collider/Shape.h>
#include <Rish/Collider/Box.h>
#include <Rish/Collider/Circle.h>
#include <Rish/Collider/Polygon.h>

namespace rl {

RigidBody2D::RigidBody2D(RigidBody2D::Type type_)
{
    mass = 10;
    velocity = Vec2(0.0f, 0.0f);
    angularVelocity = 0;
    force = Vec2(0.0, 0.0);
    friction = 0.2f;
    torque = 0.0f;
    I = 0.0f;
    restitution = 0.0f;

    RigidShapeType = type_;
}


void RigidBody2D::setBox(Vec2 pos, Vec2 wh, float angle)
{
    RigidShapeType = Type::Box;
    box = MakeRef<Box>(pos.x, pos.y, wh.x, wh.y, angle);

    shape = box;

    if (mass < FLT_MAX)
    {
        invMass = 1.0f / mass;
        I = mass * (wh.x * wh.x + wh.y * wh.y) / 12.0f;
        invI = 1.0f / I;
    }
    else
    {
        invMass = 0.0f;
        I = FLT_MAX;
        invI = 0.0f;
    }
}

void RigidBody2D::setCircle(Vec2 pos, float radius_, float angle)
{
    RigidShapeType = Type::Circle;
    circle = MakeRef<Circle>(pos.x, pos.y, radius_, angle);
    shape = circle;

    if (mass < FLT_MAX)
    {
        invMass = 1.0f / mass;
        I = mass * radius_ * radius_;
        invI = 1.0f / I;
    }
    else
    {
        invMass = 0.0f;
        I = FLT_MAX;
        invI = 0.0f;
    }
}


void RigidBody2D::setPolygon(Vec2 pt[], Vec2 pos, int pointSize, float angle)
{
    RigidShapeType = Type::Polygon;
    polygon = MakeRef<Polygon>(pt, pos, pointSize, angle);
    shape = polygon;

    // Calculate centroid and moment of Inertia
    Vec2 c( 0.0f, 0.0f );
    float area = 0.0f;
    I = 0;
    const float k_inv3 = 1.0f / 3.0f;

    for(int i1 = 0; i1 < polygon->m_vertexCount; ++i1)
    {
        // Triangle vertices, third vertex implied as (0, 0)
        Vec2 p1( polygon->m_vertices[i1] );
        int i2 = i1 + 1 < polygon->m_vertexCount ? i1 + 1 : 0;
        Vec2 p2( polygon->m_vertices[i2] );

        float D = Cross( p1, p2 );
        float triangleArea = 0.5f * D;

        area += triangleArea;

        // Use area to weight the centroid average, not just vertex position
        c += triangleArea * k_inv3 * (p1 + p2);

        float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
        float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
        I += (0.25f * k_inv3 * D) * (intx2 + inty2);
    }

    c *= 1.0f / area;

    // Translate vertices to centroid (make the centroid (0, 0)
    // for the polygon in model space)
    // Not really necessary, but I like doing this anyway
    for(int i = 0; i < polygon->m_vertexCount; ++i)
        polygon->m_vertices[i] -= c;

    float density = 1.0f;
    if (mass < FLT_MAX)
    {
        I = I * density;
        invI = 1.0f / I;
        invMass = 1.0f / mass;
    }
    else
    {
        invMass = 0.0f;
        I = FLT_MAX;
        invI = 0.0f;
    }
}

void RigidBody2D::IntegrateVelocities(float delta_t)
{
    auto tmp = delta_t * velocity;
    position += tmp;
    auto tmp2 = delta_t * angularVelocity;
    angle += tmp2;

    if (!keepingForce)
    {
        this->force.Set(0.0f, 0.0f);
        this->torque = 0.0f;
    }
}

void RigidBody2D::ComputeForce(float delta_t, Vec2 gravity)
{
    if (this->invMass == 0.0f)
    {
        return;
    }
    else
    {
        if(!this->RestrictGravity)
        {
            this->velocity += delta_t * (gravity + this->invMass * this->force);
        }
        else
        {
            this->velocity = Vec2(0.0f, 0.0f);
        }
        this->angularVelocity += delta_t * this->invI * this->torque;
    }
}

void RigidBody2D::AddForce(const Vec2 &f, const Vec2 &attachPoint)
{
    // Update the Linear Force
    force += f;

    // Update the Torque
    Vec2 delta = attachPoint - position;
    torque += Cross(delta, force);
}


}