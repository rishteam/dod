#include "EditorLayer.h"

namespace rl {

EditorLayer::EditorLayer()
: Layer("editorLayer")
{
}

void EditorLayer::onAttach()
{

}

void EditorLayer::onDetach()
{

}

void EditorLayer::onUpdate(Time dt)
{
	RL_TRACE("Test Editor Layer");
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
}

void EditorLayer::onEvent(rl::Event& event)
{

}

}



