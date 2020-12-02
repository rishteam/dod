#include <Rish/Collider/Box.h>

namespace rl {

Box::Box(float x_, float y_, float w_, float h_, float angle_)
{
    position.x = x_;
    position.y = y_;
    w = w_;
    h = h_;
    m_vertexCount = 4;

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
//        RL_CORE_INFO("{} {} {}", i1, m_normals[i1].x, m_normals[i1].y);
    }

    angle = angle_;
    setMatrix(angle_);
}


}