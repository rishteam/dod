#pragma once

#include <Rish/rlpch.h>

#include <Rish/Renderer/Camera/OrthographicCameraController.h>

namespace rl {

class RL_API EditorGrid
{
public:
    void onUpdate(const OrthographicCameraController &cameraController);

private:
    void drawLines(int offset, const glm::vec4 &color);

    OrthographicCameraBounds m_cameraBound{0.f, 0.f, 0.f,0.f};
};

} // end of namespace rl
