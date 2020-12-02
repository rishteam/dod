#include "ComponentEditPanel.h"

//
#include <Rish/Utils/FileDialog.h>
#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/ScriptableManager.h>
//
#include <Rish/Math/BoundingBox.h>
//
#include <Rish/ImGui/ImGui.h>

#define BeginDrawEditComponent(c)                                    \
    if (!m_targetEntity.hasComponent<c>()) return;                   \
    if (ImGui::CollapsingHeader(#c, ImGuiTreeNodeFlags_DefaultOpen)) \
    {                                                                \

// TODO: Expand macro for delete action

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
        ImGui::PushItemWidth(250);
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
        ImGui::PushItemWidth(250);
        auto &transform = m_targetEntity.getComponent<TransformComponent>();
        ImGui::DragFloat3("Translate", glm::value_ptr(transform.translate), 0.01f);
        ImGui::DragFloat2("Scale", glm::value_ptr(transform.scale), 0.01f);
        ImGui::DragFloat("Rotation", &transform.rotate, 0.1f);
        ImGui::SameLine();
        ImGui::HelpMarker("In degrees");
        ImGui::PopItemWidth();

        updateGroupTransform(m_targetEntity);

    }
    EndDrawEditComponent();
}

void ComponentEditPanel::updateGroupTransform(Entity targetEntity)
{
    if(targetEntity.hasComponent<GroupComponent>())
    {
        auto &gc = targetEntity.getComponent<GroupComponent>();
        const auto &groupTransform = targetEntity.getComponent<TransformComponent>();

        for(const auto& id : gc)
        {
            Entity ent = m_currentScene->getEntityByUUID(id);
            auto &sgc = ent.getComponent<SubGroupComponent>();
            auto &trans = ent.getComponent<TransformComponent>();
            sgc.setGroupPosition(groupTransform.translate);
            sgc.setOffset(groupTransform.scale/sgc.getGroupScale());
            sgc.setGroupRotate(groupTransform.rotate);

            trans.translate = sgc.calculateCurrentPosition();
            trans.scale = sgc.calculateCurrentScale();
            trans.rotate = sgc.calculateCurrentRotate();
            updateGroupTransform(ent);
        }
    }
}

template<>
void ComponentEditPanel::drawEditComponentWidget<SpriteRenderComponent>()
{
    BeginDrawEditComponent(SpriteRenderComponent);
    {
        DrawRightClickMenu(SpriteRenderComponent, false);
        //
        ImGui::PushItemWidth(250);
        auto &render = m_targetEntity.getComponent<SpriteRenderComponent>();
        ImGui::ColorEdit4("Color", glm::value_ptr(render.color));
        ImGui::PopItemWidth();

        ImGui::Separator();

        ImGui::Checkbox("Use Texture", &render.useTexture);
        if(render.useTexture)
        {
            ImGui::Text("Texture");
            {
                std::string tPath;
                ImGui::PushItemWidth(100);
                if (ImGui::Button("Load Texture"))
                {
                    std::string cur = FileSystem::GetCurrentDirectory();
                    cur += "\\assets";

                    if (FileDialog::SelectSingleFile(nullptr, cur.c_str(), tPath))
                    {
                        tPath = FileSystem::RelativePath(tPath);
                        render.loadTexture(tPath);
                    }
                }
                ImGui::PopItemWidth();

                ImGui::SameLine();
                //
                ImGui::PushItemWidth(150);
                ImGui::InputText("##texturePath", &render.texturePath, ImGuiInputTextFlags_ReadOnly);
                ImGui::PopItemWidth();
                if (render.m_texture)
                {
                    ImGui::BeginChild("texture_viewer", ImVec2(ImGui::GetWindowWidth(), 64));
                    ImGui::Image(render.m_texture->getTextureID(), ImVec2(64 * render.m_texture->getAspectRatio(), 64),
                                 ImVec2(0, 0), ImVec2(1, -1));
                    ImGui::EndChild();
                }
                else
                    ImGui::Dummy(ImVec2(64, 64));

                ImGui::PushItemWidth(250);
                ImGui::DragFloat("Tiling", &render.tiling, 1.f);
                ImGui::PopItemWidth();
            }
            //
            SubTexture2DSetting &setting = render.m_subSetting;
            if(ImGui::Checkbox("Use as SubTexture", &render.useAsSubTexture))
            {
                render.initSubTexture();
            }
            //
            if (render.useAsSubTexture)
            {
                auto &type = setting.type;
                if (ImGui::RadioButton("Sheet", type == SubTexture2DSetting::SubTextureSheet))
                    type = SubTexture2DSetting::SubTextureSheet;
                if (ImGui::RadioButton("Coordinate", type == SubTexture2DSetting::SubTextureCoordinate))
                    type = SubTexture2DSetting::SubTextureCoordinate;

                if (type == SubTexture2DSetting::SubTextureSheet)
                {
                    ImGui::DragFloat2("Pos", glm::value_ptr(setting.pos));
                    ImGui::DragFloat2("Cell Size", glm::value_ptr(setting.cellSize));
                    ImGui::DragFloat2("Sprite Grid Size", glm::value_ptr(setting.spriteGridSize));
                }
                else if (type == SubTexture2DSetting::SubTextureCoordinate)
                {
                    ImGui::DragFloat2("Left Upper", glm::value_ptr(setting.leftUpper));
                    ImGui::DragFloat2("Size", glm::value_ptr(setting.size));
                }

                if (ImGui::Button("Set SubTexture"))
                {
                    render.loadSubTexture(setting);
                }
            }
        }
    }
    EndDrawEditComponent();
    //
    if(ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
    {
        m_targetEntity.removeComponent<SpriteRenderComponent>();
    }
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
        static float size = 5.f, near = -1000.0f, far = 1000.0f;

        // Primary
        ImGui::Checkbox("Primary", &camera.primary);
        ImGui::Checkbox("Fixed Aspect", &camera.lockAspect);
        //
        if(camera.lockAspect)
        {
            // Aspect
            ImGui::PushItemWidth(250);
            ImGui::Combo("Aspect", &aspectNowSelect, aspectName, 2);
            camera.camera.setAspect(aspectList[aspectNowSelect][0] / aspectList[aspectNowSelect][1]);
            //
            ImGui::DragFloat("Size", &size, 0.1f);
            ImGui::DragFloat("Near Plane", &near, 0.1f);
            ImGui::DragFloat("Far Plane", &far, 0.1f);
            ImGui::PopItemWidth();

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
    //
    if(ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
    {
        m_targetEntity.removeComponent<CameraComponent>();
    }
}

template<>
void ComponentEditPanel::drawEditComponentWidget<NativeScriptComponent>()
{
    BeginDrawEditComponent(NativeScriptComponent);
    {
        DrawRightClickMenu(NativeScriptComponent, false);
        //
        auto &scriptComponent = m_targetEntity.getComponent<NativeScriptComponent>();
        //
        static int currentScript = 0;
        const auto &scriptList = ScriptableManager::GetScriptNames();

        // Current select script
        auto cur = std::find(scriptList.begin(), scriptList.end(), scriptComponent.scriptName);
        if(cur != scriptList.end() && ScriptableManager::GetName<EmptyScript>() != *cur)
        {
            currentScript = cur - scriptList.begin();
        }

        // Drop down combo
        ImGui::PushItemWidth(250);
        if(ImGui::Combo("Script", &currentScript, scriptList))
        {
            ScriptableManager::Bind(m_targetEntity, scriptList[currentScript]);
        }
        ImGui::PopItemWidth();

        // Script Fields
        if(scriptComponent.instance)
            scriptComponent.instance->onImGuiRender();
        else
            ImGui::Text("Fuck me plz");
    }
    EndDrawEditComponent();
    if(ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
    {
        m_targetEntity.removeComponent<NativeScriptComponent>();
    }
}

template<>
void ComponentEditPanel::drawEditComponentWidget<ParticleComponent>()
{
    BeginDrawEditComponent(ParticleComponent);
    {
        DrawRightClickMenu(ParticleComponent, false);

        auto &transform = m_targetEntity.getComponent<TransformComponent>();
        auto &emitter   = m_targetEntity.getComponent<ParticleComponent>();

        ImGui::Text("Texture");
        {
            std::string tpath;
            ImGui::PushItemWidth(50);
            if (ImGui::Button("Select")) {
                if (FileDialog::SelectSingleFile(nullptr, nullptr, tpath)) {
                    emitter.texturePath = tpath;
                    emitter.texture = Texture2D::LoadTextureVFS(emitter.texturePath);
                }
            }
            ImGui::PopItemWidth();

            ImGui::SameLine();
            ImGui::PushItemWidth(200);
            ImGui::InputText("##texturePath", &emitter.texturePath, ImGuiInputTextFlags_ReadOnly);
            ImGui::PopItemWidth();
            if (emitter.texture)
                ImGui::Image(emitter.texture->getTextureID(), ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, -1));
            else
                ImGui::Dummy(ImVec2(64, 64));
        }

        ImGui::Text("EmitData");
        {

            glm::vec2 emitterDistance(emitter.disX, emitter.disY);
            glm::vec2 emitterOffset(emitter.offset);
            glm::vec2 emitterSize(emitter.startSize, emitter.endSize);
            glm::vec2 emitterAngleRange(emitter.angleRange);
            glm::vec2 emitterSpeed(emitter.startSpeed, emitter.endSpeed);
            float emitterRotateSpeed(emitter.rotateSpeed);

            std::vector<File> file;
            FileSystem::List("assets/Effect/Particle", file);

            std::vector<std::string> filename;
            for (auto f : file) filename.push_back(f.getFilename());

            ImGui::PushItemWidth(250);

            static int currentEmitData = -1;
            if (ImGui::Combo("Select Emit Data", &currentEmitData, filename)) {
                emitter.dataPath = file[currentEmitData].getPath();
                emitter.loadEmitData();
            }

            ImGui::SameLine();
            ImGui::InputText("##EmitDataPath", &emitter.dataPath, ImGuiInputTextFlags_ReadOnly);

            ImGui::ColorEdit4("Start Color", glm::value_ptr(emitter.startColor));
            ImGui::ColorEdit4("End Color", glm::value_ptr(emitter.endColor));
            ImGui::PopItemWidth();

            ImGui::PushItemWidth(250);
            ImGui::DragFloat2("Position", glm::value_ptr(transform.translate), .1f, 0.f, 0.f, "%.2f");
            ImGui::DragFloat2("Distance", glm::value_ptr(emitterDistance), .1f, 0.f, 0.f, "%.1f");
            ImGui::DragFloat2("Offset", glm::value_ptr(emitterOffset), .1f, 0.f, 0.f, "%.1f");
            ImGui::DragFloat2("Size", glm::value_ptr(emitterSize), 0.1f, 0.f, 0.f, "%.2f");
            ImGui::DragFloat2("Angle", glm::value_ptr(emitterAngleRange), 0.5f, 0.f, 360.f, "%.2f");
            ImGui::DragFloat2("Speed", glm::value_ptr(emitterSpeed), 0.1f, 0.f, 0.f, "%.2f");
            ImGui::DragFloat("Rotate Speed", &emitterRotateSpeed, 0.01f, 0.0f, 0.0f, "%.2f");

            bool calSize = false;
            if (ImGui::DragScalar("Emit Number", ImGuiDataType_U32, &emitter.emitNumber, 1.f, 0, 0, "%d")) {
                calSize = true;
            }

            if (ImGui::DragScalar("Emit Variance", ImGuiDataType_U32, &emitter.emitVariance, 1.f, 0, 0, "%d")) {
                calSize = true;
            }

            if (ImGui::DragScalar("Particle Life (frame)", ImGuiDataType_U32, &emitter.maxParticleLife, 1.f, 0, 0,
                                  "%d")) {
                calSize = true;
            }
            if (calSize) {
                emitter.poolSize = (emitter.emitNumber + emitter.emitVariance) * (emitter.maxParticleLife + 1);
                emitter.particles.clear();
                emitter.particles.resize(emitter.poolSize);
                emitter.lastUnusedParticle = 0;
            }

            emitter.disX        = emitterDistance.x;
            emitter.disY        = emitterDistance.y;
            emitter.offset      = emitterOffset;
            emitter.startSize   = emitterSize.x;
            emitter.endSize     = emitterSize.y;
            emitter.angleRange  = emitterAngleRange;
            emitter.startSpeed  = emitterSpeed.x;
            emitter.endSpeed    = emitterSpeed.y;
            emitter.rotateSpeed = emitterRotateSpeed;

            ImGui::PopItemWidth();
            ImGui::Separator();
        }
        ImGui::Text("Particle Pool Size: %d", emitter.poolSize);
        {
            ImGui::PushItemWidth(250);
            if (ImGui::DragFloat("Sleep Time", &emitter.sleepTime, 0.1f, -1.f, FLT_MAX, "%.2f")) {
                emitter.active = true;
                emitter.sleepTimer.restart();
            }

            if (ImGui::DragFloat("Emitter Life", &emitter.life, 1.f, 0.f, 0.f, "%.1f")) {
                emitter.lifeTimer.restart();
                emitter.active = true;
            }
            ImGui::PopItemWidth();

            ImGui::Separator();
        }
        ImGui::Text("Random Control");
        {

            ImGui::PushItemWidth(250);
            ImGui::DragFloat2("Rotate Speed", glm::value_ptr(emitter.rotSpeedRand), .1f, 0.f, 0.f, "%.1f");
            ImGui::DragFloat2("Start Speed", glm::value_ptr(emitter.startSpeedRand), .1f, 0.f, 0.f, "%.1f");
            ImGui::DragFloat2("End Speed", glm::value_ptr(emitter.endSpeedRand), .1f, 0.f, 0.f, "%.1f");
            ImGui::DragFloat2("Emit Variance", glm::value_ptr(emitter.emitVarianceRand), .1f, 0.f, 100.f, "%.1f");
            ImGui::DragFloat2("Start Size", glm::value_ptr(emitter.startSizeRand), .1f, 0.f, 100.f, "%.1f");
            ImGui::DragFloat2("End Size", glm::value_ptr(emitter.endSizeRand), .1f, 0.f, 100.f, "%.1f");

            ImGui::PopItemWidth();
            ImGui::Separator();
        }

        ImGui::Checkbox("Vortex Sensitive", &emitter.vortexSensitive);
        {
            if(emitter.vortexSensitive) {

                ImGui::RadioButton("Static Vortex", &emitter.vortexType, 0); ImGui::SameLine();
                ImGui::RadioButton("Dynamic Vortex", &emitter.vortexType, 1);
                if (emitter.vortexType == STATIC_VORTEX) {

                    int id = 1;
                    for(auto &vortex : emitter.static_vortexes) {

                        ImGui::PushID(id++);

                        ImGui::PushItemWidth(250);
                        ImGui::DragFloat2("Distance", glm::value_ptr(vortex.pos), 0.1f, 0.f, 0.f, "%.1f");
                        ImGui::DragFloat2("Turbulence", glm::value_ptr(vortex.turbulence), 0.1f, 0.f, 0.f, "%.1f");
                        ImGui::DragFloat("Size", &vortex.currentSize, 0.1f, 0.f, 0.f, "%.2f");

                        ImGui::Checkbox("Draw vortex", &vortex.draw);

                        ImGui::PopItemWidth();
                        if(ImGui::Button("Delete")) {

                            if(emitter.static_vortexes.size() > 1){

                                auto it = std::find(emitter.static_vortexes.begin(), emitter.static_vortexes.end(), vortex);
                                emitter.static_vortexes.erase(it);
                            }
                        }
                        ImGui::PopID();
                        ImGui::Separator();
                    }

                    if(ImGui::Button("Add Vortex")) {

                        emitter.static_vortexes.push_back(Particle(emitter.static_vortexes.size()));
                    }
                }

                    // Dynamic vortex
                else {

                    glm::vec2 emitterVortexPosition(emitter.vortexPos);
                    glm::vec2 emitterVortexDistance(emitter.vortexDisX, emitter.vortexDisY);
                    glm::vec2 emitterVortexAngle(emitter.vortexAngleRange);
                    glm::vec2 emitterVortexSpeed(emitter.vortexStartSpeed, emitter.vortexEndSpeed);
                    glm::vec2 emitterVortexTurbulence(emitter.vortexTurbulence);
                    glm::vec2 emitterVortexSize(emitter.vortexStartSize, emitter.vortexEndSize);

                    ImGui::PushItemWidth(250);
                    ImGui::DragFloat2("Offset", glm::value_ptr(emitterVortexPosition), 0.1f, 0.f, 0.f, "%.2f");
                    ImGui::DragFloat2("Distance", glm::value_ptr(emitterVortexDistance), .1f, 0.f, 0.f, "%.1f");
                    ImGui::DragFloat2("Angle", glm::value_ptr(emitterVortexAngle), .5f, 0.f, 360.f, "%.2f");
                    ImGui::DragFloat2("Speed", glm::value_ptr(emitterVortexSpeed), 1.f, 0.f, 0.f, "%.1f");
                    ImGui::DragFloat2("Turbulence", glm::value_ptr(emitterVortexTurbulence), .1f, 0.f, 0.f, "%.1f");
                    ImGui::DragFloat2("Size", glm::value_ptr(emitterVortexSize), .05f, 0.f, 0.f, "%.2f");
                    ImGui::PopItemWidth();

                    emitter.vortexPos        = emitterVortexPosition;
                    emitter.vortexDisX       = emitterVortexDistance.x;
                    emitter.vortexDisY       = emitterVortexDistance.y;
                    emitter.vortexAngleRange = emitterVortexAngle;
                    emitter.vortexStartSpeed = emitterVortexSpeed.x;
                    emitter.vortexEndSpeed   = emitterVortexSpeed.y;
                    emitter.vortexTurbulence = emitterVortexTurbulence;
                    emitter.vortexStartSize  = emitterVortexSize.x;
                    emitter.vortexEndSize    = emitterVortexSize.y;


                    ImGui::PushItemWidth(100);
                    const ImU32 emitMin = 0, emitMax = 50, vortexLife = 1;
                    if(ImGui::DragScalar("Vortex Particle Life (frame)", ImGuiDataType_U32, &emitter.vortexMaxParticleLife, 1.f, &vortexLife, 0, "%d"))
                    {
                        emitter.vortexPoolSize = emitter.vortexEmitNumber*3;
                        emitter.dynamic_vortexes.clear();
                        emitter.dynamic_vortexes.resize(emitter.vortexPoolSize);
                        emitter.lastUnusedVortex = 0;
                    }
                    ImGui::DragScalar("Emit Per Sec", ImGuiDataType_U32, &emitter.vortexEmitNumber, 1.f, &emitMin, 0, "%d");
                    ImGui::Text("Particle Pool Size: %d", emitter.vortexPoolSize);
                    ImGui::Checkbox("Draw Vortex", &emitter.drawVortex);
                    ImGui::PopItemWidth();

                    if(ImGui::DragFloat("Vortex Sleep Time", &emitter.vortexSleepTime, 0.5f, -1.f, FLT_MAX, "%.1f"))
                    {
                        emitter.vortexSleepTimer.restart();
                        emitter.vortexActive = true;
                    }

                    ImGui::Separator();
                    ImGui::Text("Random Control");
                    ImGui::PushItemWidth(75);

                    ImGui::DragFloat("##vortexStartSpeedx", &emitter.vortexStartSpeedRand.x, .1f, 0.f, 0.f, "%.1f");
                    ImGui::SameLine();
                    ImGui::DragFloat("##vortexStartSpeedy", &emitter.vortexStartSpeedRand.y, .1f, 0.f, 0.f, "%.1f");
                    ImGui::SameLine();
                    ImGui::Text("Vortex Random Start Speed");

                    ImGui::DragFloat("##vortexEndSpeedx", &emitter.vortexEndSpeedRand.x, .1f, 0.f, 0.f, "%.1f");
                    ImGui::SameLine();
                    ImGui::DragFloat("##vortexEndSpeedy", &emitter.vortexEndSpeedRand.y, .1f, 0.f, 0.f, "%.1f");
                    ImGui::SameLine();
                    ImGui::Text("VortexRandom End Speed");

                    ImGui::DragFloat("##vortexStartSizex", &emitter.vortexStartSizeRand.x, .05f, 0.f, 100.f, "%.3f");
                    ImGui::SameLine();
                    ImGui::DragFloat("##vortexStartSizey", &emitter.vortexStartSizeRand.y, .05f, 0.f, 100.f, "%.3f");
                    ImGui::SameLine();
                    ImGui::Text("Vortex Random Start Size");

                    ImGui::DragFloat("##vortexEndSizex", &emitter.vortexEndSizeRand.x, .05f, 0.f, 100.f, "%.1f");
                    ImGui::SameLine();
                    ImGui::DragFloat("##vortexEndSizey", &emitter.vortexEndSizeRand.y, .05f, 0.f, 100.f, "%.1f");
                    ImGui::SameLine();
                    ImGui::Text("Vortex Random End Size");

                    ImGui::DragFloat("##vortexTurbulenceRandX", &emitter.vortexTurbulenceRand.x, .05f, 0, 0, "%.1f");
                    ImGui::SameLine();
                    ImGui::DragFloat("##vortexTurbulenceRandY", &emitter.vortexTurbulenceRand.y, 0.05, 0, 0, "%.1f");

                    ImGui::PopItemWidth();
                }
            }

            if(ImGui::Button("Save Emit Data as")) {

                std::string path;
                if(FileDialog::SelectSaveFile(nullptr, nullptr, path)) {

                    emitter.dataPath = path;
                    std::ofstream os(path);
                    cereal::JSONOutputArchive outputArchive(os);
                    outputArchive(cereal::make_nvp("Emit Data", emitter));
                }
            }
        }
    }
    EndDrawEditComponent();
    if(ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
    {
        m_targetEntity.removeComponent<ParticleComponent>();
    }
}

template<>
void ComponentEditPanel::drawEditComponentWidget<AmbientLightComponent>()
{
    BeginDrawEditComponent(AmbientLightComponent);
    {
        DrawRightClickMenu(AmbientLightComponent, false);

        auto &transform = m_targetEntity.getComponent<TransformComponent>();
        auto &light     = m_targetEntity.getComponent<AmbientLightComponent>();

        ImGui::ColorEdit4("Ambient Color Mask", glm::value_ptr(light.colorMask));

        ImGui::DragFloat3("Mask Position", glm::value_ptr(transform.translate));
        ImGui::DragFloat3("Mask Size", glm::value_ptr(transform.scale));
    }

    EndDrawEditComponent();
}

template<>
void ComponentEditPanel::drawEditComponentWidget<LightComponent>()
{
    BeginDrawEditComponent(LightComponent);
    {
        DrawRightClickMenu(LightComponent, false);

        auto &transform = m_targetEntity.getComponent<TransformComponent>();
        auto &light     = m_targetEntity.getComponent<LightComponent>();

        ImGui::ColorEdit4("Light Color", glm::value_ptr(light.color), ImGuiColorEditFlags_Float);

        ImGui::DragFloat3("Light Position", glm::value_ptr(transform.translate), 0.01);
        ImGui::Checkbox("Custom View Port Position", &light.customViewPos);
        if(light.customViewPos)
        {
            ImGui::DragFloat3("View Port Position", glm::value_ptr(light.viewPortPos), 0.01);
        }
        else
        {
            light.viewPortPos = transform.translate;
        }
        ImGui::DragFloat2("View Port Size", glm::value_ptr(light.viewPortSize), 0.01);

        ImGui::Checkbox("PenetrateRadius", &light.penetrateRadius);
        if(!light.penetrateRadius)
        {
            ImGui::DragFloat("Light Radius", &light.radius, 1);
        }
        ImGui::DragFloat("Light Strength", &light.strength, 1);
        ImGui::DragFloat("Shadow Scale", &light.shadowScale, 0.01);

        // Building Option List
        auto &registry = m_targetEntity.m_scene->m_registry;
        auto rigidView = registry.view<RigidBody2DComponent>();

        static std::set<std::tuple<bool, UUID, std::string>> RayCastList;
        static std::set<std::tuple<bool, UUID, std::string>> NoRayCastList;
        static std::set<std::tuple<bool, UUID, std::string>> DeleteList;

        for(auto ent : rigidView)
        {
            Entity entity{ent, m_targetEntity.m_scene};
            auto &RigidBodyID = entity.getComponent<TagComponent>().id;
            auto &RigidBodyName = entity.getComponent<TagComponent>().tag;
            auto &RigidBody = entity.getComponent<RigidBody2DComponent>();

            std::string options_single = RigidBodyName + "(" + RigidBodyID.to_string() + ")";

            if(light.ENTITY_NO_RAY_CAST.find(RigidBodyID) != light.ENTITY_NO_RAY_CAST.end())
            {
                NoRayCastList.insert(make_tuple(false, RigidBodyID, options_single));
            }

            else
            {
                RayCastList.insert(make_tuple(false, RigidBodyID, options_single));
            }
        }


        std::set<UUID>::iterator it;
        if(ImGui::ListBoxHeader("Entity Has Ray Cast", rigidView.size(), 4))
        {
            for(auto item : RayCastList)
            {
                auto tmp = item;
                if(ImGui::Selectable(std::get<2>(item).c_str(), std::get<0>(item)))
                {
                    light.ENTITY_NO_RAY_CAST.insert(std::get<1>(item));
                    NoRayCastList.insert(tmp);
                    DeleteList.insert(item);
                }
            }
            ImGui::ListBoxFooter();
        }

        for(auto dele : DeleteList)
        {
            auto it = RayCastList.find(dele);
            if(it != RayCastList.end())
            {
                RayCastList.erase(it);
            }
        }

        DeleteList.clear();

        if(ImGui::ListBoxHeader("Entity Does Not Has Ray Cast", rigidView.size(), 4))
        {
            for(auto item : NoRayCastList)
            {
                auto tmp = item;
                if(ImGui::Selectable(std::get<2>(item).c_str(), std::get<0>(item)))
                {
                    light.ENTITY_NO_RAY_CAST.erase(std::get<1>(item));
                    RayCastList.insert(tmp);
                    DeleteList.insert(item);
                }
            }
            ImGui::ListBoxFooter();
        }
//
        for(auto dele : DeleteList)
        {
            if(NoRayCastList.empty()) break;
            auto it = NoRayCastList.find(dele);
            if(it != RayCastList.end())
            {
                NoRayCastList.erase(it);
            }
        }

        DeleteList.clear();
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
        auto &trans = m_targetEntity.getComponent<TransformComponent>();
        int bodyTypeNowSelect = static_cast<int>(rigid.BodyTypeState);
        float velocityVector[2] = {rigid.velocity.x, rigid.velocity.y};
        float forceVector[2] = {rigid.force.x, rigid.force.y};
        glm::vec2 attachPoint = rigid.attachPoint;

        ImGui::Text("Physics Parameter");
        {
            // can control the physics parameter
            ImGui::PushItemWidth(250);
            ImGui::InputFloat("Mass", &rigid.mass, 1.0f, 5.0f, "%.2f");
            ImGui::InputFloat("Friction", &rigid.friction, 0.1f, 0.2f, "%.2f");
            ImGui::DragFloat2("Velocity", velocityVector, 1.0f);
            ImGui::DragFloat2("Force", forceVector, 1.0f);
            ImGui::PopItemWidth();

            // can't control the physics parameter
            auto transPoint = trans.translate;
            auto transWH = trans.scale;
            ImGui::PushItemWidth(250);
            ImGui::Text("Attach Point");
            ImGui::DragFloat2("(x, y)", glm::value_ptr(attachPoint), .1f, -transWH.x/2.f, transWH.x/2.f);
            ImGui::PopItemWidth();
            ImGui::Checkbox("Show Attach Point", &rigid.showAttachPoint);
            ImGui::Checkbox("Keeping Force", &rigid.keepingForce);
            ImGui::SameLine();
            if (rigid.keepingForce)
                ImGui::Text(": On");
            else
                ImGui::Text(": Off");
            ImGui::Text("AngularVelocity: %.2f", rigid.angularVelocity);
            ImGui::Text("Torque: %.2f", rigid.torque);
            // Update Value  of physics
            rigid.velocity.x = velocityVector[0];
            rigid.velocity.y = velocityVector[1];
            rigid.force.x = forceVector[0];
            rigid.force.y = forceVector[1];
            rigid.attachPoint.x = attachPoint.x;
            rigid.attachPoint.y = attachPoint.y;

        }

        ImGui::Checkbox("Restrict Rotation", &rigid.RestrictRotation);
        ImGui::Separator();
        ImGui::PushItemWidth(250);
        if(ImGui::Combo("BodyType", &bodyTypeNowSelect, BodyTypeString, 2))
        {
            if(static_cast<RigidBody2DComponent::BodyType>(bodyTypeNowSelect) == RigidBody2DComponent::BodyType::Static)
            {
                rigid.mass = MAX_float;
                rigid.BodyTypeState = RigidBody2DComponent::BodyType::Static;
            }
            else
            {
                rigid.mass = 10.0f;
                rigid.BodyTypeState = RigidBody2DComponent::BodyType::Dynamic;
            }
        }
        ImGui::PopItemWidth();
    }
    EndDrawEditComponent();
    //
    if(ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
    {
        m_targetEntity.removeComponent<RigidBody2DComponent>();
    }
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

        ImGui::PushItemWidth(250);
        ImGui::DragFloat2("(x, y)", translate, 0.5f);
        ImGui::DragFloat2("(w, h)", scale, 0.5f, 0.0f);
        ImGui::PopItemWidth();
        // Update the collider value
        collider.x = translate[0];
        collider.y = translate[1];
        collider.w = scale[0];
        collider.h = scale[1];

        // Collision State
        if (collider.isCollision)
        {
            ImGui::Text("isCollide: True");
            ImGui::Text("Collide Object: ");
            for(auto idx : collider.whoCollide)
            {
                ImGui::Text("%s", idx.to_string().c_str());
            }
        }
        else
        {
            ImGui::Text("isCollide: False");
            ImGui::Text("Collide Object: None");
        }


    }
    EndDrawEditComponent();
    //
    if(ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
    {
        m_targetEntity.removeComponent<BoxCollider2DComponent>();
    }
}

template<>
void ComponentEditPanel::drawEditComponentWidget<Joint2DComponent>()
{
    BeginDrawEditComponent(Joint2DComponent);
    {
        DrawRightClickMenu(Joint2DComponent, false);
        // Options List building
        auto &registry = m_targetEntity.m_scene->m_registry;
        auto view = registry.view<RigidBody2DComponent>();
        std::vector<std::tuple<bool, UUID, std::string>> OptionsList;
        for(auto ent : view)
        {
            Entity entity{ent, m_targetEntity.m_scene};
            auto &RigidBodyID = entity.getComponent<TagComponent>().id;
            auto &RigidBodyName = entity.getComponent<TagComponent>().tag;
            auto &RigidBody = entity.getComponent<RigidBody2DComponent>();
            // List box
            std::string options_single = RigidBodyName + "(" + RigidBodyID.to_string() + ")";
            OptionsList.push_back(make_tuple(false, RigidBodyID, options_single));
        }
        std::sort(OptionsList.begin(), OptionsList.end());
        // Copy to RigidBody2 OptionList
        std::vector<std::tuple<bool, UUID, std::string>> OptionsList2(OptionsList.size());
        std::copy(OptionsList.begin(), OptionsList.end(), OptionsList2.begin());

        auto &jit = m_targetEntity.getComponent<Joint2DComponent>();
        float jitAnchor[2] = {jit.anchor.x, jit.anchor.y};

        ImGui::PushItemWidth(250);
        //RigidBody1 OptionLists
        if(ImGui::ListBoxHeader("RigidBody1", view.size(), 4))
        {
            for (auto item : OptionsList)
            {
                if (ImGui::Selectable(std::get<2>(item).c_str(), std::get<0>(item)))
                {
                    std::get<0>(item) = true;
                    jit.rigidBody1 = std::get<1>(item);
                }
            }
            ImGui::ListBoxFooter();
        }

        //RigidBody2 OptionLists
        if(ImGui::ListBoxHeader("RigidBody2", view.size(), 4))
        {
            for (auto item : OptionsList2)
            {
                if (ImGui::Selectable(std::get<2>(item).c_str(), std::get<0>(item)))
                {
                    std::get<0>(item) = true;
                    jit.rigidBody2 = std::get<1>(item);
                }
            }
            ImGui::ListBoxFooter();
        }

        ImGui::Text("RigidBody1: %s", jit.rigidBody1.to_string().c_str());
        ImGui::Text("RigidBody2: %s", jit.rigidBody2.to_string().c_str());

        ImGui::DragFloat2("Anchor", jitAnchor, 1.0f);
        jit.anchor.x = jitAnchor[0];
        jit.anchor.y = jitAnchor[1];
        ImGui::PopItemWidth();
        ImGui::Separator();

        ImGui::Text("r1:   (%.f, %.f)", jit.r1.x, jit.r1.y);
        ImGui::Text("r2:   (%.f, %.f)", jit.r2.x, jit.r2.y);
        ImGui::Text("P:    (%.f, %.f)", jit.P.x, jit.P.y);
        ImGui::Text("bias: (%.f, %.f)", jit.bias.x, jit.bias.y);
        ImGui::Text("biasFactor: %.f", jit.biasFactor);
        ImGui::Text("softness:   %.f", jit.softness);
    }
    EndDrawEditComponent();
    if(ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
    {
        m_targetEntity.removeComponent<Joint2DComponent>();
    }
}

template <>
void ComponentEditPanel::drawEditComponentWidget<GroupComponent>()
{
    BeginDrawEditComponent(GroupComponent);
    {
        DrawRightClickMenu(GroupComponent, false);
        showGroupEntity(m_targetEntity);

    }
    EndDrawEditComponent();

}

void ComponentEditPanel::showGroupEntity(Entity targetEntity)
{
    if( targetEntity.hasComponent<GroupComponent>() )
    {
        auto &gc = targetEntity.getComponent<GroupComponent>();
        for(const auto &id : gc)
        {
            Entity ent = m_currentScene->getEntityByUUID(id);
            auto &sgc = ent.getComponent<SubGroupComponent>();
            auto &tag = ent.getComponent<TagComponent>();
            auto &trans = ent.getComponent<TransformComponent>();

            auto groupPos = sgc.getGroupPosition();
            auto relativePos = sgc.getRelativePosition();
            if(ImGui::TreeNode(tag.tag.c_str()))
            {
                ImGui::Text("GroupPosition:    x: %2.2f, y: %2.2f", groupPos.x, groupPos.y );
                ImGui::Text("RelativePosition: x: %2.2f, y: %2.2f", relativePos.x, relativePos.y);
                ImGui::Text("Position:         x: %2.2f, y: %2.2f",trans.translate.x, trans.translate.y);
                ImGui::Text("Scale:            x: %2.2f, y: %2.2f",trans.scale.x, trans.scale.y);
                ImGui::Text("Offset:           x: %2.2f, y: %2.2f", sgc.getOffset().x, sgc.getOffset().y);
                ImGui::Text("GroupRotate:  %10.2f", sgc.getGroupRotate());
                ImGui::Text("OriginRotate: %10.2f", sgc.getOriginRotate());
                showGroupEntity(ent);
                ImGui::TreePop();
            }
        }

    }
}

template <>
void ComponentEditPanel::drawEditComponentWidget<SubGroupComponent>()
{
    BeginDrawEditComponent(SubGroupComponent);
    {
        DrawRightClickMenu(SubGroupComponent, false);
    }
    EndDrawEditComponent();
}

template<>
void ComponentEditPanel::drawEditComponentWidget<Animation2DComponent>()
{
    BeginDrawEditComponent(Animation2DComponent);
    {
        DrawRightClickMenu(Animation2DComponent, false);
        //
        auto &ani = m_targetEntity.getComponent<Animation2DComponent>();
        //
        if(ImGui::Button("Edit"))
        {
            m_animationEditor.setTargetComponent(&ani);
            m_animationEditor.showWindow();
        }
        ImGui::SameLine();
        if(ImGui::Button("Preview"))
        {
            ani.toggle();
        }

        auto curFrame = ani.getCurrentFrame();
        if(curFrame)
        {
            ImVec2 size{(float) curFrame->getWidth(), (float) curFrame->getHeight()};
            auto textureID = curFrame->getTextureID();
            ImGui::Image(textureID, size, {0, 0}, {1, -1});
        }
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
        m_animationEditor.resetTargetComponent();
        m_animationEditor.hideWindow();
        return;
    }
    m_componentSelectionPanel.setTarget(m_targetEntity);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;
    ImGui::BeginChild("EntityComponentEdit", ImVec2(0,0), false, window_flags);
    {
        // TODO: Make this into dispatcher
        // TODO: Make these into moveable panels;
        drawEditComponentWidget<TagComponent>();
        drawEditComponentWidget<TransformComponent>();
        drawEditComponentWidget<SpriteRenderComponent>();
        drawEditComponentWidget<Animation2DComponent>();
        drawEditComponentWidget<CameraComponent>();
        drawEditComponentWidget<NativeScriptComponent>();
        drawEditComponentWidget<ParticleComponent>();
        drawEditComponentWidget<RigidBody2DComponent>();
        drawEditComponentWidget<BoxCollider2DComponent>();
        drawEditComponentWidget<Joint2DComponent>();
        drawEditComponentWidget<LightComponent>();
        drawEditComponentWidget<AmbientLightComponent>();

        // for debug
//        drawEditComponentWidget<GroupComponent>();
//        drawEditComponentWidget<SubGroupComponent>();

        // Popup
        if(ImGui::Button(ICON_FA_PLUS, ImVec2(ImGui::GetContentRegionAvailWidth(), 0)) ||
           (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))))
        {
            m_componentSelectionPanel.m_open = true;
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

    m_animationEditor.onImGuiRender();
}

} // end of namespace rl
