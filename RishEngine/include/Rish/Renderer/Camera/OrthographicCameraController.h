#pragma once

#include <glm/glm.hpp>
//
#include <Rish/Core/Core.h>
#include <Rish/Core/Time.h>
//
#include <Rish/Events/Event.h>
#include <Rish/Events/MouseEvent.h>
#include <Rish/Events/ApplicationEvent.h>
//
#include <Rish/Renderer/Camera/OrthographicCamera.h>

namespace rl {

class RL_API OrthographicCameraController
{
public:
    OrthographicCameraController(float aspect, bool rotate=false);

    void onUpdate(Time dt);
    void onEvent(Event &e);
    void onImGuiRender();

    OrthographicCamera& getCamera() { return m_camera; }
    const OrthographicCamera& getCamera() const { return m_camera; }
private:
    bool onMouseScrolled(MouseScrolledEvent &e);
    bool onWindowResized(WindowResizeEvent &e);
private:
    float m_aspect = 1280.f / 720.f;
    float m_zoom = 1.f;
    OrthographicCamera m_camera;

    float m_translateSpeed = 5.f;
    float m_rotateSpeed = 180.f;

    bool m_isAbleToRotate;
    float m_rotate = 0.f;
    glm::vec3 m_position{0.f, 0.f, 0.f};
};

}
