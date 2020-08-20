#pragma once

#include <glm/glm.hpp>
#include <Rish/Core/Core.h>

#include <Rish/Renderer/Texture2D.h>
#include <Rish/Renderer/SubTexture2D.h>
#include <Rish/Renderer/Camera/OrthographicCamera.h>

namespace rl {

class RL_API Renderer2D
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();

    // TODO: support to specify the origin
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D> &texture);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> &texture);
    static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> &texture, const glm::vec4& color);
    static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, const glm::vec4 &color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D> &subtexture, const glm::vec4& color);

    static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotate);
    static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotate);
    static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D> &texture, float rotate);
    static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> &texture, float rotate);
    static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color, float rotate);
    static void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color, float rotate);
    static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, const glm::vec4 &color, float rotate);
    static void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, const glm::vec4 &color, float rotate);

    struct Stats
    {
        uint32_t QuadCount = 0; /// Draw quad count
        uint32_t DrawCount = 0; /// Draw call count
    };

    static Stats& GetStats();
    static void ResetStats();

private:
    static void FlushStatesIfExceeds();
    static float GetTextureIndex(const Ref<Texture2D>& texture);
    static void SubmitQuad(const glm::vec4 position[4], const glm::vec4 &color, const glm::vec2 texCoords[4], float texIndex, float texTiling);

    static size_t MaxQuadCount, MaxVertexCount, MaxIndexCount;
};

} // namespace of rl
