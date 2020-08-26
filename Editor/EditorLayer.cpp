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

    m_scene = std::make_shared<rl::Scene>();
}

void EditorLayer::onAttach()
{
    RL_CORE_INFO("[EditorLayer] onAttach");
	rl::FramebufferSpecification fbspec;
	fbspec.width = 1280;
	fbspec.height = 720;
	m_framebuffer = Framebuffer::Create(fbspec);
}

void EditorLayer::onDetach()
{
    RL_CORE_INFO("[EditorLayer] onDetach");
}

void EditorLayer::onUpdate(Time dt)
{
    static float rotate = 0.f;
    rotate += 10 * dt.asSeconds();

    m_cameraController.onUpdate(dt);

	m_framebuffer->bind();

	Renderer2D::ResetStats();
	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
	RenderCommand::Clear();

    m_scene->update(m_cameraController.getCamera(), dt);

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
        // RL_CORE_TRACE("Number of Entities: {}", m_entityList.size());
        ImGui::Text("Entity List");
        {
            static std::vector<bool> s_selectableStates;
            s_selectableStates.resize(m_entityList.size());
            // Multiple select
            ImGui::ListBoxHeader("##EntityList", ImVec2(-1, 500));
            for (int i = 0; i < m_entityList.size(); i++)
            {
                if (ImGui::Selectable(m_entityList[i].getComponent<TagComponent>().tag.c_str(), s_selectableStates[i]))
                {
                    // Not press ctrl
                    if (!ImGui::GetIO().KeyCtrl)
                    {
                        std::fill(s_selectableStates.begin(), s_selectableStates.end(), false);
                    }
                    s_selectableStates[i] = !s_selectableStates[i];
                    selectedEntity = i;
                }
            }
            ImGui::ListBoxFooter();
            // Count the selected entities
            int cnt = 0;
            for(int i = 0; i < s_selectableStates.size(); i++)
            {
                if(s_selectableStates[i])
                {
                    cnt++;
                    selectedEntity = i;
                }
            }
            // If the selectedCnt > 2 then not showing the settings
            if(cnt >= 2)
                selectedEntity = -1;
        }

        ImGui::Separator();

        // Settings
        if (selectedEntity != -1)
        {
            if (m_entityList[selectedEntity].hasComponent<TagComponent>()) {
                if (ImGui::CollapsingHeader("TagComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
                    std::string tmp = m_entityList[selectedEntity].getComponent<TagComponent>().tag.c_str();
                    static char tag[32];
                    strcpy(tag, tmp.c_str());
                    ImGui::InputText("Tag", tag, IM_ARRAYSIZE(tag));
                    m_entityList[selectedEntity].getComponent<TagComponent>().tag = tag;
                }
            }

            if (m_entityList[selectedEntity].hasComponent<TransformComponent>()) {
                if (ImGui::CollapsingHeader("TransformComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto &transform = m_entityList[selectedEntity].getComponent<TransformComponent>();
                    ImGui::SliderFloat("TranslateX", &transform.translate.x, -1.f, 1.f, "%.2f");
                    ImGui::SliderFloat("TranslateY", &transform.translate.y, -1.f, 1.f, "%.2f");
                    // ImGui::SliderFloat("Z", &transform.z, -10, 10, "%.2f");`x
                    ImGui::Separator();
                    ImGui::DragFloat("ScaleX", &transform.scale.x, 0.1f, 0.f);
                    ImGui::DragFloat("ScaleY", &transform.scale.y, 0.1f, 0.f);
                }
            }

            if (m_entityList[selectedEntity].hasComponent<RenderComponent>())
            {
                if (ImGui::CollapsingHeader("RenderComponent", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    auto &renderCp = m_entityList[selectedEntity].getComponent<RenderComponent>();
                    ImGui::ColorEdit4("Color", glm::value_ptr(renderCp.color));

                    ImGui::Text("Texture");
                    {
                        std::string tPath;
                        if (ImGui::Button("Select")) {
                            if (FileDialog::SelectSingleFile(nullptr, nullptr, tPath)) {
                                renderCp.texturePath = tPath;
                                renderCp.reloadTexture = true;
                            }
                        }

                        ImGui::SameLine();
                        ImGui::InputText("##texturePath", const_cast<char *>(renderCp.texturePath.c_str()),
                                         renderCp.texturePath.size(), ImGuiInputTextFlags_ReadOnly);

                        ImGui::Image(renderCp.m_texture->getTextureID(), ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, -1));
                    }

                    ImGui::Text("Shader");
                    {
                        std::string path;
                        auto &vertPath = renderCp.vertPath;
                        if (ImGui::Button("Select##Vert")) {
                            if (FileDialog::SelectSingleFile(nullptr, nullptr, path)) {
                                vertPath = path;
                                renderCp.reloadShader = true;
                            }
                        }
                        ImGui::SameLine();
                        ImGui::InputText("##VertexShaderPath", const_cast<char *>(vertPath.c_str()),
                                         vertPath.size(), ImGuiInputTextFlags_ReadOnly);

                        auto &fragPath = renderCp.fragPath;
                        if (ImGui::Button("Select##Frag"))
                        {
                            if (FileDialog::SelectSingleFile(nullptr, nullptr, path))
                            {
                                fragPath = path;
                                renderCp.reloadShader = true;
                            }
                        }
                        ImGui::SameLine();
                        ImGui::InputText("##FragmentShaderPath", const_cast<char *>(fragPath.c_str()),
                                         fragPath.size(), ImGuiInputTextFlags_ReadOnly);
                    }
                }
            }
        }
    }
	ImGui::End();

	ImGui::Begin("Entity Manager");
    {
        if (ImGui::Button("Create Entity"))
        {
            auto ent = m_scene->createEntity();
            m_entityList.push_back(ent);
            RL_CORE_TRACE("Number of Entities: {}", m_entityList.size());
        }

        if (selectedEntity != -1)
        {
            if (ImGui::Button("Delete Entity"))
            {
                m_entityList[selectedEntity].destroy();
                auto it = std::find(m_entityList.begin(), m_entityList.end(), m_entityList[selectedEntity]);
                m_entityList.erase(it);
                selectedEntity = -1;
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

        // TODO Actually should be (void*)textureID, but it went error on it, but this can work??
        ImGui::Image((uintptr_t) textureID, ImVec2{1280, 720}, {0, 0}, {1, -1});
    }
	ImGui::End();

    defaultLogWindow.onImGuiRender();

	EndDockspace();
}

void EditorLayer::onEvent(rl::Event& event)
{
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
