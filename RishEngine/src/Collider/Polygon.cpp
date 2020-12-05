#include <Rish/Collider/Polygon.h>

#define MaxPolyVertexCount 64

namespace rl {

Polygon::Polygon(Vec2 pt_[], Vec2 pos_, int pointSize_ , float angle_)
{
    type = Shape::Type::Polygon;
    int count = pointSize_;


    // 找凸包
    // No hulls with less than 3 vertices (ensure actual polygon)
    assert(count > 2 && count <= MaxPolyVertexCount);
    count = std::min((int) count, MaxPolyVertexCount);

    // Find the right most point on the hull
    int rightMost = 0;
    float highestXCoord = pt_[0].x;
    for (int i = 1; i < count; ++i) {
        float x = pt_[i].x;
        if (x > highestXCoord) {
            highestXCoord = x;
            rightMost = i;
        }

            // If matching x then take farthest negative y
        else if (x == highestXCoord)
            if (pt_[i].y < pt_[rightMost].y)
                rightMost = i;
    }

    int hull[MaxPolyVertexCount];
    int outCount = 0;
    int indexHull = rightMost;

    for (;;) {
        hull[outCount] = indexHull;

        // Search for next index that wraps around the hull
        // by computing cross products to find the most counter-clockwise
        // vertex in the set, given the previous hull index
        int nextHullIndex = 0;
        for (int i = 1; i < (int) count; ++i) {
            // Skip if same coordinate as we need three unique
            // points in the set to perform a cross product
            if (nextHullIndex == indexHull) {
                nextHullIndex = i;
                continue;
            }

            // Cross every set of three unique vertices
            // Record each counter clockwise third vertex and add
            // to the output hull
            // See : http://www.oocities.org/pcgpe/math2d.html
            Vec2 e1 = pt_[nextHullIndex] - pt_[hull[outCount]];
            Vec2 e2 = pt_[i] - pt_[hull[outCount]];
            float c = Cross(e1, e2);
            if (c < 0.0f)
                nextHullIndex = i;

            // Cross product is zero then e vectors are on same line
            // therefore want to record vertex farthest along that line
            if (c == 0.0f && e2.LenSqr() > e1.LenSqr())
                nextHullIndex = i;
        }

        ++outCount;
        indexHull = nextHullIndex;

        // Conclude algorithm upon wrap-around
        if (nextHullIndex == rightMost) {
            m_vertexCount = outCount;
            break;
        }
    }

    // Copy vertices into shape's vertices
    for (int i = 0; i < m_vertexCount; ++i)
        m_vertices[i] = pt_[hull[i]];

    // Compute face normals
    for (int i1 = 0; i1 < m_vertexCount; ++i1) {
        int i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
        Vec2 face = m_vertices[i2] - m_vertices[i1];

        // Ensure no zero-length edges, because that's bad
        assert(face.LenSqr() > 0.00001 * 0.00001);

        // Calculate normal with 2D cross product between vector and scalar
        m_normals[i1] = Vec2(face.y, -face.x);
        m_normals[i1].Normalize();
    }


    pointSize = pointSize_;
    for(int i = 0; i < pointSize; i++)
    {
        pt[i] = pt_[i];
    }
    position = pos_;
    angle = angle_;
    setMatrix(angle);
}


}