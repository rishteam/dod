#pragma once

#include <Rish/rlpch.h>
#include <Rish/Renderer/Camera/Camera.h>

namespace rl {

/**
 * @brief Camera for Scene
 * @details Have orthographic projection attributes
 * size, near, far, aspect ratio
 */
class SceneCamera : public Camera
{
public:
    SceneCamera();
    virtual ~SceneCamera() override = default;

    void setOrthographic(float size, float near, float far);
    void setViewportSize(uint32_t width, uint32_t height);
    void setAspect(float aspect);
    float getAspect() const { return m_aspect; }

    float getOrthoSize() const { return m_orthoSize; }
private:
    void recalculateProjection();
    float m_orthoSize = 5.f;
    float m_orthoNear = -1.0f, m_orthoFar = 1.0f;

    float m_aspect = 0.f;

    // Friend class
    friend class Scene;
    friend void DrawSceneCameraDebugWindow(const SceneCamera &camera, const glm::mat4 &trans);

    // Serialization function
    friend cereal::access;
    template<typename Archive>
    void serialize(Archive &ar)
    {
        ar(
            cereal::base_class<Camera>(this),
            cereal::make_nvp("orthoSize", m_orthoSize),
            cereal::make_nvp("orthoNear", m_orthoNear),
            cereal::make_nvp("orthoFar", m_orthoFar),
            cereal::make_nvp("aspect", m_aspect)
        );
    }
};

} // end of namespace rl