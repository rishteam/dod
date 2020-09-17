#include <Rish/rlpch.h>

#include <Rish/Core/Application.h>

#include <Rish/Renderer/RendererCommand.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Renderer/VertexArray.h>
#include <Rish/Renderer/Shader.h>

#include <glm/gtc/matrix_transform.hpp>

namespace rl {

////////////////////////////////////////////////////////////////
// Quad Renderer
////////////////////////////////////////////////////////////////

struct QuadVertex
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texCoord;
    float texIndex;
    float texTiling;
};

const size_t MaxTextures = 32;
const size_t MaxQuadCount = 100000;
const size_t MaxQuadVertexCount = MaxQuadCount * 4;
const size_t MaxQuadIndexCount = MaxQuadCount * 6;

////////////////////////////////////////////////////////////////
// Line Renderer
////////////////////////////////////////////////////////////////

struct LineVertex
{
    glm::vec3 position;
    glm::vec4 color;
};

const size_t MaxLineCount = 1000;
const size_t MaxLineVertexCount = MaxLineCount * 2;
const size_t MaxLineIndexCount = MaxLineCount * 6;

////////////////////////////////////////////////////////////////

struct Renderer2DData
{
    ~Renderer2DData()
    {
        delete [] quadBuffer;
        delete [] lineBuffer;
    }
    ////////////////////////////////////////////////////////////////
    // Quad Renderer
    ////////////////////////////////////////////////////////////////
    Ref<VertexArray> quadVertexArray;
    Ref<Shader> quadTexShader;

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

    ////////////////////////////////////////////////////////////////
    // Line Renderer
    ////////////////////////////////////////////////////////////////
    Ref<VertexArray> lineVertexArray;
    Ref<Shader> lineShader;

    LineVertex *lineBuffer = nullptr;
    LineVertex *lineBufferPtr = nullptr;
    uint32_t lineIndexCount = 0;

    ////////////////////////////////////////////////////////////////
    // Common
    ////////////////////////////////////////////////////////////////
    bool sceneState = false; ///< Is the scene now active (called BeginScene())
    bool depthTest = false;  ///< Is the scene enable depth test
    Ref<Framebuffer> targetFramebuffer;
    OrthographicCamera camera;
    Renderer2D::Stats renderStats;
};
static Scope<Renderer2DData> s_data;

void Renderer2D::Init()
{
    RL_PROFILE_FUNCTION();

    s_data = MakeScope<Renderer2DData>();

    ////////////////////////////////////////////////////////////////
    // Quad Renderer
    ////////////////////////////////////////////////////////////////
    {
        s_data->quadVertexArray = VertexArray::Create();
        // vertex
        s_data->quadBuffer = new QuadVertex[MaxQuadVertexCount];
        Ref <VertexBuffer> quadVertexBuffer = VertexBuffer::Create(MaxQuadVertexCount * sizeof(QuadVertex));
        quadVertexBuffer->setLayout(BufferLayout{
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float4, "a_Color"},
                {ShaderDataType::Float2, "a_TexCoord"},
                {ShaderDataType::Float,  "a_TexIndex"},
                {ShaderDataType::Float,  "a_Tiling"}
        });
        s_data->quadVertexArray->setVertexBuffer(quadVertexBuffer);
        // index
        uint32_t *quadIndices = new uint32_t[MaxQuadIndexCount];
        uint32_t pattern[] = {0, 1, 2, 2, 3, 1};
        uint32_t offset = 0;
        for (int i = 0; i < MaxQuadIndexCount; i += 6)
        {
            for (int j = 0; j < 6; j++)
                quadIndices[i + j] = pattern[j] + offset;
            offset += 4;
        }

        Ref <IndexBuffer> squareIB = IndexBuffer::Create(quadIndices, MaxQuadIndexCount);
        s_data->quadVertexArray->setIndexBuffer(squareIB);
        delete[] quadIndices;
        //
        s_data->whiteTexture = rl::Texture2D::Create(1, 1);
        s_data->whiteTexture->setPixel(0, 0, glm::vec4(1.0, 1.0, 1.0, 1.0));
        s_data->textureSlots[0] = s_data->whiteTexture;
        // texture shader
        s_data->quadTexShader = Shader::LoadShaderVFS("/shader/quadTexture.vs", "/shader/quadTexture.fs");
        int sampler[MaxTextures];
        for (int i = 0; i < MaxTextures; i++)
            sampler[i] = i;
        s_data->quadTexShader->bind();
        s_data->quadTexShader->setIntArray("u_Textures", sampler, MaxTextures);

        // template for quad vertex
        s_data->quadVertexPosition[0] = {-0.5f, -0.5f, 0.0f, 1.0f}; // bottom left
        s_data->quadVertexPosition[1] = {0.5f, -0.5f, 0.0f, 1.0f};  // bottom right
        s_data->quadVertexPosition[2] = {-0.5f, 0.5f, 0.0f, 1.0f};  // top left
        s_data->quadVertexPosition[3] = {0.5f, 0.5f, 0.0f, 1.0f};   // top right
    }
    ////////////////////////////////////////////////////////////////
    // Line Renderer
    ////////////////////////////////////////////////////////////////
    {
        s_data->lineVertexArray = VertexArray::Create();
        s_data->lineBuffer = new LineVertex[MaxLineVertexCount];
        // vertex
        Ref<VertexBuffer> lineVB = VertexBuffer::Create(sizeof(LineVertex) * MaxLineVertexCount);
        lineVB->setLayout(BufferLayout{
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"}
        });
        s_data->lineVertexArray->setVertexBuffer(lineVB);
        // index
        uint32_t *lineIndices = new uint32_t[MaxLineIndexCount];
        for(int i = 0; i < MaxLineIndexCount; i++)
            lineIndices[i] = i;
        Ref<IndexBuffer> lineIB = IndexBuffer::Create(lineIndices, MaxLineIndexCount);
        s_data->lineVertexArray->setIndexBuffer(lineIB);
        delete [] lineIndices;
        // shader
        s_data->lineShader = Shader::LoadShaderVFS("/shader/line.vs", "/shader/line.fs");
    }
}

void Renderer2D::Shutdown()
{
    RL_PROFILE_FUNCTION();

    s_data.reset(nullptr);
}

void Renderer2D::BeginScene(const OrthographicCamera &camera, const Ref<Framebuffer>& framebuffer, bool depthTest)
{
    RL_PROFILE_RENDERER_FUNCTION();

    s_data->depthTest = depthTest;

    s_data->camera = camera;
    s_data->sceneState = true;

    if(framebuffer)
        s_data->targetFramebuffer = framebuffer;

    // Quad
    {
        s_data->quadBufferPtr = s_data->quadBuffer;
        s_data->quadIndexCount = 0;
    }

    // Line
    {
        s_data->lineBufferPtr = s_data->lineBuffer;
        s_data->lineIndexCount = 0;
    }

    // Bind the framebuffer
    if(s_data->targetFramebuffer)
        s_data->targetFramebuffer->bind();
}

void Renderer2D::EndScene()
{
    RL_PROFILE_RENDERER_FUNCTION();

    // Line
    if(s_data->lineIndexCount)
    {
        // send accumulated VB
        size_t lineBufferCurrentSize = (s_data->lineBufferPtr - s_data->lineBuffer) * sizeof(LineVertex);
        s_data->lineVertexArray->getVertexBuffer()->setData(s_data->lineBuffer, lineBufferCurrentSize);
        // Shader
        s_data->lineShader->bind();
        s_data->lineShader->setMat4("u_ViewProjection", s_data->camera.getViewProjectionMatrix());
        // Draw
        s_data->lineVertexArray->bind();
        RenderCommand::SetLineThickness(1.f);
        RenderCommand::DrawElement(DrawLines, s_data->lineVertexArray, s_data->lineIndexCount, s_data->depthTest);
        s_data->lineVertexArray->unbind();
        // Reset
        s_data->lineIndexCount = 0;

        s_data->renderStats.DrawCount++;
    }

    // Quad
    if(s_data->quadIndexCount)
    {
        // send accumulated VB
        size_t quadBufferCurrentSize = (s_data->quadBufferPtr - s_data->quadBuffer) * sizeof(QuadVertex);
        s_data->quadVertexArray->getVertexBuffer()->setData(s_data->quadBuffer, quadBufferCurrentSize);

        // Shader
        s_data->quadTexShader->bind();
        s_data->quadTexShader->setMat4("u_ViewProjection", s_data->camera.getViewProjectionMatrix());

        // Bind texture slots
        for(int i = 0; i < s_data->textureSlotAddIndex; i++)
            s_data->textureSlots[i]->bind(i);

        // Draw
        s_data->quadVertexArray->bind();
        RenderCommand::DrawElement(DrawTriangles, s_data->quadVertexArray, s_data->quadIndexCount, s_data->depthTest);
        s_data->quadVertexArray->unbind();

        // Reset
        s_data->quadIndexCount = 0;
        s_data->textureSlotAddIndex = 1;

        s_data->renderStats.DrawCount++;
    }

    // Unbind the framebuffer
    if(s_data->targetFramebuffer)
        s_data->targetFramebuffer->unbind();

    s_data->sceneState = false;
}

Renderer2D::Stats &Renderer2D::GetStats()
{
    return s_data->renderStats;
}

void Renderer2D::ResetStats()
{
    s_data->renderStats = Renderer2D::Stats();
}

////////////////////////////////////////////////////////////////
// Quad Renderer
////////////////////////////////////////////////////////////////

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
    RL_PROFILE_RENDERER_FUNCTION();

    RL_CORE_ASSERT(s_data->sceneState, "Did you forget to call BeginScene()?");

    FlushStatesIfExceeds();

    // Check if the texture is in slots
    float textureIndex = GetQuadTextureIndex(texture);

    glm::vec2 siz = size / 2.f;
    glm::vec4 posi[4] = {
        {position.x - siz.x, position.y - siz.y, position.z, 1.f}, // bottom left
        {position.x + siz.x, position.y - siz.y, position.z, 1.f}, // bottom right
        {position.x - siz.x, position.y + siz.y, position.z, 1.f}, // top left
        {position.x + siz.x, position.y + siz.y, position.z, 1.f}  // top right
    };
    glm::vec2 texCoords[4] = {
        { 0.0f, 0.0f },  // bottom left
        { 1.0f, 0.0f },  // bottom right
        { 0.0f, 1.0f },  // top left
        { 1.0f, 1.0f }   // top right
    };

    SubmitQuad(posi, color, texCoords, textureIndex, 1.f);
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
    RL_PROFILE_RENDERER_FUNCTION();

    RL_CORE_ASSERT(s_data->sceneState, "Did you forget to call BeginScene()?");

    FlushStatesIfExceeds();

    // Check if the texture is in slots
    float textureIndex = GetQuadTextureIndex(texture);

    glm::mat4 transform = glm::translate(glm::mat4(1.f), position) *
            glm::rotate(glm::mat4(1.f), glm::radians(rotate), glm::vec3(0.f, 0.f, 1.f)) *
            glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});
    glm::vec4 posi[4];
    for(int i = 0; i < 4; i++)
        posi[i] = transform * s_data->quadVertexPosition[i];

    glm::vec2 texCoords[4] = {
        { 0.0f, 0.0f }, // bottom left
        { 1.0f, 0.0f }, // bottom right
        { 0.0f, 1.0f }, // top left
        { 1.0f, 1.0f }  // top right
    };

    SubmitQuad(posi, color, texCoords, textureIndex, 1.f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SubTexture2D
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref <SubTexture2D> &subtexture,
                          const glm::vec4 &color)
{
    DrawQuad(glm::vec3{position.x, position.y, 0.f}, size, subtexture, color);
}

// TODO: refactor this (copy from DrawQuad(Texture2D)
void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref <SubTexture2D> &subtexture,
                          const glm::vec4 &color)
{
    RL_PROFILE_RENDERER_FUNCTION();

    RL_CORE_ASSERT(s_data->sceneState, "Did you forget to call BeginScene()?");

    FlushStatesIfExceeds();

    const Ref<Texture2D> texture = subtexture->getTexture();

    // Check if the texture is in slots
    float textureIndex = GetQuadTextureIndex(texture);

    glm::vec2 siz = size / 2.f;
    glm::vec4 posi[4] = {
        {position.x - siz.x, position.y - siz.y, position.z, 1.f}, // bottom left
        {position.x + siz.x, position.y - siz.y, position.z, 1.f}, // bottom right
        {position.x - siz.x, position.y + siz.y, position.z, 1.f}, // top left
        {position.x + siz.x, position.y + siz.y, position.z, 1.f}  // top right
    };
    const glm::vec2 *texCoords = subtexture->getTextureCoords();

    SubmitQuad(posi, color, texCoords, textureIndex, 1.f);
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref <SubTexture2D> &subtexture,
                                 const glm::vec4 &color, float rotate)
{
    DrawRotatedQuad(glm::vec3{position.x, position.y, 0.f}, size, subtexture, color, rotate);
}

// TODO: refactor this (copy from DrawRotatedQuad(Texture2D)
void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref <SubTexture2D> &subtexture,
                                 const glm::vec4 &color, float rotate)
{
    RL_PROFILE_RENDERER_FUNCTION();

    RL_CORE_ASSERT(s_data->sceneState, "Did you forget to call BeginScene()?");

    FlushStatesIfExceeds();

    const Ref<Texture2D> texture = subtexture->getTexture();
    const glm::vec2 *texCoords = subtexture->getTextureCoords();

    // Check if the texture is in slots
    float textureIndex = GetQuadTextureIndex(texture);

    // prepare position
    glm::mat4 transform = glm::translate(glm::mat4(1.f), position) *
                          glm::rotate(glm::mat4(1.f), glm::radians(rotate), glm::vec3(0.f, 0.f, 1.f)) *
                          glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});
    glm::vec4 posi[4];
    for(int i = 0; i < 4; i++)
        posi[i] = transform * s_data->quadVertexPosition[i];

    SubmitQuad(posi, color, texCoords, textureIndex, 1.f);
}

float Renderer2D::GetQuadTextureIndex(const Ref<Texture2D>& texture)
{
    float textureIndex = 0.f;
    // Search texture
    for (int i = 1; i < s_data->textureSlotAddIndex; i++)
        if (*(s_data->textureSlots[i]) == *texture)
        {
            textureIndex = (float)i;
            break;
        }
    // if not found then add it to the slot
    if (textureIndex == 0.f)
    {
        textureIndex = (float)s_data->textureSlotAddIndex;
        s_data->textureSlots[s_data->textureSlotAddIndex] = texture;
        s_data->textureSlotAddIndex++;
    }
    return textureIndex;
}

void Renderer2D::FlushStatesIfExceeds()
{
    // Split draw call if it exceeds the limits
    if(s_data->quadIndexCount >= MaxQuadCount || s_data->textureSlotAddIndex >= MaxTextures-1)
    {
        EndScene();
        BeginScene(s_data->camera, nullptr);
    }
}

void Renderer2D::SubmitQuad(const glm::vec4 *position, const glm::vec4 &color,
                            const glm::vec2 *texCoords, float texIndex,
                            float texTiling)
{
    // Add vertices of a quad to buffer
    for(int i = 0; i < 4; i++)
    {
        s_data->quadBufferPtr->position = position[i];
        s_data->quadBufferPtr->color = color;
        s_data->quadBufferPtr->texCoord = texCoords[i];
        s_data->quadBufferPtr->texIndex = texIndex;
        s_data->quadBufferPtr->texTiling = texTiling;
        s_data->quadBufferPtr++;
    }
    //
    s_data->quadIndexCount += 6;
    s_data->renderStats.QuadCount++;
}

////////////////////////////////////////////////////////////////
// Line Renderer
////////////////////////////////////////////////////////////////

void Renderer2D::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
{
    RL_PROFILE_RENDERER_FUNCTION();

    RL_CORE_ASSERT(s_data->sceneState, "Did you forget to call BeginScene()?");

    if(s_data->lineIndexCount >= MaxLineCount)
    {
        EndScene();
        BeginScene(s_data->camera, nullptr);
    }

    s_data->lineBufferPtr->position = p0;
    s_data->lineBufferPtr->color = color;
    s_data->lineBufferPtr++;
    s_data->lineBufferPtr->position = p1;
    s_data->lineBufferPtr->color = color;
    s_data->lineBufferPtr++;

    s_data->lineIndexCount += 2;
    s_data->renderStats.LineCount++;
}

void Renderer2D::DrawLine(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec4 &color)
{
    DrawLine(glm::vec3(p0, 0.f), glm::vec3(p1, 0.f), color);
}

} // namespace of rl
