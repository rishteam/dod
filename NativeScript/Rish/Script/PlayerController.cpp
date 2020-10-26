#include <Rish/Script/PlayerController.h>

namespace rl {

void PlayerController::onCreate()
{

}

void PlayerController::onDestroy()
{

}

void PlayerController::onUpdate(Time dt)
{
    auto &trans = GetComponent<TransformComponent>();
    auto &rigid = GetComponent<RigidBody2DComponent>();
    auto &boxc = GetComponent<BoxCollider2DComponent>();
    rigid.attachPoint = Vec2(0.0f, 0.0f);
    // TODO: HOT Fixed
    trans.rotate = 0.0f;

    static bool prevJump = false;

    Vec2 &velocity = rigid.velocity;
    if (Input::IsKeyPressed(Keyboard::Up) && !prevJump)
    {
        velocity.y = jumpSpeed;
    }
    prevJump = Input::IsKeyPressed(Keyboard::Up);

    if (Input::IsKeyPressed(Keyboard::Left))
    {
        if(velocity.x >= -walkSpeedLimit)
            velocity.x -= walkAccel * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Right))
    {
        if(velocity.x <= walkSpeedLimit)
            velocity.x += walkAccel * dt.asSeconds();
    }

    // Camera follows player
    auto view = GetScene().m_registry.view<TransformComponent, CameraComponent>();
    for(auto ent : view)
    {
        auto entity = GetEntityByHandle(ent);
        auto &camera = entity.getComponent<CameraComponent>();
        auto &transform = entity.getComponent<TransformComponent>();
        if(camera.primary)
        {
            transform.translate.x = trans.translate.x;
        }
    }
}

void PlayerController::onImGuiRender()
{
    ImGui::DragFloat("Walk Accelerate", &walkAccel, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Walk Speed Limit", &walkSpeedLimit, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Jump Speed", &jumpSpeed, 10.0f, 0.0f, 100.0f);
}

}