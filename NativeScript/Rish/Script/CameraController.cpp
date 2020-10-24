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

    if(Input::IsMouseButtonPressed(Mouse::Left))
    {
        float x = Math::random_float(-10.f, 10.f);
        float y = Math::random_float(-10.f, 10.f);
        auto ent = GetScene().createEntity();
        auto &transform = ent.getComponent<TransformComponent>();
        transform.translate.x = x;
        transform.translate.y = y;
        auto &render = ent.addComponent<RenderComponent>();
    }
}

void CameraController::onCreate()
{

}

void CameraController::onDestroy()
{

}

}