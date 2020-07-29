#include <Rish/rlpch.h>
#include <Rish/Core/FileSystem.h>

#include "EditorLayer.h"

namespace rl {

EditorLayer::EditorLayer()
    : Layer("editorLayer")
{
	VFS::Get()->Mount("shader", "Editor/assets/shader");
	VFS::Get()->Mount("texture", "Editor/assets/texture");

	m_scene = std::make_shared<rl::Scene>();
}

void EditorLayer::onAttach()
{
	rl::FramebufferSpecification fbspec;
	fbspec.width = 1920;
	fbspec.height = 1000;
	m_framebuffer = std::make_shared<rl::Framebuffer>(fbspec);

	// auto square = m_scene->createEntity("Square");
	// square.addComponent<TestQuadComponent>(std::make_shared<rl::VertexArray>(), "/shader/vertexShader/vertexSrc.glsl", "/shader/fragmentShader/fragSrc.glsl");
	// square.addComponent<SpriteRendererComponent>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// m_squarEntity = square;
}

void EditorLayer::onDetach()
{
}

void EditorLayer::onUpdate(Time dt)
{
	// RL_TRACE("Test Editor Layer");
	m_framebuffer->bind();

	m_scene->update(0);

	m_framebuffer->unbind();
}

void EditorLayer::onImGuiRender()
{
	ImGui::Begin("Entity");
	// RL_CORE_TRACE("Number of Entities: {}", m_squareEntity.size());
	ImGui::ListBoxHeader("Entity", ImVec2(250, 600));

	static int selected = -1;
	for(int i = 0 ; i < m_squareEntity.size() ; i++)
	{
		// RL_TRACE("{}", m_squareEntity[i].getComponent<TagComponent>().tag.c_str());
		if(ImGui::Selectable(m_squareEntity[i].getComponent<TagComponent>().tag.c_str()))
			selected = i;
	}

	ImGui::ListBoxFooter();
	

	ImGui::Separator();

	if(selected != -1)
	{
		if(m_squareEntity[selected].hasComponent<TagComponent>())
		{
			if(ImGui::CollapsingHeader("TagComponent"))
			{
				std::string tmp = m_squareEntity[selected].getComponent<TagComponent>().tag.c_str();;
				static char tag[32];
				strcpy(tag, tmp.c_str());
				ImGui::InputText("Tag", tag, IM_ARRAYSIZE(tag));
				m_squareEntity[selected].getComponent<TagComponent>().tag = tag;
			}
		}

		if(m_squareEntity[selected].hasComponent<TransformComponent>())
		{
			if (ImGui::CollapsingHeader("TransformComponent"))
			{
				auto &transform = m_squareEntity[selected].getComponent<TransformComponent>();
				ImGui::SliderFloat("TranslateX", &transform.translate.x, -1.f, 1.f, "%.2f");
				ImGui::SliderFloat("TranslateY", &transform.translate.y, -1.f, 1.f, "%.2f");
				// ImGui::SliderFloat("Z", &transform.z, -10, 10, "%.2f");`x
				ImGui::Separator();
				ImGui::DragFloat("ScaleX", &transform.scale.x, 0.1f, 0.f);
				ImGui::DragFloat("ScaleY", &transform.scale.y, 0.1f, 0.f);
			}
		}

		if (m_squareEntity[selected].hasComponent<SpriteRendererComponent>())
		{
			if (ImGui::CollapsingHeader("SpriteRendererComponent"))
			{
				auto &color = m_squareEntity[selected].getComponent<SpriteRendererComponent>().color;
				ImGui::ColorEdit4("Color", glm::value_ptr(color));
			}
		}

		if (m_squareEntity[selected].hasComponent<TestQuadComponent>())
		{
			if (ImGui::CollapsingHeader("TestQuadComponent"))
			{
				auto& test = m_squareEntity[selected].getComponent<TestQuadComponent>();
				std::string path;
				if(ImGui::Button("Select"))
				{
					if(FileDialog::SelectSingleFile(nullptr, nullptr, path))
					{
						test.path = path;
						test.reload = true;
					}
				}
				static char texturePath[150];
				strcpy(texturePath, test.path.c_str());
				ImGui::SameLine();
				ImGui::InputText("", texturePath, IM_ARRAYSIZE(texturePath));
			}
		}

		if(m_squareEntity[selected].hasComponent<ShaderComponent>())
		{
			if(ImGui::CollapsingHeader("ShaderComponent"))
			{
				auto& shader = m_squareEntity[selected].getComponent<ShaderComponent>();
				std::string path;
				if (ImGui::Button("Open Vert"))
				{
					if(FileDialog::SelectSingleFile(nullptr, nullptr, path))
					{
						shader.vertPath = path;
						shader.reload = true;
					}
				}
				static char vertPath[150];
				strcpy(vertPath, shader.vertPath.c_str());
				ImGui::SameLine();
				ImGui::InputText("", vertPath, IM_ARRAYSIZE(vertPath));

				if (ImGui::Button("Open Frag"))
				{
					if(FileDialog::SelectSingleFile(nullptr, nullptr, path))
					{
						shader.fragPath = path;
						shader.reload = true;
					}
				}
				static char fragPath[150];
				strcpy(fragPath, shader.fragPath.c_str());
				ImGui::SameLine();
				ImGui::InputText("", fragPath, IM_ARRAYSIZE(fragPath));
			}
		}
	}

	ImGui::End();

	ImGui::Begin("Entity Manager");
	if(ImGui::Button("Create Entity"))
	{
		// RL_CORE_TRACE("{}", tag);
		auto square = m_scene->createEntity();
		// square.addComponent<TestQuadComponent>(std::make_shared<rl::VertexArray>(), "/shader/vertexShader/vertexSrc.glsl", "/shader/fragmentShader/fragSrc.glsl");
		// square.addComponent<SpriteRendererComponent>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_squareEntity.push_back(square);
		RL_CORE_TRACE("Number of Entities: {}", m_squareEntity.size());
	}

	if(selected != -1)
	{
		if(ImGui::Button("Delete Entity"))
		{
			m_squareEntity[selected].destroy();
			auto it = std::find(m_squareEntity.begin(), m_squareEntity.end(), m_squareEntity[selected]);
//			RL_TRACE("Delete {}", it->getComponent<TagComponent>().tag.c_str());
			m_squareEntity.erase(it);
			selected = -1;
		}
	}

	ImGui::Separator();

	const char *components[] = {"TagComponent", "TransformComponent", "SpriteRendererComponent", "TestQuadComponent", "ShaderComponent"};
	static int selectedComponent = -1;
	if(selected != -1)
	{
		if(ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("Components");
		}

		if(ImGui::BeginPopup("Components"))
		{
			for(int i = 0 ; i < IM_ARRAYSIZE(components) ; i++)
			{
				if(ImGui::Selectable(components[i]))
				{
					selectedComponent = i;
					switch (i)
					{
						case 0:
							if (!m_squareEntity[selected].hasComponent<TagComponent>())
							{
								m_squareEntity[selected].addComponent<TagComponent>();
							}
						break;

						case 1:
							if (!m_squareEntity[selected].hasComponent<TransformComponent>())
							{
								m_squareEntity[selected].addComponent<TransformComponent>();
							}
						break;
						
						case 2:
							if (!m_squareEntity[selected].hasComponent<SpriteRendererComponent>())
							{
								m_squareEntity[selected].addComponent<SpriteRendererComponent>();
							}
						break;

						case 3:
							if (!m_squareEntity[selected].hasComponent<TestQuadComponent>())
							{
								m_squareEntity[selected].addComponent<TestQuadComponent>(std::make_shared<rl::VertexArray>());
							}
						break;

						case 4:
							if(!m_squareEntity[selected].hasComponent<ShaderComponent>())
							{
								m_squareEntity[selected].addComponent<ShaderComponent>();
							}
						break;
					}
				}
			}
			ImGui::EndPopup();
		}

		static int selectedDeleteComponent = -1;
		if(ImGui::Button("Delete Component"))
		{
			ImGui::OpenPopup("Delete_Component");
		}

		if(ImGui::BeginPopup("Delete_Component"))
		{
			for(int i = 0 ; i < IM_ARRAYSIZE(components) ; i++)
			{
				if(ImGui::Selectable(components[i]))
				{
					switch(i)
					{
						case 0:
							ImGui::Text("Cannot Delete TagComponent!!!!");
						break;

						case 1:
							if(m_squareEntity[selected].hasComponent<TransformComponent>())
								m_squareEntity[selected].removeComponent<TransformComponent>();
						break;

						case 2:
							if (m_squareEntity[selected].hasComponent<SpriteRendererComponent>())
								m_squareEntity[selected].removeComponent<SpriteRendererComponent>();
						break;

						case 3:
							if (m_squareEntity[selected].hasComponent<TestQuadComponent>())
								m_squareEntity[selected].removeComponent<TestQuadComponent>();
						break;

						case 4:
							if(m_squareEntity[selected].hasComponent<ShaderComponent>())
								m_squareEntity[selected].removeComponent<ShaderComponent>();
						break;
					}
				}
			}
			ImGui::EndPopup();
		}

	}


	ImGui::End();

	ImGui::Begin("cc");
	ImGui::Text("789");
	ImGui::End();

	ImGui::Begin("Scene");
	uint32_t textureID = m_framebuffer->getColorAttachmentRendererID();

	// TODO Actually should be (void*)textureID, but it went error on it, but this can work??
	ImGui::Image((uintptr_t)textureID, ImVec2{1280, 720});
	ImGui::End();
}

void EditorLayer::onEvent(rl::Event& event)
{

}

}