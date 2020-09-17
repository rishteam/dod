#pragma once

#include <glm/glm.hpp>
#include <Rish/Core/Core.h>

#include <Rish/Renderer/Framebuffer.h>
#include <Rish/Renderer/Texture2D.h>
#include <Rish/Renderer/SubTexture2D.h>
#include <Rish/Renderer/Camera/OrthographicCamera.h>

namespace rl {

class RL_API Renderer2D
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const OrthographicCamera &camera, const Ref<Framebuffer>& framebuffer=nullptr, bool depthTest=false);
    static void EndScene();

    // TODO: support to specify the origin
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D> &texture);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> &texture);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D> &texture, const glm::vec4 &color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> &texture, const glm::vec4& color);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D> &subtexture, const glm::vec4 &color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D> &subtexture, const glm::vec4& color);

    static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotate);
    static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotate);
    static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D> &texture, float rotate);
    static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> &texture, float rotate);
    static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color, float rotate);
    static void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color, float rotate);
    static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, const glm::vec4 &color, float rotate);
    static void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture, const glm::vec4 &color, float rotate);

    static void DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color=glm::vec4(1.f));
    static void DrawLine(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec4 &color=glm::vec4(1.f));

    static void DrawRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color=glm::vec4(1.f));

    static void DrawCircleLine(const glm::vec2 &position, const float radius, const glm::vec4 &color=glm::vec4(1.f));

    struct Stats
    {
        uint32_t QuadCount = 0; /// Draw quad count
        uint32_t LineCount = 0; /// Draw line count
        uint32_t DrawCount = 0; /// Draw call count
    };

    static Stats& GetStats();
    static void ResetStats();

private:
    // TODO: move to TextureQuadRenderer
    static void FlushStatesIfExceeds();
    static float GetQuadTextureIndex(const Ref<Texture2D>& texture);
    static void SubmitQuad(const glm::vec4 position[4], const glm::vec4 &color, const glm::vec2 texCoords[4], float texIndex, float texTiling);
};

} // namespace of rl
