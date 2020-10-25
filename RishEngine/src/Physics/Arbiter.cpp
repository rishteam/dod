#include <Rish/Physics/Arbiter.h>

namespace rl {

Arbiter::Arbiter(Ref <RigidBody2D> &b1_, Ref <RigidBody2D> &b2_)
{
    if (b1_ < b2_)
    {
        body1 = b1_;
        body2 = b2_;
    }
    else
    {
        body1 = b2_;
        body2 = b1_;
    }

    numContacts = calContactPoints(this->contacts, body1, body2);

    friction = sqrtf(body1->getFriction() * body2->getFriction());
}

//預備好所需要的參數，包含k, delta_v, n, bias
void Arbiter::preStep(float inv_dt)
{
    // 必須要有個允許穿透值，否則在沒有穿透時計算衝量，會有抖動(jitter)發生
    const float k_allowedPenetration = 0.1f;
    // bangar 修正，是一個介於0~1之間的數字，越小則越慢才修正
    float k_biasFactor = rl::PhysicsWorld::positionCorrection ? 0.2f : 0.0f;

    for (int i = 0; i < numContacts; ++i)
    {
        Contact *c = contacts + i;

        Vec2 r1 = c->position - body1->position;
        Vec2 r2 = c->position - body2->position;

        //實際計算出兩接觸點法向量的衝量
        float rn1 = Dot(r1, c->normal);
        float rn2 = Dot(r2, c->normal);
        //算出effective mass
        float kNormal = body1->invMass + body2->invMass;
        kNormal += body1->invI * (Dot(r1, r1) - rn1 * rn1) + body2->invI * (Dot(r2, r2) - rn2 * rn2);
        c->massNormal = 1.0f / kNormal;


        //算出切線的法向量衝量
        Vec2 tangent = Cross(c->normal, 1.0f); // ??
        float rt1 = Dot(r1, tangent);
        float rt2 = Dot(r2, tangent);
        float kTangent = body1->invMass + body2->invMass;
        kTangent += body1->invI * (Dot(r1, r1) - rt1 * rt1) + body2->invI * (Dot(r2, r2) - rt2 * rt2);
        c->massTangent = 1.0f / kTangent;

        // Bias velocity and impulse
        // (bargar / delta_t)
        c->bias = -k_biasFactor * inv_dt * Min(0.0f, c->separation + k_allowedPenetration);

        // Apply normal + friction impulse
        // 修正衝量大小 + 修正方向(能將兩物體最有效分開的向量)，切線衝量大小 + 切線方向修正
        // Euler-method
        if (rl::PhysicsWorld::accumulateImpulses)
        {
            Vec2 P = c->Pn * c->normal + c->Pt * tangent;

            body1->velocity -= body1->invMass * P;
            body1->angularVelocity -= body1->invI * Cross(r1, P);

            body2->velocity += body2->invMass * P;
            body2->angularVelocity += body2->invI * Cross(r2, P);
        }
    }

}

//透過衝量，計算出速度
void Arbiter::applyImpulse()
{
    for (int i = 0; i < numContacts; ++i)
    {
        // Get contact point
        // 撈出接觸點
        Contact *c = contacts + i;

        // 找出直心與接觸點的向量(r1, r2)
        c->r1 = c->position - body1->position;
        c->r2 = c->position - body2->position;

        // 修正衝量的計算 dot(dv, n)/ kn (p.21)

        // Relative velocity at contact
        Vec2 dv = body2->velocity + Cross(body2->angularVelocity, c->r2) - body1->velocity -
                  Cross(body1->angularVelocity, c->r1);

        // Compute normal impulse
        float vn = Dot(dv, c->normal);

        // massNormal = 1 / kn
        float dPn = c->massNormal * (-vn + c->bias);


        // 修正的衝量都必須介於0以上，否則會越陷越深
        // Max(dPn, 0)
        // accumulateImpulses，會考慮前一幀的衝量，加上要修正的變化量
        if (rl::PhysicsWorld::accumulateImpulses)
        {
            // Clamp the accumulated impulse
            float Pn0 = c->Pn; // old
            c->Pn = Max(Pn0 + dPn, 0.0f); // new clamp 0
            dPn = c->Pn - Pn0; // new - old
        }
        else
        {
            dPn = Max(dPn, 0.0f);
        }

        // -----------以下是Apply to Velocity--------------
        // Apply contact impulse to Velocity

        // 以下是正向部分
        Vec2 Pn = dPn * c->normal; // impluse with direction

        body1->velocity -= body1->invMass * Pn;
        body1->angularVelocity -= body1->invI * Cross(c->r1, Pn);

        body2->velocity += body2->invMass * Pn;
        body2->angularVelocity += body2->invI * Cross(c->r2, Pn);

        // Relative velocity at contact
        // 更新兩物體的相對速度(線性、角速度)，丟給切線方向計算
        dv = body2->velocity + Cross(body2->angularVelocity, c->r2) - body1->velocity -
             Cross(body1->angularVelocity, c->r1);

        // 以下是切線部分
        Vec2 tangent = Cross(c->normal, 1.0f); // why 1.f?
        // dv * n(tangent)
        float vt = Dot(dv, tangent);
        // massTangent = 1 / kn(tangent)
        float dPt = c->massTangent * (-vt);

        if (rl::PhysicsWorld::accumulateImpulses)
        {
            // Compute friction impulse
            float maxPt = friction * c->Pn;
            // Clamp friction
            // 摩擦力後的衝量會介於-uPt ~ + uPt之間
            float oldTangentImpulse = c->Pt;
            c->Pt = Clamp(oldTangentImpulse + dPt, -maxPt, maxPt);
            dPt = c->Pt - oldTangentImpulse;
        }
        else
        {
            float maxPt = friction * dPn;
            dPt = Clamp(dPt, -maxPt, maxPt);
        }

        // Apply contact impulse
        Vec2 Pt = dPt * tangent;

        body1->velocity -= body1->invMass * Pt;
        body1->angularVelocity -= body1->invI * Cross(c->r1, Pt);

        body2->velocity += body2->invMass * Pt;
        body2->angularVelocity += body2->invI * Cross(c->r2, Pt);
    }
}

// update the arbiter and calculate the pulse
// 更新arbiter中contact point的數量，以及其中的衝量大小
void Arbiter::updateContactAndCalImpluse(Contact *contacts, int numContacts)
{
    Contact mergedContacts[2];

    for (int i = 0; i < numContacts; i++)
    {
        Contact *cNew = contacts + i;
        int k = -1;
        // 如果前一幀與這一幀的feature value不一樣，才更新下一幀的速率
        for (int j = 0; j < numContacts; ++j)
        {
            Contact *cOld = contacts + j;
            if (cNew->feature.value == cOld->feature.value)
            {
                k = j;
                break;
            }
        }

        if (k > -1)
        {
            Contact *c = mergedContacts + i;
            Contact *cOld = contacts + k;
            *c = *cNew;
            if (rl::PhysicsWorld::warmStarting)
            {
                c->Pn = cOld->Pn;
                c->Pt = cOld->Pt;
                c->Pnb = cOld->Pnb;
            }
            else
            {
                c->Pn = 0.0f;
                c->Pt = 0.0f;
                c->Pnb = 0.0f;
            }
        }
        else
        {
            mergedContacts[i] = contacts[i];
        }
    }

    for (int i = 0; i < numContacts; ++i)
        this->contacts[i] = mergedContacts[i];

    this->numContacts = numContacts;
}

//計算接觸點(V-clip算法)
int Arbiter::calContactPoints(Contact *contacts, Ref <RigidBody2D> &b1,
                              Ref <RigidBody2D> &b2)
{
    //setup
    Vec2 hA = Vec2(b1->wh.x * 0.5, b1->wh.y * 0.5);
    Vec2 hB = Vec2(b2->wh.x * 0.5, b2->wh.y * 0.5);


    Vec2 posA = Vec2(b1->position.x, b1->position.y);
    Vec2 posB = Vec2(b2->position.x, b2->position.y);

    //Rotate Matrix
    //Mat22 RotA(degreesToRadians(360 - bodyA->getRotation())), RotB(degreesToRadians(360 -bodyB->getRotation()));
    Mat22 RotA(b1->getPhysicsData().second), RotB(b2->getPhysicsData().second);
    //Transpose Matrix
    Mat22 RotAT = RotA.Transpose();
    Mat22 RotBT = RotB.Transpose();

    //delta position
    //red-vector
    Vec2 dp = posB - posA;
    //ch
    Vec2 dA = RotAT * dp;
    Vec2 dB = RotBT * dp;


    Mat22 C = RotAT * RotB;
    Mat22 absC = Abs(C);
    Mat22 absCT = absC.Transpose();

    // magic formula 檢測SAT
    // source: https://stackoverflow.com/questions/40047219/in-box2d-lite-i-cant-figure-out-what-the-box-a-faces-and-box-b-faces-check?answertab=oldest#tab-top

    // Box A faces
    Vec2 faceA = Abs(dA) - hA - absC * hB;

    // Box B faces
    Vec2 faceB = Abs(dB) - absCT * hA - hB;

    // Find best axis
    Axis axis;
    float separation;
    Vec2 normal;


    // 證明: https://www.randygaul.net/2013/03/28/custom-physics-engine-part-2-manifold-generation/
    // 找出最適合的最小穿透軸
    axis = FACE_A_X;
    separation = faceA.x;
    normal = dA.x > 0.0f ? RotA.col1 : -RotA.col1;

    //magic-formula 決定最小穿透軸
    const float relativeTol = 0.95f;
    const float absoluteTol = 0.01f;
    if (faceA.y > relativeTol * separation + absoluteTol * hA.y)
    {
        axis = FACE_A_Y;
        separation = faceA.y;
        normal = dA.y > 0.0f ? RotA.col2 : -RotA.col2;
    }


    if (faceB.x > relativeTol * separation + absoluteTol * hB.x)
    {
        axis = FACE_B_X;
        separation = faceB.x;
        normal = dB.x > 0.0f ? RotB.col1 : -RotB.col1;
    }

    if (faceB.y > relativeTol * separation + absoluteTol * hB.y)
    {
        axis = FACE_B_Y;
        separation = faceB.y;
        normal = dB.y > 0.0f ? RotB.col2 : -RotB.col2;
    }

    // Setup clipping plane data based on the separating axis
    Vec2 frontNormal, sideNormal;
    //與人碰撞的edge
    // Vec2 v; (點點)
    FeaturePair fp; //(點點夾的兩邊)
    ClipVertex incidentEdge[2];
    float front, negSide, posSide;
    char negEdge, posEdge;

    // Compute the clipping lines and the line segment to be clipped.
    // 計算出各種的直線方程式
    switch (axis)
    {
    case FACE_A_X:
    {
        //reference face vector
        frontNormal = normal;
        front = Dot(posA, frontNormal) + hA.x;
        sideNormal = RotA.col2;
        // dot product is to compute c (ax + by)
        float side = Dot(posA, sideNormal);
        // side
        negSide = -side + hA.y;
        posSide = side + hA.y;
        negEdge = EDGE3;
        posEdge = EDGE1;
        ComputeIncidentEdge(incidentEdge, hB, posB, RotB, frontNormal);
    }
        break;

    case FACE_A_Y:
    {
        frontNormal = normal;
        front = Dot(posA, frontNormal) + hA.y;
        sideNormal = RotA.col1;
        float side = Dot(posA, sideNormal);
        negSide = -side + hA.x;
        posSide = side + hA.x;
        negEdge = EDGE2;
        posEdge = EDGE4;
        ComputeIncidentEdge(incidentEdge, hB, posB, RotB, frontNormal);
    }
        break;

    case FACE_B_X:
    {
        frontNormal = -normal;
        front = Dot(posB, frontNormal) + hB.x;
        sideNormal = RotB.col2;
        float side = Dot(posB, sideNormal);
        negSide = -side + hB.y;
        posSide = side + hB.y;
        negEdge = EDGE3;
        posEdge = EDGE1;
        ComputeIncidentEdge(incidentEdge, hA, posA, RotA, frontNormal);
    }
        break;

    case FACE_B_Y:
    {
        frontNormal = -normal;
        front = Dot(posB, frontNormal) + hB.y;
        sideNormal = RotB.col1;
        float side = Dot(posB, sideNormal);
        negSide = -side + hB.x;
        posSide = side + hB.x;
        negEdge = EDGE2;
        posEdge = EDGE4;
        ComputeIncidentEdge(incidentEdge, hA, posA, RotA, frontNormal);
    }
        break;
    }

    // clip other face with 5 box planes (1 face plane, 4 edge planes)

    ClipVertex clipPoints1[2];
    ClipVertex clipPoints2[2];
    int np = 0;

    // Clip to box side 1
    // 先測試negtive-side，決定候選點
    np = ClipSegmentToLine(clipPoints1, incidentEdge, -sideNormal, negSide, negEdge);
    if (np < 2)
        return 0;

    // Clip to negative box side 1
    // 再放入positive-side，進行第二次的過濾
    np = ClipSegmentToLine(clipPoints2, clipPoints1, sideNormal, posSide, posEdge);
    if (np < 2)
        return 0;

    // Now clipPoints2 contains the clipping points.
    // Due to roundoff, it is possible that clipping removes all points.

    int numContacts_ = 0;
    for (int i = 0; i < 2; ++i)
    {
        float separation = Dot(frontNormal, clipPoints2[i].v) - front;

        if (separation <= 0)
        {
            contacts[numContacts_].separation = separation;
            contacts[numContacts_].normal = normal;
            // slide contact point onto reference face (easy to cull)
            contacts[numContacts_].position = clipPoints2[i].v - separation * frontNormal;
            contacts[numContacts_].feature = clipPoints2[i].fp;

            if (axis == FACE_B_X || axis == FACE_B_Y)
                Flip(contacts[numContacts_].feature);
            ++numContacts_;
        }
    }
    return numContacts_;
}

} // end of namespace rl