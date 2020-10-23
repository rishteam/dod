#include <Rish/Physics/PhysicsWorld.h>

bool rl::PhysicsWorld::accumulateImpulses = true;
bool rl::PhysicsWorld::warmStarting = false;
bool rl::PhysicsWorld::positionCorrection = true;
float rl::PhysicsWorld::width;
float rl::PhysicsWorld::height;
// world coordinate
Vec2 rl::PhysicsWorld::m_center = Vec2(0, 0);

rl::PhysicsWorld::PhysicsWorld(Vec2 gravity_)
{
    gravity = gravity_;
}

rl::PhysicsWorld::PhysicsWorld(Vec2 gravity_, float width_, float height_)
{
    gravity = gravity_;
    width = width_;
    height = height_;
}

void rl::PhysicsWorld::Clear()
{
    //release space
    for (auto obj : bodies)
    {
        obj.reset();
    }
    bodies.clear();
    joints.clear();
    arbiters.clear();
}

void rl::PhysicsWorld::Add(const Ref<RigidBody2D> &body)
{
    bodies.push_back(body);
}

void rl::PhysicsWorld::AddJoints(const Ref<Joint> &joint)
{
    joints.push_back(joint);
}

void rl::PhysicsWorld::Step(float dt)
{
    float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;
    timeStep = dt;


    //Boardphase detection
    this->BoardPhase();

    // Compute forces
    for(int i = 0; i < bodies.size(); i++)
    {
        if (bodies.at(i)->invMass == 0.0f)
            continue;
        bodies.at(i)->ComputeForce(dt, gravity);
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
        bodies.at(i)->IntegrateVelocities(dt);
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
                if(!bodies.at(i)->isCollider || !bodies.at(j)->isCollider)
                    continue;

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
    auto box = MakeRef<RigidBody2D>(Vec2(0.0f, 10.0f), Vec2(20.0f, 10.0f), 10);
//    box->setAngle(degreesToRadians(30));
    box->angle = -0.25f;
    this->Add(box);
    //地板
    auto floor = rl::MakeRef<RigidBody2D>(Vec2(0.0f, -5.0f * 2), Vec2(100.0f, 20.0f), MAX_float);
    this->Add(floor);
}
void rl::PhysicsWorld::demo2()
{
    this->Clear();
    //地板
    auto floor = MakeRef<RigidBody2D>(Vec2(0.0f, -10.0f), Vec2(100.0f, 20.0f), MAX_float);
    floor->friction = 0.2f;
    this->Add(floor);

    //箱子
    auto box = MakeRef<RigidBody2D>(Vec2(15.0f, 22.0f), Vec2(2.5f, 2.5f), 10);
    box->friction = 0.2f;
    this->Add(box);

    //關節點
    auto j1 = MakeRef<Joint>();
    j1->Set(floor, box, Vec2(0.0f, 22.0f));
    this->AddJoints(j1);
}
void rl::PhysicsWorld::demo3()
{
    this->Clear();

    auto floor = MakeRef<RigidBody2D>(Vec2(0.0f, -10.0f), Vec2(100.0f, 20.0f), MAX_float);
    this->Add(floor);

    auto floor2 = MakeRef<RigidBody2D>(Vec2(-4.0f, 22.0f), Vec2(26.0f, 1.0f), MAX_float);
    floor2->angle = -0.25f;
    this->Add(floor2);

    auto floor3 = MakeRef<RigidBody2D>(Vec2(10.5f, 19.0f), Vec2(0.5f, 2.0f), MAX_float);
    this->Add(floor3);

    auto floor4 = MakeRef<RigidBody2D>(Vec2(4.0f, 14.0f), Vec2(26.0f, 1.0f), MAX_float);
    floor4->angle = 0.25f;
    this->Add(floor4);

    auto floor5 = MakeRef<RigidBody2D>(Vec2(-10.5f, 11.0f), Vec2(0.5f, 3.0f), MAX_float);
    this->Add(floor5);

    auto floor6 = MakeRef<RigidBody2D>(Vec2(-4.0f, 6.0f), Vec2(26.0f, 1.0f), MAX_float);
    floor6->angle = -0.25f;
    this->Add(floor6);

    float friction[5] = {0.75f, 0.5f, 0.35f, 0.1f, 0.0f};
    for (int i = 0; i < 5; ++i)
    {
        auto b = MakeRef<RigidBody2D>(Vec2(-15.0f + 4.0f * i, 30), Vec2(1.0f, 1.0f), 10);
        b->friction = friction[i];
        this->Add(b);
    }
}
void rl::PhysicsWorld::demo4()
{
    this->Clear();
    auto floor = MakeRef<RigidBody2D>(Vec2(0.0f, -10.0f), Vec2(100.0f, 20.0f), MAX_float);
    floor->friction = 1.0f;
    this->Add(floor);

    for (int i = 0; i < 10; ++i)
    {
//        float x = Random(-0.1f, 0.1f);
        auto box = MakeRef<RigidBody2D>(Vec2(0, 5.1f + 10.5f * i), Vec2(5.0f, 5.0f), 10);
        box->friction = 0.2f;
        this->Add(box);
    }
}
void rl::PhysicsWorld::demo5()
{
    this->Clear();
    auto floor = MakeRef<RigidBody2D>(Vec2(0.0f, -10.0f), Vec2(100.0f, 20.0f), MAX_float);
    floor->friction = 1.0f;
    this->Add(floor);
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 10-j; ++i) {
            float x_idx = 7.5;
            float y_idx = 5.5;
            auto box = MakeRef<RigidBody2D>(Vec2(-35.0 + (x_idx/2 * j) + x_idx * i, 13.0 + y_idx * j), Vec2(5,5), 10);
            box->friction = 0.5;
            this->Add(box);
        }
    }
}
void rl::PhysicsWorld::demo6()
{
    this->Clear();
    //floor
    auto floor = MakeRef<RigidBody2D>(Vec2(0.0f, -10.0f), Vec2(100.0f, 20.0f), MAX_float);
    floor->friction = 1.0f;
    this->Add(floor);

    //teer
    auto teer = MakeRef<RigidBody2D>(Vec2(0.0f, 5.0f), Vec2(30.0f, 1.0f), 100);
    this->Add(teer);

    auto box = MakeRef<RigidBody2D>(Vec2(-10.0f, 10.0f), Vec2(1,1), 1);
    box->friction = 0.5;
    this->Add(box);

    auto box2 = MakeRef<RigidBody2D>(Vec2(-8.0f, 10.0f), Vec2(1,1), 1);
    box2->friction = 0.5;
    this->Add(box2);

    auto box3 = MakeRef<RigidBody2D>(Vec2(10.0f, 20.0f), Vec2(5,5), 1000);
    box3->friction = 0.5;
    this->Add(box3);

    auto j = MakeRef<Joint>();
    j->Set(floor, teer, Vec2(0.0f, 5.0f));
    this->AddJoints(j);
}
void rl::PhysicsWorld::demo7()
{
    this->Clear();
    //floor
    auto floor = MakeRef<RigidBody2D>(Vec2(0.0f, -10.0f), Vec2(100.0f, 20.0f), MAX_float);
    floor->friction = 1.0f;
    this->Add(floor);

    const int numPlanks = 15;
    float mass = 50.0f;

    for (int i = 0; i < numPlanks; ++i)
    {
        auto plank = MakeRef<RigidBody2D>(Vec2(-17 + 2.5 * i, 10.0f), Vec2(2.0f,0.5f), 10);
        plank->friction = 0.2f;
        this->Add(plank);
    }

    // Tuning
    float frequencyHz = 2.0f;
    float dampingRatio = 0.7f;

    // frequency in radians
    float omega = 2.0f * M_PI * frequencyHz;

    // damping coefficient
    float d = 2.0f * mass * dampingRatio * omega;

    // spring stifness
    float k = mass * omega * omega;

    // magic formulas
    float softness = 1.0f / (d + this->timeStep * k);
    float biasFactor = this->timeStep * k / (d + this->timeStep * k);

    for (int i = 0; i < numPlanks ; ++i)
    {
        auto j1 = MakeRef<Joint>();
        j1->Set(this->bodies.at(i), this->bodies.at(i+1), Vec2(-18.125f + 2.5f * i, 10.0f));
        j1->softness = softness;
        j1->biasFactor = biasFactor;
        this->AddJoints(j1);
    }

    auto j2 = MakeRef<Joint>();
    j2->Set(floor , this->bodies.at(numPlanks), Vec2(-18.125f + 2.5f * numPlanks, 10.0f));
    j2->softness = softness;
    j2->biasFactor = biasFactor;
    this->AddJoints(j2);
}
void rl::PhysicsWorld::demo8()
{
    this->Clear();
    //floor
    auto floor = MakeRef<RigidBody2D>(Vec2(0.0f, -10.0f), Vec2(100.0f, 20.0f), MAX_float);
    floor->friction = 1.0f;
    this->Add(floor);

    auto plat = MakeRef<RigidBody2D>(Vec2(-9.0f, 20.0f), Vec2(28.0f, 1.0f), MAX_float);
    plat->friction = 1.0f;
    this->Add(plat);

    auto pend = MakeRef<RigidBody2D>(Vec2(-27.5f, 10.0f), Vec2(2.0f, 20.0f), MAX_float);
    pend->friction = 1.0f;
    this->Add(pend);

    auto box = MakeRef<RigidBody2D>(Vec2(-35.5f, 30.0f), Vec2(1.0f, 1.0f), 10);
    this->Add(box);
    auto j = MakeRef<Joint>();
    j->Set(pend, box, Vec2(-27.5f, 30.0f));
    this->AddJoints(j);

    for (int i = 0; i < 10; ++i)
    {
        auto domino = MakeRef<RigidBody2D>(Vec2(-20.0f + 2.25f * i, 30.0f), Vec2(1.0f, 10.0f), 10);
        domino->friction = 0.1f;
        this->Add(domino);
    }

    auto skew = MakeRef<RigidBody2D>(Vec2(3.0f, 12.5f), Vec2(40.0f, 1.0f), MAX_float);
    skew->angle = 0.25;
    this->Add(skew);

    auto teeter = MakeRef<RigidBody2D>(Vec2(-7, 2.5), Vec2(37.5, 1), 100);
    this->Add(teeter);

    auto j2 = MakeRef<Joint>();
    j2->Set(floor, teeter, Vec2(-7, 2.5));
    this->AddJoints(j2);

    auto floor_box = MakeRef<RigidBody2D>(Vec2(13.5, 6.5), Vec2(5, 5), 10);
    this->Add(floor_box);

    auto floor_box_cap = MakeRef<RigidBody2D>(Vec2(13.5, 9.5), Vec2(5.0, 1.0), 10);
    this->Add(floor_box_cap);

    auto j3 = MakeRef<Joint>();
    j3->Set(floor_box_cap, floor_box, Vec2(16, 9));
    this->AddJoints(j3);

    auto j4 = MakeRef<Joint>();
    j4->Set(floor, floor_box, Vec2(13.5, 6.5));
    this->AddJoints(j4);
}
void rl::PhysicsWorld::demo9()
{
    this->Clear();
    //floor
    auto floor = MakeRef<RigidBody2D>(Vec2(0.0f, -10.0f), Vec2(100.0f, 20.0f), MAX_float);
    floor->friction = 1.0f;
    this->Add(floor);

    float mass = 10.0f;
    const int numPlanks = 15;
    // Tuning
    float frequencyHz = 4.0f;
    float dampingRatio = 0.7f;

    // frequency in radians
    float omega = 2.0f * M_PI * frequencyHz;

    // damping coefficient
    float d = 2.0f * mass * dampingRatio * omega;

    // spring stiffness
    float k = mass * omega * omega;

    // magic formulas
    float softness = 1.0f / (d + timeStep * k);
    float biasFactor = timeStep * k / (d + timeStep * k);

    const float y = 12.0f;

    for (int i = 0; i < numPlanks; ++i)
    {
        auto box = MakeRef<RigidBody2D>(Vec2(0.5 + i * 2.0, 20.0f), Vec2(1.75f, 1.0f), 10);
        box->friction = 0.2f;
        box->angle = 0.0f;
        this->Add(box);
    }

    for (int i = 0; i < numPlanks ; ++i)
    {
        auto j1 =MakeRef<Joint>();
        j1->Set(this->bodies.at(i), this->bodies.at(i+1), Vec2( i * 2.0, 20.0f));
        j1->softness = softness;
        j1->biasFactor = biasFactor;
        this->AddJoints(j1);
    }
}

