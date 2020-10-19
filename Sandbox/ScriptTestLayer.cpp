#include "ScriptTestLayer.h"
#include "Script.h"

ScriptTestLayer::ScriptTestLayer()
    : Layer("ScriptTestLayer")
{
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");

    ScriptableManager::Register<CameraController>();
    ScriptableManager::Register<SpriteRoatate>();

    m_scene = MakeRef<Scene>();

    std::string sceneFile = VFS::ReadTextFile("/scene/test2.sce");
    std::stringstream ss{sceneFile};
    cereal::JSONInputArchive in(ss);
    in(cereal::make_nvp("Scene", m_scene));

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
