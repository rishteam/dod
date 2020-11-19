#include "SandboxLayer.h"

#include <Rish/Scene/SystemManager.h>
#include <Rish/Animation/Component.h>

template<typename T>
void save(const std::string &name, T& obj, const std::string &path)
{
    std::ofstream of(path);
    cereal::JSONOutputArchive outputArchive(of);
    outputArchive(cereal::make_nvp(name, obj));
}

template<typename T>
void load(const std::string &name, T& obj, const std::string &path)
{
    std::stringstream oos(FileSystem::ReadTextFile(path));
    cereal::JSONInputArchive inputArchive(oos);
    inputArchive(cereal::make_nvp(name, obj));
}

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("ExampleSandboxLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio(), false, true)
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    VFS::Mount("shader", "Sandbox/assets");
    VFS::Mount("texture", "Sandbox/assets");
    VFS::Mount("animation", "assets/animation");

    m_testTexture = Texture2D::LoadTextureVFS("/texture/RTS_Crate.png");
//    load("animation", m_testTexture, "assets/a.rtex");

    m_scene = MakeRef<Scene>();
    SystemManager::Init(m_scene);

    m_scene->m_debugScene = true;

    auto ent = m_scene->createEntity("Camera");
    auto &cam = ent.addComponent<CameraComponent>();
    cam.primary = true;
    cam.camera.setAspect(16.f / 9.f);
    cam.camera.setOrthographic(5.f, -1.f, 1.f);

    ent = m_scene->createEntity("test");
    auto &ani = ent.addComponent<Animation2DComponent>();

    ani.duration = 3.f;
    ani.textureList.push_back(Texture2D::LoadTextureVFS("/animation/reimu-hover0.png"));
    ani.textureList.push_back(Texture2D::LoadTextureVFS("/animation/reimu-hover1.png"));
    ani.textureList.push_back(Texture2D::LoadTextureVFS("/animation/reimu-hover2.png"));
    ani.textureList.push_back(Texture2D::LoadTextureVFS("/animation/reimu-hover3.png"));

    ent = m_scene->createEntity("aaaa");
    ent.addComponent<SpriteRenderComponent>();
}

ExampleSandboxLayer::~ExampleSandboxLayer()
{
    SystemManager::Shutdown();
}

void ExampleSandboxLayer::onAttach()
{
    ImGui::LoadIniSettingsFromDisk("assets/layout/ExampleSandboxLayer.ini");

    m_scene->onRuntimeInit();
    m_scene->onScenePlay();
}

void ExampleSandboxLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("assets/layout/ExampleSandboxLayer.ini");
    m_scene->onSceneStop();
}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);

    Renderer2D::BeginScene(m_cameraController.getCamera());
    Renderer2D::DrawQuad({0.f, 0.f}, {1.f, 1.f}, m_testTexture);
    Renderer2D::EndScene();

    m_scene->onUpdate(dt);
}

void ExampleSandboxLayer::onImGuiRender()
{
    m_scene->onImGuiRender();
}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
    EventDispatcher dispatcher(event);
    dispatcher.dispatch<KeyPressedEvent>(RL_BIND_EVENT_FUNC(ExampleSandboxLayer::onKeyPressEvent));
}

bool ExampleSandboxLayer::onKeyPressEvent(KeyPressedEvent &event)
{
    if(event.keyCode == Keyboard::Tilde)
    {
        m_scene->m_debugScene ^= true;
    }
    return false;
}
