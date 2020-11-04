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
    auto &render =  GetComponent<SpriteRenderComponent>();

    Vec2 &velocity = rigid.velocity;
    if (rigid.velocity.x <= StopSpeed && rigid.velocity.y <= StopSpeed)
    {
        if (!boxc.isCollision && playerAction == PlayerAction::Jump)
        {
            return;
        }
        else
        {
            playerAction = PlayerAction::Stand;
            jumpTimesCounter = 0;
        }
    }
    if (Input::IsKeyPressed(Keyboard::Up) && !prevJump && jumpTimesCounter < jumpLimitTimes)
    {
        velocity.y = jumpSpeed;
        playerAction = PlayerAction::Jump;
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
        playerAction = PlayerAction::Ducking;
    }
    prevJump = Input::IsKeyPressed(Keyboard::Up);
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

void PlayerController::onImGuiRender()
{
    ImGui::DragFloat("Walk Accelerate", &walkAccel, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Walk Speed Limit", &walkSpeedLimit, 10.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Jump Speed", &jumpSpeed, 10.0f, 0.0f, 100.0f);

    switch (playerAction) {
        case PlayerAction::Stand:
            ImGui::Text("playerAction: Stand");
            break;
        case PlayerAction::Ducking:
            ImGui::Text("playerAction: Ducking");
            break;
        case PlayerAction::Jump:
            ImGui::Text("playerAction: Jumping");
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

void PlayerController::setSprite(SpriteRenderComponent &render, PlayerAction &playerAction, PlayerFace &playerFace)
{
    // Player Draw State
    // TODO: Simplify, Flip features

    render.loadTexture("assets\\texture\\mario\\characters.gif");

    SubTexture2DSetting setting;
    setting.type = SubTexture2DSetting::SubTextureCoordinate;

    switch (playerAction)
    {
        // TODO: set state graphic
        case PlayerAction::Stand:
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
        case PlayerAction::Ducking:
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
        case PlayerAction::Jump:
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
