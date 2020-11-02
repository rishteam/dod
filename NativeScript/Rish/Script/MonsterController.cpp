#include <Rish/Script/MonsterController.h>


namespace rl
{

void MonsterController::onCreate()
{

}

void MonsterController::onDestroy()
{

}

void MonsterController::onUpdate(Time dt)
{
    auto &rigid = GetComponent<RigidBody2DComponent>();
    auto &trans = GetComponent<TransformComponent>();;
    auto &boxc = GetComponent<BoxCollider2DComponent>();
    auto &rend = GetComponent<SpriteRenderComponent>();

    // TODO: Hard Fixed
    trans.rotate = 0.0f;
    rigid.angle = 0.0f;

    setGraphic(rend, monsterState);
    switch (monsterState) {
        case MonsterState::Left:
            rigid.velocity.x = -1.0f;
            break;
        case MonsterState::Right:
            rigid.velocity.x = 1.0f;
            break;
    }

    // if state Change, rigid value must will be calculated by physics
    if(stateChange == true)
    {
        stateChange = false;
        return;
    }

    // TODO: should be optimize
    if (boxc.whoCollide.size() >= 2)
    {
        for(auto uuid : boxc.whoCollide)
        {
            if(GetScene().getEntityByUUID(uuid).hasComponent<NativeScriptComponent>())
            {
                auto collideEntity = GetScene().getEntityByUUID(uuid);
                if (collideEntity.getComponent<NativeScriptComponent>().scriptName == "rl::EventBoxController")
                {
                    if (monsterState == MonsterState::Left)
                    {
                        monsterState = MonsterState::Right;
                    }
                    else if (monsterState == MonsterState::Right)
                    {
                        monsterState = MonsterState::Left;
                    }
                    stateChange = true;
                }
            }

        }
    }
}

void MonsterController::onImGuiRender()
{
    ImGui::Text("State: %d", static_cast<int>(monsterState));
}

void MonsterController::setGraphic(SpriteRenderComponent &rend, MonsterState &monsterState)
{
    // TODO: Simplify, Flip features
    rend.useTexture = true;
    rend.useAsSubTexture = true;
    rend.texturePath = "assets\\texture\\mario\\role.png";
    rend.m_subSetting.type = SubTexture2DSetting::SubTextureCoordinate;

    switch (monsterState) {
        case MonsterState::Left:
            rend.m_subSetting.leftUpper = glm::vec2(181, 198.5);
            rend.m_subSetting.size = glm::vec2(18, 30);
            break;
        case MonsterState::Right:
            rend.m_subSetting.leftUpper = glm::vec2(294, 198.5);
            rend.m_subSetting.size = glm::vec2(18, 30);
            break;
    }
    rend.loadSubTexture();
}

};
