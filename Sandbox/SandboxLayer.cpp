#include "SandboxLayer.h"

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

//    m_testTexture = Texture2D::LoadTextureVFS("/texture/RTS_Crate.png");
    load("animation", m_testTexture, "assets/a.rtex");
}

void ExampleSandboxLayer::onAttach()
{
    ImGui::LoadIniSettingsFromDisk("assets/layout/ExampleSandboxLayer.ini");

//    m_component.currentFrame = 0;
//    m_component.duration = 87.0f;
//    m_component.reverseDuration = 56.f;
//    m_component.loop = true;
//    m_component.reverse = false;
//    m_component.texturePrefix = "Test";
//    m_component.textureList.push_back(m_testTexture);
//    m_component.textureList.push_back(m_testTexture);
//    m_component.textureList.push_back(m_testTexture);

//    std::ofstream of("assets/test.rani");
//    cereal::JSONOutputArchive outputArchive(of);
//    outputArchive(cereal::make_nvp("AnimationComponent", m_component));
    load("AnimationComponent", m_component, "assets/test.rani");

    std::string path;
    VFS::ResolvePhysicalPath("/animation/reimu-hover.ani", path);
    m_testAni.loadConfig(path);
}

void ExampleSandboxLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("assets/layout/ExampleSandboxLayer.ini");
}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);

    int cnt = 0;

    Renderer2D::BeginScene(m_cameraController.getCamera());
    Renderer2D::DrawQuad({cnt, 0.f}, {1.f, 1.f}, m_testTexture);
    Renderer2D::EndScene();
}

void ExampleSandboxLayer::onImGuiRender()
{
}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}
