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

void DrawDebugSceneWindow(entt::registry &registry, Scene *scene)
{
    char title[50];
    sprintf(title, "Debug Scene %p", scene);
    //
    ImGui::Begin(title);
    registry.each([&](auto ent) {
        Entity entity{ent, scene};
        std::string &tag = entity.getComponent<TagComponent>().tag;
        std::string id = entity.getComponent<TagComponent>().id.to_string();
        if(ImGui::TreeNodeEx(tag.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("UUID = %s", id.c_str());

            DrawDebugTransformComponentInfo(entity);
            DrawDebugCameraComponentInfo(entity);
            DrawDebugSpriteRenderComponentInfo(entity);
            DrawDebugNativeScriptComponentInfo(entity);
            DrawDebugParticleComponentInfo(entity);
            DrawDebugGroupComponentInfo(entity);

            ImGui::TreePop();
        }
    });
    ImGui::End();
}

void DrawDebugTransformComponentInfo(Entity entity)
{
    auto &transform = entity.getComponent<TransformComponent>();
    if(ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::DragFloat3("Translate", glm::value_ptr(transform.translate), 0.01f);
        ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.01f);
        ImGui::DragFloat("Rotation", &transform.rotate, 0.1f);
        ImGui::TreePop();
    }
}

void DrawDebugCameraComponentInfo(Entity entity)
{
    if(!entity.hasComponent<CameraComponent>())
        return;
    auto &camera = entity.getComponent<CameraComponent>();
    if(ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Primary", &camera.primary);
        ImGui::Checkbox("Fixed Aspect", &camera.lockAspect);
        //
        auto &cam = camera.camera;
        ImGui::Text("Aspect = %f", cam.getAspect());
        ImGui::Text("Size   = %f", cam.getOrthoSize());

        ImGui::TreePop();
    }
}

void PrintTextureInfo(Ref<Texture2D> tex)
{
    ImGui::Text("tex id = %d", tex->getTextureID());
    ImGui::Text("width = %d height = %d", tex->getWidth(), tex->getHeight());
    ImGui::Text("path = %s", tex->getPath().c_str());
}

void DrawDebugSpriteRenderComponentInfo(Entity entity)
{
    if(!entity.hasComponent<SpriteRenderComponent>())
        return;
    auto &render = entity.getComponent<SpriteRenderComponent>();
    if(ImGui::TreeNodeEx("Render", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();
        if(render.m_texture)
        {
            ImGui::Text("path = %s", render.m_texture->getPath().c_str());
            PrintTextureInfo(render.m_texture);
        }
        else
            ImGui::Text("m_texture = nullptr");
        ImGui::Unindent();
        ImGui::Text("-------------------");

        if(render.m_subtexture)
        {
            auto &setting = render.m_subSetting;
            //
            ImGui::Indent();
            if(setting.type == SubTexture2DSetting::SubTextureType::SubTextureSheet)
            {
                ImGui::Text("pos = %.2f %.2f", setting.pos.x, setting.pos.y);
                ImGui::Text("cellSize = %.2f %.2f", setting.cellSize.x, setting.cellSize.y);
                ImGui::Text("spriteGridSize = %.2f %.2f", setting.spriteGridSize.x, setting.spriteGridSize.y);
            }
            else if(setting.type == SubTexture2DSetting::SubTextureType::SubTextureCoordinate)
            {
                ImGui::Text("leftUpper = %.2f %.2f", setting.leftUpper.x, setting.leftUpper.y);
                ImGui::Text("size = %.2f %.2f", setting.size.x, setting.size.y);
            }
            PrintTextureInfo(render.m_subtexture->getTexture());
            ImGui::Unindent();
        }

        ImGui::TreePop();
    }
}

void DrawDebugNativeScriptComponentInfo(Entity entity)
{
    if(!entity.hasComponent<NativeScriptComponent>())
        return;
    auto &nsc = entity.getComponent<NativeScriptComponent>();
    if(ImGui::TreeNodeEx("NativeScript", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("instance = %p", (void *)nsc.instance.get());
        ImGui::Text("script   = %s", nsc.scriptName.c_str());
        ImGui::Text("valid    = %d", nsc.valid);
        ImGui::Indent();
        if(nsc.instance)
            nsc.instance->onImGuiRender();
        ImGui::Unindent();
        ImGui::TreePop();
    }
}

void DrawDebugParticleComponentInfo(Entity entity)
{
    if(!entity.hasComponent<ParticleComponent>())
        return;

}

void DrawDebugGroupComponentInfo(Entity entity)
{
    if(!entity.hasComponent<GroupComponent>())
        return;
    auto &gc = entity.getComponent<GroupComponent>();
    if(ImGui::TreeNodeEx("GroupComponent", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Child: ");
        for(auto i : gc.childEntity)
        {
            ImGui::PushID(&i);
            ImGui::BulletText("%s", i.to_c_str());
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

void DrawDebugTextureInfo(Ref<Texture2D> texture)
{
    ImGui::Text("%s %dx%d", texture->getPath().c_str(), texture->getWidth(), texture->getHeight());
}

}