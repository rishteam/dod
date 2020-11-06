#include <Rish/Script/Spawner.h>

#include <Rish/Script/TestScript.h>

namespace rl {

void Spawner::onCreate()
{
}

void Spawner::onDestroy()
{
}

void Spawner::onUpdate(Time dt)
{
    static int cnt = 0;
    auto &trans = GetEntity().getComponent<TransformComponent>();
    if(m_spawnPerSecond > 0.f && clk.getElapsedTime() >= m_spawnPerSecond)
    {
        Entity ent = CreateEntity(fmt::format("Spawnd {}", cnt++));
        m_spawned.insert(ent);
        ent.getComponent<TransformComponent>() = trans;
        ent.addComponent<SpriteRenderComponent>("/texture/RTS_Crate.png");
        ent.addComponent<RigidBody2DComponent>(10.f);
        ent.addComponent<BoxCollider2DComponent>(0.f, 0.f, trans.scale.x, trans.scale.y);
        ent.addComponent<NativeScriptComponent>().bind<TestScript>(ent);
        clk.restart();
    }
    //
    std::set<Entity> delTarget;
    for(Entity ent : m_spawned)
    {
        // Check if ent is still valid
        if(!ent)
        {
            delTarget.insert(ent);
            continue;
        }
        //
        auto &entTrans = ent.getComponent<TransformComponent>();
        if(glm::distance(trans.translate, entTrans.translate) >= 100.f)
        {
            delTarget.insert(ent);
            ent.destroy();
        }
    }
    //
    for(Entity ent : delTarget)
        m_spawned.erase(ent);
}

void Spawner::onImGuiRender()
{
    ImGui::DragFloat("Spawn per second", &m_spawnPerSecond);
    ImGui::Text("Alive");
    for(Entity ent : m_spawned)
    {
        if(!ent)
            continue;
        ImGui::PushID((void*)&ent);
        const UUID &uuid = ent.getUUID();
        const std::string &name = ent.getName();
        ImGui::Text("%s %s", uuid.to_c_str(), name.c_str());
        ImGui::PopID();
    }
}

}