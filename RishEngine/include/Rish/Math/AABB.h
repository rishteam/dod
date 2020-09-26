#pragma once

#include <Rish/rlpch.h>

namespace rl {
namespace Math {

/**
 * @brief
 * @param position
 * @param size
 * @param p
 * @return
 */
bool AABB2DPoint(const glm::vec3 &position, const glm::vec3 &size, const glm::vec2 &p);

/**
 * @brief AABB of two quad
 * @details <http://davidhsu666.com/archives/gamecollisiondetection/>
 * @param p1 Position of Quad1
 * @param s1 Size of Quad1
 * @param p2 Position of Quad2
 * @param s2 Size of Quad2
 * @return True/False
 */
bool AABB2DQuad(const glm::vec3 &p1, const glm::vec3 &s1, const glm::vec3 &p2, const glm::vec3 &s2);

} // end of namespace Math
} // end of namespace rl
