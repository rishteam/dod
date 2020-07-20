#include "EditorLayer.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace rl {

EditorLayer::EditorLayer()
: Layer("editorLayer")
{

	RL_TRACE("Current path is {}", fs::current_path());

	m_vertexArray.reset(new rl::VertexArray());

	float vertices[3 * 7] = {
		-0.9f,  0.9f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.1f,  0.9f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		-0.4f,  0.1f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	};

	std::shared_ptr<rl::VertexBuffer> vertexBuffer;
	vertexBuffer.reset(new rl::VertexBuffer(vertices, sizeof(vertices)));

	rl::BufferLayout layout = {

		{rl::ShaderDataType::Float3, "a_Position"},
		{rl::ShaderDataType::Float4, "a_Color"}};

	vertexBuffer->setLayout(layout);
	m_vertexArray->addVertexBuffer(vertexBuffer);

	uint32_t indices[3] = {0, 1, 2};
	std::shared_ptr<rl::IndexBuffer> indexBuffer;
	indexBuffer.reset(new rl::IndexBuffer(indices, sizeof(indices) / sizeof(uint32_t)));
	m_vertexArray->setIndexBuffer(indexBuffer);

	m_vertexArray->unbind(); // Always remember to UNBIND if AMD

	testVA.reset(new rl::VertexArray());

	float square[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	}; 

	std::shared_ptr<rl::VertexBuffer> squareVB;
	squareVB.reset(new rl::VertexBuffer(square, sizeof(square)));
	squareVB->setLayout({
		{rl::ShaderDataType::Float3, "a_Position"},
		{rl::ShaderDataType::Float2, "a_TexCoord"}
	});
	testVA->addVertexBuffer(squareVB);

	uint32_t squareIndices[6] = {0, 1, 2, 2, 0, 3};
	std::shared_ptr<rl::IndexBuffer> squareIB;
	squareIB.reset(new rl::IndexBuffer(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	testVA->setIndexBuffer(squareIB);

	testVA->unbind();

	m_shader.reset(new rl::Shader("Editor\\asset\\shader\\vertexShader\\vertexSrc.glsl", "Editor\\asset\\shader\\fragmentShader\\fragSrc.glsl"));

	testShader.reset(new rl::Shader("Editor\\asset\\shader\\vertexShader\\blueVertSrc.glsl", "Editor\\asset\\shader\\fragmentShader\\blueFragSrc.glsl"));

	m_texture.reset(new rl::Texture2D("Editor\\asset\\texture\\1.png"));

	testShader->bind();
	testShader->uploadUniformInt("u_Texture", 0);
}

void EditorLayer::onAttach()
{
	rl::FramebufferSpecification fbspec;
	fbspec.width = 1920;
	fbspec.height = 1000;
	m_framebuffer.reset(new rl::Framebuffer(fbspec));
}

void EditorLayer::onDetach()
{
}

void EditorLayer::onUpdate(Time dt)
{
	RL_TRACE("Test Editor Layer");

	m_framebuffer->bind();

	m_shader->bind();
	m_vertexArray->bind();
	glDrawElements(GL_TRIANGLES, m_vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
	m_vertexArray->unbind();

	m_texture->bind();
	testShader->bind();
	testVA->bind();
	glDrawElements(GL_TRIANGLES, testVA->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
	testVA->unbind();
	m_texture->unbind();


	m_framebuffer->unbind();
}

void EditorLayer::onImGuiRender()
{
	ImGui::Begin("aa");
	ImGui::Text("This is Editor");
	ImGui::End();

	ImGui::Begin("bb");
	ImGui::Text("456");
	ImGui::End();

	ImGui::Begin("cc");
	ImGui::Text("789");
	ImGui::End();

	ImGui::Begin("TestFramebuffer");
	uint32_t textureID = m_framebuffer->getColorAttachmentRendererID();

	// TODO Actually should be (void*)textureID, but it went error on it, but this can work??
	ImGui::Image((uintptr_t)textureID, ImVec2{1280, 720});
	ImGui::End();
}

void EditorLayer::onEvent(rl::Event& event)
{

}

}



