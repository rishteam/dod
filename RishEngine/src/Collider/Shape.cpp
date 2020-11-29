#include <Rish/Collider/Shape.h>
#include <Rish/Collider/Collision.h>

namespace rl {

Vec2 Shape::GetSupport(const Vec2 dir)
{
    float bestProjection = -FLT_MAX;
    Vec2 bestVertex;

    for (int i = 0; i < m_vertexCount; ++i) {
        Vec2 v = m_vertices[i];
        float projection = Dot(v, dir);

        if (projection > bestProjection) {
            bestVertex = v;
            bestProjection = projection;
        }
    }
    return bestVertex;
}

void Shape::setMatrix(float radians)
{
    angle = radians;
    u.SetRotateMatrix(radians);
}

}
