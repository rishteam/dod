#pragma once

#include <glm/glm.hpp>
#include <Rish/Core/Core.h>

#include <Rish/Renderer/Camera/OrthographicCamera.h>

namespace rl {

class RL_API Renderer2D
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();

    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
};

} // namespace of rl
