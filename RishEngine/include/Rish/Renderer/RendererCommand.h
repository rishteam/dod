#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>
#include <Rish/Renderer/VertexArray.h>

#include <glm/glm.hpp>

namespace rl {

class RL_API RenderCommand
{
public:
    static void SetClearColor(const glm::vec4 &color);

    static void Clear();

    static void DrawElement(const std::shared_ptr<VertexArray> &vertexArray);
};

}
