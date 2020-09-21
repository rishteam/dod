#include <Rish/Math/AABB.h>

namespace rl {
namespace Math {

bool AABB2DPoint(const glm::vec3 &position, const glm::vec3 &size, const glm::vec2 &p)
{
    glm::vec3 pos = position;
    glm::vec3 halfSize = size / 2.f;
    glm::vec3 upperRight = pos + halfSize, bottomLeft = pos - halfSize;
    //
    return bottomLeft.x <= p.x && bottomLeft.y <= p.y &&
           p.x <= upperRight.x && p.y <= upperRight.y;
}

bool AABB2DQuad(const glm::vec3 &p1, const glm::vec3 &s1, const glm::vec3 &p2, const glm::vec3 &s2)
{
    float minX1 = p1.x - s1.x / 2.f,
          maxX1 = p1.x + s1.x / 2.f,
          minY1 = p1.y - s1.y / 2.f,
          maxY1 = p1.y + s1.y / 2.f;

    float minX2 = p2.x - s2.x / 2.f,
          maxX2 = p2.x + s2.x / 2.f,
          minY2 = p2.y - s2.y / 2.f,
          maxY2 = p2.y + s2.y / 2.f;

    return maxX1 > minX2 && maxX2 > minX1 &&
           maxY1 > minY2 && maxY2 > minY1;
}

} // end of namespace Math
} // end of namespace rl
