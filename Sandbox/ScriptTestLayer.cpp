#include "ScriptTestLayer.h"

class TestScript : public ScriptableEntity
{
public:
    TestScript()
    {
    }
    ~TestScript() override
    {
    }

    void onUpdate(Time dt) override
    {
        if(clk.getElapsedTime() >= 1.f)
        {
            RL_INFO("onUpdate = {}", dt.asSeconds());
            clk.restart();
        }
    }

    void onImGuiRender() override
    {
    }

private:
    Clock clk;
    float m_speed = 10.f;
};

class CameraController : public ScriptableEntity
{
public:
    void onUpdate(Time dt) override
    {
        auto &trans = getComponent<TransformComponent>().translate;

        if(Input::IsKeyPressed(Keyboard::W))
            trans.y += m_speed * dt.asSeconds();
        if(Input::IsKeyPressed(Keyboard::S))
            trans.y -= m_speed * dt.asSeconds();
        if(Input::IsKeyPressed(Keyboard::A))
            trans.x -= m_speed * dt.asSeconds();
        if(Input::IsKeyPressed(Keyboard::D))
            trans.x += m_speed * dt.asSeconds();
    }

    void onImGuiRender() override
    {
        auto &trans = getComponent<TransformComponent>().translate;
        ImGui::DragFloat3("Translate", glm::value_ptr(trans));
        ImGui::DragFloat("Speed", &m_speed);

        ImGui::Checkbox("Inverted", &m_inverted);
    }

private:
    float m_speed = 10.f;
    bool m_inverted = false;
};

ScriptTestLayer::ScriptTestLayer()
    : Layer("ScriptTestLayer")
{
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");

    m_scene = MakeRef<Scene>();

    auto ent = m_scene->createEntity("Test");
    ent.addComponent<NativeScriptComponent>().bind<TestScript>();
    auto &render = ent.addComponent<RenderComponent>();
    render.m_texture = Texture2D::LoadTextureVFS("/texture/1.png");
    m_testSprite = render.m_texture;

    ent = m_scene->createEntity("camera");
    ent.addComponent<NativeScriptComponent>().bind<CameraController>();
    auto &camera = ent.addComponent<CameraComponent>();
    camera.primary = true;
    camera.camera.setAspect(16.f / 9.f);

    auto & transform = ent.getComponent<TransformComponent>();
    transform.scale.y = 5;
    transform.scale.x = 5 * camera.camera.getAspect();
}

ScriptTestLayer::~ScriptTestLayer()
{
}

void ScriptTestLayer::onAttach()
{
    ImGui::LoadIniSettingsFromDisk("assets/layout/ScriptTestLayer.ini");

    m_scene->onScenePlay();
}

void ScriptTestLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("assets/layout/ScriptTestLayer.ini");

    m_scene->onSceneStop();
}

void ScriptTestLayer::onUpdate(rl::Time dt)
{
    m_scene->onUpdate(dt);
}

void ScriptTestLayer::onImGuiRender()
{
    m_scene->onImGuiRender();
}

void ScriptTestLayer::onEvent(Event &event)
{
    EventDispatcher dispatcher(event);
    dispatcher.dispatch<KeyPressedEvent>(RL_BIND_EVENT_FUNC(ScriptTestLayer::onKeyPressEvent));
}

bool ScriptTestLayer::onKeyPressEvent(KeyPressedEvent &event)
{
    if(event.keyCode == Keyboard::Tilde)
    {
        m_scene->m_debugScene ^= true;
    }
    return false;
}
