#pragma once

#include <Rish/Core/Time.h>
#include <Rish/Input/Input.h>
//
#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/Component.h>
//

// TODO: Find a more suitable place for Scripts
namespace rl {

class CameraController : public ScriptableEntity
{
public:
    void onUpdate(Time dt) override
    {
        auto &trans = getComponent<TransformComponent>().translate;

        if (Input::IsKeyPressed(Keyboard::W))
            trans.y += m_speed * dt.asSeconds();
        if (Input::IsKeyPressed(Keyboard::S))
            trans.y -= m_speed * dt.asSeconds();
        if (Input::IsKeyPressed(Keyboard::A))
            trans.x -= m_speed * dt.asSeconds();
        if (Input::IsKeyPressed(Keyboard::D))
            trans.x += m_speed * dt.asSeconds();

//        if(Input::IsMouseButtonPressed(Mouse::Left))
//        {
            auto [x, y] = Input::GetMousePosition();
//            RL_INFO("{} {}", x,  y);
//        }

        if(Input::IsKeyPressed(Keyboard::Space))
        {
            Input::SetMousePosition(0.5f, 0.5f);
        }
    }

    void onImGuiRender() override
    {
        auto &trans = getComponent<TransformComponent>().translate;
        ImGui::DragFloat3("Translate", glm::value_ptr(trans));
        ImGui::DragFloat("Speed", &m_speed);

    }

    RL_SCRIPT_SERIALIZE()
    {
        RL_SERIALIZE("speed", m_speed);
    }

private:
    float m_speed = 10.f;
    bool m_inverted = false;
};

class SpriteRoatate : public ScriptableEntity
{
public:
    void onUpdate(Time dt) override
    {
        auto &trans = getComponent<TransformComponent>();
        trans.rotate += 100.f * dt.asSeconds();
        trans.rotate = std::fmod(trans.rotate, 360.f);
    }

    void onImGuiRender() override
    {
    }

    RL_SCRIPT_EMPTY_SERIALIZE()
};

}

RL_REGISTER_SCRIPT_TYPE(rl::CameraController)
RL_REGISTER_SCRIPT_TYPE(rl::SpriteRoatate)