#include "ComponentEditPanel.h"

#include <Rish/Utils/FileDialog.h>
#include <Rish/Scene/ScriptableEntity.h>

#include <Rish/ImGui.h>

#define BeginDrawEditComponent(c)                                    \
    if (!m_targetEntity.hasComponent<c>()) return;                   \
    if (ImGui::CollapsingHeader(#c, ImGuiTreeNodeFlags_DefaultOpen)) \
    {                                                                \

#define EndDrawEditComponent() }

#define DrawRightClickMenu(c, disable)              \
    if(drawEditComponentRightClickMenu<c>(disable)) \
        return;

namespace rl {

template<>
void ComponentEditPanel::drawEditComponentWidget<TagComponent>()
{
    BeginDrawEditComponent(TagComponent);
    {
        DrawRightClickMenu(TagComponent, true);
        //
        std::string &tag = m_targetEntity.getComponent<TagComponent>().tag;
        std::string id = m_targetEntity.getComponent<TagComponent>().id.to_string();
        // TODO: Wrap?
        ImGui::PushItemWidth(300);
        ImGui::InputText("Tag", &tag);
        ImGui::InputText("Id", &id, ImGuiInputTextFlags_ReadOnly);
        ImGui::PopItemWidth();
    }
    EndDrawEditComponent();
}

template<>
void ComponentEditPanel::drawEditComponentWidget<TransformComponent>()
{
    BeginDrawEditComponent(TransformComponent);
    {
        DrawRightClickMenu(TagComponent, true);
        //
        auto &transform = m_targetEntity.getComponent<TransformComponent>();
        ImGui::DragFloat3("Translate", glm::value_ptr(transform.translate), 0.01f);
        ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.01f);
        ImGui::DragFloat("Rotation", &transform.rotate, 0.1f);
        ImGui::SameLine();
        ImGui::HelpMarker("In degrees");
    }
    EndDrawEditComponent();
}

template<>
void ComponentEditPanel::drawEditComponentWidget<RenderComponent>()
{
    BeginDrawEditComponent(RenderComponent);
    {
        DrawRightClickMenu(RenderComponent, false);
        //
        auto &render = m_targetEntity.getComponent<RenderComponent>();
        ImGui::ColorEdit4("Color", glm::value_ptr(render.color));

        ImGui::Text("Texture");
        {
            std::string tPath;
            if (ImGui::Button("Select")) {
                if (FileDialog::SelectSingleFile(nullptr, nullptr, tPath)) {
                    render.texturePath = tPath;
                    render.m_texture = Texture2D::LoadTextureVFS(render.texturePath);
                }
            }

            ImGui::SameLine();
            ImGui::InputText("##texturePath", &render.texturePath, ImGuiInputTextFlags_ReadOnly);
            if(render.m_texture)
                ImGui::Image(render.m_texture->getTextureID(), ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, -1));
            else
                ImGui::Dummy(ImVec2(64, 64));
        }

        ImGui::Text("Shader");
        {
            std::string path;
            auto &vertPath = render.vertPath;
            if (ImGui::Button("Select##Vert")) {
                if (FileDialog::SelectSingleFile(nullptr, nullptr, path)) {
                    vertPath = path;
                    render.m_shader = Shader::LoadShaderVFS(render.vertPath, render.fragPath);
                }
            }
            ImGui::SameLine();
            ImGui::InputText("##VertexShaderPath", &vertPath, ImGuiInputTextFlags_ReadOnly);

            auto &fragPath = render.fragPath;
            if (ImGui::Button("Select##Frag"))
            {
                if (FileDialog::SelectSingleFile(nullptr, nullptr, path))
                {
                    fragPath = path;
                    render.m_shader = Shader::LoadShaderVFS(render.vertPath, render.fragPath);
                }
            }
            ImGui::SameLine();
            ImGui::InputText("##FragmentShaderPath", &fragPath, ImGuiInputTextFlags_ReadOnly);
        }
    }
    EndDrawEditComponent();
}

template<>
void ComponentEditPanel::drawEditComponentWidget<CameraComponent>()
{
    BeginDrawEditComponent(CameraComponent);
    {
        DrawRightClickMenu(CameraComponent, false);
        //
        auto &camera = m_targetEntity.getComponent<CameraComponent>();
        auto &transform = m_targetEntity.getComponent<TransformComponent>();
        //
        static float aspectList[][2] = {{16.f, 9.f}, {4, 3}};
        static const char *aspectName[2] = {"16 : 9", "4 : 3"};
        static int aspectNowSelect = 0;
        static float size = 5.f, near = -1.0f, far = 1.0f;

        // Primary
        ImGui::Checkbox("Primary", &camera.primary);
        ImGui::Checkbox("Fixed Aspect", &camera.lockAspect);
        //
        if(camera.lockAspect)
        {
            // Aspect
            ImGui::Combo("Aspect", &aspectNowSelect, aspectName, 2);
            camera.camera.setAspect(aspectList[aspectNowSelect][0] / aspectList[aspectNowSelect][1]);
            //
            ImGui::DragFloat("Size", &size, 0.1f);
            ImGui::DragFloat("Near Plane", &near, 0.1f);
            ImGui::DragFloat("Far Plane", &far, 0.1f);
            camera.camera.setOrthographic(size, near, far);
            //
            transform.scale.y = size;
            transform.scale.x = size * camera.camera.getAspect();
        }
        else
        {
            camera.camera.setAspect(transform.scale.x / transform.scale.y);
            camera.camera.setOrthographic(transform.scale.y, near, far);
        }
    }
    EndDrawEditComponent();
}

template<>
void ComponentEditPanel::drawEditComponentWidget<NativeScriptComponent>()
{
    BeginDrawEditComponent(NativeScriptComponent);
    {
        DrawRightClickMenu(NativeScriptComponent, false);
        //
        auto &scriptName =  m_targetEntity.getComponent<NativeScriptComponent>().scriptName;
        ImGui::Text("Script = %s", scriptName.c_str());
    }
    EndDrawEditComponent();
}

template<>
void ComponentEditPanel::drawEditComponentWidget<RigidBody2DComponent>()
{
    BeginDrawEditComponent(RigidBody2DComponent);
    {
        DrawRightClickMenu(RigidBody2DComponent, false);
        static const char *BodyTypeString[2] = {"Static", "Dynamic"};
        auto &rigid = m_targetEntity.getComponent<RigidBody2DComponent>();
        int bodyTypeNowSelect = static_cast<int>(rigid.BodyTypeState);
        float velocityVector[2] = {rigid.velocity.x, rigid.velocity.y};
        float forceVector[2] = {rigid.force.x, rigid.force.y};
        ImGui::Text("Physics Parameter");
        {
            // can control the physics parameter
            ImGui::InputFloat("Mass", &rigid.mass, 1.0f, 5.0f, "%.2f");
            ImGui::InputFloat("Friction", &rigid.friction, 0.1f, 0.2f, "%.2f");
            ImGui::DragFloat2("Velocity", velocityVector, 1.0f);
            ImGui::DragFloat2("Force", forceVector, 1.0f);
            rigid.velocity.x = velocityVector[0];
            rigid.velocity.y = velocityVector[1];
            rigid.force.x = forceVector[0];
            rigid.force.y = forceVector[1];
            // can't control the physics parameter
            ImGui::Text("AngularVelocity: %.2f", rigid.angularVelocity);
            ImGui::Text("Torque: %.2f", rigid.torque);
        }
        ImGui::Separator();
        ImGui::Text("Body Type");
        {
            ImGui::Combo("Body type", &bodyTypeNowSelect, BodyTypeString, 2);
            if(static_cast<RigidBody2DComponent::BodyType>(bodyTypeNowSelect) == RigidBody2DComponent::BodyType::Static)
            {
               rigid.mass = MAX_float;
               rigid.BodyTypeState = RigidBody2DComponent::BodyType::Static;
            }
            else
            {
                rigid.BodyTypeState = RigidBody2DComponent::BodyType::Dynamic;
            }
        }

        // TODO: Vec2 change to glm::Vec2
        // velocity, force

    }
    EndDrawEditComponent();
}

template<>
void ComponentEditPanel::drawEditComponentWidget<BoxCollider2DComponent>()
{
    BeginDrawEditComponent(BoxCollider2DComponent);
    {
        DrawRightClickMenu(BoxCollider2DComponent, false);
        auto &collider = m_targetEntity.getComponent<BoxCollider2DComponent>();
        auto &trans = m_targetEntity.getComponent<TransformComponent>().scale;
        float translate[2] = {collider.x, collider.y};
        float scale[2] = {collider.w, collider.h};

        ImGui::DragFloat2("(x, y)", translate, 0.5f);
        ImGui::DragFloat2("(w, h)", scale, 0.5f, 0.0f);
        collider.x = translate[0];
        collider.y = translate[1];
        collider.w = scale[0];
        collider.h = scale[1];
    }
    EndDrawEditComponent();
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
    {
        // TODO: Make this into dispatcher
        drawEditComponentWidget<TagComponent>();
        drawEditComponentWidget<TransformComponent>();
        drawEditComponentWidget<RenderComponent>();
        drawEditComponentWidget<CameraComponent>();
        drawEditComponentWidget<NativeScriptComponent>();
        drawEditComponentWidget<RigidBody2DComponent>();
        drawEditComponentWidget<BoxCollider2DComponent>();
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
    }
    ImGui::EndChild();
    ImGui::End();
}

} // end of namespace rl