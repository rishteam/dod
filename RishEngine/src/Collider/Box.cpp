#include <Rish/Collider/Box.h>
#include <Rish/Collider/Circle.h>
#include <Rish/Collider/Polygon.h>

namespace rl {

Box::Box(float x_, float y_, float w_, float h_)
{
    position.x = x_;
    position.y = y_;
    w = w_;
    h = h_;
    angle = 0.0f;
    m_vertices[0] = Vec2(w / 2.0, -h / 2.0);
    m_vertices[1] = Vec2(w / 2.0, h / 2.0);
    m_vertices[2] = Vec2(-w / 2.0, h / 2.0);
    m_vertices[3] = Vec2(-w / 2.0, -h / 2.0);

    // Compute face normals
    for(int i1 = 0; i1 < m_vertexCount; ++i1)
    {
        int i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
        Vec2 face = m_vertices[i2] - m_vertices[i1];

        // Ensure no zero-length edges, because that's bad
        assert( face.LenSqr( ) > EPSILON * EPSILON );

        // Calculate normal with 2D cross product between vector and scalar
        m_normals[i1] = Vec2( face.y, -face.x );
        m_normals[i1].Normalize( );
    }

    setMatrix(0.0f);
    angle = 0.0f;
}


}