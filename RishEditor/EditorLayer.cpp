#include <Rish/rlpch.h>

#include <Rish/Core/Time.h>
#include <Rish/Core/FileSystem.h>
#include <Rish/Renderer/Renderer.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Utils/FileDialog.h>

#include <IconsFontAwesome5.h>
#include <Rish/ImGui.h>

#include "EditorLayer.h"

namespace rl {

EditorLayer::EditorLayer()
    : Layer("EditorLayer")
{
	VFS::Mount("shader", "assets/editor/shader");
	VFS::Mount("texture", "assets/editor/texture");

    ImGui::LoadIniSettingsFromDisk("RishEditor/imgui.ini");

    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());

    m_scene = MakeRef<Scene>();
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

void EditorLayer::onAttach()
{
    RL_CORE_INFO("[EditorLayer] onAttach");
	FramebufferSpecification fbspec;
	fbspec.width = 1280;
	fbspec.height = 720;
    m_editorFramebuffer = Framebuffer::Create(fbspec);
    m_sceneFramebuffer = Framebuffer::Create(fbspec);

    for(auto &panel : m_panelList)
        panel->onAttach(m_scene);
}

void EditorLayer::onDetach()
{
    RL_CORE_INFO("[EditorLayer] onDetach");

    ImGui::SaveIniSettingsToDisk("RishEditor/imgui.ini");

    for(auto &panel : m_panelList)
        panel->onDetach();
}

void EditorLayer::onUpdate(Time dt)
{
    auto cameraController = m_editController->getCameraController();

    // Resize the framebuffer if user resize the viewport
    auto framebufferSpec = m_editorFramebuffer->getSpecification();
    auto framebufferSize = glm::vec2{framebufferSpec.width, framebufferSpec.height};
    if(m_sceneViewportPanelSize != framebufferSize &&
        m_sceneViewportPanelSize.x > 0.f && m_sceneViewportPanelSize.y > 0.f)
    {
        m_editorFramebuffer->resize((uint32_t)m_sceneViewportPanelSize.x, (uint32_t)m_sceneViewportPanelSize.y);
        cameraController->onResize(m_sceneViewportPanelSize.x, m_sceneViewportPanelSize.y);
    }
    // TODO: Rendering Queue
    Renderer2D::ResetStats();
    Renderer2D::BeginScene(cameraController->getCamera(), m_editorFramebuffer);
    {
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
        RenderCommand::Clear();
        //
        m_editController->onUpdate(dt);
        //
        auto transGroup = m_scene->m_registry.group<TransformComponent, RenderComponent>();
        for(auto entity: transGroup)
        {
            auto &transform = transGroup.get<TransformComponent>(entity);
            auto &render = transGroup.get<RenderComponent>(entity);

            // TODO: make these into entt function
            if(render.init)
            {
                render.m_texture = Texture2D::LoadTextureVFS(render.texturePath);
                render.m_shader = Shader::LoadShaderVFS(render.vertPath, render.fragPath);
                render.init = false;
            }

            if(render.m_texture)
                Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.m_texture, render.color);
            else
                Renderer2D::DrawQuad(transform.translate, glm::vec2(transform.scale), render.color);
        }
        //
    }
    Renderer2D::EndScene();

    m_scene->onUpdate(m_sceneFramebuffer, dt);
}

void EditorLayer::onImGuiRender()
{
    ImGui::BeginDockspace("EditorDockspace");
    // Menu Bar
    onImGuiMainMenuRender();

    if(m_editController->isSelected())
    {
        auto ent = m_editController->getTarget();
        m_sceneHierarchyPanel->resetTarget();
        m_sceneHierarchyPanel->addTarget(ent);
    }

    m_sceneHierarchyPanel->onImGuiRender();

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

	ImGui::Begin("Entity Manager");
    {
    }
	ImGui::End();

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
        auto size = ImGui::GetContentRegionAvail();
        uint32_t textureID = m_sceneFramebuffer->getColorAttachmentRendererID();
        ImGui::Image(textureID, size, {0, 0}, {1, -1});
    }
    ImGui::End();

	ImGui::PopStyleVar();

    m_scene->onImGuiRender();

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
                    for(auto & panel : m_panelList)
                        panel->setContext(m_scene);
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
            ImGui::MenuItem("Editor Grid", nullptr, &m_editController->m_debugEditorGrid);
            ImGui::MenuItem("Editor Camera", nullptr, &m_editController->m_debugCameraController);
            ImGui::MenuItem("Editor Controller", nullptr, &m_editController->m_debugEditorController);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

}

void EditorLayer::onEvent(rl::Event& e)
{
    m_editController->onEvent(e);
}

}
