#include "Rish/Physics/World.h"

bool World::accumulateImpulses = true;
bool World::warmStarting = true;
bool World::positionCorrection = true;
float World::width;
float World::height;
COLLISION World::collision_type = COLLISION::SAT;
Vec2 World::m_center = Vec2(0, 0);

World::World(Vec2 gravity_, float width_, float height_)
{
    gravity = gravity_;
    width = width_;
    height = height_;
}

void World::Clear()
{
    //release space
    for(auto ptr : bodies)
    {
        Box* box = dynamic_cast<Box*>(ptr);
        delete box;
    }
    bodies.clear();
    joints.clear();
    arbiters.clear();
}

void World::Add(Shape* body)
{
    bodies.push_back(body);
}

void World::AddJoints(Joint* joint)
{
    joints.push_back(joint);
}

void World::Step(float delta_t)
{
    float inv_dt = delta_t > 0.0f ? 1.0f / delta_t : 0.0f;

    //change the physics coordinate
    for(int i = 0; i < bodies.size(); i++)
    {
        Box* box = dynamic_cast<Box*>(bodies.at(i));
        box->TransformPhysicsCoordinate(box->getPosition().x, box->getPosition().y, box->getwidth(), box->getheight(), box->getRotation());
    }

    //Boardphase detection
    BoardPhase();

    // Compute forces.
    for(int i = 0; i < bodies.size(); i++)
    {
        Box* box = dynamic_cast<Box*>(bodies.at(i));
        if (box->invMass == 0.0f)
            continue;
        box->ComputeForce(delta_t, gravity);
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
        Box* box = dynamic_cast<Box*>(bodies.at(i));
        box->IntegrateVelocities(delta_t);
        auto tmp2 = World::ConvertWorldToScreen(box->position);
        box->setPosition(tmp2.x, tmp2.y);
        box->setRotation(-radiansToDegrees(box->angle));
        box->force = Vec2(0.0f,0.0f);
        box->torque = 0.0f;
    }
}

void World::BoardPhase()
{
    for(int i = 0; i < bodies.size(); i++)
    {
        Box* box1 = dynamic_cast<Box*>(bodies.at(i));

        for(int j = i+1; j < bodies.size(); j++)
        {
            Box* box2 = dynamic_cast<Box*>(bodies.at(j));

            if(box1->invMass == 0.0f && box2->invMass == 0.0f)
                continue;

            //add in Arbiter
            Arbiter newArb(box1, box2);
            ArbiterKey key(box1, box2);
            auto iter = arbiters.find(key);

            // SAT collision
            if(bodies[i]->isCollide(*bodies[j]))
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

Vec2 World::ChangeToPhysicsWorld(const Vec2& ps)
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

Vec2 World::ConvertWorldToScreen(const Vec2& pw)
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

