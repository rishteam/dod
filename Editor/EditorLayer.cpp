#include <Rish/rlpch.h>

#include <Rish/Core/FileSystem.h>
#include <Rish/Renderer/Renderer.h>

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

    m_sceneHierarchyPanel.setContext(m_scene);
    m_componentEditPanel.setContext(m_scene);
}

void EditorLayer::onDetach()
{
    RL_CORE_INFO("[EditorLayer] onDetach");
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
	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
			    // open file
				std::string filename = rl::FileSystem::ReadTextFile("Scene.json");
				RL_TRACE("Scene file = {}", filename);

				// deserialize
				std::stringstream oos(filename);
				cereal::JSONInputArchive inputArchive(oos);
				inputArchive(cereal::make_nvp("Scene", m_scene));

                m_entityList = m_scene->getAllEntities();
				std::reverse(m_entityList.begin(), m_entityList.end());
			}

            if (ImGui::MenuItem("Save"))
            {
                // TODO: Maybe implement a function return ofstream from rl::FileSystem
                std::ofstream os("Scene.json");
                cereal::JSONOutputArchive outputArchive(os);

                outputArchive(cereal::make_nvp("Scene", m_scene));
            }

            ImGui::Separator();

            if(ImGui::MenuItem("Exit"))
            {
                Application::Get().close();
            }

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

    static int selectedEntity = -1;

	ImGui::Begin("Entity");
    {
        m_sceneHierarchyPanel.onImGuiRender();

        ImGui::Separator();

        if (m_sceneHierarchyPanel.isSelected())
        {
            m_componentEditPanel.setTarget(m_sceneHierarchyPanel.getSelectedEntity());
        }
        m_componentEditPanel.onImGuiRender();
    }
	ImGui::End();

	ImGui::Begin("Entity Manager");
    {

        if (m_sceneHierarchyPanel.isSelected())
        {
            if (ImGui::Button("Delete Entity"))
            {
                m_sceneHierarchyPanel.getSelectedEntity().destroy();
                m_sceneHierarchyPanel.resetSelected();
            }
        }

        ImGui::Separator();

        const char *components[] = {"TagComponent", "TransformComponent", "RenderComponent"};
        static int selectedComponent = -1;
        if (selectedEntity != -1) {
            if (ImGui::Button("Add Component")) {
                ImGui::OpenPopup("Components");
            }

            if (ImGui::BeginPopup("Components")) {
                for (int i = 0; i < IM_ARRAYSIZE(components); i++) {
                    if (ImGui::Selectable(components[i])) {
                        selectedComponent = i;
                        switch (i) {
                            case 0:
                                if (!m_entityList[selectedEntity].hasComponent<TagComponent>()) {
                                    m_entityList[selectedEntity].addComponent<TagComponent>();
                                }
                                break;
                            case 1:
                                if (!m_entityList[selectedEntity].hasComponent<TransformComponent>()) {
                                    m_entityList[selectedEntity].addComponent<TransformComponent>();
                                }
                                break;

                            case 2:
                                if (!m_entityList[selectedEntity].hasComponent<RenderComponent>()) {
                                    m_entityList[selectedEntity].addComponent<RenderComponent>();
                                }
                                break;
                        }
                    }
                }
                ImGui::EndPopup();
            }

            static int selectedDeleteComponent = -1;
            if (ImGui::Button("Delete Component")) {
                ImGui::OpenPopup("Delete_Component");
            }

            if (ImGui::BeginPopup("Delete_Component")) {
                for (int i = 0; i < IM_ARRAYSIZE(components); i++) {
                    if (ImGui::Selectable(components[i])) {
                        switch (i) {
                            case 0:
                                ImGui::Text("Cannot Delete TagComponent!!!!");
                                break;

                            case 1:
                                if (m_entityList[selectedEntity].hasComponent<TransformComponent>())
                                    m_entityList[selectedEntity].removeComponent<TransformComponent>();
                                break;

                            case 2:
                                if (m_entityList[selectedEntity].hasComponent<RenderComponent>())
                                    m_entityList[selectedEntity].removeComponent<RenderComponent>();
                                break;
                        }
                    }
                }
                ImGui::EndPopup();
            }

        }
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

    defaultLogWindow.onImGuiRender();

	EndDockspace();
}

void EditorLayer::onEvent(rl::Event& event)
{
    if(m_sceneWindowFocused)
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
