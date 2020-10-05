#include <Rish/rlpch.h>

#include <Rish/Core/Time.h>
#include <Rish/Core/FileSystem.h>
#include <Rish/Renderer/Renderer.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Utils/FileDialog.h>

#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/ScriptableManager.h>

#include <Rish/ImGui.h>
#include <imgui_internal.h>

#include "EditorLayer.h"

namespace rl {

EditorLayer::EditorLayer()
    : Layer("EditorLayer")
{
	VFS::Mount("shader", "assets/editor/shader");
	VFS::Mount("texture", "assets/editor/texture");
    VFS::Mount("icon", "assets/editor/icon");

    ImGui::LoadIniSettingsFromDisk("RishEditor/imgui.ini");

    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());

    m_editorScene = MakeRef<Scene>();
    m_scene = m_editorScene;
    m_runtimeScene = MakeRef<Scene>();
    //
    m_editController = MakeRef<EditController>();
    m_panelList.push_back(m_editController);
    //
    m_sceneHierarchyPanel = MakeRef<SceneHierarchyPanel>();
    m_panelList.push_back(m_sceneHierarchyPanel);
    //
    m_componentEditPanel = MakeRef<ComponentEditPanel>();
    m_panelList.push_back(m_componentEditPanel);
}

class CameraController : public ScriptableEntity
{
public:
    void onUpdate(Time dt) override
    {
        auto &trans = getComponent<TransformComponent>().translate;
        RL_INFO("onUpdate() {}", dt.asSeconds());

        float speed = 10.f;

        if(Input::IsKeyPressed(Keyboard::W))
            trans.y += speed * dt.asSeconds();
        if(Input::IsKeyPressed(Keyboard::S))
            trans.y -= speed * dt.asSeconds();
        if(Input::IsKeyPressed(Keyboard::A))
            trans.x -= speed * dt.asSeconds();
        if(Input::IsKeyPressed(Keyboard::D))
            trans.x += speed * dt.asSeconds();
    }

    void onImGuiRender() override
    {
        auto &trans = getComponent<TransformComponent>().translate;
        ImGui::DragFloat3("Translate", glm::value_ptr(trans));
    }
};

class SpriteRoatate : public ScriptableEntity
{
public:
    void onUpdate(Time dt) override
    {
        auto &trans = getComponent<TransformComponent>();
        trans.rotate += 100.f * dt.asSeconds();
        trans.rotate = std::fmod(trans.rotate, 360.f);
    }

    void onImGuiRender() override
    {
    }
};

void EditorLayer::onAttach()
{
    RL_CORE_INFO("[EditorLayer] onAttach");
	FramebufferSpecification fbspec;
	fbspec.width = 1280;
	fbspec.height = 720;
    m_editorFramebuffer = Framebuffer::Create(fbspec);
    m_sceneFramebuffer = Framebuffer::Create(fbspec);
    // Attach all panels
    for(auto &panel : m_panelList)
        panel->onAttach(m_scene);

    // Test
    ScriptableManager::Register<SpriteRoatate>();
    ScriptableManager::Register<CameraController>();

    Entity debugEntity = m_scene->createEntity("DebugCamera");
    debugEntity.addComponent<CameraComponent>();
    debugEntity.addComponent<NativeScriptComponent>().bind<CameraController>();

//    debugEntity = m_scene->createEntity("DebugSprite");
//    debugEntity.addComponent<RenderComponent>();
//    debugEntity.addComponent<NativeScriptComponent>().bind<SpriteRoatate>();

    debugEntity = m_scene->createEntity("static physcis");
    debugEntity.addComponent<RenderComponent>();
    debugEntity.addComponent<RigidBody2DComponent>();
    debugEntity.addComponent<BoxCollider2DComponent>();

    auto &rigbd = debugEntity.getComponent<RigidBody2DComponent>();
    auto &box = debugEntity.getComponent<BoxCollider2DComponent>();
    rigbd.mass = MAX_float;
    box.x = 0.0f;
    box.y = 0.0f;
    box.w = 3.0f;
    box.h = 3.0f;

    debugEntity = m_scene->createEntity("Physics 2");
    debugEntity.addComponent<RenderComponent>();
    debugEntity.addComponent<RigidBody2DComponent>();

    debugEntity = m_scene->createEntity("Physics 3");
    debugEntity.addComponent<RenderComponent>();
    debugEntity.addComponent<RigidBody2DComponent>();

    debugEntity = m_scene->createEntity("Physics 4");
    debugEntity.addComponent<RenderComponent>();
    debugEntity.addComponent<RigidBody2DComponent>();

}

void EditorLayer::onDetach()
{
    RL_CORE_INFO("[EditorLayer] onDetach");

    ImGui::SaveIniSettingsToDisk("RishEditor/imgui.ini");
    // Detach all panels
    for(auto &panel : m_panelList)
        panel->onDetach();
}

void EditorLayer::onUpdate(Time dt)
{
    switch(m_scene->getSceneState())
    {
        case Scene::SceneState::Editor:

        break;

        case Scene::SceneState::Play:

        break;

        case Scene::SceneState::Pause:

        break;
    }
    //
    auto cameraController = m_editController->getCameraController();

    // Resize the framebuffer if user resize the viewport
    auto framebufferSpec = m_editorFramebuffer->getSpecification();
    auto framebufferSize = glm::vec2{framebufferSpec.width, framebufferSpec.height};
    if(m_sceneViewportPanelSize != framebufferSize &&
        m_sceneViewportPanelSize.x > 0.f && m_sceneViewportPanelSize.y > 0.f)
    {
        m_editorFramebuffer->resize((uint32_t)m_sceneViewportPanelSize.x,
                                    (uint32_t)m_sceneViewportPanelSize.y);
        cameraController->onResize(m_sceneViewportPanelSize.x, m_sceneViewportPanelSize.y);
    }

    // TODO: Rendering Queue
    /////////////////////////////////////////////////////////////////////////////////////////////
    // Editor
    /////////////////////////////////////////////////////////////////////////////////////////////
    Renderer2D::ResetStats();
    m_editorFramebuffer->bind();
    {
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
        RenderCommand::Clear();
        //
        Renderer2D::BeginScene(cameraController->getCamera());
        m_editController->onUpdate(dt);
        Renderer2D::EndScene();
    }
    m_editorFramebuffer->unbind();

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Scene
    /////////////////////////////////////////////////////////////////////////////////////////////
    m_sceneFramebuffer->bind();
    {
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
        RenderCommand::Clear();
        //
        m_scene->onUpdate(dt);
    }
    m_sceneFramebuffer->unbind();
}

void EditorLayer::onImGuiRender()
{
    ImGui::BeginDockspace("EditorDockspace");
    // Menu Bar
    onImGuiMainMenuRender();

    // Select from editor
    if(m_editController->isSelected())
    {
        auto ent = m_editController->getTarget();
        m_sceneHierarchyPanel->resetTarget();
        m_sceneHierarchyPanel->addTarget(ent);
    }

    // Update SceneHierarchyPanel
    m_sceneHierarchyPanel->onImGuiRender();

    // When SceneHierarchyPanel selected one entity
    if (m_sceneHierarchyPanel->selectedSize() == 1 &&
        m_sceneHierarchyPanel->isSelected())
    {
        auto ent = *m_sceneHierarchyPanel->getSelectedEntities().begin();
        m_componentEditPanel->setTarget(ent);
        m_editController->setTarget(ent);
    }
    else
    {
        m_componentEditPanel->resetSelected();
        m_editController->resetSelected();
    }

    m_componentEditPanel->onImGuiRender();

    // Scene View
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin(ICON_FA_BORDER_ALL " Scene");
    {
        // update edit controller
        m_editController->onImGuiRender();
        // Update viewport size (For resizing the framebuffer)
        auto size = ImGui::GetContentRegionAvail();
        m_sceneViewportPanelSize = glm::vec2{size.x, size.y};
        // show scene
        uint32_t textureID = m_editorFramebuffer->getColorAttachmentRendererID();
        ImGui::Image(textureID, size, {0, 0}, {1, -1});
    }
	ImGui::End();

    ImVec2 size; // debug
    ImGui::Begin(ICON_FA_GAMEPAD " Game");
    {
        size = ImGui::GetContentRegionAvail();
        float fullH{};
        fullH = size.y;
        size.y = size.x * 1.f / m_scene->getMainCamera().getAspect();
        float dummyH = (fullH - size.y) / 2.f;

        m_scene->onViewportResize((uint32_t)size.x, (uint32_t)size.y);

        uint32_t textureID = m_sceneFramebuffer->getColorAttachmentRendererID();
        ImGui::Dummy({size.x, dummyH});
        ImGui::Image(textureID, size, {0, 0}, {1, -1});
    }
    ImGui::End();
	ImGui::PopStyleVar();

	m_scene->onImGuiRender();

    ImGui::Begin("Entity Manager");
    {
    }
    ImGui::End();

    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav);
    {
        if(ImGui::Button(ICON_FA_PLAY))
        {
            if(m_scene->getSceneState() == Scene::SceneState::Editor)
            {
                m_editorScene->copySceneTo(m_runtimeScene);
                switchCurrentScene(m_runtimeScene);
                m_scene->onScenePlay();
            }
            else
                m_scene->setSceneState(Scene::SceneState::Play); // TODO: remoove me
        }
        ImGui::SameLine();
        if(ImGui::Button(ICON_FA_PAUSE))
        {
            m_scene->onScenePause();
        }
        ImGui::SameLine();
        if(ImGui::Button(ICON_FA_STOP))
        {
            if(m_scene->getSceneState() == Scene::SceneState::Play)
            {
                m_scene->onSceneStop();
                switchCurrentScene(m_editorScene);
            }
        }
    }
    ImGui::End();

	// Console
    ImGui::Begin(ICON_FA_TERMINAL " Console");
    ImGui::Text("TODO");
    ImGui::End();

    // Log window
    defaultLogWindow.onImGuiRender();

	ImGui::EndDockspace();

	m_errorModal.onImGuiRender();
}

void EditorLayer::onImGuiMainMenuRender()
{
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("New Scene", "Ctrl+N", false, false))
            {
                RL_ERROR("Not implemented");
            }

            if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
            {
                // Open File
                std::string path, content;
                if(FileDialog::SelectSingleFile(nullptr, nullptr, path))
                {
                    content = FileSystem::ReadTextFile(path);
                    m_scenePath = path;
                    RL_INFO("content = {}", content);
                }

                // Deserialize
                std::string exceptionMsg;
                std::stringstream oos(content);
                bool failLoad = false;
                try
                {
                    cereal::JSONInputArchive inputArchive(oos);
                    inputArchive(cereal::make_nvp("Scene", m_scene));
                }
                catch (cereal::RapidJSONException &e)
                {
                    RL_CORE_ERROR("Failed to load scene {}", e.what());
                    exceptionMsg = e.what();
                    failLoad = true;
                }
                catch (cereal::Exception &e)
                {
                    RL_CORE_ERROR("Failed to load scene {}", e.what());
                    exceptionMsg = e.what();
                    failLoad = true;
                }
                // If success
                if(!failLoad)
                {
                    m_sceneLoaded = true;

                    // Because the panels are now holding strong ref to the scene
                    // We need to reset the context
                    setContextToPanels(m_scene);
                }
                else
                {
                    m_sceneLoaded = false;

                    m_errorModal.setMessage(fmt::format("Failed to load scene {}.\n{}", m_scenePath, exceptionMsg));
                }
            }

            ImGui::Separator();

            // TODO: hot reload?
            if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, m_sceneLoaded))
            {
                // TODO: Maybe implement a function return ofstream from rl::FileSystem
                std::ofstream os(m_scenePath);
                cereal::JSONOutputArchive outputArchive(os);
                outputArchive(cereal::make_nvp("Scene", m_scene));
            }

            if (ImGui::MenuItem("Save Scene as", "Ctrl-Shift+S"))
            {
                std::string path;
                if(FileDialog::SelectSaveFile(nullptr, nullptr, path))
                {
                    // TODO: Maybe implement a function return ofstream from rl::FileSystem
                    std::ofstream os(path);
                    cereal::JSONOutputArchive outputArchive(os);
                    outputArchive(cereal::make_nvp("Scene", m_scene));
                }
            }

            ImGui::Separator();

            if(ImGui::MenuItem("Exit", "Ctrl+W"))
            {
                Application::Get().close();
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Tools"))
        {
            if(ImGui::MenuItem("Renderer Statistics"))
            {
                // TODO: Use overlay
                auto stat = Renderer2D::GetStats();
                RL_INFO("Renderer2D: quad = {}, line = {}, draw = {}", stat.QuadCount, stat.LineCount, stat.DrawCount);
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Debug"))
        {
            if(ImGui::BeginMenu("Edit Controller"))
            {
                ImGui::MenuItem("Editor Grid", nullptr, &m_editController->m_debugEditorGrid);
                ImGui::MenuItem("Editor Camera", nullptr, &m_editController->m_debugCameraController);
                ImGui::MenuItem("Editor Controller", nullptr, &m_editController->m_debugEditorController);
                ImGui::MenuItem("Show Icons", nullptr, &m_editController->m_debugShowIcon);
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Scene"))
            {
                ImGui::MenuItem("Scene Camera", nullptr, &m_scene->m_debugCamera);
                ImGui::MenuItem("Physics Debug", nullptr, &m_scene->m_debugPhysics);
                ImGui::MenuItem("Camera Component Debug", nullptr, &m_scene->m_debugCameraComponent);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void EditorLayer::onEvent(rl::Event& e)
{
    m_editController->onEvent(e);
}

void EditorLayer::setContextToPanels(const Ref <Scene> &scene)
{
    for(auto & panel : m_panelList)
        panel->setContext(scene);
}

void EditorLayer::switchCurrentScene(const Ref<Scene> &scene)
{
    m_scene = scene;
    setContextToPanels(scene);

    // Reset Editor Panel target
    m_editController->resetTarget();
    m_sceneHierarchyPanel->resetTarget();
    m_componentEditPanel->resetTarget();
}

}
