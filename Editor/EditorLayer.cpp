#include <Rish/rlpch.h>

#include <Rish/Core/FileSystem.h>
#include <Rish/Renderer/Renderer.h>

#include <imgui.h>

#include "EditorLayer.h"

namespace rl {

EditorLayer::EditorLayer()
    : Layer("editorLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio(), true)
{
	VFS::Mount("shader", "assets/shader");
	VFS::Mount("texture", "assets/texture");

	RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());

	m_shader = rl::Shader::LoadShaderVFS("/shader/vertexShader/vertexSrc.glsl", "/shader/fragmentShader/fragSrc.glsl");
    testShader = rl::Shader::LoadShaderVFS("/shader/vertexShader/blueVertSrc.glsl", "/shader/fragmentShader/blueFragSrc.glsl");

	m_texture = rl::Texture2D::LoadTextureVFS("/texture/1.png");

	testShader->bind();
	testShader->setInt("u_Texture", 0);

    m_scene = std::make_shared<rl::Scene>();
}

void EditorLayer::onAttach()
{
	rl::FramebufferSpecification fbspec;
	fbspec.width = 1280;
	fbspec.height = 720;
	m_framebuffer = Framebuffer::Create(fbspec);
}

void EditorLayer::onDetach()
{
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

    Renderer2D::BeginScene(m_cameraController.getCamera());
    Renderer2D::DrawQuad({0.f, 0.f}, {1.f, 1.f}, m_texture);

    Renderer2D::DrawRotatedQuad({-1.f, 0.f}, {0.8f, 0.8f}, m_texture, rotate);

    int gridWidth = 10, gridHeight = 10;
    for(int i = 0; i < gridWidth; i++)
        for(int j = 0; j < gridHeight; j++)
        {
            if((i+j) % 2)
                rl::Renderer2D::DrawRotatedQuad({i, j, 0.0f}, {0.8f, 0.8f}, {i / 10.f, j / 10.f, 1.f, 1.f}, rotate);
            else
                rl::Renderer2D::DrawQuad({i, j, 0.0f}, {0.8f, 0.8f}, {i / 10.f, j / 10.f, 1.f, 1.f});
        }
    Renderer2D::EndScene();

    m_scene->update(dt);

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

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

    static int selected = -1;

	ImGui::Begin("Entity");
    {
        // RL_CORE_TRACE("Number of Entities: {}", m_entityList.size());
        ImGui::Text("Entity List");
        ImGui::ListBoxHeader("##EntityList", ImVec2(-1, 500));

        for (int i = 0; i < m_entityList.size(); i++) {
            // RL_TRACE("{}", m_entityList[i].getComponent<TagComponent>().tag.c_str());
            if (ImGui::Selectable(m_entityList[i].getComponent<TagComponent>().tag.c_str()))
                selected = i;
        }

        ImGui::ListBoxFooter();

        ImGui::Separator();

        if (selected != -1) {
            if (m_entityList[selected].hasComponent<TagComponent>()) {
                if (ImGui::CollapsingHeader("TagComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
                    std::string tmp = m_entityList[selected].getComponent<TagComponent>().tag.c_str();
                    static char tag[32];
                    strcpy(tag, tmp.c_str());
                    ImGui::InputText("Tag", tag, IM_ARRAYSIZE(tag));
                    m_entityList[selected].getComponent<TagComponent>().tag = tag;
                }
            }

            if (m_entityList[selected].hasComponent<TransformComponent>()) {
                if (ImGui::CollapsingHeader("TransformComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto &transform = m_entityList[selected].getComponent<TransformComponent>();
                    ImGui::SliderFloat("TranslateX", &transform.translate.x, -1.f, 1.f, "%.2f");
                    ImGui::SliderFloat("TranslateY", &transform.translate.y, -1.f, 1.f, "%.2f");
                    // ImGui::SliderFloat("Z", &transform.z, -10, 10, "%.2f");`x
                    ImGui::Separator();
                    ImGui::DragFloat("ScaleX", &transform.scale.x, 0.1f, 0.f);
                    ImGui::DragFloat("ScaleY", &transform.scale.y, 0.1f, 0.f);
                }
            }


            if (m_entityList[selected].hasComponent<RenderComponent>()) {
                if (ImGui::CollapsingHeader("ShaderComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto &color = m_entityList[selected].getComponent<RenderComponent>().color;
                    ImGui::ColorEdit4("Color", glm::value_ptr(color));
                    auto &render = m_entityList[selected].getComponent<RenderComponent>();

                    ImGui::Text("Texture");
                    std::string tPath;
                    if (ImGui::Button("Select")) {
                        if (FileDialog::SelectSingleFile(nullptr, nullptr, tPath)) {
                            render.path = tPath;
                            render.reloadTexture = true;
                        }
                    }

                    static char texPath[150];
                    strcpy(texPath, render.path.c_str());
                    ImGui::SameLine();
                    ImGui::InputText("", texPath, IM_ARRAYSIZE(texPath));

                    ImGui::Text("Shader");
                    std::string path;
                    if (ImGui::Button("Select##Vert")) {
                        if (FileDialog::SelectSingleFile(nullptr, nullptr, path)) {
                            render.vertPath = path;
                            render.reload = true;
                        }
                    }
                    static char vertPath[150];
                    strcpy(vertPath, render.vertPath.c_str());
                    ImGui::SameLine();
                    ImGui::InputText("", vertPath, IM_ARRAYSIZE(vertPath));

                    if (ImGui::Button("Select##Frag")) {
                        if (FileDialog::SelectSingleFile(nullptr, nullptr, path)) {
                            render.fragPath = path;
                            render.reload = true;
                        }
                    }
                    static char fragPath[150];
                    strcpy(fragPath, render.fragPath.c_str());
                    ImGui::SameLine();
                    ImGui::InputText("", fragPath, IM_ARRAYSIZE(fragPath));
                }
            }
        }
    }
	ImGui::End();

	ImGui::Begin("Entity Manager");
    {
        if (ImGui::Button("Create Entity")) {
            // RL_CORE_TRACE("{}", tag);
            auto square = m_scene->createEntity();
            // square.addComponent<TestQuadComponent>(std::make_shared<rl::VertexArray>(), "/shader/vertexShader/vertexSrc.glsl", "/shader/fragmentShader/fragSrc.glsl");
            // square.addComponent<SpriteRendererComponent>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            m_entityList.push_back(square);
            RL_CORE_TRACE("Number of Entities: {}", m_entityList.size());
        }

        if (selected != -1) {
            if (ImGui::Button("Delete Entity")) {
                m_entityList[selected].destroy();
                auto it = std::find(m_entityList.begin(), m_entityList.end(), m_entityList[selected]);
                m_entityList.erase(it);
                selected = -1;
            }
        }

        ImGui::Separator();

        const char *components[] = {"TagComponent", "TransformComponent", "RenderComponent"};
        static int selectedComponent = -1;
        if (selected != -1) {
            if (ImGui::Button("Add Component")) {
                ImGui::OpenPopup("Components");
            }

            if (ImGui::BeginPopup("Components")) {
                for (int i = 0; i < IM_ARRAYSIZE(components); i++) {
                    if (ImGui::Selectable(components[i])) {
                        selectedComponent = i;
                        switch (i) {
                            case 0:
                                if (!m_entityList[selected].hasComponent<TagComponent>()) {
                                    m_entityList[selected].addComponent<TagComponent>();
                                }
                                break;
                            case 1:
                                if (!m_entityList[selected].hasComponent<TransformComponent>()) {
                                    m_entityList[selected].addComponent<TransformComponent>();
                                }
                                break;

                            case 2:
                                if (!m_entityList[selected].hasComponent<RenderComponent>()) {
                                    m_entityList[selected].addComponent<RenderComponent>();
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
                                if (m_entityList[selected].hasComponent<TransformComponent>())
                                    m_entityList[selected].removeComponent<TransformComponent>();
                                break;

                            case 2:
                                if (m_entityList[selected].hasComponent<RenderComponent>())
                                    m_entityList[selected].removeComponent<RenderComponent>();
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
