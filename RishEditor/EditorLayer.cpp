#include <Rish/rlpch.h>

#include <Rish/Core/Time.h>
#include <Rish/Core/FileSystem.h>
#include <Rish/Input/Input.h>
#include <Rish/Renderer/Renderer.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Utils/FileDialog.h>

#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/ScriptableManager.h>
#include <Rish/Effect/Particle/ParticleSystem.h>

#include <Rish/Physics/PhysicsSystem.h>

#include <Rish/Debug/DebugWindow.h>

#include <Rish/ImGui.h>
#include <imgui_internal.h>

#include <Rish/Scene/ScriptableManager.h>

#include <Rish/Script/Script.h>

#include "EditorLayer.h"

namespace rl {

EditorLayer::EditorLayer()
    : Layer("EditorLayer")
{
	VFS::Mount("shader", "assets/editor/shader");
	VFS::Mount("texture", "assets/editor/texture");
    VFS::Mount("icon", "assets/editor/icon");

    Input::SetInEditor(true);

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
    // Simple Panels
    m_helpPanel = MakeRef<HelpPanel>();
    m_simplePanelList.push_back(m_helpPanel);
    //
    m_aboutPanel = MakeRef<AboutPanel>();
    m_simplePanelList.push_back(m_aboutPanel);
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

    for(auto &panel : m_simplePanelList)
        panel->onAttach();

    // Test
    ScriptableManager::Register<SpriteRoatate>();
    ScriptableManager::Register<CameraController>();
    ScriptableManager::Register<PlayerController>();
}

void EditorLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("assets/layout/editor.ini");
    // Detach all panels
    for(auto &panel : m_panelList)
        panel->onDetach();
    //
    for(auto &panel : m_simplePanelList)
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
        m_editController->onUpdate(dt);

        RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::One);
        Renderer2D::BeginScene(cameraController->getCamera(), false);
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
    //
    // Sync the target entities set
    static std::set<Entity> sceneHierarchyPrevSet{};
    static std::set<Entity> editControllerPrevSet{};

    // Update the panels
    m_sceneHierarchyPanel->onImGuiRender();
    m_componentEditPanel->onImGuiRender();

    // If SceneHierarchyPanel changed
    if(sceneHierarchyPrevSet != m_sceneHierarchyPanel->getTargets())
    {
        m_editController->resetTarget();
        m_editController->addTarget(m_sceneHierarchyPanel->getTargets());
    }

    // If EditController changed
    if(editControllerPrevSet != m_editController->getTargets())
    {
        m_sceneHierarchyPanel->resetTarget();
        m_sceneHierarchyPanel->addTarget(m_editController->getTargets());
    }

    sceneHierarchyPrevSet = m_sceneHierarchyPanel->getTargets();
    editControllerPrevSet = m_editController->getTargets();

    auto &entSet = m_editController->getTargets();
    if(entSet.size() == 1)
    {
        m_componentEditPanel->setTarget(*entSet.begin());
    }
    else
        m_componentEditPanel->resetTarget();
    //
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

    ImGui::Begin(ICON_FA_GAMEPAD " Game");
    {
        // Pull states
        bool isGameWindowFocus = ImGui::IsWindowFocused();
        bool isGameWindowHover = ImGui::IsWindowHovered();

        // Allow input only when game window is focus and hover
        Input::SetMouseState(isGameWindowFocus && isGameWindowHover);

        // Get current window attributes
        ImVec2 size = ImGui::GetContentRegionAvail();
        float fullH = size.y;
        size.y = size.x * 1.f / m_currentScene->getMainCamera().getAspect();
        float dummyH = (fullH - size.y) / 2.f;

        // Respond the resize to the current scene
        m_currentScene->onViewportResize((uint32_t)size.x, (uint32_t)size.y);

        ImVec2 windowCenter = ImGui::GetWindowPos() + ImGui::GetCursorPos() + ImVec2{size.x / 2.f, fullH / 2.f};
        // Set *in editor* mouse pos for Input module
        auto pos = ImGui::GetMousePosRelatedToWindowNormalizeCenter();
        pos.y = pos.y / (size.y / fullH);
        Input::SetGameWindowInEditor(windowCenter.x, windowCenter.y, size.x, size.y);
        Input::OnMouseMove(pos.x, pos.y);

        // Draw the Game
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
                PhysicsSystem::onInit(m_currentScene);
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
        ImGui::SameLine();

        if(ImGui::Button(ICON_FA_BORDER_ALL)){
            m_editController->changeShowGrid();
        }
        ImGui::SameLine();

        // MoveMode button
        if(ImGui::Button(ICON_FA_ARROWS_ALT))
        {
            m_editController->changeGizmoMode(EditController::Gizmo::MoveMode);
        }
        ImGui::SameLine();

        // ZoomMode button
        if(ImGui::Button(ICON_FA_EXPAND))
        {
            m_editController->changeGizmoMode(EditController::Gizmo::ZoomMode);
        }
        ImGui::SameLine();

        // Scale button
        if( ImGui::Button(ICON_FA_EXPAND_ARROWS_ALT) )
        {
            m_editController->changeGizmoMode(EditController::Gizmo::ScaleMode);
        }
        ImGui::SameLine();

        // RotationMode button
        if(ImGui::Button(ICON_FA_SYNC))
        {
            m_editController->changeGizmoMode(EditController::Gizmo::RotationMode);
        }
        ImGui::SameLine();

    }
    ImGui::End();

	// Console
    ImGui::Begin(ICON_FA_TERMINAL " Console");
    ImGui::Text("TODO");
    ImGui::End();

    // Log window
    defaultLogWindow.onImGuiRender();

	ImGui::EndDockspace();

	// Modals
	m_errorModal.onImGuiRender();

	// Simple Panels
    for(auto &panel : m_simplePanelList)
        panel->onImGuiRender();

	// Debug Scene Window
	if(m_debugNativeScript)
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

                // Reset the target
                m_editController->resetTarget();
                m_sceneHierarchyPanel->resetTarget();
                m_componentEditPanel->resetTarget();

                // Switch to new scene
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
                if(ImGui::BeginMenu("Editor Grid"))
                {
//                    ImGui::MenuItem("Show", nullptr, &m_editController->m_showGrid);
                    ImGui::MenuItem("Debug info", nullptr, &m_editController->m_debugEditorGrid);
                    ImGui::EndMenu();
                }
                ImGui::MenuItem("Editor Camera", nullptr, &m_editController->m_debugCameraController);
                ImGui::MenuItem("Editor Controller", nullptr, &m_editController->m_debugEditorController);
                ImGui::MenuItem("Show Icons", nullptr, &m_editController->m_debugShowIcon);
                ImGui::MenuItem("Simulate Particle In Editor", nullptr, &m_editController->m_debugSimulateParticle);
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Scene"))
            {
                ImGui::MenuItem("Editor Scene", nullptr, &m_editorScene->m_debugScene);
                if(m_runtimeScene)
                    ImGui::MenuItem("Runtime Scene", nullptr, &m_runtimeScene->m_debugScene);
                else
                    ImGui::MenuItem("Runtime Scene", nullptr, nullptr, false);
                ImGui::MenuItem("Native Script", nullptr, &m_debugNativeScript);
                ImGui::MenuItem("Scene Camera", nullptr, &m_currentScene->m_debugCamera);
                ImGui::MenuItem("Physics Debug", nullptr, &m_currentScene->m_debugPhysics);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Help"))
        {
            if(ImGui::MenuItem("Help", nullptr))
            {
                m_helpPanel->showPanel();
            }
            if(ImGui::MenuItem("About", nullptr))
            {
                m_aboutPanel->showPanel();
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

    // Recover the target after switch the scene if set
    if(isTargetSet)
    {
        auto entity = scene->getEntityByUUID(id);
        //
        m_editController->addTarget(entity);
        m_sceneHierarchyPanel->addTarget(entity);
        m_componentEditPanel->setTarget(entity);
    }
}

}
