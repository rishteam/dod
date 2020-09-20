#include "Rish/Physics/PhysicsWorld.h"

bool rl::PhysicsWorld::accumulateImpulses = true;
bool rl::PhysicsWorld::warmStarting = true;
bool rl::PhysicsWorld::positionCorrection = true;
float rl::PhysicsWorld::width;
float rl::PhysicsWorld::height;
// world coordinate
Vec2 rl::PhysicsWorld::m_center = Vec2(0, 0);

rl::PhysicsWorld::PhysicsWorld(Vec2 gravity_, float width_, float height_)
{
    gravity = gravity_;
    width = width_;
    height = height_;
}

void rl::PhysicsWorld::Clear()
{
    //release space
    bodies.clear();
    joints.clear();
    arbiters.clear();
}

void rl::PhysicsWorld::Add(const Ref<RigidBody2D> &body)
{
    bodies.push_back(body);
}

void rl::PhysicsWorld::AddJoints(Ref<Joint> joint)
{
    joints.push_back(joint);
}

void rl::PhysicsWorld::Step(float delta_t)
{
    float inv_dt = delta_t > 0.0f ? 1.0f / delta_t : 0.0f;

    //Boardphase detection
    this->BoardPhase();

    // Compute forces
    for(int i = 0; i < bodies.size(); i++)
    {
        if (bodies.at(i)->invMass == 0.0f)
            continue;
        bodies.at(i)->ComputeForce(delta_t, gravity);
    }

    //Pre-step arbiter
    for (auto jit : joints)
    {
        jit->PreStep(inv_dt);
    }

    for (auto arb = arbiters.begin(); arb != arbiters.end(); ++arb)
    {
        arb->second.PreStep(inv_dt);
    }


    for (int i = 0; i < this->iterations; ++i)
    {
        //Apply impulse
        for (auto arb = arbiters.begin(); arb != arbiters.end(); ++arb) {
            arb->second.ApplyImpulse();
        }
      for (auto jit : joints)
      {
          jit->ApplyImpulse();
      }
    }

    // Integrate Velocities
    for(int i = 0; i < bodies.size(); i++)
    {
        bodies.at(i)->IntegrateVelocities(delta_t);
        bodies.at(i)->force = Vec2(0.0f,0.0f);
        bodies.at(i)->torque = 0.0f;
    }
}

void rl::PhysicsWorld::BoardPhase()
{
    for(int i = 0; i < bodies.size(); i++)
    {
        for(int j = i+1; j < bodies.size(); j++)
        {
            if(bodies.at(i)->invMass == 0.0f && bodies.at(j)->invMass == 0.0f)
                continue;

            //add in Arbiter
            Arbiter newArb(bodies.at(i), bodies.at(j));
            ArbiterKey key(bodies.at(i), bodies.at(j));
            auto iter = arbiters.find(key);

            // SAT collision
            auto box1 = MakeRef<Box>(bodies.at(i));
            auto box2 = MakeRef<Box>(bodies.at(j));
            if(box1->isCollide(box2))
            {
                //add new arbiter
                if (iter == arbiters.end())
                {
                    arbiters.insert(std::pair<ArbiterKey, Arbiter>(key, newArb));
                }
                //update arbiter
                else
                {
                    iter->second.update(newArb.contacts, newArb.numContacts);
                }
            }
            //no collision
            else
            {
                arbiters.erase(key);
            }
        }
    }
}

Vec2 rl::PhysicsWorld::ChangeToPhysicsWorld(const Vec2& ps)
{
    float w = float(width);
    float h = float(height);
    float u = ps.x / w;
    float v = (h - ps.y) / h;

    float ratio = w / h;
    Vec2 extents(ratio * 25.0f, 25.0f);

    Vec2 lower = m_center - extents;
    Vec2 upper = m_center + extents;

    Vec2 pw;
    pw.x = (1.0f - u) * lower.x + u * upper.x;
    pw.y = (1.0f - v) * lower.y + v * upper.y;
    return pw;
};

Vec2 rl::PhysicsWorld::ConvertWorldToScreen(const Vec2& pw)
{
    float w = float(width);
    float h = float(height);
    float ratio = w / h;

    Vec2 extents(ratio * 25.0f, 25.0f);

    Vec2 lower = m_center - extents;
    Vec2 upper = m_center + extents;

    float u = (pw.x - lower.x) / (upper.x - lower.x);
    float v = (pw.y - lower.y) / (upper.y - lower.y);

    Vec2 ps;
    ps.x = u * w;
    ps.y = (1.0f - v) * h;
    return ps;
};

void rl::PhysicsWorld::demo1()
{
    this->Clear();
    //方塊
    auto floor = MakeRef<RigidBody2D>(Vec2(0.0f, 1.0f), Vec2(0.25f, 0.25f), 10);
    this->Add(floor);
    //地板
    auto box = rl::MakeRef<RigidBody2D>(Vec2(0.0f, 0.0f), Vec2(0.5f, 0.5f), MAX_float);
    this->Add(box);
}
void rl::PhysicsWorld::demo2()
{

}
void rl::PhysicsWorld::demo3()
{

}
void rl::PhysicsWorld::demo4()
{

}
void rl::PhysicsWorld::demo5()
{

}
void rl::PhysicsWorld::demo6()
{

}
void rl::PhysicsWorld::demo7()
{

}
void rl::PhysicsWorld::demo8()
{

}
void rl::PhysicsWorld::demo9()
{

}

