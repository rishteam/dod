#include <Rish/Script/Player.h>

namespace rl {

void Player::onCreate()
{
    clock.restart();
}

void Player::onDestroy()
{

}

void Player::onUpdate(Time dt)
{
    auto &trans = GetComponent<TransformComponent>();
    auto &rigid = GetComponent<RigidBody2DComponent>();
    auto &render = GetComponent<SpriteRenderComponent>();
    auto &playerCollider = GetComponent<BoxCollider2DComponent>();
    auto &light = GetComponent<LightComponent>();

    Vec2 &velocity = rigid.velocity;
    velocity.x = 0.001;
    velocity.y = 0.001;
    if (Input::IsKeyPressed(Keyboard::Left))
    {
//        if(velocity.x >= -walkSpeedLimit)
            trans.translate.x -= walkAccel * dt.asSeconds();
//            velocity.x -= walkAccel * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Right))
    {
//        if(velocity.x <= walkSpeedLimit)
            trans.translate.x += walkAccel * dt.asSeconds();
//            velocity.x += walkAccel * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Down))
    {
//        if(velocity.y >= -walkSpeedLimit)
            trans.translate.y -= walkAccel * dt.asSeconds();
//            velocity.y -= walkAccel * dt.asSeconds();
    }
    if (Input::IsKeyPressed(Keyboard::Up))
    {
//        if(velocity.y <= walkSpeedLimit)
            trans.translate.y += walkAccel * dt.asSeconds();
//            velocity.y += walkAccel * dt.asSeconds();
    }

    // TODO: Trigger with Object (ex: mushroom, star, monster...)
//    detectObject();

    // Camera follows player
    {
        auto view = GetScene().m_registry.view<TransformComponent, CameraComponent>();
        for(auto ent : view)
        {
            auto entity = GetEntityByHandle(ent);
            auto &camera = entity.getComponent<CameraComponent>();
            auto &transform = entity.getComponent<TransformComponent>();
            if(camera.primary)
            {
                transform.translate.x = trans.translate.x;
                transform.translate.y = trans.translate.y;
            }
        }
    }


    // Light/Hp decrease
    {
        if(clock.getElapsedTime() > 5.f)
        {
            light.strength -= 0.8;

            clock.restart();

            if(light.strength <= 0)
            {
                life = false;
                light.strength = 0;
            }
        }
    }

    auto hpPack = GetScene().m_registry.view<TransformComponent, BoxCollider2DComponent>();

    for(auto ent : hpPack)
    {
        auto entity = GetEntityByHandle(ent);
        auto &collider = entity.getComponent<BoxCollider2DComponent>();
        auto &tag = entity.getComponent<TagComponent>();
        if(std::find(playerCollider.whoCollide.begin(), playerCollider.whoCollide.end(), tag.id) != playerCollider.whoCollide.end())
        {
            if(tag.tag.substr(0, 4) == "pack")
            {
                light.strength += 3;
                GetScene().m_registry.destroy(entity);
            }
        }
    }

    auto particle = GetScene().m_registry.view<TransformComponent, ParticleComponent>();

    for(auto ent : particle)
    {
        auto entity = GetEntityByHandle(ent);
        auto &tag = entity.getComponent<TagComponent>();
        auto &snow = entity.getComponent<ParticleComponent>();
        auto &snow_trans = entity.getComponent<TransformComponent>();

        if(tag.tag == "Snow")
        {
            snow_trans.translate = trans.translate;
        }
    }
}

void Player::onImGuiRender()
{
    ImGui::DragFloat("Walk Accelerate", &walkAccel, 10.0f, 0.0f, 100.0f);
}

}
