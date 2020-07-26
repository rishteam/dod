#include <Rish/Renderer/Camera/OrthographicCamera.h>

#include <glm/gtc/matrix_transform.hpp>

namespace rl {

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
    : m_viewMatrix(1.f),
      m_projectionMatrix(glm::ortho(left, right, bottom, top, -1.f, 1.f)),
      m_vpMatrix(m_projectionMatrix * m_viewMatrix)
{
}

void OrthographicCamera::recalculateMatrix()
{
    glm::mat4 transform = glm::translate(glm::mat4(1.f), -m_position) *
            glm::rotate(glm::mat4(1.f), glm::radians(-m_rotation), glm::vec3(0.f, 0.f, 1.f));

    m_viewMatrix = transform;
    m_vpMatrix = m_projectionMatrix * m_viewMatrix;
}

void OrthographicCamera::resizeCamera(float left, float right, float bottom, float top)
{
    m_projectionMatrix = glm::ortho(left, right, bottom, top, -1.f, 1.f);
    recalculateMatrix();
}

} // end of namespace rl
