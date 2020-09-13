#include <Rish/rlpch.h>

#include <Rish/Core/Time.h>
#include <Rish/Core/FileSystem.h>
#include <Rish/Renderer/Renderer.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Utils/FileDialog.h>

#include <IconsFontAwesome5.h>
#include <imgui.h>

#include "EditorLayer.h"

namespace rl {

EditorLayer::EditorLayer()
    : Layer("editorLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio())
{
	VFS::Mount("shader", "assets/editor/shader");
	VFS::Mount("texture", "assets/editor/texture");

    ImGui::LoadIniSettingsFromDisk("Editor/imgui.ini");

    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());

    m_scene = MakeRef<Scene>();
}

void EditorLayer::onAttach()
{
    RL_CORE_INFO("[EditorLayer] onAttach");
	FramebufferSpecification fbspec;
	fbspec.width = 1280;
	fbspec.height = 720;
	m_framebuffer = Framebuffer::Create(fbspec);

    m_sceneHierarchyPanel.onAttach(m_scene);
    m_componentEditPanel.onAttach(m_scene);
    m_editorGrid.onAttach();
}

void EditorLayer::onDetach()
{
    RL_CORE_INFO("[EditorLayer] onDetach");

    ImGui::SaveIniSettingsToDisk("Editor/imgui.ini");

    m_sceneHierarchyPanel.onDetach();
    m_componentEditPanel.onDetach();
    m_editorGrid.onDetach();
}

void EditorLayer::onUpdate(Time dt)
{
    // Resize the framebuffer if user resize the viewport
    auto framebufferSpec = m_framebuffer->getSpecification();
    auto framebufferSize = glm::vec2{framebufferSpec.width, framebufferSpec.height};
    if(m_sceneViewportPanelSize != framebufferSize &&
        m_sceneViewportPanelSize.x > 0.f && m_sceneViewportPanelSize.y > 0.f)
    {
        m_framebuffer->resize((uint32_t)m_sceneViewportPanelSize.x, (uint32_t)m_sceneViewportPanelSize.y);
        m_cameraController.onResize(m_sceneViewportPanelSize.x, m_sceneViewportPanelSize.y);
    }
    //
    m_cameraController.setState(m_sceneWindowFocused);
    m_cameraController.onUpdate(dt);
    //
    Renderer2D::ResetStats();
    Renderer2D::BeginScene(m_cameraController.getCamera(), m_framebuffer);
    {
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
        RenderCommand::Clear();
        //
        m_editorGrid.onUpdate(m_cameraController);
        //
        m_scene->update(m_cameraController.getCamera(), dt);
        //
    }
    Renderer2D::EndScene();
}

void EditorLayer::onImGuiRender()
{
    BeginDockspace();
    // Menu Bar
    onImGuiMainMenuRender();

    m_editorGrid.onImGuiRender();
    m_cameraController.onImGuiRender();

    m_sceneHierarchyPanel.onImGuiRender();

    // TODO: should these code exist?
    if (m_sceneHierarchyPanel.selectedSize() == 1 &&
        m_sceneHierarchyPanel.isSelected())
    {
        m_componentEditPanel.setTarget(m_sceneHierarchyPanel.getSelectedEntity());
    }
    else
        m_componentEditPanel.resetSelected();
    m_componentEditPanel.onImGuiRender();

	ImGui::Begin("Entity Manager");
    {

    }
	ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Scene");
	ImGui::PopStyleVar();
    {
        // Update viewport resize
        auto size = ImGui::GetContentRegionAvail();
        m_sceneViewportPanelSize = glm::vec2{size.x, size.y};
        // show
        uint32_t textureID = m_framebuffer->getColorAttachmentRendererID();
        ImGui::Image((ImTextureID)textureID, size, {0, 0}, {1, -1});
        // states
        m_sceneWindowFocused = ImGui::IsWindowFocused();
        m_sceneWindowHovered = ImGui::IsWindowHovered();
    }
	ImGui::End();

    ImGui::Begin(ICON_FA_TERMINAL " Console");
    ImGui::End();

    defaultLogWindow.onImGuiRender();

	EndDockspace();

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

                if(!failLoad)
                {
                    m_sceneLoaded = true;

                    // Because the panels are now holding strong ref to the scene
                    // We need to reset the context
                    m_sceneHierarchyPanel.setContext(m_scene);
                    m_componentEditPanel.setContext(m_scene);
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

        ImGui::EndMenuBar();
    }
}

void EditorLayer::onEvent(rl::Event& event)
{
    if(m_sceneWindowFocused && m_sceneWindowHovered)
        m_cameraController.onEvent(event);
}

void EditorLayer::BeginDockspace()
{
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetWorkPos());
        ImGui::SetNextWindowSize(viewport->GetWorkSize());
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
}

void EditorLayer::EndDockspace()
{
    ImGui::End();
}

}
