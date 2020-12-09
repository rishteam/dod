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
