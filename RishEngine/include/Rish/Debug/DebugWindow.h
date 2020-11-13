#pragma once

#include <Rish/rlpch.h>
#include <Rish/ImGui/ImGui.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Scene/Scene.h>

namespace rl {

void DrawSceneDebugWindow(const char *name, Ref<Scene> scene);
void DrawSceneCameraDebugWindow(const SceneCamera & camera, const glm::mat4 &trans);

void DrawDebugSceneWindow(entt::registry &registry, Scene *scene);
void DrawDebugTransformComponentInfo(Entity entity);
void DrawDebugCameraComponentInfo(Entity entity);
void DrawDebugSpriteRenderComponentInfo(Entity entity);
void DrawDebugNativeScriptComponentInfo(Entity entity);
void DrawDebugParticleComponentInfo(Entity entity);

void DrawDebugTextureInfo(Ref<Texture2D> texture);

}