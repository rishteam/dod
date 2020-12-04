#include <Rish/Script/PlayerController_GameDemo.h>

namespace rl {

    void PlayerController_GameDemo::onCreate()
    {
        playerAction = PlayerAction::Stand;
        playerFace = PlayerFace::Right;
    }

    void PlayerController_GameDemo::onDestroy()
    {

    }

    void PlayerController_GameDemo::onUpdate(Time dt)
    {
        auto &trans = GetComponent<TransformComponent>();
        auto &rigid = GetComponent<RigidBody2DComponent>();
        auto &boxc = GetComponent<BoxCollider2DComponent>();
        auto &render = GetComponent<SpriteRenderComponent>();

        Vec2 &velocity = rigid.velocity;
        if (Input::IsKeyPressed(Keyboard::Left))
        {
            if(velocity.x >= -walkSpeedLimit)
                velocity.x -= walkAccel * dt.asSeconds();
            playerFace = PlayerFace::Left;
        }
        if (Input::IsKeyPressed(Keyboard::Right))
        {
            if(velocity.x <= walkSpeedLimit)
                velocity.x += walkAccel * dt.asSeconds();
            playerFace = PlayerFace::Right;
        }
        if (Input::IsKeyPressed(Keyboard::Down))
        {
            playerAction = PlayerAction::Ducking;
        }
        prevJump = Input::IsKeyPressed(Keyboard::Up);

        // TODO: Trigger with Object (ex: mushroom, star, monster...)
        detectObject();
        //
        setSprite(render,playerAction, playerFace);

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

    void PlayerController_GameDemo::onImGuiRender()
    {
        ImGui::DragFloat("Walk Accelerate", &walkAccel, 10.0f, 0.0f, 100.0f);
        ImGui::DragFloat("Walk Speed Limit", &walkSpeedLimit, 10.0f, 0.0f, 100.0f);
        ImGui::DragFloat("Jump Speed", &jumpSpeed, 10.0f, 0.0f, 100.0f);

        ImGui::Text("playerAction: %d", static_cast<PlayerAction>(playerAction));
        ImGui::Text("playerFace: %d", static_cast<PlayerFace>(playerFace));
    }


    void PlayerController_GameDemo::setSprite(SpriteRenderComponent &render, PlayerAction &playerAction, PlayerFace &playerFace)
    {
        auto &trans = GetComponent<TransformComponent>();
        auto &boxc = GetComponent<BoxCollider2DComponent>();
        // Player Draw State
        // TODO: Simplify, Flip features
        render.loadTexture("assets\\texture\\mario\\characters.gif");

        SubTexture2DSetting setting;
        setting.type = SubTexture2DSetting::SubTextureCoordinate;

        //
        render.loadSubTexture(setting);
    }


    void PlayerController_GameDemo::detectObject()
    {
        auto &collideList = GetComponent<BoxCollider2DComponent>().whoCollide;
        for(auto uuid : collideList)
        {
            Entity collideEntity = GetScene().getEntityByUUID(uuid);
            if(!collideEntity.m_scene)
                break;

        }

    }

}
