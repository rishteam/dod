#include <Rish/rlpch.h>
#include <Rish/Core/FileSystem.h>
#include <Rish/Renderer/Renderer.h>

#include <imgui.h>

#include "EditorLayer.h"

namespace rl {

EditorLayer::EditorLayer()
    : Layer("editorLayer"),
      m_camera(-1.6f, 1.6f, -0.9f, 0.9f)
{
	VFS::Mount("shader", "assets/shader");
	VFS::Mount("texture", "assets/texture");

	RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());

	m_shader = rl::Shader::LoadShaderVFS("/shader/vertexShader/vertexSrc.glsl", "/shader/fragmentShader/fragSrc.glsl");
    testShader = rl::Shader::LoadShaderVFS("/shader/vertexShader/blueVertSrc.glsl", "/shader/fragmentShader/blueFragSrc.glsl");

	m_texture = rl::Texture2D::LoadTextureVFS("/texture/1.png");

	testShader->bind();
	testShader->setInt("u_Texture", 0);
}

void EditorLayer::onAttach()
{
    RL_INFO("[EditorLayer] attach");
	rl::FramebufferSpecification fbspec;
	fbspec.width = 1920;
	fbspec.height = 1000;
	m_framebuffer = std::make_shared<rl::Framebuffer>(fbspec);
}

void EditorLayer::onDetach()
{
    RL_INFO("[EditorLayer] detach");
}

void EditorLayer::onUpdate(Time dt)
{
    static float rotate = 0.f;
    rotate += 10 * dt.asSeconds();

	m_framebuffer->bind();

    Renderer2D::BeginScene(m_camera);
    Renderer2D::DrawQuad({0.f, 0.f}, {1.f, 1.f}, m_texture);

    Renderer2D::DrawRotatedQuad({-1.f, 0.f}, {0.8f, 0.8f}, m_texture, rotate);
    Renderer2D::EndScene();

	m_framebuffer->unbind();
}

void EditorLayer::onImGuiRender()
{
    BeginDockSpace();
    {
        ImGui::Begin("DummyA");
        ImGui::End();

        ImGui::Begin("DummyB");
        ImGui::End();

        ImGui::Begin("DummyC");
        ImGui::End();

        ImGui::Begin("TestFramebuffer");
        {
            uint32_t textureID = m_framebuffer->getColorAttachmentRendererID();
            ImGui::Image((ImTextureID) textureID, ImVec2{1280, 720});
        }
        ImGui::End();

        defaultLogWindow.onImGuiRender();
    }
	EndDockSpace();
}

void EditorLayer::onEvent(rl::Event& event)
{
}

void BeginDockSpace()
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

void EndDockSpace()
{
    ImGui::End();
}

}