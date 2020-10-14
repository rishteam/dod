#pragma once

#include <Rish/rlpch.h>
#include <Rish/ImGui.h>

#include <Rish/Scene/Scene.h>

namespace rl {

void DrawSceneDebugWindow(const char *name, Ref<Scene> scene);
void DrawSceneCameraDebugWindow(const SceneCamera & camera, const glm::mat4 &trans);

}