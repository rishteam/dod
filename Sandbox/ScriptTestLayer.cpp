#include "ScriptTestLayer.h"
#include "Script.h"

ScriptTestLayer::ScriptTestLayer()
    : Layer("ScriptTestLayer")
{
    VFS::Mount("shader", "Sandbox/assets");
    VFS::Mount("texture", "Sandbox/assets");
    VFS::Mount("setting", "assets");

    ScriptableManager::Register<CameraController>();
    ScriptableManager::Register<SpriteRoatate>();

    m_scene = MakeRef<Scene>();

//    std::string sceneFile = VFS::ReadTextFile("/scene/test.sce");
//    std::stringstream ss{sceneFile};
//    cereal::JSONInputArchive in(ss);
//    in(cereal::make_nvp("Scene", m_scene));

    auto ent = m_scene->createEntity("A");
    ent.addComponent<RenderComponent>();
    ent.addComponent<NativeScriptComponent>().bind<CameraController>();

    ent = m_scene->createEntity("B");
    auto &cam = ent.addComponent<CameraComponent>();
    cam.primary = true;
    cam.camera.setAspect(16.f / 9.f);
    cam.camera.setOrthographic(5.f, -1.f, 1.f);

    m_scene->onRuntimeInit();
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
