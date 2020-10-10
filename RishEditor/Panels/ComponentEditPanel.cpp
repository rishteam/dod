#include "ComponentEditPanel.h"

#include <Rish/Utils/FileDialog.h>
#include <Rish/Scene/ScriptableEntity.h>
#include <Rish/Scene/ScriptableManager.h>

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
        auto &script     = m_targetEntity.getComponent<NativeScriptComponent>();
        //
        static int currentScript = 0;
        const auto &optionList = ScriptableManager::GetScriptNames();
        //
        auto cur = std::find(optionList.begin(), optionList.end(), script.scriptName);
        if(cur != optionList.end() && ScriptableManager::GetName<EmptyScript>() != *cur)
        {
            currentScript = cur - optionList.begin();
        }
        //
        if(ImGui::Combo("Script", &currentScript, optionList))
        {
            ScriptableManager::Unbind(script);
            ScriptableManager::Bind(script, optionList[currentScript]);
        }
    }
    EndDrawEditComponent();
}

// TODO Particle Component
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
            if(ImGui::Button("Select"))
            {
                if(FileDialog::SelectSingleFile(nullptr, nullptr, tpath))
                {
                    emitter.texturePath = tpath;
                    emitter.texture = Texture2D::LoadTextureVFS(emitter.texturePath);
                }
            }

            ImGui::SameLine();
            ImGui::InputText("##texturePath", &emitter.texturePath, ImGuiInputTextFlags_ReadOnly);
            if(emitter.texture)
                ImGui::Image(emitter.texture->getTextureID(), ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, -1));
            else
                ImGui::Dummy(ImVec2(64, 64));


            ImGui::Text("EmitData");
            std::string dataPath;
            if(ImGui::Button("Select##dataPath"))
            {
                if(FileDialog::SelectSingleFile(nullptr, nullptr, dataPath))
                {
                    emitter.dataPath = dataPath;
                    emitter.loadEmitData();
                }
            }
            ImGui::SameLine();
            ImGui::InputText("##EmitDataPath", &emitter.dataPath, ImGuiInputTextFlags_ReadOnly);

            ImGui::ColorEdit4("Start Color", glm::value_ptr(emitter.startColor));
            ImGui::ColorEdit4("End Color", glm::value_ptr(emitter.endColor));

            ImGui::PushItemWidth(100);
            ImGui::DragFloat("PosX", &transform.translate.x, 0.1f, 0.f, 0.f, "%.2f");
            ImGui::SameLine();
            ImGui::DragFloat("PosY", &transform.translate.y, 0.1f, 0.f, 0.f, "%.2f");

            ImGui::DragFloat("DistanceX", &emitter.disX, .1f, 0.f, 0.f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat("DistanceY", &emitter.disY, .1f, 0.f, 0.f, "%.1f");

            ImGui::DragFloat("Offset##x", &emitter.offset.x, .1f, 0.f, 0.f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat("Offset##y", &emitter.offset.y, .1f, 0.f, 0.f, "%.1f");

            ImGui::Separator();

            ImGui::DragFloat("Start Size", &emitter.startSize, 0.1f, 0.f, 0.f, "%.2f");
            ImGui::SameLine();
            ImGui::DragFloat("End Size", &emitter.endSize, 0.1f, 0.f, 0.f, "%.2f");

            ImGui::Separator();

            ImGui::DragFloat("AngleX", &emitter.angleRange.x, 0.5f, 0.f, 360.f, "%.2f");
            ImGui::SameLine();
            ImGui::DragFloat("AngleY", &emitter.angleRange.y, 0.5f, 0.f, 360.f, "%.2f");

            ImGui::Separator();

            ImGui::DragFloat("Start Speed", &emitter.startSpeed, 0.1f, 0.f, 0.f, "%.2f");
            ImGui::SameLine();
            ImGui::DragFloat("End Speed", &emitter.endSpeed, 0.1f, 0.0f, 0.0f, "%.2f");

            ImGui::Separator();

            ImGui::PopItemWidth();

            ImGui::DragFloat("Rotate Speed", &emitter.rotateSpeed, 0.01f, 0.0f, 0.0f, "%.2f");
            ImGui::Separator();

            ImGui::PushItemWidth(100);

            bool calSize = false;
            if(ImGui::DragScalar("Emit Number", ImGuiDataType_U32, &emitter.emitNumber, 1.f, 0, 0, "%d"))
            {
                calSize = true;
            }

            if(ImGui::DragScalar("Emit Variance", ImGuiDataType_U32, &emitter.emitVariance, 1.f, 0, 0, "%d"))
            {
                calSize = true;
            }

            if(ImGui::DragScalar("Particle Life (frame)", ImGuiDataType_U32, &emitter.maxParticleLife, 1.f, 0,0, "%d"))
            {
                calSize = true;
            }
            if(calSize)
            {
                emitter.poolSize = (emitter.emitNumber + emitter.emitVariance) * (emitter.maxParticleLife + 1);
                emitter.particles.clear();
                emitter.particles.resize(emitter.poolSize);
                emitter.lastUnusedParticle = 0;
            }

            ImGui::Text("Particle Pool Size: %d", emitter.poolSize);
            if(ImGui::DragFloat("Sleep Time", &emitter.sleepTime, 0.1f, -1.f, FLT_MAX, "%.2f"))
            {
                emitter.active = true;
                emitter.sleepTimer.restart();
            }
            ImGui::PopItemWidth();

            if(ImGui::DragFloat("Emitter Life", &emitter.life, 1.f, 0.f, 0.f, "%.1f"))
            {
                emitter.lifeTimer.restart();
                emitter.active = true;
            }
            ImGui::Separator();
            ImGui::Text("Random Control");
            ImGui::PushItemWidth(75);
            ImGui::DragFloat("##RandomControlx", &emitter.rotSpeedRand.x, .1f, 0.f, 0.f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat("##RandomControly", &emitter.rotSpeedRand.y, .1f, 0.f, 0.f, "%.1f");
            ImGui::SameLine();
            ImGui::Text("Random Rotate Speed");

            ImGui::DragFloat("##StartSpeedx", &emitter.startSpeedRand.x, .1f, 0.f, 0.f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat("##StartSpeedy", &emitter.startSpeedRand.y, .1f, 0.f, 0.f, "%.1f");
            ImGui::SameLine();
            ImGui::Text("Random Start Speed");

            ImGui::DragFloat("##endSpeedx", &emitter.endSpeedRand.x, .1f, 0.f, 0.f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat("##endSpeedy", &emitter.endSpeedRand.y, .1f, 0.f, 0.f, "%.1f");
            ImGui::SameLine();
            ImGui::Text("Random End Speed");

            ImGui::DragFloat("##emitVariancex", &emitter.emitVarianceRand.x, .1f, 0.f, 100.f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat("##emitVariancey", &emitter.emitVarianceRand.y, .1f, 0.f, 100.f, "%.1f");
            ImGui::SameLine();
            ImGui::Text("Random Emit Variance");

            ImGui::DragFloat("##startSizex", &emitter.startSizeRand.x, .1f, 0.f, 100.f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat("##startSizey", &emitter.startSizeRand.y, .1f, 0.f, 100.f, "%.1f");
            ImGui::SameLine();
            ImGui::Text("Random Start Size");

            ImGui::DragFloat("##endSizex", &emitter.endSizeRand.x, .1f, 0.f, 100.f, "%.1f");
            ImGui::SameLine();
            ImGui::DragFloat("##endSizey", &emitter.endSizeRand.y, .1f, 0.f, 100.f, "%.1f");
            ImGui::SameLine();
            ImGui::Text("Random End Size");

            ImGui::PopItemWidth();

            ImGui::Checkbox("Vortex Sensitive", &emitter.vortexSensitive);

            if(emitter.vortexSensitive) {

                ImGui::RadioButton("Static Vortex", &emitter.vortexType, 0); ImGui::SameLine();
                ImGui::RadioButton("Dynamic Vortex", &emitter.vortexType, 1);
                if (emitter.vortexType == STATIC_VORTEX) {

                    int id = 1;
                    for(auto &vortex : emitter.static_vortexes) {

                        ImGui::PushID(id++);

                        ImGui::PushItemWidth(100);


                        ImGui::DragFloat("Vortex DistanceX", &vortex.pos.x, 0.1f, 0.f, 0.f, "%.1f");
                        ImGui::SameLine();
                        ImGui::DragFloat("Vortex DistanceY", &vortex.pos.y, 0.1, 0.f, 0.f, "%.1f");

                        ImGui::DragFloat("Vortex TurbulenceX", &vortex.turbulence.x, 0.1f, 0.f, 0.f, "%.1f");
                        ImGui::SameLine();
                        ImGui::DragFloat("Vortex TurbulenceY", &vortex.turbulence.y, 0.1f, 0.f, 0.f, "%.1f");

                        ImGui::DragFloat("Vortex Size", &vortex.currentSize, 0.1f, 0.f, 0.f, "%.2f");

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

                    ImGui::PushItemWidth(100);

                    ImGui::DragFloat("Vortex Offset##X", &emitter.vortexPos.x, 0.1f, 0.f, 0.f, "%.2f");
                    ImGui::SameLine();
                    ImGui::DragFloat("Vortex Offset##Y", &emitter.vortexPos.y, 0.1f, 0.f, 0.f, "%.2f");

                    ImGui::DragFloat("Vortex DistanceX", &emitter.vortexDisX, .1f, 0.f, 0.f, "%.1f");
                    ImGui::SameLine();
                    ImGui::DragFloat("Vortex DistanceY", &emitter.vortexDisY, .1f, 0.f, 0.f, "%.1f");

                    ImGui::DragFloat("Vortex AngleX", &emitter.vortexAngleRange.x, 0.5f, 0.f, 360.f, "%.2f");
                    ImGui::SameLine();
                    ImGui::DragFloat("Vortex AngleY", &emitter.vortexAngleRange.y, 0.5f, 0.f, 360.f, "%.2f");

                    ImGui::DragFloat("Vortex Start Speed", &emitter.vortexStartSpeed, 1.f, 0.f, 0.f, "%.1f");
                    ImGui::SameLine();
                    ImGui::DragFloat("Vortex End Speed", &emitter.vortexEndSpeed, 1.f, 0.f, 0.f, "%.1f");

                    ImGui::DragFloat("Vortex TurbulenceX", &emitter.vortexTurbulence.x, 0.1f, 0.f, 0.f, "%.1f");
                    ImGui::SameLine();
                    ImGui::DragFloat("Vortex TurbulenceY", &emitter.vortexTurbulence.y, 0.1f, 0.f, 0.f, "%.1f");

                    ImGui::DragFloat("Vortex Start Size", &emitter.vortexStartSize, 0.05f, 0.f, 0.f, "%.2f");
                    ImGui::SameLine();
                    ImGui::DragFloat("Vortex End Size", &emitter.vortexEndSize, 0.05f, 0.f, 0.f, "%.2f");

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
        return;
    }
    m_componentSelectionPanel.setTarget(m_targetEntity);

    ImGui::BeginChild("EntityComponentEdit");
    {
        // TODO: Make this into dispatcher
        //
        drawEditComponentWidget<TagComponent>();
        drawEditComponentWidget<TransformComponent>();
        drawEditComponentWidget<RenderComponent>();
        drawEditComponentWidget<CameraComponent>();
        drawEditComponentWidget<NativeScriptComponent>();
        drawEditComponentWidget<ParticleComponent>();

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
