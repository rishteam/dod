#include <Rish/Collider/Shape.h>
#include <Rish/Collider/Box.h>
#include <Rish/Collider/Circle.h>
#include <Rish/Collider/Polygon.h>
#include <Rish/Physics/Arbiter.h>

namespace rl {

typedef bool (*CollisionCallback)(Arbiter* arb, Ref<Shape> a, Ref<Shape> b);

static float FindAxisLeastPenetration(int *faceIdx, Ref<Box> A, Ref<Box> B)
{
    float bestDistance = -FLT_MAX;
    int bestIndex;

    for (int i = 0; i < A->m_vertexCount; ++i) {
        // Retrieve a face normal from A
        // n 為reference face
        Vec2 n = A->m_normals[i];
        // nw = n World Space
        Vec2 nw = A->u * n;

        // Transform face normal into B's model space
        Mat22 buT = B->u.Transpose();
        // n變成 n 在 b 的local space
        n = buT * nw;

        // Retrieve support point from B along -n
        // s 離-n的最遠點
        Vec2 s = B->GetSupport(-n);

        // Retrieve vertex on face from A, transform into
        // B's model space
        Vec2 v = A->m_vertices[i];

        v = A->u * v + A->position;
        // v為A的目前枚舉到的一點，並轉為world space
        v -= B->position;
        v = buT * v;

        // Compute penetration distance (in B's model space)
        float d = Dot(n, s - v);

        // Store greatest distance
        if (d > bestDistance) {
            bestDistance = d;
            bestIndex = i;
        }
    }

    *faceIdx = bestIndex;
    return bestDistance;
}

static float FindAxisLeastPenetration(int *faceIdx, Ref<Polygon> A, Ref<Polygon> B)
{
    float bestDistance = -FLT_MAX;
    int bestIndex;

    for (int i = 0; i < A->m_vertexCount; ++i) {
        // Retrieve a face normal from A
        // n 為reference face
        Vec2 n = A->m_normals[i];
        // nw = n World Space
        Vec2 nw = A->u * n;

        // Transform face normal into B's model space
        Mat22 buT = B->u.Transpose();
        // n變成 n 在 b 的local space
        n = buT * nw;

        // Retrieve support point from B along -n
        // s 離-n的最遠點
        Vec2 s = B->GetSupport(-n);

        // Retrieve vertex on face from A, transform into
        // B's model space
        Vec2 v = A->m_vertices[i];

        v = A->u * v + A->position;
        // v為A的目前枚舉到的一點，並轉為world space
        v -= B->position;
        v = buT * v;

        // Compute penetration distance (in B's model space)
        float d = Dot(n, s - v);

        // Store greatest distance
        if (d > bestDistance) {
            bestDistance = d;
            bestIndex = i;
        }
    }

    *faceIdx = bestIndex;
    return bestDistance;
}

static float FindAxisLeastPenetration(int *faceIdx, Ref<Box> A, Ref<Polygon> B)
{
    float bestDistance = -FLT_MAX;
    int bestIndex;

    for (int i = 0; i < A->m_vertexCount; ++i) {
        // Retrieve a face normal from A
        // n 為reference face
        Vec2 n = A->m_normals[i];
        // nw = n World Space
        Vec2 nw = A->u * n;

        // Transform face normal into B's model space
        Mat22 buT = B->u.Transpose();
        // n變成 n 在 b 的local space
        n = buT * nw;

        // Retrieve support point from B along -n
        // s 離-n的最遠點
        Vec2 s = B->GetSupport(-n);

        // Retrieve vertex on face from A, transform into
        // B's model space
        Vec2 v = A->m_vertices[i];

        v = A->u * v + A->position;
        // v為A的目前枚舉到的一點，並轉為world space
        v -= B->position;
        v = buT * v;

        // Compute penetration distance (in B's model space)
        float d = Dot(n, s - v);

        // Store greatest distance
        if (d > bestDistance) {
            bestDistance = d;
            bestIndex = i;
        }
    }

    *faceIdx = bestIndex;
    return bestDistance;
}

static float FindAxisLeastPenetration(int *faceIdx, Ref<Polygon> A, Ref<Box> B)
{
    float bestDistance = -FLT_MAX;
    int bestIndex;

    for (int i = 0; i < A->m_vertexCount; ++i) {
        // Retrieve a face normal from A
        // n 為reference face
        Vec2 n = A->m_normals[i];
        // nw = n World Space
        Vec2 nw = A->u * n;

        // Transform face normal into B's model space
        Mat22 buT = B->u.Transpose();
        // n變成 n 在 b 的local space
        n = buT * nw;

        // Retrieve support point from B along -n
        // s 離-n的最遠點
        Vec2 s = B->GetSupport(-n);

        // Retrieve vertex on face from A, transform into
        // B's model space
        Vec2 v = A->m_vertices[i];

        v = A->u * v + A->position;
        // v為A的目前枚舉到的一點，並轉為world space
        v -= B->position;
        v = buT * v;

        // Compute penetration distance (in B's model space)
        float d = Dot(n, s - v);

        // Store greatest distance
        if (d > bestDistance) {
            bestDistance = d;
            bestIndex = i;
        }
    }

    *faceIdx = bestIndex;
    return bestDistance;
}

static void FindIncidentFace(Vec2 *v, Ref<Shape> Refer, Ref<Shape> Inc, int refIdx)
{
    Vec2 referenceNormal = Refer->m_normals[refIdx];

    // Calculate normal in incident's frame of reference
    referenceNormal = Refer->u * referenceNormal; // To world space
    referenceNormal = Inc->u.Transpose() * referenceNormal; // To incident's model space

    // Find most anti-normal face on incident polygon
    int incidentFace = 0;
    float minDot = FLT_MAX;
    for (int i = 0; i < Inc->m_vertexCount; ++i) {
        float dot = Dot(referenceNormal, Inc->m_normals[i]);
        if (dot < minDot) {
            minDot = dot;
            incidentFace = i;
        }
    }

    // Assign face vertices for incidentFace
    v[0] = Inc->u * Inc->m_vertices[incidentFace] + Inc->position;
    incidentFace = incidentFace + 1 >= (int) Inc->m_vertexCount ? 0 : incidentFace + 1;
    v[1] = Inc->u * Inc->m_vertices[incidentFace] + Inc->position;
}

static int Clip(Vec2 n, float c, Vec2 *face)
{
    int sp = 0;
    Vec2 out[2] = {
            face[0],
            face[1]
    };

    // Retrieve distances from each endpoint to the line
    // d = ax + by - c
    float d1 = Dot(n, face[0]) - c;
    float d2 = Dot(n, face[1]) - c;

    // If negative (behind plane) clip
    if (d1 <= 0.0f) out[sp++] = face[0];
    if (d2 <= 0.0f) out[sp++] = face[1];

    // If the points are on different sides of the plane
    // 兩點跨平面，求交點
    if (d1 * d2 < 0.0f) // less than to ignore -0.0f
    {
        // Push interesection point
        float alpha = d1 / (d1 - d2);
        out[sp] = face[0] + alpha * (face[1] - face[0]);
        ++sp;
    }

    // Assign our new converted values
    face[0] = out[0];
    face[1] = out[1];

    return sp;
}

static bool BiasGreaterThan(float a, float b)
{
    const float k_biasRelative = 0.95f;
    const float k_biasAbsolute = 0.01f;
    return a >= b * k_biasRelative + a * k_biasAbsolute;
}

// Collision All Case
static bool Box2Box(Arbiter *arb, Ref<Shape> a, Ref<Shape> b)
{
    Ref<Box> box1 = std::dynamic_pointer_cast<Box>(a);
    Ref<Box> box2 = std::dynamic_pointer_cast<Box>(b);

    arb->contactCounter = 0;
    box1->setMatrix(box1->angle);
    box2->setMatrix(box2->angle);

    // Check for a separating axis with A's face planes
    int faceA;
    float penetrationA = FindAxisLeastPenetration(&faceA, box1, box2);
    if (penetrationA >= 0.0f)
        return false;

    // Check for a separating axis with B's face planes
    int faceB;
    float penetrationB = FindAxisLeastPenetration(&faceB, box2, box1);
    if (penetrationB >= 0.0f)
        return false;

    int referenceIndex;
    // Always point from a to b
    // 設定為 A 指向 B，若沒符合則flip
    bool flip = false;

    Ref<Shape> RefPoly; // Reference
    Ref<Shape> IncPoly; // Incident

    // Determine which shape contains reference face
    if (BiasGreaterThan(penetrationA, penetrationB)) {
        RefPoly = box1;
        IncPoly = box2;
        referenceIndex = faceA;
        flip = false;
    } else {
        RefPoly = box2;
        IncPoly = box1;
        referenceIndex = faceB;
        flip = true;
    }

    // World space incident face
    Vec2 incidentFace[2];

    FindIncidentFace(incidentFace, RefPoly, IncPoly, referenceIndex);

    //        y
    //        ^  ->n       ^
    //      +---c ------posPlane--
    //  x < | i |\
          //      +---+ c-----negPlane--
    //             \       v
    //              r
    //
    //  r : reference face
    //  i : incident poly
    //  c : clipped point
    //  n : incident normal

    // Setup reference face vertices
    // 利用點紀錄reference face
    Vec2 v1 = RefPoly->m_vertices[referenceIndex];
    referenceIndex = referenceIndex + 1 == RefPoly->m_vertexCount ? 0 : referenceIndex + 1;
    Vec2 v2 = RefPoly->m_vertices[referenceIndex];

    // Transform vertices to world space
    v1 = RefPoly->u * v1 + RefPoly->position;
    v2 = RefPoly->u * v2 + RefPoly->position;

    // Calculate reference face side normal in world space
    Vec2 sidePlaneNormal = (v2 - v1);
    // to Normalize vector (轉成單位向量)
    sidePlaneNormal.Normalize();

    // Orthogonalize
    // n 單位向量
    Vec2 refFaceNormal(sidePlaneNormal.y, -sidePlaneNormal.x);

    // ax + by = c
    // c is distance from origin
    float refC = Dot(refFaceNormal, v1);
    float negSide = Dot(-sidePlaneNormal, v1);
    float posSide = Dot(sidePlaneNormal, v2);

    // Clip incident face to reference face side planes
    if (Clip(-sidePlaneNormal, negSide, incidentFace) < 2)
        return false; // Due to floating point error, possible to not have required points

    if (Clip(sidePlaneNormal, posSide, incidentFace) < 2)
        return false; // Due to floating point error, possible to not have required points

    // Flip
    arb->normal = flip ? -refFaceNormal : refFaceNormal;

    // 透過clip截斷點，incidentFace
    // Keep points behind reference face
    int cp = 0; // clipped points behind reference face
    float separation = Dot(refFaceNormal, incidentFace[0]) - refC;
    if (separation <= 0.0f) {
        arb->contacts[cp].position = incidentFace[0];
        arb->penetration = -separation;
        ++cp;
    } else {
        arb->penetration = 0;
    }

    separation = Dot(refFaceNormal, incidentFace[1]) - refC;
    if (separation <= 0.0f) {
        arb->contacts[cp].position = incidentFace[1];
        arb->penetration += -separation;
        ++cp;

        // Average penetration
        arb->penetration /= (float) cp;
    }

    arb->contactCounter = cp;
    return true;
}

static bool Circle2Circle(Arbiter *arb, Ref<Shape> a, Ref<Shape> b)
{
    Ref<Circle> cir1 = std::dynamic_pointer_cast<Circle>(b);
    Ref<Circle> cir2 = std::dynamic_pointer_cast<Circle>(a);
    // n
    Vec2 normal = cir1->position - cir2->position;

    float dist_sqr = normal.LenSqr();
    float radius = cir1->radius + cir2->radius;

    // not Collision
    if (dist_sqr >= radius * radius) {
        arb->contactCounter = 0;
        return false;
    }

    float distance = std::sqrt(dist_sqr);

    arb->contactCounter = 1;

    // Overlap
    if (distance == 0.0f) {
        arb->penetration = cir1->radius;
        arb->normal = Vec2(1, 0);
        arb->contacts[0].position = cir1->position;
    }
        //
    else {
        arb->penetration = radius - distance;
        // Faster than using Normalized since we already performed sqrt
        arb->normal = normal / distance;
        arb->contacts[0].position = arb->normal * cir1->radius  + cir2->position;
    }

    return true;
}

static bool Polygon2Polygon(Arbiter *arb, Ref<Shape> a, Ref<Shape> b)
{
    Ref<Polygon> poly1 = std::dynamic_pointer_cast<Polygon>(a);
    Ref<Polygon> poly2 = std::dynamic_pointer_cast<Polygon>(b);

    arb->contactCounter = 0;
    poly1->setMatrix(poly1->angle);
    poly2->setMatrix(poly2->angle);

    // Check for a separating axis with A's face planes
    int faceA = 0;
    float penetrationA = FindAxisLeastPenetration(&faceA, poly1, poly2);
    if (penetrationA >= 0.0f)
        return false;

    // Check for a separating axis with B's face planes
    int faceB;
    float penetrationB = FindAxisLeastPenetration(&faceB, poly2, poly1);
    if (penetrationB >= 0.0f)
        return false;

    int referenceIndex;
    // Always point from a to b
    // 設定為 A 指向 B，若沒符合則flip
    bool flip = false;

    Ref<Polygon> RefPoly; // Reference
    Ref<Polygon> IncPoly; // Incident

    // Determine which shape contains reference face
    if (BiasGreaterThan(penetrationA, penetrationB)) {
        RefPoly = poly1;
        IncPoly = poly2;
        referenceIndex = faceA;
        flip = false;
    } else {
        RefPoly = poly2;
        IncPoly = poly1;
        referenceIndex = faceB;
        flip = true;
    }

    // World space incident face
    Vec2 incidentFace[2];

    FindIncidentFace(incidentFace, RefPoly, IncPoly, referenceIndex);
    //        y
    //        ^  ->n       ^
    //      +---c ------posPlane--
    //  x < | i |\
  //      +---+ c-----negPlane--
    //             \       v
    //              r
    //
    //  r : reference face
    //  i : incident poly
    //  c : clipped point
    //  n : incident normal

    // Setup reference face vertices
    // 利用點紀錄reference face
    Vec2 v1 = RefPoly->m_vertices[referenceIndex];
    referenceIndex = referenceIndex + 1 == RefPoly->m_vertexCount ? 0 : referenceIndex + 1;
    Vec2 v2 = RefPoly->m_vertices[referenceIndex];

    // Transform vertices to world space
    v1 = RefPoly->u * v1 + RefPoly->position;
    v2 = RefPoly->u * v2 + RefPoly->position;

    // Calculate reference face side normal in world space
    Vec2 sidePlaneNormal = (v2 - v1);
    // to Normalize vector (轉成單位向量)
    sidePlaneNormal.Normalize();

    // Orthogonalize
    // n 單位向量
    Vec2 refFaceNormal(sidePlaneNormal.y, -sidePlaneNormal.x);

    // ax + by = c
    // c is distance from origin
    float refC = Dot(refFaceNormal, v1);
    float negSide = Dot(-sidePlaneNormal, v1);
    float posSide = Dot(sidePlaneNormal, v2);

    // Clip incident face to reference face side planes
    if (Clip(-sidePlaneNormal, negSide, incidentFace) < 2)
        return false; // Due to floating point error, possible to not have required points

    if (Clip(sidePlaneNormal, posSide, incidentFace) < 2)
        return false; // Due to floating point error, possible to not have required points

    // Flip
    arb->normal = flip ? -refFaceNormal : refFaceNormal;

    // 透過clip截斷點，incidentFace
    // Keep points behind reference face
    int cp = 0; // clipped points behind reference face
    float separation = Dot(refFaceNormal, incidentFace[0]) - refC;
    if (separation <= 0.0f) {
        arb->contacts[cp].position = incidentFace[0];
        arb->penetration = -separation;
        ++cp;
    } else {
        arb->penetration = 0;
    }
    separation = Dot(refFaceNormal, incidentFace[1]) - refC;
    if (separation <= 0.0f) {
        arb->contacts[cp].position = incidentFace[1];
        arb->penetration += -separation;
        ++cp;

        // Average penetration
        arb->penetration /= (float) cp;
    }

    arb->contactCounter = cp;
    return true;
}

static bool Circle2Box(Arbiter *arb, Ref<Shape> a, Ref<Shape> b)
{
    Ref<Box> box1 = std::dynamic_pointer_cast<Box>(b);
    Ref<Circle> c = std::dynamic_pointer_cast<Circle>(a);

    arb->contactCounter = 0;
    box1->setMatrix(box1->angle);
    c->setMatrix(c->angle);

    // Transform circle center to Polygon model space
    // 找最小穿透軸
    Vec2 center = c->position;
    center = box1->u.Transpose() * (center - box1->position);

    // Find edge with minimum penetration
    // Exact concept as using support points in Polygon vs Polygon
    float separation = -FLT_MAX;
    int faceNormal = 0;
    for (int i = 0; i < box1->m_vertexCount; ++i) {
        float s = Dot(box1->m_normals[i], center - box1->m_vertices[i]);
        if (s > c->radius )
            return false;
        if (s > separation) {
            separation = s;
            faceNormal = i;
        }
    }

    // Grab face's vertices
    // 用點決定Reference Face
    Vec2 v1 = box1->m_vertices[faceNormal];
    int i2 = faceNormal + 1 < box1->m_vertexCount ? faceNormal + 1 : 0;
    Vec2 v2 = box1->m_vertices[i2];

    // Check to see if center is within polygon
    if (separation < EPSILON) {
        arb->contactCounter = 1;
        arb->normal = -(box1->u * box1->m_normals[faceNormal]);
        arb->contacts[0].position = arb->normal * c->radius  + c->position;
        arb->penetration = c->radius;
        return true;
    }

    // Determine which voronoi region of the edge center of circle lies within
    float dot1 = Dot(center - v1, v2 - v1);
    float dot2 = Dot(center - v2, v1 - v2);
    arb->penetration = c->radius - separation;

    // Closest to v1
    // 靠近v1
    if (dot1 <= 0.0f) {
        // 檢查是否分離
        if (DistSqr(center, v1) > c->radius * c->radius)
            return false;

        arb->contactCounter = 1;
        Vec2 n = v1 - center;
        n = box1->u * n;
        n.Normalize();
        arb->normal = n;
        v1 = box1->u * v1 + box1->position;
        arb->contacts[0].position = v1;
    }
        // Closest to v2
        // 靠近v2
    else if (dot2 <= 0.0f) {
        if (DistSqr(center, v2) > c->radius * c->radius)
            return false;

        arb->contactCounter = 1;
        Vec2 n = v2 - center;
        v2 = box1->u * v2 + box1->position;
        arb->contacts[0].position = v2;
        n = box1->u * n;
        n.Normalize();
        arb->normal = n;
    }
        // Closest to face
        // 靠近面
    else {
        Vec2 n = box1->m_normals[faceNormal];
        if (Dot(center - v1, n) > c->radius)
            return false;
        n = box1->u * n;
        arb->normal = -n;
        arb->contacts[0].position = arb->normal * c->radius + c->position;
        arb->contactCounter = 1;
    }
    return true;
}

static bool Box2Circle(Arbiter *arb, Ref<Shape> a, Ref<Shape> b)
{
    Circle2Box(arb, b, a);
    arb->normal = -arb->normal;
}

static bool Circle2Polygon(Arbiter *arb, Ref<Shape> a, Ref<Shape> b)
{
    Ref<Circle> cir = std::dynamic_pointer_cast<Circle>(a);
    Ref<Polygon> poly = std::dynamic_pointer_cast<Polygon>(b);

    arb->contactCounter = 0;
    poly->setMatrix(poly->angle);
    cir->setMatrix(cir->angle);

    // Transform circle center to Polygon model space
    // 找最小穿透軸
    Vec2 center = cir->position;
    center = poly->u.Transpose() * (center - poly->position);

    // Find edge with minimum penetration
    // Exact concept as using support points in Polygon vs Polygon
    float separation = -FLT_MAX;
    int faceNormal = 0;
    for (int i = 0; i < poly->m_vertexCount; ++i) {
        float s = Dot(poly->m_normals[i], center - poly->m_vertices[i]);
        if (s > cir->radius)
            return false;
        if (s > separation) {
            separation = s;
            faceNormal = i;
        }
    }

    // Grab face's vertices
    // 用點決定Reference Face
    Vec2 v1 = poly->m_vertices[faceNormal];
    int i2 = faceNormal + 1 < poly->m_vertexCount ? faceNormal + 1 : 0;
    Vec2 v2 = poly->m_vertices[i2];

    // Check to see if center is within polygon
    if (separation < EPSILON) {
        arb->contactCounter = 1;
        arb->normal = -(poly->u * poly->m_normals[faceNormal]);
        arb->contacts[0].position = arb->normal * cir->radius + cir->position;
        arb->penetration = cir->radius;
        return true;
    }

    // Determine which voronoi region of the edge center of circle lies within
    float dot1 = Dot(center - v1, v2 - v1);
    float dot2 = Dot(center - v2, v1 - v2);
    arb->penetration = cir->radius - separation;

    // Closest to v1
    // 靠近v1
    if (dot1 <= 0.0f) {
        // 檢查是否分離
        if (DistSqr(center, v1) > cir->radius * cir->radius)
            return false;

        arb->contactCounter = 1;
        Vec2 n = v1 - center;
        n = poly->u * n;
        n.Normalize();
        arb->normal = n;
        v1 = poly->u * v1 + poly->position;
        arb->contacts[0].position = v1;
    }

        // Closest to v2
        // 靠近v2
    else if (dot2 <= 0.0f) {
        if (DistSqr(center, v2) > cir->radius * cir->radius)
            return false;

        arb->contactCounter = 1;
        Vec2 n = v2 - center;
        v2 = poly->u * v2 + poly->position;
        arb->contacts[0].position = v2;
        n = poly->u * n;
        n.Normalize();
        arb->normal = n;
    }

        // Closest to face
        // 靠近面
    else {
        Vec2 n = poly->m_normals[faceNormal];
        if (Dot(center - v1, n) > cir->radius)
            return false;
        n = poly->u * n;
        arb->normal = -n;
        arb->contacts[0].position = arb->normal * cir->radius + cir->position;
        arb->contactCounter = 1;
    }
    return true;
}

static bool Polygon2Circle(Arbiter *arb, Ref<Shape> a, Ref<Shape> b)
{
    Circle2Polygon(arb, b, a);
    arb->normal = -arb->normal;
}

static bool Box2Polygon(Arbiter *arb, Ref<Shape> a, Ref<Shape> b)
{
    Ref<Box> box1 = std::dynamic_pointer_cast<Box>(a);
    Ref<Polygon> p = std::dynamic_pointer_cast<Polygon>(b);

    arb->contactCounter = 0;
    box1->setMatrix(box1->angle);
    p->setMatrix(p->angle);

    // Check for a separating axis with A's face planes
    int faceA;
    float penetrationA = FindAxisLeastPenetration(&faceA, box1, p);
    if (penetrationA >= 0.0f)
        return false;

    // Check for a separating axis with B's face planes
    int faceB;
    float penetrationB = FindAxisLeastPenetration(&faceB, p, box1);
    if (penetrationB >= 0.0f)
        return false;

    int referenceIndex;
    // Always point from a to b
    // 設定為 A 指向 B，若沒符合則flip
    bool flip = false;

    Ref<Shape> RefPoly; // Reference
    Ref<Shape> IncPoly; // Incident

    // Determine which shape contains reference face
    if (BiasGreaterThan(penetrationA, penetrationB)) {
        RefPoly = box1;
        IncPoly = p;
        referenceIndex = faceA;
        flip = false;
    } else {
        RefPoly = p;
        IncPoly = box1;
        referenceIndex = faceB;
        flip = true;
    }

    // World space incident face
    Vec2 incidentFace[2];

    FindIncidentFace(incidentFace, RefPoly, IncPoly, referenceIndex);

    //        y
    //        ^  ->n       ^
    //      +---c ------posPlane--
    //  x < | i |\
  //      +---+ c-----negPlane--
    //             \       v
    //              r
    //
    //  r : reference face
    //  i : incident poly
    //  c : clipped point
    //  n : incident normal

    // Setup reference face vertices
    // 利用點紀錄reference face
    Vec2 v1 = RefPoly->m_vertices[referenceIndex];
    referenceIndex = referenceIndex + 1 == RefPoly->m_vertexCount ? 0 : referenceIndex + 1;
    Vec2 v2 = RefPoly->m_vertices[referenceIndex];

    // Transform vertices to world space
    v1 = RefPoly->u * v1 + RefPoly->position;
    v2 = RefPoly->u * v2 + RefPoly->position;

    // Calculate reference face side normal in world space
    Vec2 sidePlaneNormal = (v2 - v1);
    // to Normalize vector (轉成單位向量)
    sidePlaneNormal.Normalize();

    // Orthogonalize
    // n 單位向量
    Vec2 refFaceNormal(sidePlaneNormal.y, -sidePlaneNormal.x);

    // ax + by = c
    // c is distance from origin
    float refC = Dot(refFaceNormal, v1);
    float negSide = Dot(-sidePlaneNormal, v1);
    float posSide = Dot(sidePlaneNormal, v2);

    // Clip incident face to reference face side planes
    if (Clip(-sidePlaneNormal, negSide, incidentFace) < 2)
        return false; // Due to floating point error, possible to not have required points

    if (Clip(sidePlaneNormal, posSide, incidentFace) < 2)
        return false; // Due to floating point error, possible to not have required points

    // Flip
    arb->normal = flip ? -refFaceNormal : refFaceNormal;


    // 透過clip截斷點，incidentFace
    // Keep points behind reference face
    int cp = 0; // clipped points behind reference face
    float separation = Dot(refFaceNormal, incidentFace[0]) - refC;
    if (separation <= 0.0f) {
        arb->contacts[cp].position = incidentFace[0];
        arb->penetration = -separation;
        ++cp;
    } else {
        arb->penetration = 0;
    }

    separation = Dot(refFaceNormal, incidentFace[1]) - refC;
    if (separation <= 0.0f) {
        arb->contacts[cp].position = incidentFace[1];
        arb->penetration = -separation;
        ++cp;

        // Average penetration
        arb->penetration /= (float) cp;
    }

    arb->contactCounter = cp;
    return true;
}

static bool Polygon2Box(Arbiter *arb, Ref<Shape> a, Ref<Shape> b)
{
    Box2Polygon(arb, b, a);
    arb->normal = -arb->normal;
}


extern CollisionCallback Dispatch[3][3] =
{
        {
                Circle2Circle, Circle2Box, Circle2Polygon
        },
        {
                Box2Circle, Box2Box, Box2Polygon
        },
        {
                Polygon2Circle, Polygon2Box, Polygon2Polygon
        },
};


}


