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
    auto &rend =  GetComponent<SpriteRenderComponent>();
    auto &prevState = playerState;
    // TODO: Hard Fixed
    trans.rotate = 0.0f;
    rigid.angle = 0.0f;

    Vec2 &velocity = rigid.velocity;
    //printf("Vx: %f, Vy: %f\n", velocity.x, velocity.y);
    if (rigid.velocity.x <= 1e-3 && rigid.velocity.y <= 1e-3)
    {
        if (!boxc.isCollision && playerState == PlayerState::Jump)
        {
            return;
        }
        else
        {
            playerState = PlayerState::Stand;
            jumpTimesCounter = 0;
        }
    }
    if (Input::IsKeyPressed(Keyboard::Up) && !prevJump && jumpTimesCounter < jumpLimitTimes)
    {
        velocity.y = jumpSpeed;
        playerState = PlayerState::Jump;
        jumpTimesCounter++;

    }
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
        playerState = PlayerState::Ducking;
    }

    prevJump = Input::IsKeyPressed(Keyboard::Up);
    setSprite(rend, playerState, playerFace);

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

    switch (playerState) {
        case PlayerState::Stand:
            ImGui::Text("playerState: Stand");
            break;
        case PlayerState::Ducking:
            ImGui::Text("playerState: Ducking");
            break;
        case PlayerState::Jump:
            ImGui::Text("playerState: Jumping");
            break;
    }

    switch (playerFace){
        case PlayerFace::Right:
            ImGui::Text("playerFace: Right");
            break;
        case PlayerFace::Left:
            ImGui::Text("playerFace: Left");
            break;
    }

}

void PlayerController::setSprite(SpriteRenderComponent &render, PlayerState &playerState, PlayerFace &playerFace)
{
    // Player Draw State
    // TODO: Simplify, Flip features

    render.loadTexture("assets\\texture\\mario\\characters.gif");

    SubTexture2DSetting setting;
    setting.type = SubTexture2DSetting::SubTextureCoordinate;

    switch (playerState)
    {
        // TODO: set state graphic
        case PlayerState::Stand:
        {
            if (playerFace == PlayerFace::Right)
            {
                setting.leftUpper = glm::vec2(256, 1);
                setting.size = glm::vec2(21, 33);
            }
            else if (playerFace == PlayerFace::Left)
            {
                setting.leftUpper = glm::vec2(238, 0);
                setting.size = glm::vec2(21, 33);
            }
            break;
        }
        case PlayerState::Ducking:
        {
            if (playerFace == PlayerFace::Right)
            {
                setting.leftUpper = glm::vec2(276, 0);
                setting.size = glm::vec2(21, 33);
            }
            else if (playerFace == PlayerFace::Left)
            {
                setting.leftUpper = glm::vec2(219, 0);
                setting.size = glm::vec2(21, 33);
            }
            break;
        }
        case PlayerState::Jump:
        {
            if (playerFace == PlayerFace::Right)
            {
                setting.leftUpper = glm::vec2(367, 1);
                setting.size      = glm::vec2(19, 33);
            }
            else if (playerFace == PlayerFace::Left)
            {
                setting.leftUpper = glm::vec2(126, 1);
                setting.size      = glm::vec2(21, 33);
            }
            break;
        }
    }
    //
    render.loadSubTexture(setting);
}

}