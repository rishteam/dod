#include <Rish/rlpch.h>
#include <Rish/Input/Input.h>
#include <Rish/Renderer/Camera/OrthographicCameraController.h>

#include <imgui.h>

namespace rl {

OrthographicCameraController::OrthographicCameraController(float aspect, bool rotate)
    : m_aspect(aspect),
      m_zoom(1.f),
      m_bounds(-m_aspect * m_zoom, m_aspect * m_zoom, -m_zoom, m_zoom),
      m_camera(m_bounds.left, m_bounds.right, m_bounds.bottom, m_bounds.top)
{
    m_isAbleToRotate = rotate;
}

void OrthographicCameraController::onUpdate(Time dt)
{
    glm::mat2 rot = {
        std::cos(m_camera.getRotationRadians()), std::sin(m_camera.getRotationRadians()),
        -std::sin(m_camera.getRotationRadians()), std::cos(m_camera.getRotationRadians())
    };
    glm::vec2 dir[4] = {
        {0.f,  1.f},  // W
        {0.f, -1.f},  // S
        {-1.f, 0.f},  // A
        {1.f,  0.f}   // D
    };
    for(int i = 0; i < 4; i++)
    {
        dir[i] = rot * dir[i];
        dir[i] *= m_translateSpeed;
    }

    if(rl::Input::isKeyPressed(rl::Keyboard::W))
        m_position += glm::vec3(dir[0], 0.f) * dt.asSeconds();
    else if(rl::Input::isKeyPressed(rl::Keyboard::S))
        m_position += glm::vec3(dir[1], 0.f) * dt.asSeconds();
    if(rl::Input::isKeyPressed(rl::Keyboard::A))
        m_position += glm::vec3(dir[2], 0.f) * dt.asSeconds();
    else if(rl::Input::isKeyPressed(rl::Keyboard::D))
        m_position += glm::vec3(dir[3], 0.f) * dt.asSeconds();
    m_camera.setPosition(m_position);

    if(!m_isAbleToRotate)
        return;

    if(rl::Input::isKeyPressed(rl::Keyboard::Q))
    {
        m_rotate -= m_rotateSpeed * dt.asSeconds();
    }
    else if(rl::Input::isKeyPressed(rl::Keyboard::E))
    {
        m_rotate += m_rotateSpeed * dt.asSeconds();
    }
    if(m_rotate > 360.f || m_rotate < -360.f)
        m_rotate = std::fmod(m_rotate, 360.f);

    m_camera.setRotation(m_rotate);
}

void OrthographicCameraController::onEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<MouseScrolledEvent>(RL_BIND_EVENT_FUNC(OrthographicCameraController::onMouseScrolled));
    dispatcher.dispatch<WindowResizeEvent>(RL_BIND_EVENT_FUNC(OrthographicCameraController::onWindowResized));
}

void OrthographicCameraController::onImGuiRender()
{
    ImGui::Begin("Debug");
    ImGui::BeginGroup();
    ImGui::Text("Camera");
    ImGui::Text("Position %.2f %.2f %.2f", m_position.x, m_position.y, m_position.z);
    ImGui::Text("Rotate %.2f", m_rotate);

    auto &cam = m_camera.getViewProjectionMatrix();
    ImGui::Text("View Projection Matrix");
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            ImGui::PushID(&cam[i][j]);
            ImGui::Text("%.2f", static_cast<double>(cam[i][j])); ImGui::SameLine();
            ImGui::PopID();
        }
        ImGui::NewLine();
    }
    ImGui::EndGroup();
    ImGui::End();
}

bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent &e)
{
    m_zoom -= e.yOffset * 0.25f;
    m_zoom = std::max(m_zoom, 0.25f);
    //
    m_bounds = OrthographicCameraBounds{-m_aspect * m_zoom, m_aspect * m_zoom, -m_zoom, m_zoom};
    m_camera.resizeCamera(m_bounds.left, m_bounds.right, m_bounds.bottom, m_bounds.top);
    return false;
}

bool OrthographicCameraController::onWindowResized(WindowResizeEvent &e)
{
    m_aspect = (float)e.m_width / (float)e.m_height;
    //
    m_bounds = OrthographicCameraBounds{-m_aspect * m_zoom, m_aspect * m_zoom, -m_zoom, m_zoom};
    m_camera.resizeCamera(m_bounds.left, m_bounds.right, m_bounds.bottom, m_bounds.top);
    return false;
}

}
