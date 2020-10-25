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

struct QuadShape
{
    QuadVertex p[4];

    friend bool operator<(const QuadShape &lhs, const QuadShape &rhs)
    {
        return lhs.p[0].position.z < rhs.p[0].position.z;
    }
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

struct LineShape
{
    LineVertex p[2];

    friend bool operator<(const LineShape &lhs, const LineShape &rhs)
    {
        return lhs.p[0].position.z < rhs.p[0].position.z;
    }
};

const size_t MaxLineCount = 500000;
const size_t MaxLineVertexCount = MaxLineCount * 2;
const size_t MaxLineIndexCount = MaxLineCount * 6;

////////////////////////////////////////////////////////////////

struct Renderer2DData
{
    ~Renderer2DData()
    {
    }

    ////////////////////////////////////////////////////////////////
    // Texture
    ////////////////////////////////////////////////////////////////
    // Texture for empty-texture
    Ref<Texture2D> whiteTexture;
    uint32_t whiteTextureSlot = 0;

    // TODO: Use asset handle to uniquely identify a asset
    // (slot -> textureID)
    std::array<Ref<Texture2D>, MaxTextures> textureSlots;
    uint32_t textureSlotAddIndex = 1;

    ////////////////////////////////////////////////////////////////
    // Quad Renderer
    ////////////////////////////////////////////////////////////////
    /**
     * @brief Quad Renderer
     * @details This should only be construct ONLY after Renderer2D is ready
     */
    struct QuadRenderer
    {
        void init()
        {
            quadVertexArray = VertexArray::Create();

            // Initialize Vertex Buffer
            Ref <VertexBuffer> quadVertexBuffer = VertexBuffer::Create(MaxQuadVertexCount * sizeof(QuadVertex));
            quadVertexBuffer->setLayout(BufferLayout{
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float4, "a_Color"},
                {ShaderDataType::Float2, "a_TexCoord"},
                {ShaderDataType::Float,  "a_TexIndex"},
                {ShaderDataType::Float,  "a_Tiling"}
            });
            quadVertexArray->setVertexBuffer(quadVertexBuffer);

            quadShapeList.reserve(MaxQuadCount);

            // Initialize Index Buffer
            auto *quadIndices = new uint32_t[MaxQuadIndexCount];
            uint32_t pattern[] = {0, 1, 2, 2, 3, 1};
            uint32_t offset = 0;
            for (int i = 0; i < MaxQuadIndexCount; i += 6)
            {
                for (int j = 0; j < 6; j++)
                    quadIndices[i + j] = pattern[j] + offset;
                offset += 4;
            }
            // Create the Index Buffer
            Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndices, MaxQuadIndexCount);
            quadVertexArray->setIndexBuffer(squareIB);
            delete[] quadIndices; // deltete cpu side

            // texture shader
            quadTexShader = Shader::LoadShaderVFS("/shader/quadTexture.vs", "/shader/quadTexture.fs");
            int sampler[MaxTextures];
            for (int i = 0; i < MaxTextures; i++)
                sampler[i] = i;
            quadTexShader->bind();
            quadTexShader->setIntArray("u_Textures", sampler, MaxTextures);
        }

        void reset()
        {
            quadIndexCount = 0;
            quadShapeList.clear();
        }

        void submit(const glm::vec4 position[4], const glm::vec4 &color, const glm::vec2 texCoords[4],
                    float texIndex, float texTiling)
        {
            QuadShape submitQuad{};
            // Add vertices of a quad to buffer
            for(int i = 0; i < 4; i++)
            {
                QuadVertex tmp{};
                tmp.position    = position[i];
                tmp.color       = color;
                tmp.texCoord    = texCoords[i];
                tmp.texIndex    = texIndex;
                tmp.texTiling   = texTiling;
                submitQuad.p[i] = tmp;
            }
            //
            quadIndexCount += 6;
            //
            quadShapeList.push_back(submitQuad);
        }

        void draw(bool DepthTest, bool IsEditorCamera,
                  const OrthographicCamera &OrthoCamera, const glm::mat4 &ViewProjMatrix)
        {
            // Send accumulated VB
            auto &quadList = quadShapeList;
            // Sort by z-axis
            if(DepthTest)
                sort(quadList.begin(), quadList.end());
            // set the VB
            quadVertexArray->getVertexBuffer()->setData(&quadList[0].p[0], quadList.size() * sizeof(QuadShape));

            // Shader
            quadTexShader->bind();
            quadTexShader->setMat4("u_ViewProjection",
               IsEditorCamera ?
                   OrthoCamera.getViewProjectionMatrix() :
                   ViewProjMatrix);

            // Draw
            quadVertexArray->bind();
            RenderCommand::DrawElement(DrawTriangles, quadVertexArray, quadIndexCount, DepthTest);
            quadVertexArray->unbind();
        }

        operator bool() const
        {
            return quadIndexCount > 0;
        }

        bool exceedDrawIndex() const
        {
            return quadIndexCount >= MaxQuadIndexCount;
        }

        Ref<VertexArray> quadVertexArray;     ///< Line renderer vertex array
        Ref<Shader> quadTexShader;            ///< Line renderer shader
        uint32_t quadIndexCount = 0;          ///< 現在有多少個 index
        std::vector<QuadShape> quadShapeList; ///< Quad data
    };
    QuadRenderer quadRenderer;

    glm::vec4 quadVertexPosition[4]{};

    ////////////////////////////////////////////////////////////////
    // Line Renderer
    ////////////////////////////////////////////////////////////////
    /**
     * @brief Line Renderer
     * @details This should only be construct ONLY after Renderer2D is ready
     */
    struct LineRenderer
    {
        void init()
        {
            lineVertexArray = VertexArray::Create();

            // Initialize Vertex Buffer
            Ref<VertexBuffer> lineVB = VertexBuffer::Create(sizeof(LineVertex) * MaxLineVertexCount);
            lineVB->setLayout(BufferLayout{
                    {ShaderDataType::Float3, "a_Position"},
                    {ShaderDataType::Float4, "a_Color"}
            });
            lineVertexArray->setVertexBuffer(lineVB);
            // Reserve size for line data list
            lineShapeList.reserve(MaxLineVertexCount);

            // Initialize Index Buffer
            uint32_t *lineIndices = new uint32_t[MaxLineIndexCount];
            for(int i = 0; i < MaxLineIndexCount; i++)
                lineIndices[i] = i;
            // Set IB
            Ref<IndexBuffer> lineIB = IndexBuffer::Create(lineIndices, MaxLineIndexCount);
            lineVertexArray->setIndexBuffer(lineIB);
            delete [] lineIndices;

            // shader
            lineShader = Shader::LoadShaderVFS("/shader/line.vs", "/shader/line.fs");
        }

        void reset()
        {
            lineIndexCount = 0;
            lineShapeList.clear();
        }

        void submit(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
        {
            LineShape submitLine{};
            // p0
            submitLine.p[0].position = p0;
            submitLine.p[0].color = color;
            // p1
            submitLine.p[1].position = p1;
            submitLine.p[1].color = color;

            lineShapeList.push_back(submitLine);
            lineIndexCount += 2;
        }

        void draw(bool DepthTest, bool IsEditorCamera,
                  const OrthographicCamera &OrthoCamera, const glm::mat4 &ViewProjMatrix)
        {
            // send accumulated VB
            auto &lineList = lineShapeList;
            // Sort by z-axis
            if(DepthTest)
                sort(lineList.begin(), lineList.end());
            // set the VB
            lineVertexArray->getVertexBuffer()->setData(&lineList[0].p[0], lineList.size() * sizeof(LineShape));

            // Shader
            lineShader->bind();
            lineShader->setMat4("u_ViewProjection",
                IsEditorCamera ?
                    OrthoCamera.getViewProjectionMatrix() :
                    ViewProjMatrix);

            // Draw
            lineVertexArray->bind();
            RenderCommand::SetLineThickness(1.f);
            RenderCommand::DrawElement(DrawLines, lineVertexArray, lineIndexCount, DepthTest);
            lineVertexArray->unbind();
        }

        bool exceedDrawIndex() const
        {
            return lineIndexCount >= MaxLineCount;
        }

        operator bool() const
        {
            return lineIndexCount > 0;
        }
        //
        Ref<VertexArray> lineVertexArray;     ///< Line renderer vertex array
        Ref<Shader> lineShader;               ///< Line renderer shader
        std::vector<LineShape> lineShapeList; ///< Line data
        uint32_t lineIndexCount = 0;          ///< Index count
    };
    LineRenderer bgLineRenderer; ///< Background Line Renderer
    LineRenderer fgLineRenderer; ///< Foreground Line Renderer

    ////////////////////////////////////////////////////////////////
    // Common
    ////////////////////////////////////////////////////////////////
    bool sceneState = false; ///< Is the scene now active (called BeginScene())
    bool depthTest = false;  ///< Is the scene enable depth test
    //
    OrthographicCamera orthoCamera;
    bool isEditorCamera = false; // TODO: this is dirty
    Camera camera = glm::mat4(1.f);
    glm::mat4 m_viewProjMatrix;
    //
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
        // Init quad renderer
        s_data->quadRenderer.init();

        // Create default white texture
        s_data->whiteTexture = rl::Texture2D::Create(1, 1);
        s_data->whiteTexture->setPixel(0, 0, glm::vec4(1.0, 1.0, 1.0, 1.0));
        s_data->textureSlots[0] = s_data->whiteTexture;

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
        s_data->fgLineRenderer.init();
        s_data->bgLineRenderer.init();
    }
}

void Renderer2D::Shutdown()
{
    RL_PROFILE_FUNCTION();

    s_data.reset(nullptr);
}

void Renderer2D::BeginScene(const OrthographicCamera &camera, bool depthTest)
{
    RL_PROFILE_RENDERER_FUNCTION();

    s_data->depthTest = depthTest;

    s_data->isEditorCamera = true;
    s_data->orthoCamera = camera;
    //
    s_data->sceneState = true;

    // Quad
    {
        s_data->quadRenderer.reset();
    }

    // Line
    {
        s_data->fgLineRenderer.reset();
        s_data->bgLineRenderer.reset();
    }
}

void Renderer2D::BeginScene(const Camera &camera, const glm::mat4 &transform,
                            bool depthTest)
{
    RL_PROFILE_RENDERER_FUNCTION();

    s_data->depthTest = depthTest;

    s_data->isEditorCamera = false;
    s_data->camera = camera;
    s_data->m_viewProjMatrix = camera.getProjection() * glm::inverse(transform); // TODO: not support rotate now
    //
    s_data->sceneState = true;

    // Quad
    {
        s_data->quadRenderer.reset();
    }

    // Line
    {
        s_data->fgLineRenderer.reset();
        s_data->bgLineRenderer.reset();
    }
}

void Renderer2D::EndScene()
{
    RL_PROFILE_RENDERER_FUNCTION();

    // Background Line
    auto &bgLine = s_data->bgLineRenderer;
    if(bgLine)
    {
        bgLine.draw(s_data->depthTest, s_data->isEditorCamera, s_data->orthoCamera, s_data->m_viewProjMatrix);
        s_data->renderStats.DrawCount++;
    }

    // Quad
    if(s_data->quadRenderer)
    {
        // Bind texture slots
        for(int i = 0; i < s_data->textureSlotAddIndex; i++)
            s_data->textureSlots[i]->bind(i);

        s_data->quadRenderer.draw(s_data->depthTest, s_data->isEditorCamera, s_data->orthoCamera, s_data->m_viewProjMatrix);

        // Reset
        s_data->quadRenderer.reset();
        s_data->textureSlotAddIndex = 1;
        //
        s_data->renderStats.DrawCount++;
    }

    // Foreground Line
    auto &fgLine = s_data->fgLineRenderer;
    if(fgLine)
    {
        fgLine.draw(s_data->depthTest, s_data->isEditorCamera, s_data->orthoCamera, s_data->m_viewProjMatrix);
        s_data->renderStats.DrawCount++;
    }

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
    if(s_data->quadRenderer.exceedDrawIndex() || s_data->textureSlotAddIndex >= MaxTextures-1)
    {
        EndScene();
        BeginScene(s_data->orthoCamera, s_data->depthTest);
    }
}

void Renderer2D::SubmitQuad(const glm::vec4 *position, const glm::vec4 &color,
                            const glm::vec2 *texCoords, float texIndex,
                            float texTiling)
{
    s_data->quadRenderer.submit(position, color, texCoords, texIndex, texTiling);
    s_data->renderStats.QuadCount++;
}

////////////////////////////////////////////////////////////////
// Line Renderer
////////////////////////////////////////////////////////////////

void Renderer2D::DrawBgLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
{
    RL_PROFILE_RENDERER_FUNCTION();

    RL_CORE_ASSERT(s_data->sceneState, "Did you forget to call BeginScene()?");

    if(s_data->bgLineRenderer.exceedDrawIndex())
    {
        EndScene();
        BeginScene(s_data->orthoCamera, s_data->depthTest);
    }

    s_data->bgLineRenderer.submit(p0, p1, color);
    s_data->renderStats.LineCount++;
}

void Renderer2D::DrawBgLine(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec4 &color)
{
    DrawBgLine(glm::vec3(p0, 0.f), glm::vec3(p1, 0.f), color);
}

void Renderer2D::DrawFgLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
{
    RL_PROFILE_RENDERER_FUNCTION();

    RL_CORE_ASSERT(s_data->sceneState, "Did you forget to call BeginScene()?");

    if(s_data->fgLineRenderer.exceedDrawIndex())
    {
        EndScene();
        BeginScene(s_data->orthoCamera, s_data->depthTest);
    }

    s_data->fgLineRenderer.submit(p0, p1, color);
    s_data->renderStats.LineCount++;
}

void Renderer2D::DrawFgLine(const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec4 &color)
{
    DrawFgLine(glm::vec3(p0, 0.f), glm::vec3(p1, 0.f), color);
}

void Renderer2D::DrawRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
{
    glm::vec2 t = position;
    glm::vec2 s = size / 2.f;
    glm::vec2 p[4] = {
        {t.x - s.x, t.y - s.y},
        {t.x + s.x, t.y - s.y},
        {t.x + s.x, t.y + s.y},
        {t.x - s.x, t.y + s.y}
    };
    for(int i = 0; i < 4; i++)
        Renderer2D::DrawFgLine(p[i], p[(i+1)%4], color);
}

void Renderer2D::DrawRotatedRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, float rotate)
{
    glm::vec2 p[4] = {
        {-0.5f, -0.5f},
        {+0.5f, -0.5f},
        {+0.5f, +0.5f},
        {-0.5f, +0.5f}
    };

    glm::mat4 trans = glm::translate(glm::mat4(1.f), glm::vec3(position, 0.f)) *
                      glm::rotate(glm::mat4(1.f), glm::radians(rotate), glm::vec3(0.f, 0.f, 1.f)) *
                      glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});

    for(int i = 0; i < 4; i++)
        p[i] = trans * glm::vec4(p[i], 0.f, 1.f);

    for(int i = 0; i < 4; i++)
        Renderer2D::DrawFgLine(p[i], p[(i+1)%4], color);
}

void Renderer2D::DrawCircleLine(const glm::vec2 &position, const float radius, const glm::vec4 &color)
{
    const int pointCount = 30;
    glm::vec2 p[pointCount];
    float d = 0.f;
    for(auto & i : p)
    {
        i = position + glm::vec2{radius * std::sin(glm::radians(d)), radius * std::cos(glm::radians(d))};
        d += 360.f / pointCount;
    }
    for(int i = 0; i < pointCount; i++)
        DrawFgLine(p[i], p[(i+1)%pointCount], color);
}

void Renderer2D::DrawFgRect(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
{
    glm::vec2 t = position;
    glm::vec2 s = size / 2.f;
    glm::vec2 p[4] = {
        {t.x - s.x, t.y - s.y},
        {t.x + s.x, t.y - s.y},
        {t.x + s.x, t.y + s.y},
        {t.x - s.x, t.y + s.y}
    };
    for(int i = 0; i < 4; i++)
        Renderer2D::DrawFgLine(p[i], p[(i+1)%4], color);
    //
    s_data->renderStats.RectCount++;
}

void Renderer2D::DrawFgRotatedRect(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, float rotate)
{
    glm::vec2 p[4] = {
        {-0.5f, -0.5f},
        {+0.5f, -0.5f},
        {+0.5f, +0.5f},
        {-0.5f, +0.5f}
    };

    glm::mat4 trans = glm::translate(glm::mat4(1.f), position) *
                      glm::rotate(glm::mat4(1.f), glm::radians(rotate), glm::vec3(0.f, 0.f, 1.f)) *
                      glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});

    for(int i = 0; i < 4; i++)
        p[i] = trans * glm::vec4(p[i], 0.f, 1.f);

    for(int i = 0; i < 4; i++)
        Renderer2D::DrawFgLine(p[i], p[(i+1)%4], color);
    //
    s_data->renderStats.RectCount++;
}

void Renderer2D::DrawBgRect(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
{
    glm::vec2 t = position;
    glm::vec2 s = size / 2.f;
    glm::vec2 p[4] = {
        {t.x - s.x, t.y - s.y},
        {t.x + s.x, t.y - s.y},
        {t.x + s.x, t.y + s.y},
        {t.x - s.x, t.y + s.y}
    };
    for(int i = 0; i < 4; i++)
        Renderer2D::DrawBgLine(p[i], p[(i+1)%4], color);
    //
    s_data->renderStats.RectCount++;
}

void Renderer2D::DrawBgRotatedRect(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, float rotate)
{
    glm::vec2 p[4] = {
        {-0.5f, -0.5f},
        {+0.5f, -0.5f},
        {+0.5f, +0.5f},
        {-0.5f, +0.5f}
    };

    glm::mat4 trans = glm::translate(glm::mat4(1.f), position) *
                      glm::rotate(glm::mat4(1.f), glm::radians(rotate), glm::vec3(0.f, 0.f, 1.f)) *
                      glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});

    for(int i = 0; i < 4; i++)
        p[i] = trans * glm::vec4(p[i], 0.f, 1.f);

    for(int i = 0; i < 4; i++)
        Renderer2D::DrawBgLine(p[i], p[(i+1)%4], color);
    //
    s_data->renderStats.RectCount++;
}

void Renderer2D::DrawFgRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
{
    DrawFgRect(glm::vec3(position, 0.f), size, color);
}

void Renderer2D::DrawFgRotatedRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, float rotate)
{
    DrawFgRotatedRect(glm::vec3(position, 0.f), size, color, rotate);
}

void Renderer2D::DrawBgRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
{
    DrawBgRect(glm::vec3(position, 0.f), size, color);
}

void Renderer2D::DrawBgRotatedRect(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, float rotate)
{
    DrawBgRotatedRect(glm::vec3(position, 0.f), size, color, rotate);
}

} // namespace of rl
