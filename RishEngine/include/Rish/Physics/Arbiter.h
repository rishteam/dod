#pragma once

#include <Rish/Physics/VClip.h>
#include <Rish/Physics/RigidBody2D.h>
#include <Rish/Physics/PhysicsWorld.h>
#include <Rish/Physics/vector_math.h>

//被撞的body2, 撞人的body1
namespace rl {
    struct Contact {
        Vec2 position;
        Vec2 normal;
        Vec2 r1, r2;
        float separation;
        //累加衝量
        float Pn = 0.0f;    // accumulated normal impulse
        //切線方向的衝量
        float Pt = 0.0f;    // accumulated tangent impulse
        //衝量加位置的偏差量
        float Pnb = 0.0f;    // accumulated normal impulse for position bias
        float massNormal, massTangent;
        float bias;
        FeaturePair feature;
    };

    class ArbiterKey {
    public:
        //小的放前面
        ArbiterKey(Ref<RigidBody2D> &b1, Ref<RigidBody2D> &b2) {
            if (b1 < b2) {
                body1 = b1;
                body2 = b2;
            } else {
                body1 = b2;
                body2 = b1;
            }
        }

        ~ArbiterKey() = default;

        Ref<RigidBody2D> body1;
        Ref<RigidBody2D> body2;
    };

    inline bool operator<(const ArbiterKey &a1, const ArbiterKey &a2) {
        if (a1.body1 < a2.body1)
            return true;

        if (a1.body1 == a2.body1 && a1.body2 < a2.body2)
            return true;

        return false;
    }


    enum Axis {
        FACE_A_X,
        FACE_A_Y,
        FACE_B_X,
        FACE_B_Y
    };

    enum EdgeNumbers {
        NO_EDGE = 0,
        EDGE1,
        EDGE2,
        EDGE3,
        EDGE4
    };

    struct ClipVertex {
        ClipVertex() { fp.value = 0; }

        Vec2 v;
        FeaturePair fp;
    };

    static void Flip(FeaturePair &fp) {
        Swap(fp.e.inEdge1, fp.e.inEdge2);
        Swap(fp.e.outEdge1, fp.e.outEdge2);
    }

    // 他在做這件事: https://github.com/phenomLi/Blog/issues/42
    //clipPoints1, incidentEdge, -sideNormal, negSide, negEdge
    static int ClipSegmentToLine(ClipVertex vOut[2], ClipVertex vIn[2],
                                 const Vec2 &normal, float offset, char clipEdge) {
        // Start with no output points
        int numOut = 0;

        // Calculate the distance of end points to the line
        float distance0 = Dot(normal, vIn[0].v) - offset;
        float distance1 = Dot(normal, vIn[1].v) - offset;

        // If the points are behind the plane
        if (distance0 <= 0.0f) vOut[numOut++] = vIn[0];
        if (distance1 <= 0.0f) vOut[numOut++] = vIn[1];

        // If the points are on different sides of the plane
        // 若兩點的位置在面的不同邊，會算出其相交的頂點
        if (distance0 * distance1 < 0.0f) {
            // Find intersection point of edge and plane
            float interp = distance0 / (distance0 - distance1);
            vOut[numOut].v = vIn[0].v + interp * (vIn[1].v - vIn[0].v);
            if (distance0 > 0.0f) {
                vOut[numOut].fp = vIn[0].fp;
                vOut[numOut].fp.e.inEdge1 = clipEdge;
                vOut[numOut].fp.e.inEdge2 = NO_EDGE;
            } else {
                vOut[numOut].fp = vIn[1].fp;
                vOut[numOut].fp.e.outEdge1 = clipEdge;
                vOut[numOut].fp.e.outEdge2 = NO_EDGE;
            }
            ++numOut;
        }

        return numOut;
    }

    // 計算入射的邊
    static void ComputeIncidentEdge(ClipVertex c[2], const Vec2 &h, const Vec2 &pos,
                                    const Mat22 &Rot, const Vec2 &normal) {
        // The normal is from the reference box. Convert it
        // to the incident boxe's frame and flip sign.
        Mat22 RotT = Rot.Transpose();
        // n is inverse of reference face vector
        // -normal * RotT
        Vec2 n = -(RotT * normal);
        // change positive
        Vec2 nAbs = Abs(n);

        if (nAbs.x > nAbs.y) {
            if (Sign(n.x) > 0.0f) {
                //V_4
                c[0].v.Set(h.x, -h.y);
                c[0].fp.e.inEdge2 = EDGE3;
                c[0].fp.e.outEdge2 = EDGE4;
                //V_1
                c[1].v.Set(h.x, h.y);
                c[1].fp.e.inEdge2 = EDGE4;
                c[1].fp.e.outEdge2 = EDGE1;
            } else {
                //V_2
                c[0].v.Set(-h.x, h.y);
                c[0].fp.e.inEdge2 = EDGE1;
                c[0].fp.e.outEdge2 = EDGE2;
                //V_3
                c[1].v.Set(-h.x, -h.y);
                c[1].fp.e.inEdge2 = EDGE2;
                c[1].fp.e.outEdge2 = EDGE3;
            }
        } else {
            if (Sign(n.y) > 0.0f) {
                c[0].v.Set(h.x, h.y);
                c[0].fp.e.inEdge2 = EDGE4;
                c[0].fp.e.outEdge2 = EDGE1;

                c[1].v.Set(-h.x, h.y);
                c[1].fp.e.inEdge2 = EDGE1;
                c[1].fp.e.outEdge2 = EDGE2;
            } else {
                c[0].v.Set(-h.x, -h.y);
                c[0].fp.e.inEdge2 = EDGE2;
                c[0].fp.e.outEdge2 = EDGE3;

                c[1].v.Set(h.x, -h.y);
                c[1].fp.e.inEdge2 = EDGE3;
                c[1].fp.e.outEdge2 = EDGE4;
            }
        }
        //旋轉和轉換clip點到original vertex position
        c[0].v = pos + Rot * c[0].v;
        c[1].v = pos + Rot * c[1].v;
    }

    class Arbiter {
    public:
        Arbiter(Ref<RigidBody2D> &b1_, Ref<RigidBody2D> &b2_);

        ~Arbiter() = default;

        void update(Contact *contacts, int numContacts);

        void PreStep(float inv_dt);

        void ApplyImpulse();

        int calContactPoints(Contact *contacts, Ref<RigidBody2D> &b1, Ref<RigidBody2D> &b2);

        Contact contacts[2];
        int numContacts = 0;

        Ref<RigidBody2D> body1;
        Ref<RigidBody2D> body2;

        //combined friction
        float friction = 0.0f;
    };
}
