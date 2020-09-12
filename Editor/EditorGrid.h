#pragma once

#include <Rish/rlpch.h>

#include <Rish/Renderer/Camera/OrthographicCameraController.h>

namespace rl {

class RL_API EditorGrid
{
public:
    void onUpdate(const OrthographicCameraController &cameraController);

private:
    void drawLines(float offset, const glm::vec4 &color);

    OrthographicCameraBounds m_currentBound{0.f, 0.f, 0.f, 0.f};
//    glm::vec3 m_cameraPos{0.f};
};

} // end of namespace rl
