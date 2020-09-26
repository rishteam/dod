#include <Rish/Scene/SceneCamera.h>

namespace rl {

SceneCamera::SceneCamera()
{
    recalculateProjection();
}

void SceneCamera::setOrthographic(float size, float near, float far)
{
    m_orthoSize = size;
    m_orthoNear = near;
    m_orthoFar = far;
    recalculateProjection();
}

void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
{
    m_aspect = (float)width / (float)height;
    recalculateProjection();
}

void SceneCamera::setAspect(float aspect)
{
    m_aspect = aspect;
    recalculateProjection();
}

void SceneCamera::recalculateProjection()
{
    float orthoLeft = -m_orthoSize * m_aspect * 0.5f;
    float orthoRight = m_orthoSize * m_aspect * 0.5f;
    float orthoBottom = -m_orthoSize * 0.5f;
    float orthoTop = m_orthoSize * 0.5f;

    m_projection = glm::ortho(orthoLeft, orthoRight,
                              orthoBottom, orthoTop, m_orthoNear, m_orthoFar);
}

} // end of namespace rl
