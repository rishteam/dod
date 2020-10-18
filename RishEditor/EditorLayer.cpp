#include <Rish/rlpch.h>

#include <Rish/Core/Time.h>
#include <Rish/Core/FileSystem.h>
#include <Rish/Renderer/Renderer.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Utils/FileDialog.h>

#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/ScriptableManager.h>
#include <Rish/Effect/Particle/ParticleSystem.h>

#include <Rish/Debug/DebugWindow.h>

#include <Rish/ImGui.h>
#include <imgui_internal.h>

// TODO: Remove ME
#include "Script.h"
// TODO: Remove ME

#include "EditorLayer.h"

namespace rl {

EditorLayer::EditorLayer()
    : Layer("EditorLayer")
{
	VFS::Mount("shader", "assets/editor/shader");
	VFS::Mount("texture", "assets/editor/texture");
    VFS::Mount("icon", "assets/editor/icon");

    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());

    m_editorScene = MakeRef<Scene>();
    m_runtimeScene = nullptr;
    //
    m_editController = MakeRef<EditController>();
    m_panelList.push_back(m_editController);
    //
    m_sceneHierarchyPanel = MakeRef<SceneHierarchyPanel>();
    m_panelList.push_back(m_sceneHierarchyPanel);
    //
    m_componentEditPanel = MakeRef<ComponentEditPanel>();
    m_panelList.push_back(m_componentEditPanel);
    //
    switchCurrentScene(m_editorScene);
}

void EditorLayer::onAttach()
{
    ImGui::LoadIniSettingsFromDisk("assets/layout/editor.ini");
    RL_CORE_INFO("[EditorLayer] onAttach");
	FramebufferSpecification fbspec;
	fbspec.width = 1280;
	fbspec.height = 720;
    m_editorFramebuffer = Framebuffer::Create(fbspec);
    m_sceneFramebuffer = Framebuffer::Create(fbspec);
    // Attach all panels
    for(auto &panel : m_panelList)
        panel->onAttach(m_currentScene);

    // Test
    ScriptableManager::Register<SpriteRoatate>();
    ScriptableManager::Register<CameraController>();

    Entity debugEntity = m_currentScene->createEntity("DebugCamera");
    debugEntity.addComponent<CameraComponent>();
    debugEntity.addComponent<NativeScriptComponent>().bind<CameraController>();

    debugEntity = m_currentScene->createEntity("DebugSprite");
    debugEntity.addComponent<RenderComponent>();
    debugEntity.addComponent<NativeScriptComponent>().bind<SpriteRoatate>();

    debugEntity = m_currentScene->createEntity("static physcis");
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

    debugEntity = m_currentScene->createEntity("Physics 2");
    debugEntity.addComponent<RenderComponent>();
    debugEntity.addComponent<RigidBody2DComponent>();
    debugEntity.addComponent<NativeScriptComponent>().bind<SpriteRoatate>();

    debugEntity = m_currentScene->createEntity("Physics 3");
    debugEntity.addComponent<RenderComponent>();
    debugEntity.addComponent<RigidBody2DComponent>();

    debugEntity = m_currentScene->createEntity("Physics 4");
    debugEntity.addComponent<RenderComponent>();
    debugEntity.addComponent<RigidBody2DComponent>();

    debugEntity = m_currentScene->createEntity("ParticleTest");
    debugEntity.addComponent<ParticleComponent>();
}

void EditorLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("assets/layout/editor.ini");
    // Detach all panels
    for(auto &panel : m_panelList)
        panel->onDetach();
}

void EditorLayer::onUpdate(Time dt)
{
    auto cameraController = m_editController->getCameraController();

    // Resize the framebuffer if user resize the viewport
    auto framebufferSpec = m_editorFramebuffer->getSpecification();
    auto framebufferSize = glm::vec2{framebufferSpec.width, framebufferSpec.height};
    if (m_sceneViewportPanelSize != framebufferSize &&
        m_sceneViewportPanelSize.x > 0.f && m_sceneViewportPanelSize.y > 0.f) {
        m_editorFramebuffer->resize((uint32_t) m_sceneViewportPanelSize.x,
                                    (uint32_t) m_sceneViewportPanelSize.y);
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

        RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::One);
        Renderer2D::BeginScene(cameraController->getCamera());
        {
            ParticleSystem::onEditorRender(m_currentScene);
            if(m_editController->m_debugSimulateParticle)
                ParticleSystem::onRender(m_currentScene);
        }
        Renderer2D::EndScene();
        RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::OneMinusSrcAlpha);
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
        m_currentScene->onUpdate(dt);
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
        size.y = size.x * 1.f / m_currentScene->getMainCamera().getAspect();
        float dummyH = (fullH - size.y) / 2.f;

        m_currentScene->onViewportResize((uint32_t)size.x, (uint32_t)size.y);

        uint32_t textureID = m_sceneFramebuffer->getColorAttachmentRendererID();
        ImGui::Dummy({size.x, dummyH});
        ImGui::Image(textureID, size, {0, 0}, {1, -1});
    }
    ImGui::End();
	ImGui::PopStyleVar();

	m_currentScene->onImGuiRender();

    ImGui::Begin("Entity Manager");
    {
    }
    ImGui::End();

    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav);
    {
        // Play button
        if(ImGui::Button(ICON_FA_PLAY))
        {
            if(m_currentScene->getSceneState() == Scene::SceneState::Editor)
            {
                m_runtimeScene = MakeRef<Scene>();
                m_editorScene->copySceneTo(m_runtimeScene);
                switchCurrentScene(m_runtimeScene);
                m_currentScene->onScenePlay();
            }
            else
                m_currentScene->setSceneState(Scene::SceneState::Play); // TODO: remoove me
        }
        ImGui::SameLine();

        // Pause button
        if(ImGui::Button(ICON_FA_PAUSE))
        {
            m_currentScene->onScenePause();
        }
        ImGui::SameLine();

        // Stop button
        if(ImGui::Button(ICON_FA_STOP))
        {
            if(m_currentScene->getSceneState() == Scene::SceneState::Play)
            {
                m_currentScene->onSceneStop();
                switchCurrentScene(m_editorScene);
                m_runtimeScene.reset();
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

	// Debug Scene Window
	if(m_debugScene)
    {
	    ImGui::Begin("Scene Debug");
        ImGui::Text("CurrentScene: %p", (void*)m_currentScene.get());
        ImGui::Separator();
        DrawSceneDebugWindow("EditorScene", m_editorScene);
        DrawSceneDebugWindow("RuntimeScene", m_runtimeScene);
	    ImGui::End();
    }
}

void EditorLayer::onImGuiMainMenuRender()
{
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("New Scene", "Ctrl+N"))
            {
                // TODO: not save warning
                m_editorScene = nullptr;
                m_editorScene = MakeRef<Scene>();
                switchCurrentScene(m_editorScene);
            }

            if (ImGui::MenuItem("Open Scene", "Ctrl+O", false, m_currentScene == m_editorScene))
            {
                // Open File
                std::string path, content;
                if(FileDialog::SelectSingleFile(nullptr, nullptr, path))
                {
                    content = FileSystem::ReadTextFile(path);
                    m_scenePath = path;
//                    RL_INFO("content = {}", content);
                }

                // Deserialize
                std::string exceptionMsg;
                std::stringstream oos(content);
                bool failLoad = false;
                try
                {
                    cereal::JSONInputArchive inputArchive(oos);
                    inputArchive(cereal::make_nvp("Scene", m_editorScene));
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
                    setContextToPanels(m_editorScene);

                    m_editorScene->onEditorInit();
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
                outputArchive(cereal::make_nvp("Scene", m_editorScene));
            }

            if (ImGui::MenuItem("Save Scene as", "Ctrl-Shift+S"))
            {
                std::string path;
                if(FileDialog::SelectSaveFile("sce", nullptr, path))
                {
                    // TODO: Maybe implement a function return ofstream from rl::FileSystem
                    std::ofstream os(path);
                    cereal::JSONOutputArchive outputArchive(os);
                    outputArchive(cereal::make_nvp("Scene", m_editorScene));
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
                ImGui::MenuItem("Simulate Particle In Editor", nullptr, &m_editController->m_debugSimulateParticle);
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Scene"))
            {
                ImGui::MenuItem("Scene", nullptr, &m_debugScene);
                ImGui::MenuItem("Scene Camera", nullptr, &m_currentScene->m_debugCamera);
                ImGui::MenuItem("Physics Debug", nullptr, &m_currentScene->m_debugPhysics);
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
    UUID id;
    bool isTargetSet = false;

    // Get the UUID of current selection if scene is not nullptr
    if(m_sceneHierarchyPanel->getContext())
    {
        auto &ent = m_sceneHierarchyPanel->getTargets();
        //
        if(ent.size() == 1)
        {
            id = ent.begin()->getUUID();
            isTargetSet = true;
        }
    }

    // Switch the scene
    m_currentScene = scene;
    setContextToPanels(scene);

    // Reset Editor Panel target
    m_editController->resetTarget();
    m_sceneHierarchyPanel->resetTarget();
    m_componentEditPanel->resetTarget();

    if(isTargetSet)
    {
        auto entity = scene->getEntityByUUID(id);
        //
        m_editController->setTarget(entity);
        m_sceneHierarchyPanel->addTarget(entity);
        m_componentEditPanel->setTarget(entity);
    }
}

}
