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
        auto [x, y] = Input::GetMousePosition();
        auto ent = GetScene().createEntity();
        ent.addComponent<RenderComponent>();
        ent.getComponent<TransformComponent>().translate = glm::vec3(x, y, 0.f);
    }
}

void CameraController::onCreate()
{
    RL_INFO("scene = {}", (void *)GetEntity().m_scene);
}

void CameraController::onDestroy()
{
    RL_INFO("scene = {}", (void *)GetEntity().m_scene);
}

}