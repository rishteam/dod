#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>
#include <Rish/Renderer/RendererCommand.h>

namespace rl {

class RL_API Renderer
{
public:
    static void BeginScene();
    static void EndScene();

    static void Submit(const std::shared_ptr<VertexArray> &vertexArray);
};

} // end of namespace rl