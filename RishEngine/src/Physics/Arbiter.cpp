#include <Rish/Physics/Arbiter.h>
#include <Rish/Collider/Collision.h>

namespace rl {

Arbiter::Arbiter(Ref <RigidBody2D> &b1_, Ref <RigidBody2D> &b2_)
{
    b1 = b1_;
    b2 = b2_;
    contacts[0].Pn = 0;
    contacts[0].Pt = 0;
    contacts[1].Pn = 0;
    contacts[1].Pt = 0;
}

void Arbiter::Solve()
{
    b1->shape->setMatrix(b1->angle);
    b2->shape->setMatrix(b2->angle);
    // dispatch function
    Dispatch[(int)b1->RigidShapeType][(int)b2->RigidShapeType]( this, b1->shape, b2->shape);
}


//預備好所需要的參數，包含k, delta_v, n, bias
void Arbiter::PreStep(float inv_dt, Vec2 gravity)
{
    // Calculate average restitution
    e = std::min( b1->restitution, b2->restitution );

    // Calculate friction
    f = std::sqrt( b1->friction * b2->friction );

    for(int i = 0; i < contactCounter; ++i)
    {
        // Calculate radii from COM to contact
        Vec2 ra = contacts[i].position - b1->position;
        Vec2 rb = contacts[i].position - b2->position;

        Vec2 rv = b2->velocity + Cross( b2->angularVelocity, rb ) -
                  b1->velocity - Cross( b1->angularVelocity, ra );

        // Determine if we should perform a resting collision or not
        // The idea is if the only thing moving this object is gravity,
        // then the collision should be performed without any restitution
        if(rv.LenSqr( ) < (inv_dt * gravity).LenSqr( ) + EPSILON)
            e = 0.0f;

        Vec2 tangent = Cross(normal, 1.0f);

        // 將前次累積的衝量，先將施加在物體上
        if (PhysicsWorld::accumulateImpulses)
        {
            // Apply normal + friction impulse
            Vec2 P = contacts[i].Pn * normal + contacts[i].Pt * tangent;

            b1->velocity -= b1->invMass * P;
            b1->angularVelocity -= b1->invI * Cross(ra, P);

            b2->velocity += b2->invMass * P;
            b2->angularVelocity += b2->invI * Cross(rb, P);
        }
    }

}

//透過衝量，計算出速度
void Arbiter::ApplyImpulse()
{
    for(int i = 0; i < contactCounter; ++i)
    {
        // 質心指到Contact Point之向量，目的為了計算相對速度
        Vec2 ra = contacts[i].position - b1->position;
        Vec2 rb = contacts[i].position - b2->position;

        // 相對速度計算 v2 - v1
        Vec2 dv = b2->velocity + Cross( b2->angularVelocity, rb ) -
                  b1->velocity - Cross( b1->angularVelocity, ra );

        // Relative velocity along the normal
        // DeltaV . N (分子)
        float vn = Dot( dv, normal );

        float raCrossN = Cross( ra, normal );
        float rbCrossN = Cross( rb, normal );
        float effectiveMass = b1->invMass + b2->invMass + raCrossN * raCrossN * b1->invI + rbCrossN * rbCrossN * b2->invI;

        // 恢復係數 * DeltaV . N
        float j = -(1.0f + e) * vn;
        // 有效質量
        j /= effectiveMass;
        // 若有兩點，則分配給兩個點去修正
        j /= contactCounter;

        if (PhysicsWorld::accumulateImpulses)
        {
            // Clamp the accumulated impulse
            float Pn0 = contacts[i].Pn;
            contacts[i].Pn = Max(j + Pn0, 0.0f);
            j = contacts[i].Pn - Pn0;
        }
        else
        {
            j = Max(j, 0.0f);
        }


        // 修正衝量方向 * 修正衝量大小
        Vec2 impulse = normal * j;

        // 透過衝量進行修正
        b1->velocity += b1->invMass * -impulse;
        b1->angularVelocity += b1->invI * Cross( ra, -impulse );

        b2->velocity += b2->invMass * impulse;
        b2->angularVelocity += b2->invI * Cross( rb, impulse );

        // -----------------------以下是摩擦力處理-------------------------------

        // 經過修正過後產出新的Delta V
        dv = b2->velocity + Cross( b2->angularVelocity, rb ) -
             b1->velocity - Cross( b1->angularVelocity, ra );

        // 取切線方向的向量
        Vec2 t = Cross(normal, 1.0f);

        // 切線方向上的衝量
        float jt = -Dot( dv, t );
        jt /= effectiveMass;
        jt /= contactCounter;

        if (PhysicsWorld::accumulateImpulses)
        {
            float maxPt = f * contacts[i].Pn;
            // Clamp friction
            float oldTangentImpulse = contacts[i].Pt;
            contacts[i].Pt = Clamp(oldTangentImpulse + jt, -maxPt, maxPt);
            jt = contacts[i].Pt - oldTangentImpulse;
        }
        else
        {
            float maxPt = f * j;
            jt = Clamp(jt, -maxPt, maxPt);
        }

        Vec2 tangentImpulse = jt * t;
        //透過切線方向上(也就是摩擦力)的衝量進行修正
        b1->velocity += b1->invMass * -tangentImpulse;
        b1->angularVelocity += b1->invI * Cross( ra, -tangentImpulse );

        b2->velocity += b2->invMass * tangentImpulse;
        b2->angularVelocity += b2->invI * Cross( rb, tangentImpulse );
    }
}

// 邦加修正
void Arbiter::PositionalCorrection()
{
    // Penetration allowance
    const float k_slop = 0.01f;
    // Penetration percentage to correct
    const float percent = 0.2f;
    Vec2 correction = (std::max( penetration - k_slop, 0.0f ) / (b1->invMass + b2->invMass)) * normal * percent;
    b1->position -= correction * b1->invMass;
    b2->position += correction * b2->invMass;
}

// update the arbiter and calculate the pulse
// 更新arbiter中contact point的數量，以及其中的衝量大小
void Arbiter::Update()
{
    Dispatch[(int)b1->RigidShapeType][(int)b2->RigidShapeType](this, b1->shape, b2->shape);

    if(!PhysicsWorld::warmStarting)
    {
        for (int i = 0; i < contactCounter; i++) {
            contacts[i].Pn = 0;
            contacts[i].Pt = 0;
        }
    }
}


} // end of namespace rl