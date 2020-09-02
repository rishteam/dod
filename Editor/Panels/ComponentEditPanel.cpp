#include "ComponentEditPanel.h"

#include <Rish/Utils/FileDialog.h>

#include <imgui.h>
#include <imgui_stdlib.h>

namespace rl {

void ComponentEditPanel::onAttach(const Ref<Scene> &scene)
{
    SceneTargetPanel::onAttach(scene);
    m_componentSelectionPanel.onAttach(scene);
}

template<>
void ComponentEditPanel::drawEditComponentWidget<TagComponent>()
{
    if (!m_targetEntity.hasComponent<TagComponent>()) return;
    //
    if (ImGui::CollapsingHeader("TagComponent", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if(drawEditComponentrightClickMenu<TagComponent>(true))
            return;
        //
        std::string &tag = m_targetEntity.getComponent<TagComponent>().tag;
        ImGui::InputText("Tag", &tag);
    }
}

template<>
void ComponentEditPanel::drawEditComponentWidget<TransformComponent>()
{
    if (!m_targetEntity.hasComponent<TransformComponent>()) return;
    //
    if (ImGui::CollapsingHeader("TransformComponent", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if(drawEditComponentrightClickMenu<TransformComponent>())
            return;
        //
        auto &transform = m_targetEntity.getComponent<TransformComponent>();
        ImGui::SliderFloat("TranslateX", &transform.translate.x, -1.f, 1.f, "%.2f");
        ImGui::SliderFloat("TranslateY", &transform.translate.y, -1.f, 1.f, "%.2f");
        // ImGui::SliderFloat("Z", &transform.z, -10, 10, "%.2f");`x
        ImGui::Separator();
        ImGui::DragFloat("ScaleX", &transform.scale.x, 0.1f, 0.f);
        ImGui::DragFloat("ScaleY", &transform.scale.y, 0.1f, 0.f);
    }
}

template<>
void ComponentEditPanel::drawEditComponentWidget<RenderComponent>()
{
    if (!m_targetEntity.hasComponent<RenderComponent>()) return;

    if (ImGui::CollapsingHeader("RenderComponent", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if(drawEditComponentrightClickMenu<RenderComponent>())
            return;
        //
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

void ComponentEditPanel::onImGuiRender()
{
    if(!m_targetEntity) return;
    m_componentSelectionPanel.setTarget(m_targetEntity);

    ImGui::BeginChild("EntityComponentEdit");
    // TODO: Make this into dispatcher
    drawEditComponentWidget<TagComponent>();
    drawEditComponentWidget<TransformComponent>();
    drawEditComponentWidget<RenderComponent>();

    // Popup
    if(ImGui::Button(ICON_FA_PLUS, ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
    {
        ImGui::OpenPopup("ComponentSelectionPanel");
    }
    if(ImGui::BeginPopup("ComponentSelectionPanel"))
    {
        m_componentSelectionPanel.onImGuiRender();
        ImGui::EndPopup();
    }

    ImGui::EndChild();
}


} // end of namespace rl