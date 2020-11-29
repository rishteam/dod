#include <Rish/rlpch.h>

// Core utilities
#include <Rish/Core/Application.h>
#include <Rish/Core/Time.h>
#include <Rish/Core/FileSystem.h>
#include <Rish/Input/Input.h>
#include <Rish/Renderer/Renderer.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Utils/FileDialog.h>
#include <Rish/Utils/String.h>

// Script
#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/ScriptableManager.h>
#include <Rish/Scene/SystemManager.h>

// Systems
#include <Rish/Effect/Particle/ParticleSystem.h>
#include <Rish/Scene/System/NativeScriptSystem.h>
#include <Rish/Physics/PhysicsSystem.h>
#include <Rish/Scene/System/SpriteRenderSystem.h>
#include <Rish/Animation/Animation2DSystem.h>

// Components
#include <Rish/Animation/Component.h>

#include <Rish/Debug/DebugWindow.h>
#include <Rish/Debug/ImGuiLogWindow.h>

#include <Rish/ImGui/MenuAction.h>
#include <imgui_internal.h>

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

    // Initialize main panels
    {
        m_editController = MakeRef<EditController>();
        m_panelList.push_back(m_editController);
        //
        m_sceneHierarchyPanel = MakeRef<SceneHierarchyPanel>();
        m_panelList.push_back(m_sceneHierarchyPanel);
        //
        m_componentEditPanel = MakeRef<ComponentEditPanel>();
        m_panelList.push_back(m_componentEditPanel);
        //
        m_statusBarPanel = MakeRef<StatusBarPanel>();
        m_panelList.push_back(m_statusBarPanel);
    }

    // Initialize Simple Panels
    {
        m_helpPanel = MakeRef<HelpPanel>();
        m_simplePanelList.push_back(m_helpPanel);
        //
        m_aboutPanel = MakeRef<AboutPanel>();
        m_simplePanelList.push_back(m_aboutPanel);
        //
        m_settingPanel = MakeRef<SettingPanel>();
        m_simplePanelList.push_back(m_settingPanel);
        // Bind contexts
        for(auto &p : m_simplePanelList)
            p->setContext(this);
    }

    // Switch the current scene
    switchCurrentScene(m_editorScene);

    // Start auto save thread
    // TODO: Make RishEngine handle thread management
//    m_autoSaveRun = true;
//    std::thread autoSaveThread(&EditorLayer::autoSave, this);
//    autoSaveThread.detach();

    // Actions
    // TODO: Make actions into callback function?
    // TODO: Rethink the whole action and shortcut situation and think about the undo&redo functionality
    initShortCut();
}

EditorLayer::~EditorLayer()
{
    SystemManager::Shutdown();
}

void EditorLayer::onAttach()
{
    ImGui::LoadIniSettingsFromDisk("assets/layout/editor.ini");
    RL_CORE_INFO("[EditorLayer] onAttach");
    //
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

    ScriptableManager::Init();

    loadSetting("setting.conf");

    if(m_editorSetting.isDefaultOpenScene)
    {
        openScene(m_editorSetting.path);
        m_scenePath = m_editorSetting.path;
    }

    auto ent = m_currentScene->createEntity("AniTestEnt");
    auto &ani = ent.addComponent<Animation2DComponent>();

}

void EditorLayer::onDetach()
{
    ScriptableManager::Shutdown();

    // Close Auto Save Thread
    m_autoSaveRun = false;

    ImGui::SaveIniSettingsToDisk("assets/layout/editor.ini");
    // Detach all panels
    for(auto &panel : m_panelList)
        panel->onDetach();
    //
    for(auto &panel : m_simplePanelList)
        panel->onDetach();

    if(m_editorSetting.saveSettingOnExit)
        saveSetting();
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
        RenderCommand::Clear(RenderCommand::ClearBufferTarget::ColorBuffer | RenderCommand::ClearBufferTarget::DepthBuffer);
        //
        m_editController->onUpdate(dt);

        // Particle System
        {
            RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha, RenderCommand::BlendFactor::One);
            Renderer2D::BeginScene(cameraController->getCamera(), false);
            ParticleSystem::onEditorRender(m_editController->m_debugSimulateParticle);
            Renderer2D::EndScene();
            RenderCommand::SetBlendFunc(RenderCommand::BlendFactor::SrcAlpha,
                                        RenderCommand::BlendFactor::OneMinusSrcAlpha);
        }
        // Animation2D
        {
            Renderer2D::BeginScene(cameraController->getCamera(), true);
            Animation2DSystem::OnEditorRender();
            Renderer2D::EndScene();
        }
    }
    m_editorFramebuffer->unbind();

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Scene
    /////////////////////////////////////////////////////////////////////////////////////////////
    m_sceneFramebuffer->bind();
    {
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
        RenderCommand::Clear(RenderCommand::ClearBufferTarget::ColorBuffer | RenderCommand::ClearBufferTarget::DepthBuffer);
        //
        m_currentScene->onUpdate(dt);
        m_currentScene->onRender();
    }
    m_sceneFramebuffer->unbind();
}

void EditorLayer::onImGuiRender()
{
    ImGui::BeginDockspace("EditorDockspace");
    // Menu Bar
    onImGuiMainMenuRender();

    // Panel sync
    {
        // Sync the target entities set
        static std::set<Entity> sceneHierarchyPrevSet{};
        static std::set<Entity> editControllerPrevSet{};

        // Update the panels
        m_sceneHierarchyPanel->onImGuiRender();
        m_componentEditPanel->onImGuiRender();

        // If SceneHierarchyPanel changed
        if (sceneHierarchyPrevSet != m_sceneHierarchyPanel->getTargets())
        {
            m_editController->resetTarget();
            m_editController->addTarget(m_sceneHierarchyPanel->getTargets());
        }

        // If EditController changed
        if (editControllerPrevSet != m_editController->getTargets())
        {
            m_sceneHierarchyPanel->resetTarget();
            m_sceneHierarchyPanel->addTarget(m_editController->getTargets());
        }

        sceneHierarchyPrevSet = m_sceneHierarchyPanel->getTargets();
        editControllerPrevSet = m_editController->getTargets();

        auto &entSet = m_editController->getTargets();
        if (entSet.size() == 1)
        {
            m_componentEditPanel->setTarget(*entSet.begin());
        }
        else
            m_componentEditPanel->resetTarget();
    }

    // Scene View
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(m_gameWindowTitle);
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

    ImGui::Begin(m_sceneWindowTitle);
    {
        // Pull states
        m_isSceneWindowFocus = ImGui::IsWindowFocused();

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
	ImGui::PopStyleVar();

	m_currentScene->onImGuiRender();

    m_statusBarPanel->onImGuiRender();

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
                m_currentScene->setSceneState(Scene::SceneState::Play); // TODO: remove me
            ImGui::SetWindowFocus(m_gameWindowTitle);
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
            if(m_currentScene->getSceneState() != Scene::SceneState::Editor)
            {
                m_currentScene->onSceneStop();
                switchCurrentScene(m_editorScene);
                m_runtimeScene.reset();
            }
            ImGui::SetWindowFocus(m_sceneWindowTitle);
        }
        ImGui::SameLine();

        if(ImGui::Button(ICON_FA_BORDER_ALL))
        {
            m_editController->toggleShowGrid();
        }
        ImGui::SameLine();

        // MoveMode button
        if(ImGui::Button(ICON_FA_ARROWS_ALT))
        {
            m_editController->changeGizmoMode(Gizmo::GizmoMode::MoveMode);
        }
        ImGui::SameLine();

        // ZoomMode button
        if(ImGui::Button(ICON_FA_EXPAND))
        {
            m_editController->changeGizmoMode(Gizmo::GizmoMode::ZoomMode);
        }
        ImGui::SameLine();

        // Scale button
        if(ImGui::Button(ICON_FA_EXPAND_ARROWS_ALT))
        {
            m_editController->changeGizmoMode(Gizmo::GizmoMode::ScaleMode);
        }
        ImGui::SameLine();

        // RotationMode button
        if(ImGui::Button(ICON_FA_SYNC))
        {
            m_editController->changeGizmoMode(Gizmo::GizmoMode::RotationMode);
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

	// Simple Panels
    for(auto &panel : m_simplePanelList)
        panel->onImGuiRender();

	ImGui::EndDockspace();

    // Modals
	m_errorModal.onImGuiRender();

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
        m_isMenuBarFocus = ImGui::IsWindowFocused();

        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("New Scene", "Ctrl+N"))
            {
                m_scenePath = "";
                newScene();
            }

            if(ImGui::MenuItem("Open Scene", "Ctrl+O", false))
            {
                newScene();
                // Open File
                std::string path, content;
                if(FileDialog::SelectSingleFile("sce",
                    (FileSystem::GetCurrentDirectory() + "\\assets").c_str(),
                    path))
                {
                    m_scenePath = path;
                    openScene(m_scenePath);
                }
            }

            ImGui::Separator();

            // TODO: hot reload?
            if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, m_sceneLoaded))
            {
                saveScene(m_scenePath);
            }

            if (ImGui::MenuItem("Save Scene as", "Ctrl-Shift+S"))
            {
                std::string path;
                if(FileDialog::SelectSaveFile("sce", nullptr, path))
                {
                    if(!String::endswith(path, ".sce"))
                        path += ".sce";
                    m_scenePath = path;
                    saveScene(m_scenePath);
                }
            }

            ImGui::Separator();

            if(ImGui::MenuItem("Exit", "Ctrl+W"))
            {
                Application::Get().close();
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Edit"))
        {
            ImGui::MenuItem(m_sceneAction.getAction("Copy"));
            ImGui::MenuItem(m_sceneAction.getAction("Paste"));
            ImGui::Separator();
            ImGui::MenuItem(m_sceneAction.getAction("Delete"));
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

            ImGui::Separator();

            if( ImGui::MenuItem("Setting", nullptr) )
            {
                m_settingPanel->showPanel();
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Debug"))
        {
            if(ImGui::BeginMenu("Edit Controller"))
            {
                if(ImGui::BeginMenu("Editor Grid"))
                {
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

    onShortcutActionUpdate();
}

void EditorLayer::onEvent(rl::Event& e)
{
    EventDispatcher dispatcher{e};
    dispatcher.dispatch<WindowCloseEvent>(RL_BIND_EVENT_FUNC(EditorLayer::onWindowCloseEvent));
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

    // Register the scene to systems
    SystemManager::Init(m_currentScene);

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setting

void EditorLayer::loadSetting(const std::string &path)
{
    std::string content;
    content = FileSystem::ReadTextFile(path);
    //
    std::stringstream oos(content);
    cereal::JSONInputArchive inputArchive(oos);

    try
    {
        inputArchive(cereal::make_nvp("settings", m_editorSetting));
    }
    catch (cereal::RapidJSONException &e)
    {
        RL_CORE_ERROR("Failed to load scene {}", e.what());
        m_errorModal.setMessage(e.what());
    }
    catch (cereal::Exception &e)
    {
        RL_CORE_ERROR("Failed to load scene {}", e.what());
        m_errorModal.setMessage(e.what());
    }
}

void EditorLayer::saveSetting()
{
    std::ofstream fp("setting.conf");
    cereal::JSONOutputArchive outputArchive(fp);
    outputArchive(cereal::make_nvp("settings", m_editorSetting));
}

void EditorLayer::newScene()
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

void EditorLayer::openScene(const std::string &path)
{
    // Get content
    std::string content = FileSystem::ReadTextFile(path);

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
    if (!failLoad)
    {
        m_sceneLoaded = true;

        // Because the panels are now holding strong ref to the scene
        // We need to reset the context
        switchCurrentScene(m_editorScene);

        m_editorScene->onEditorInit();
    }
    else
    {
        m_sceneLoaded = false;
        m_statusBarPanel->sendMessage(fmt::format("Failed to load scene {}.\n{}", m_scenePath, exceptionMsg));
        m_errorModal.setMessage(fmt::format("Failed to load scene {}.\n{}", m_scenePath, exceptionMsg));
    }
}

void EditorLayer::saveScene(const std::string &path)
{
    // TODO: Maybe implement a function return ofstream from rl::FileSystem
    std::ofstream os(path);
    cereal::JSONOutputArchive outputArchive(os);
    outputArchive(cereal::make_nvp("Scene", m_editorScene));
}

// TODO: Refactor Timer into timer thread
// TODO: Use condition_variable
void EditorLayer::autoSave()
{
    Clock clock;

    while(m_autoSaveRun)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if((clock.getElapsedTime() > m_editorSetting.autoSaveSecond) && (!m_scenePath.empty()))
        {
            saveScene(m_scenePath);
            m_statusBarPanel->sendMessage(fmt::format("Auto Save: {}", m_scenePath));
            clock.restart();
        }
    }
}

void EditorLayer::initShortCut()
{
    m_sceneAction.createAction("Select All", ImCtrl | ImActionKey_A);
    m_sceneAction.createAction("Copy", ImCtrl | ImActionKey_C);
    //
    m_sceneAction.createAction("Paste", ImCtrl | ImActionKey_V);
    m_sceneAction.getAction("Paste")->setEnabled(false);
    //
    m_sceneAction.createAction("Delete", ImActionKey_Delete);
    m_sceneAction.createAction("Cancel", ImActionKey_Escape);
    //
    m_sceneAction.createAction("Group", ImCtrl | ImActionKey_G);
}

void EditorLayer::onShortcutActionUpdate()
{

    m_sceneAction.getAction("Copy")->setEnabled(m_editController->isSelected());
    m_sceneAction.getAction("Delete")->setEnabled(m_editController->isSelected());
    m_sceneAction.getAction("Cancel")->setEnabled(m_editController->isSelected());

    if(m_isSceneWindowFocus || m_isMenuBarFocus || m_sceneHierarchyPanel->isWindowFocus())
    {
        if(m_sceneAction.getAction("Select All")->IsShortcutPressed())
        {
            m_currentScene->m_registry.each([&](auto entityID) {
                Entity entity(entityID, m_currentScene.get());
                m_editController->addTarget(entity);
            });
            m_statusBarPanel->sendMessage("Select All Entity");
        }

        if (m_sceneAction.getAction("Copy")->IsShortcutPressed())
        {
            m_copyList.clear();
            for(auto &ent : m_sceneHierarchyPanel->getSelectedEntities())
            {
                m_copyList.push_back(ent);
            }

            if(!m_copyList.empty())
            {
                m_sceneAction.getAction("Paste")->setEnabled(true);
            }
            m_statusBarPanel->sendMessage("Copy Selected Entity");
        }

        if(m_sceneAction.getAction("Paste")->IsShortcutPressed())
        {
            m_sceneHierarchyPanel->resetTarget();
            for(auto &ent : m_copyList)
            {
                m_sceneHierarchyPanel->addTarget(ent);
                m_sceneHierarchyPanel->duplicateTargetEntities();
            }
            m_copyList.clear();
            m_sceneAction.getAction("Paste")->setEnabled(false);
            m_statusBarPanel->sendMessage("Paste copy Entity");
        }

        if(m_sceneAction.getAction("Delete")->IsShortcutPressed())
        {
            m_sceneHierarchyPanel->deleteTargetEntities();
            m_sceneHierarchyPanel->resetTarget();
            m_statusBarPanel->sendMessage("Delete Selected Entity");
        }

        if(m_sceneAction.getAction("Group")->IsShortcutPressed())
        {
            m_sceneHierarchyPanel->groupTargetEntities();
        }

        if(m_sceneAction.getAction("Cancel")->IsShortcutPressed())
        {
            m_sceneHierarchyPanel->resetTarget();
            m_statusBarPanel->sendMessage("Cancel Select Entity");
        }
    }

}

void EditorLayer::changeShortCut(const char *name, int shortcut, bool *selected)
{
    m_sceneAction.removeAction(name);
    m_sceneAction.createAction(name, shortcut, selected);
}

}
