#include "ComponentEditPanel.h"

#include <Rish/Utils/FileDialog.h>

#include <IconsFontAwesome5.h>
#include <imgui.h>
#include <imgui_stdlib.h>

namespace rl {

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
        ImGui::DragFloat3("Translate", glm::value_ptr(transform.translate), 0.01f);
        ImGui::Separator();
        ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.01f);
    }
}

template<>
void ComponentEditPanel::drawEditComponentWidget<RenderComponent>()
{
    if (!m_targetEntity.hasComponent<RenderComponent>()) return;
    //
    if (ImGui::CollapsingHeader("RenderComponent", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if(drawEditComponentrightClickMenu<RenderComponent>())
            return;
        //
        auto &render = m_targetEntity.getComponent<RenderComponent>();
        ImGui::ColorEdit4("Color", glm::value_ptr(render.color));

        ImGui::Text("Texture");
        {
            std::string tPath;
            if (ImGui::Button("Select")) {
                if (FileDialog::SelectSingleFile(nullptr, nullptr, tPath)) {
                    render.texturePath = tPath;
                    render.reloadTexture = true;
                }
            }

            ImGui::SameLine();
            ImGui::InputText("##texturePath", const_cast<char *>(render.texturePath.c_str()),
                             render.texturePath.size(), ImGuiInputTextFlags_ReadOnly);

            ImGui::Image(render.m_texture->getTextureID(), ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, -1));
        }

        ImGui::Text("Shader");
        {
            std::string path;
            auto &vertPath = render.vertPath;
            if (ImGui::Button("Select##Vert")) {
                if (FileDialog::SelectSingleFile(nullptr, nullptr, path)) {
                    vertPath = path;
                    render.reloadShader = true;
                }
            }
            ImGui::SameLine();
            ImGui::InputText("##VertexShaderPath", const_cast<char *>(vertPath.c_str()),
                             vertPath.size(), ImGuiInputTextFlags_ReadOnly);

            auto &fragPath = render.fragPath;
            if (ImGui::Button("Select##Frag"))
            {
                if (FileDialog::SelectSingleFile(nullptr, nullptr, path))
                {
                    fragPath = path;
                    render.reloadShader = true;
                }
            }
            ImGui::SameLine();
            ImGui::InputText("##FragmentShaderPath", &fragPath, ImGuiInputTextFlags_ReadOnly);
        }
    }
}

void ComponentEditPanel::onAttach(const Ref<Scene> &scene)
{
    SceneTargetPanel::onAttach(scene);
    m_componentSelectionPanel.onAttach(scene);
}

void ComponentEditPanel::onImGuiRender()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin(ICON_FA_INFO_CIRCLE " Inspector");
    ImGui::PopStyleVar();
    if(!m_targetEntity)
    {
        ImGui::End();
        return;
    }
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
    ImGui::End();
}

} // end of namespace rl