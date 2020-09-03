#include <Rish/rlpch.h>

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
	VFS::Mount("shader", "assets/shader");
	VFS::Mount("texture", "assets/texture");

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
}

void EditorLayer::onDetach()
{
    RL_CORE_INFO("[EditorLayer] onDetach");
    m_sceneHierarchyPanel.onDetach();
    m_componentEditPanel.onDetach();
}

void EditorLayer::onUpdate(Time dt)
{
    m_cameraController.setState(m_sceneWindowFocused);
    m_cameraController.onUpdate(dt);

	m_framebuffer->bind();
    {
        Renderer2D::ResetStats();
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
        RenderCommand::Clear();

        m_scene->update(m_cameraController.getCamera(), dt);
    }
	m_framebuffer->unbind();
}

void EditorLayer::onImGuiRender()
{
    BeginDockspace();
    // Menu Bar
    onImGuiMainMenuRender();

    m_sceneHierarchyPanel.onImGuiRender();

    // TODO: should these code exist?
    if (m_sceneHierarchyPanel.isSelected())
        m_componentEditPanel.setTarget(m_sceneHierarchyPanel.getSelectedEntity());
    m_componentEditPanel.onImGuiRender();

	ImGui::Begin("Entity Manager");
    {
    }
	ImGui::End();

	ImGui::Begin("Scene");
    {
        uint32_t textureID = m_framebuffer->getColorAttachmentRendererID();
        ImGui::Image((uintptr_t) textureID, ImVec2{1280, 720}, {0, 0}, {1, -1});
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
                std::stringstream oos(content);
                bool failLoad = false;
                try
                {
                    cereal::JSONInputArchive inputArchive(oos);
                    inputArchive(cereal::make_nvp("Scene", m_scene));
                }
                catch (cereal::RapidJSONException &e)
                {
                    RL_CORE_ERROR("Failed to load scene: {}", e.what());
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

                    m_errorModal.setMessage(fmt::format("Failed to load scene {}.", m_scenePath));
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
                RL_INFO("Renderer2D: quad = {}, draw = {}", stat.QuadCount, stat.DrawCount);
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
