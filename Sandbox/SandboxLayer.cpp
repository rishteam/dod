#include "SandboxLayer.h"

#include <Rish/Scene/SystemManager.h>
#include <Rish/Animation/Component.h>

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("ExampleSandboxLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio(), false, true)
{
    VFS::Mount("shader", "Sandbox/assets");
    VFS::Mount("texture", "Sandbox/assets");
    VFS::Mount("animation", "assets/animation");

    m_testTexture = Texture2D::LoadTextureVFS("/texture/RTS_Crate.png");

    m_testSound = Sound::LoadSoundVFS("/music/peko_mono.ogg");

//    m_scene = MakeRef<Scene>();
//    SystemManager::Init(m_scene);
//
//    m_scene->m_debugScene = true;
//    m_scene->m_debugCamera = true;
//
//    auto ent = m_scene->createEntity("Camera");
//    auto &cam = ent.addComponent<CameraComponent>();
//    cam.primary = true;
//    cam.camera.setAspect(16.f / 9.f);
//    cam.camera.setOrthographic(5.f, -1.f, 1.f);
//
//    ent = m_scene->createEntity("test");
//    auto &ani = ent.addComponent<Animation2DComponent>();

//    Texture2DOption opt;
//    opt.minFilter = opt.magFilter = Texture2DFilter::TexNearest;
//
//    ani.duration = 1.f;
//    ani.textureList.push_back(Texture2D::LoadTextureVFS("/animation/reimu-hover0.png", false, opt));
//    ani.textureList.push_back(Texture2D::LoadTextureVFS("/animation/reimu-hover1.png", false, opt));
//    ani.textureList.push_back(Texture2D::LoadTextureVFS("/animation/reimu-hover2.png", false, opt));
//    ani.textureList.push_back(Texture2D::LoadTextureVFS("/animation/reimu-hover3.png", false, opt));

//    loadFromFile("test", ani, "assets/test.rani");

//    ent = m_scene->createEntity("aaaa");
//    auto &sprite = ent.addComponent<SpriteRenderComponent>();
//    sprite.m_texture = m_testTexture;
//    sprite.init = false;

#ifdef TEST_FPS
    m_frameTime.reserve(dataSetSize);
    m_fps.reserve(dataSetSize);
#endif
}

ExampleSandboxLayer::~ExampleSandboxLayer()
{
    SystemManager::Shutdown();
}

void ExampleSandboxLayer::onAttach()
{
    ImGui::LoadIniSettingsFromDisk("assets/layout/ExampleSandboxLayer.ini");

    m_cameraController.setZoom(200);

#ifdef TEST_FPS
    for(int i = 0; i < spriteNumber; i++)
        m_boxList.push_back(TestBox{glm::vec2{Math::RandomFloat(-100.f, 100.f), Math::RandomFloat(-100.f, 100.f)},
            glm::vec2{Math::RandomFloat(0.1f, 5.f), Math::RandomFloat(0.1f, 5.f)}});
#endif

    m_testSound->play();

//    m_scene->onRuntimeInit();
//    m_scene->onScenePlay();
}

void ExampleSandboxLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("assets/layout/ExampleSandboxLayer.ini");

#ifdef TEST_FPS
    std::ofstream of("data.txt", std::ofstream::out | std::ofstream::app);

    of << spriteNumber << ' ';

    float fps = 0.f;
    for(auto i : m_fps)
        fps += i;
    fps /= m_fps.size();
    of << fps << ' ';

    float frameTime = 0.f;
    for(auto i : m_frameTime)
        frameTime += i;
    frameTime /= m_frameTime.size();
    of << frameTime << '\n';
#endif

//    m_scene->onSceneStop();
}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);

    Renderer2D::ResetStats();

    {
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
        RenderCommand::Clear(RenderCommand::ClearBufferTarget::ColorBuffer | RenderCommand::ClearBufferTarget::DepthBuffer);

        Renderer2D::BeginScene(m_cameraController.getCamera(), false);
        for(auto &i : m_boxList)
            Renderer2D::DrawQuad(i.position, i.size, m_testTexture);
        Renderer2D::EndScene();
    }

#ifdef TEST_FPS
    if(clk.getElapsedTime() >= 1.f)
    {
        m_frameTime.push_back(Application::Get().getFrameTime());
        m_fps.push_back(Application::Get().getFps());
        clk.restart();
    }

    if(m_fps.size() >= dataSetSize)
    {
        Application::Get().close();
    }
#endif

//    m_scene->onUpdate(dt);
//    m_scene->onRender();
}

void ExampleSandboxLayer::onImGuiRender()
{
    auto stat = Renderer2D::GetStats();
    ImGui::Begin("Renderer");
    ImGui::Text("fps = %d, frame time = %f", Application::Get().getFps(), Application::Get().getFrameTime());
    ImGui::Separator();
    ImGui::Text("Quad = %d", stat.QuadCount);
    ImGui::Text("Draw = %d", stat.DrawCount);
    ImGui::End();

    Renderer2D::OnImGuiRender();

    ImGui::Begin("Sound Test");
    if(ImGui::Button("Play"))
    {
        m_testSound->play();
    }
    if(ImGui::Button("Pause"))
    {
        m_testSound->pause();
    }
    if(ImGui::Button("Stop"))
    {
        m_testSound->stop();
    }
    ImGui::Text("%.2f/%.2f s",
        m_testSound->getPlayingOffset().asSeconds(), m_testSound->getLength().asSeconds());

    float volume = m_testSound->getVolume();
    ImGui::SliderFloat("Volume", &volume, 0, 1);
    m_testSound->setVolume(volume);

    ImGui::End();

//    m_scene->onImGuiRender();
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
//        m_scene->m_debugScene ^= true;
//        m_scene->m_debugCamera ^= true;
    }
    return false;
}
