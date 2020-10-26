#include <Rish/Script/CameraController.h>

namespace rl {

void CameraController::onUpdate(rl::Time dt)
{
    auto &trans = GetComponent<TransformComponent>().translate;

    if (Input::IsKeyPressed(Keyboard::W))
        trans.y += m_speed * dt.asSeconds();
    if (Input::IsKeyPressed(Keyboard::S))
        trans.y -= m_speed * dt.asSeconds();
    if (Input::IsKeyPressed(Keyboard::A))
        trans.x -= m_speed * dt.asSeconds();
    if (Input::IsKeyPressed(Keyboard::D))
        trans.x += m_speed * dt.asSeconds();

}

void CameraController::onCreate()
{

}

void CameraController::onDestroy()
{

}

}
