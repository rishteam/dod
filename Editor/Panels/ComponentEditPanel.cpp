#include "ComponentEditPanel.h"

#include <Rish/Utils/FileDialog.h>

#include <imgui.h>

namespace rl {

void ComponentEditPanel::onImGuiRender()
{
    if(!m_targetEntity)
        return;

    ImGui::BeginChild("EntityComponentEdit");
    if (m_targetEntity.hasComponent<TagComponent>())
    {
        if (ImGui::CollapsingHeader("TagComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
            std::string tmp = m_targetEntity.getComponent<TagComponent>().tag;
            static char tag[32];
            strcpy(tag, tmp.c_str());
            ImGui::InputText("Tag", tag, IM_ARRAYSIZE(tag));
            m_targetEntity.getComponent<TagComponent>().tag = tag;
        }
    }

    if (m_targetEntity.hasComponent<TransformComponent>())
    {
        if (ImGui::CollapsingHeader("TransformComponent", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto &transform = m_targetEntity.getComponent<TransformComponent>();
            ImGui::SliderFloat("TranslateX", &transform.translate.x, -1.f, 1.f, "%.2f");
            ImGui::SliderFloat("TranslateY", &transform.translate.y, -1.f, 1.f, "%.2f");
            // ImGui::SliderFloat("Z", &transform.z, -10, 10, "%.2f");`x
            ImGui::Separator();
            ImGui::DragFloat("ScaleX", &transform.scale.x, 0.1f, 0.f);
            ImGui::DragFloat("ScaleY", &transform.scale.y, 0.1f, 0.f);
        }
    }

    if (m_targetEntity.hasComponent<RenderComponent>())
    {
        if (ImGui::CollapsingHeader("RenderComponent", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto &renderCp = m_targetEntity.getComponent<RenderComponent>();
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
    ImGui::EndChild();
}

} // end of namespace rl