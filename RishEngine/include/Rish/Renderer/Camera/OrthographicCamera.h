#pragma once

#include <Rish/rlpch.h>
//
#include <glm/glm.hpp>

namespace rl {

/**
 * @brief Orthographic Camera For 2D scene
 */
class RL_API OrthographicCamera
{
public:
    /**
     * @brief Ctor
     * @details ![](https://i.imgur.com/QzARlUo.png)
     *
     * @param left Left plane
     * @param right Right plane
     * @param bottom Bottom plane
     * @param top Top plane
     */
    OrthographicCamera(float left, float right, float bottom, float top);

    void resizeCamera(float left, float right, float bottom, float top);

    void setPosition(const glm::vec3 &p) { m_position = p; recalculateMatrix(); }
    const glm::vec3 getPosition() const { return m_position; }

    void setRotation(float r) { m_rotation = r; recalculateMatrix(); }
    float getRotation() const { return m_rotation; }

    const glm::mat4 getViewMatrix() const { return m_viewMatrix; }
    const glm::mat4 getProjectionMatrix() const { return m_projectionMatrix; }
    const glm::mat4 getViewProjectionMatrix() const { return m_vpMatrix; }
private:
    void recalculateMatrix();

    glm::vec3 m_position{0.f, 0.f, 0.f}; ///< Position
    float m_rotation = 0.f;                  ///< Rotation

    glm::mat4 m_viewMatrix{1.f};          ///< View Matrix
    glm::mat4 m_projectionMatrix{1.f};    ///< Projection Matrix
    glm::mat4 m_vpMatrix{1.f};            ///< View Projection Matrix \f$\text{Projeciton} \cdot \text{View}\f$
};

} // end of namespace rl
