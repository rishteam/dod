#include "ParkourPlayerController.h"

namespace rl{

void ParkourPlayerController::onCreate()
{

}

void ParkourPlayerController::onDestroy()
{

}

void ParkourPlayerController::onUpdate(Time dt)
{

    if(!HasComponent<BoxCollider2DComponent>() || !HasComponent<RigidBody2DComponent>())
        return;

    auto &rigid = GetComponent<RigidBody2DComponent>();
    auto &boxc  = GetComponent<BoxCollider2DComponent>();
    auto &trans = GetComponent<TransformComponent>();

    Vec2 &velocity = rigid.velocity;
    if ( -m_stopSpeed < rigid.velocity.y && rigid.velocity.y < m_stopSpeed &&
         m_jumpSpeed > m_jumpSpeedLimit/2)
    {
        playerAction = PlayerAction::Stand;
        m_jumpTimesCounter = 0;
    }

    if ( Input::IsKeyPressed(Keyboard::Space) && !m_prevJump && m_jumpTimesCounter < m_jumpLimitTimes)
    {
        velocity.y = m_jumpSpeed ;
        m_jumpSpeed = 0.f;
        playerAction = PlayerAction::Jump;
        m_jumpTimesCounter++;
    }
    m_prevJump = Input::IsKeyPressed(Keyboard::Space);

    if( m_jumpSpeed < m_jumpSpeedLimit )
        m_jumpSpeed += m_jumpSpeedLimit * dt.asSeconds() * 2.f;
    else
        m_jumpSpeed = m_jumpSpeedLimit;

    if (Input::IsKeyPressed(Keyboard::Left))
    {
        if(velocity.x > -m_walkSpeedLimit)
            velocity.x -= m_walkAccel * dt.asSeconds();
        else
            velocity.x = -m_walkSpeedLimit;
    }
    if (Input::IsKeyPressed(Keyboard::Right))
    {
        if(velocity.x < m_walkSpeedLimit)
            velocity.x += m_walkAccel * dt.asSeconds();
        else
            velocity.x = m_walkSpeedLimit;
    }


}

void ParkourPlayerController::onImGuiRender()
{

    if(!HasComponent<BoxCollider2DComponent>())
        ImGui::Text("Need Component: BoxCollider2DComponent");
    if(!HasComponent<RigidBody2DComponent>())
        ImGui::Text("Need Component: RigidBody2DComponent");
    if(!HasComponent<BoxCollider2DComponent>() || !HasComponent<RigidBody2DComponent>())
        return;

    ImGui::DragFloat("Walk Accelerate", &m_walkAccel, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Walk Speed Limit", &m_walkSpeedLimit, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Jump Speed", &m_jumpSpeedLimit, 10.0f, 0.0f, 100.0f);

    ImGui::Separator();
    ImGui::Text("Debug");
    ImGui::DragFloat("Now Jump Spped", &m_jumpSpeed);
    if(playerAction==PlayerAction::Stand)
        ImGui::Text("Stand");
    else
        ImGui::Text("Jump");
}

}
