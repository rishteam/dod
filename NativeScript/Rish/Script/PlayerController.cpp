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
    // TODO: HOT Fixed
    trans.rotate = 0.0f;
    rigid.angle = 0.0f;

    Vec2 &velocity = rigid.velocity;
    if (rigid.velocity.x == 0 && rigid.velocity.y == 0)
    {
        playerState = PlayerState::STAND;
    }
    // playerState == PlayerState::STAND
    if (Input::IsKeyPressed(Keyboard::Up) && !prevJump)
    {
        velocity.y = jumpSpeed;
        playerState = PlayerState::JUMP;
    }
    if (Input::IsKeyPressed(Keyboard::Left))
    {
        if(velocity.x >= -walkSpeedLimit)
            velocity.x -= walkAccel * dt.asSeconds();
        playerState = PlayerState::LEFT;
    }
    if (Input::IsKeyPressed(Keyboard::Right))
    {
        if(velocity.x <= walkSpeedLimit)
            velocity.x += walkAccel * dt.asSeconds();
        playerState = PlayerState::RIGHT;
    }
    if (Input::IsKeyPressed(Keyboard::Down))
    {
        playerState = PlayerState::DOWN;
    }

    prevJump = Input::IsKeyPressed(Keyboard::Up);
    setGraphic(rend, playerState, prevState);

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
    ImGui::Text("State: %d", static_cast<int>(playerState));
}

void PlayerController::setGraphic(SpriteRenderComponent &rend, PlayerState &currentState, PlayerState &previousState)
{
    // Player Draw State
    // TODO: Simplify, Flip features
    rend.useTexture = true;
    rend.useAsSubTexture = true;
    rend.texturePath = "assets\\texture\\mario\\characters.gif";
    rend.m_subSetting.type = SubTexture2DSetting::SubTextureCoordinate;

    switch (currentState)
    {
        // TODO: set state graphic
        case PlayerState::STAND:
            rend.m_subSetting.leftUpper = glm::vec2(256, 1);
            rend.m_subSetting.size = glm::vec2(21, 33);
            break;
        case PlayerState::JUMP:
            if (previousState == PlayerState::RIGHT)
            {
                rend.m_subSetting.leftUpper = glm::vec2(367, 0);
                rend.m_subSetting.size = glm::vec2(21, 33);
            }
            else if (previousState == PlayerState::LEFT)
            {
                rend.m_subSetting.leftUpper = glm::vec2(126, 0);
                rend.m_subSetting.size = glm::vec2(21, 33);
            }
            else
            {
                rend.m_subSetting.leftUpper = glm::vec2(367, 0);
                rend.m_subSetting.size = glm::vec2(21, 33);
            }
            break;
        case PlayerState::RIGHT:
            rend.m_subSetting.leftUpper = glm::vec2(256, 1);
            rend.m_subSetting.size = glm::vec2(21, 33);
            break;
        case PlayerState::LEFT:
            rend.m_subSetting.leftUpper = glm::vec2(237, 0);
            rend.m_subSetting.size = glm::vec2(21, 33);
            break;
        case PlayerState::DOWN:
            if (previousState == PlayerState::RIGHT)
            {
                rend.m_subSetting.leftUpper = glm::vec2(275, 0);
                rend.m_subSetting.size = glm::vec2(21, 33);
            }
            else if (previousState == PlayerState::LEFT)
            {
                rend.m_subSetting.leftUpper = glm::vec2(218, 0);
                rend.m_subSetting.size = glm::vec2(21, 33);
            }
            else
            {
                rend.m_subSetting.leftUpper = glm::vec2(275, 0);
                rend.m_subSetting.size = glm::vec2(21, 33);
            }
            break;
    }
    rend.loadSubTexture();
}

}