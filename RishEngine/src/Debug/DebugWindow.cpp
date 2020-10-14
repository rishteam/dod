#include <Rish/Debug/DebugWindow.h>

#include <Rish/Scene/ScriptableEntity.h>

namespace rl {

void DrawSceneDebugWindow(const char *name, Ref<Scene> scene)
{
    static char tmp[30];
    std::sprintf(tmp, "%s: %p", name, (void *) scene.get());
    //
    if (ImGui::TreeNodeEx(tmp, ImGuiTreeNodeFlags_DefaultOpen)) {
        if (scene) {
            auto view = scene->m_registry.view<NativeScriptComponent>();
            for (auto ent : view) {
                Entity entity{ent, scene.get()};
                auto &nsc = entity.getComponent<NativeScriptComponent>();
                ImGui::Text("%s", entity.getName().c_str());
                ImGui::Indent();
                ImGui::Text("%s", entity.getUUID().to_string().c_str());
                ImGui::Text("instance: %p", (void *) nsc.instance.get());
                ImGui::Text("scriptName: %s", nsc.scriptName.c_str());
                ImGui::Text("valid: %d", nsc.valid);
                nsc.instance->onImGuiRender();
                ImGui::Unindent();
            }
        }
        ImGui::TreePop();
    }
}

void DrawSceneCameraDebugWindow(const SceneCamera &camera, const glm::mat4 &trans)
{
    ImGui::Begin("Scene Debug");
    ImGui::Text("Projection");
    auto proj = camera.getProjection();
    for(int i = 0; i < 4; i++)
    {
        ImGui::PushID(i);
        for(int j = 0; j < 4; j++)
        {
            ImGui::PushID(j);
            ImGui::Text("%.2f", static_cast<double>(proj[i][j])); ImGui::SameLine();
            ImGui::PopID();
        }
        ImGui::NewLine();
        ImGui::PopID();
    }
    ImGui::Separator();
    ImGui::Text("Camera Transform");
    for(int i = 0; i < 4; i++)
    {
        ImGui::PushID(i);
        for(int j = 0; j < 4; j++)
        {
            ImGui::PushID(j);
            ImGui::Text("%.2f", static_cast<double>(trans[i][j])); ImGui::SameLine();
            ImGui::PopID();
        }
        ImGui::NewLine();
        ImGui::PopID();
    }
    ImGui::Text("Size = %.2f", camera.m_orthoSize);
    ImGui::Text("Near = %.2f, Far = %.2f", camera.m_orthoNear, camera.m_orthoFar);
    ImGui::Text("Aspect = %.2f", camera.m_aspect);
    ImGui::End();
}

}