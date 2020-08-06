#pragma once

#include <glm/glm.hpp>
#include <Rish/Core/Core.h>

#include <Rish/Renderer/Texture2D.h>
#include <Rish/Renderer/Camera/OrthographicCamera.h>

namespace rl {

class RL_API Renderer2D
{
public:
    // TODO: Use settings instead?
    struct RL_API QuadSettings
    {
        glm::vec3 position = glm::vec3(0.f);
        glm::vec2 size     = glm::vec2(1.f);
        glm::vec4 color    = glm::vec4(1.f);
        Ref<Texture2D> texture;
        float textureTiling = 1.f;
        float rotate = 0.f;
    };
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();

    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D> &texture);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> &texture);
    static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref <Texture2D> &texture, const glm::vec4 &color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> &texture, const glm::vec4& color);

    static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotate);
    static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotate);
    static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D> &texture, float rotate);
    static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> &texture, float rotate);
    static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref <Texture2D> &texture, const glm::vec4 &color, float rotate);
    static void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color, float rotate);

    struct Stats
    {
        uint32_t QuadCount = 0; /// Draw quad count
        uint32_t DrawCount = 0; /// Draw call count
    };

    static Stats& GetStats();
    static void ResetStats();

private:
    static size_t MaxQuadCount, MaxVertexCount, MaxIndexCount;
};

} // namespace of rl
