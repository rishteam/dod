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

struct OrthographicCameraBounds
{
    OrthographicCameraBounds(float Left, float Right, float Bottom, float Top)
    {
        left = Left;
        right = Right;
        bottom = Bottom;
        top = Top;
    }

    float left, right;
    float top, bottom;

    float getWidth() const { return right - left; }
    float getHeight() const { return top - bottom; }
};

class RL_API OrthographicCameraController
{
public:
    OrthographicCameraController(float aspect, bool rotate=false);

    void onUpdate(Time dt);
    void onEvent(Event &e);
    void onImGuiRender();

    void setState(bool state) { m_enableState = state; }

    OrthographicCamera& getCamera() { return m_camera; }
    const OrthographicCamera& getCamera() const { return m_camera; }
    const OrthographicCameraBounds& getBounds() const { return m_bounds; }

    float getRotate() const { return m_rotate; }
private:
    bool onMouseScrolled(MouseScrolledEvent &e);
    bool onWindowResized(WindowResizeEvent &e);
private:
    bool m_enableState = true;
    float m_aspect = 1280.f / 720.f;
    float m_zoom = 1.f;
    OrthographicCameraBounds m_bounds;
    OrthographicCamera m_camera;

    float m_translateSpeed = 5.f;
    float m_rotateSpeed = 180.f;

    bool m_isAbleToRotate;
    float m_rotate = 0.f;  // degrees, counter-clock wise
    glm::vec3 m_position{0.f, 0.f, 0.f};
};

}
