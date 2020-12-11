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
    auto &boxc = GetComponent<BoxCollider2DComponent>();

    Vec2 &velocity = rigid.velocity;
    if (rigid.velocity.x <= m_stopSpeed && rigid.velocity.y <= m_stopSpeed)
    {
        if (!boxc.isCollision && playerAction == PlayerAction::Jump)
        {
            return;
        }
        else
        {
            playerAction = PlayerAction::Stand;
            m_jumpTimesCounter = 0;
        }
    }
    if ( Input::IsKeyPressed(Keyboard::Space) && !m_prevJump && m_jumpTimesCounter < m_jumpLimitTimes)
    {
        velocity.y = m_jumpSpeed;
        playerAction = PlayerAction::Jump;
        m_jumpTimesCounter++;

    }
    if (Input::IsKeyPressed(Keyboard::Left))
    {
        if(velocity.x >= -m_walkSpeedLimit)
            velocity.x -= m_walkAccel * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Right))
    {
        if(velocity.x <= m_walkSpeedLimit)
            velocity.x += m_walkAccel * dt.asSeconds();
    }
    m_prevJump = Input::IsKeyPressed(Keyboard::Space);

}

void ParkourPlayerController::onImGuiRender()
{

    if(!HasComponent<BoxCollider2DComponent>())
        ImGui::Text("Need Component: BoxCollider2DComponent");
    if(!HasComponent<RigidBody2DComponent>())
        ImGui::Text("Need Component: RigidBody2DComponent");

    ImGui::DragFloat("Walk Accelerate", &m_walkAccel, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Walk Speed Limit", &m_walkSpeedLimit, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Jump Speed", &m_jumpSpeed, 10.0f, 0.0f, 100.0f);

}



}
