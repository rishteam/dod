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
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
}
)glsl";

const char *texturedFS = R"glsl(
#version 330 core

in vec2 v_TexCoord;

out vec4 color;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    color = texColor * u_Color;
}
)glsl";

struct Renderer2DStorage
{
    Ref<VertexArray> quadVertexArray;
    Ref<Shader> textureShader;
    Ref<Texture2D> whiteTexture;
};

static Scope<Renderer2DStorage> s_data;

void Renderer2D::Init()
{
    s_data = MakeScope<Renderer2DStorage>();
    s_data->quadVertexArray = VertexArray::Create();
    // vertex
    float square[] = {
        -0.5f, -0.5f, 0.0f, 0.f, 0.f, // bottom left
         0.5f, -0.5f, 0.0f, 1.f, 0.f, // bottom right
         0.5f,  0.5f, 0.0f, 1.f, 1.f, // top right
        -0.5f,  0.5f, 0.0f, 0.f, 1.f  // top left
    };
    Ref<VertexBuffer> squareVB = VertexBuffer::Create(square, sizeof(square));
    squareVB->setLayout(BufferLayout{
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float2, "a_TexCoord"}
    });
    s_data->quadVertexArray->setVertexBuffer(squareVB);
    // index
    uint32_t squareIndex[] = {0, 1, 2, 2, 3, 0};
    Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndex, 6);
    s_data->quadVertexArray->setIndexBuffer(squareIB);

    s_data->whiteTexture = rl::Texture2D::Create(1, 1);
    s_data->whiteTexture->setPixel(0, 0, glm::vec4(1.0, 1.0, 1.0, 1.0));

    // texture shader
    s_data->textureShader = MakeRef<Shader>(texturedVS, texturedFS);
    s_data->textureShader->bind();
    s_data->textureShader->setInt("u_Texture", 0);
}

void Renderer2D::Shutdown()
{
    s_data.reset(nullptr);
}

void Renderer2D::BeginScene(const OrthographicCamera &camera)
{
    s_data->textureShader->bind();
    s_data->textureShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
}

void Renderer2D::EndScene()
{
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

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,
                          const glm::vec4 &color)
{
    DrawQuad({position.x, position.y, 0.0}, size, texture, color);
}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,
                          const glm::vec4 &color)
{
    glm::mat4 transform = glm::translate(glm::mat4(1.f), position) *
                          glm::scale(glm::mat4(1.f), glm::vec3(size.x, size.y, 1.f));
    s_data->textureShader->bind();
    s_data->textureShader->setMat4("u_Transform", transform);
    s_data->textureShader->setFloat4("u_Color", color);
    s_data->textureShader->setInt("u_Texture", 0);
    texture->bind();

    s_data->quadVertexArray->bind();
    RenderCommand::DrawElement(s_data->quadVertexArray);
    s_data->quadVertexArray->unbind();

    texture->unbind();
}

} // namespace of rl
