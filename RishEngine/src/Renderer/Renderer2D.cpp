#include <Rish/rlpch.h>

#include <Rish/Renderer/RendererCommand.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Renderer/VertexArray.h>
#include <Rish/Renderer/Shader.h>

#include <glm/gtc/matrix_transform.hpp>

namespace rl {

const char * texturedVS = R"glsl(
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_Tiling;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;
out float v_Tiling;

uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
    //
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    v_Tiling = a_Tiling;
}
)glsl";
const char *texturedFS = R"glsl(
#version 330 core

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_Tiling;

out vec4 color;

uniform sampler2D u_Textures[32];

void main()
{
    int index = int(v_TexIndex);
    vec4 texColor = texture(u_Textures[index], v_TexCoord * v_Tiling);
    color = texColor * v_Color;
}
)glsl";

struct QuadVertex
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texCoord;
    float texIndex;
    float texTiling;
};

const size_t MaxTextures = 32;

struct Renderer2DData
{
    ~Renderer2DData()
    {
        delete [] quadBuffer;
    }
    Ref<VertexArray> quadVertexArray;
    Ref<VertexBuffer> quadVertexBuffer;
    Ref<Shader> textureShader;

    // Texture for empty-texture
    Ref<Texture2D> whiteTexture;
    uint32_t whiteTextureSlot = 0;

    QuadVertex *quadBuffer = nullptr;
    QuadVertex *quadBufferPtr = nullptr;
    uint32_t quadIndexCount = 0;

    glm::vec4 quadVertexPosition[4]{};

    // TODO: Use asset handle to uniquely identify a asset
    // (slot -> textureID)
    std::array<Ref<Texture2D>, MaxTextures> textureSlots;
    uint32_t textureSlotAddIndex = 1;

    OrthographicCamera camera;

    Renderer2D::Stats renderStats;
};
static Scope<Renderer2DData> s_data;

size_t Renderer2D::MaxQuadCount = 100000;
size_t Renderer2D::MaxVertexCount = MaxQuadCount * 4;
size_t Renderer2D::MaxIndexCount = MaxQuadCount * 6;

void Renderer2D::Init()
{
    RL_PROFILE_FUNCTION();

    s_data = MakeScope<Renderer2DData>();
    s_data->quadVertexArray = VertexArray::Create();

    // vertex
    s_data->quadBuffer = new QuadVertex[MaxVertexCount];
    s_data->quadVertexBuffer = VertexBuffer::Create(MaxVertexCount * sizeof(QuadVertex));
    s_data->quadVertexBuffer->setLayout(BufferLayout{
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Float2, "a_TexCoord"},
        {ShaderDataType::Float, "a_TexIndex"},
        {ShaderDataType::Float, "a_Tiling"}
    });
    s_data->quadVertexArray->setVertexBuffer(s_data->quadVertexBuffer);

    // index
    uint32_t *quadIndices = new uint32_t[MaxIndexCount];
    uint32_t pattern[] = {0, 1, 2, 2, 3, 1};
    uint32_t offset = 0;
    for (int i = 0; i < MaxIndexCount; i += 6)
    {
        for (int j = 0; j < 6; j++)
            quadIndices[i + j] = pattern[j] + offset;
        offset += 4;
    }

    Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndices, MaxIndexCount);
    s_data->quadVertexArray->setIndexBuffer(squareIB);
    delete [] quadIndices;
    //
    s_data->whiteTexture = rl::Texture2D::Create(1, 1);
    s_data->whiteTexture->setPixel(0, 0, glm::vec4(1.0, 1.0, 1.0, 1.0));
    s_data->textureSlots[0] = s_data->whiteTexture;

    // texture shader
    s_data->textureShader = MakeRef<Shader>(texturedVS, texturedFS);
    s_data->textureShader->bind();
    //
    int sampler[MaxTextures];
    for(int i = 0; i < MaxTextures; i++)
        sampler[i] = i;
    s_data->textureShader->setIntArray("u_Textures", sampler, MaxTextures);

    // template for quad vertex
    s_data->quadVertexPosition[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
    s_data->quadVertexPosition[1] = { 0.5f, -0.5f, 0.0f, 1.0f};
    s_data->quadVertexPosition[2] = {-0.5f,  0.5f, 0.0f, 1.0f};
    s_data->quadVertexPosition[3] = { 0.5f,  0.5f, 0.0f, 1.0f};
}

void Renderer2D::Shutdown()
{
    RL_PROFILE_FUNCTION();

    s_data.reset(nullptr);
}

void Renderer2D::BeginScene(const OrthographicCamera &camera)
{
    RL_PROFILE_FUNCTION();

    s_data->camera = camera;

    s_data->textureShader->bind();
    s_data->textureShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

    // Reset
    s_data->quadBufferPtr = s_data->quadBuffer;
    s_data->quadIndexCount = 0;
}

void Renderer2D::EndScene()
{
    RL_PROFILE_FUNCTION();

    // Bind texture slots
    for(int i = 0; i < s_data->textureSlotAddIndex; i++)
        s_data->textureSlots[i]->bind(i);

    // flush
    {
        RL_PROFILE_SCOPE("Upload VB");
        size_t quadBufferCurrentSize = (s_data->quadBufferPtr - s_data->quadBuffer) * sizeof(QuadVertex);
        s_data->quadVertexBuffer->setData(s_data->quadBuffer, quadBufferCurrentSize);
    }

    // Draw
    {
        RL_PROFILE_SCOPE("Draw");
        s_data->quadVertexArray->bind();
        RenderCommand::DrawElement(s_data->quadVertexArray, s_data->quadIndexCount);
        s_data->quadVertexArray->unbind();

        s_data->renderStats.DrawCount++;
    }

    // Reset
    s_data->quadIndexCount = 0;
    s_data->textureSlotAddIndex = 1;
}

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
{
    DrawQuad({position.x, position.y, 0.0}, size, s_data->whiteTexture, color);
}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
{
    DrawQuad(position, size, s_data->whiteTexture, color);
}

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture)
{
    DrawQuad({position.x, position.y, 0.0}, size, texture, glm::vec4(1.f));
}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture)
{
    DrawQuad(position, size, texture, glm::vec4(1.f));
}

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref <Texture2D> &texture, const glm::vec4 &color)
{
    DrawQuad({position.x, position.y, 0.0}, size, texture, color);
}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const glm::vec4 &color)
{
    RL_PROFILE_FUNCTION();

    // Split draw call if it exceeds the limits
    if(s_data->quadIndexCount >= MaxQuadCount || s_data->textureSlotAddIndex >= MaxTextures-1)
    {
        EndScene();
        BeginScene(s_data->camera);
    }

    // Check if the texture is in slots
    float textureIndex = 0.f;
    for (int i = 1; i < s_data->textureSlotAddIndex; i++)
        if (*(s_data->textureSlots[i]) == *texture)
        {
            textureIndex = (float) i;
            break;
        }
    if (textureIndex == 0.f) // if not then add it
    {
        textureIndex = (float)s_data->textureSlotAddIndex;
        s_data->textureSlots[s_data->textureSlotAddIndex] = texture;
        s_data->textureSlotAddIndex++;
    }

    s_data->quadBufferPtr->position =glm::vec4{ position.x, position.y, position.z, 1.f };
    s_data->quadBufferPtr->color = color;
    s_data->quadBufferPtr->texCoord = { 0.0f, 0.0f };
    s_data->quadBufferPtr->texIndex = textureIndex;
    s_data->quadBufferPtr->texTiling = 1.f;
    s_data->quadBufferPtr++;

    s_data->quadBufferPtr->position = glm::vec4{ position.x + size.x, position.y, position.z, 1.f };
    s_data->quadBufferPtr->color = color;
    s_data->quadBufferPtr->texCoord = { 1.0f, 0.0f };
    s_data->quadBufferPtr->texIndex = textureIndex;
    s_data->quadBufferPtr->texTiling = 1.f;
    s_data->quadBufferPtr++;

    s_data->quadBufferPtr->position = glm::vec4{ position.x, position.y + size.y, position.z, 1.f };
    s_data->quadBufferPtr->color = color;
    s_data->quadBufferPtr->texCoord = { 0.0f, 1.0f };
    s_data->quadBufferPtr->texIndex = textureIndex;
    s_data->quadBufferPtr->texTiling = 1.f;
    s_data->quadBufferPtr++;

    s_data->quadBufferPtr->position = glm::vec4{ position.x + size.x, position.y + size.y, position.z, 1.f };
    s_data->quadBufferPtr->color = color;
    s_data->quadBufferPtr->texCoord = { 1.0f, 1.0f };
    s_data->quadBufferPtr->texIndex = textureIndex;
    s_data->quadBufferPtr->texTiling = 1.f;
    s_data->quadBufferPtr++;

    s_data->quadIndexCount += 6;
    s_data->renderStats.QuadCount++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, float rotate)
{
    DrawRotatedQuad({position.x, position.y, 0.0}, size, s_data->whiteTexture, color, rotate);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, float rotate)
{
    DrawRotatedQuad(position, size, s_data->whiteTexture, color, rotate);
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref <Texture2D> &texture,
                                 float rotate)
{
    DrawRotatedQuad({position.x, position.y, 0.0}, size, texture, glm::vec4(1.f), rotate);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref <Texture2D> &texture,
                                 float rotate)
{
    DrawRotatedQuad(position, size, texture, glm::vec4(1.f), rotate);
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref <Texture2D> &texture,
                                 const glm::vec4 &color, float rotate)
{
    DrawRotatedQuad({position.x, position.y, 0.f}, size, texture, color, rotate);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref <Texture2D> &texture,
                                 const glm::vec4 &color, float rotate)
{
    RL_PROFILE_FUNCTION();

    // Split draw call if it exceeds the limits
    if(s_data->quadIndexCount >= MaxQuadCount || s_data->textureSlotAddIndex >= MaxTextures-1)
    {
        EndScene();
        BeginScene(s_data->camera);
    }

    // Check if the texture is in slots
    float textureIndex = 0.f;
    for (int i = 1; i < s_data->textureSlotAddIndex; i++)
        if (*(s_data->textureSlots[i]) == *texture)
        {
            textureIndex = (float) i;
            break;
        }
    if (textureIndex == 0.f) // if not then add it
    {
        textureIndex = (float)s_data->textureSlotAddIndex;
        s_data->textureSlots[s_data->textureSlotAddIndex] = texture;
        s_data->textureSlotAddIndex++;
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.f), position) *
            glm::rotate(glm::mat4(1.f), glm::radians(rotate), glm::vec3(0.f, 0.f, 1.f)) *
            glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});

    s_data->quadBufferPtr->position = transform * s_data->quadVertexPosition[0];
    s_data->quadBufferPtr->color = color;
    s_data->quadBufferPtr->texCoord = { 0.0f, 0.0f };
    s_data->quadBufferPtr->texIndex = textureIndex;
    s_data->quadBufferPtr->texTiling = 1.f;
    s_data->quadBufferPtr++;

    s_data->quadBufferPtr->position = transform * s_data->quadVertexPosition[1];
    s_data->quadBufferPtr->color = color;
    s_data->quadBufferPtr->texCoord = { 1.0f, 0.0f };
    s_data->quadBufferPtr->texIndex = textureIndex;
    s_data->quadBufferPtr->texTiling = 1.f;
    s_data->quadBufferPtr++;

    s_data->quadBufferPtr->position = transform * s_data->quadVertexPosition[2];
    s_data->quadBufferPtr->color = color;
    s_data->quadBufferPtr->texCoord = { 0.0f, 1.0f };
    s_data->quadBufferPtr->texIndex = textureIndex;
    s_data->quadBufferPtr->texTiling = 1.f;
    s_data->quadBufferPtr++;

    s_data->quadBufferPtr->position = transform * s_data->quadVertexPosition[3];
    s_data->quadBufferPtr->color = color;
    s_data->quadBufferPtr->texCoord = { 1.0f, 1.0f };
    s_data->quadBufferPtr->texIndex = textureIndex;
    s_data->quadBufferPtr->texTiling = 1.f;
    s_data->quadBufferPtr++;

    s_data->quadIndexCount += 6;
    s_data->renderStats.QuadCount++;
}

Renderer2D::Stats &Renderer2D::GetStats()
{
    return s_data->renderStats;
}

void Renderer2D::ResetStats()
{
    s_data->renderStats = Renderer2D::Stats();
}

} // namespace of rl
