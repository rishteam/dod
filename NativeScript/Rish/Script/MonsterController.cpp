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


    if (boxc.whoCollide.size() == 2)
    {
        for(auto uuid : boxc.whoCollide)
        {
            if(GetScene().getEntityByUUID(uuid).hasComponent<NativeScriptComponent>())
            {
                auto collideEntity = GetScene().getEntityByUUID(uuid);
                if (collideEntity.getComponent<NativeScriptComponent>().scriptName == "rl::EventBoxController")
                {
                    if (monsterState == MonsterState::Left && )
                    {
                        monsterState = MonsterState::Right;
                        dirtyFlag += 1;
                        rigid.velocity.x = 2.0f;
                    }
                    else if (monsterState == MonsterState::Right && dirtyFlag == false)
                    {
                        monsterState = MonsterState::Left;
                        dirtyFlag += 1;
                        rigid.velocity.x = -2.0f;
                    }
                }
            }

        }
    }

    switch (monsterState) {
        case MonsterState::Left:
            rigid.velocity.x = 1.0f;
            if()
            break;
        case MonsterState::Right:
            rigid.velocity.x = 1.0f;

            break;
    }
}

void MonsterController::onImGuiRender()
{
    ImGui::Text("State: %d", static_cast<int>(monsterState));
}

void MonsterController::setGraphic(SpriteRenderComponent &rend, MonsterController &currentState)
{

}

};
