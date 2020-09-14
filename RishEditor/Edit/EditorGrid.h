#pragma once

#include <Rish/rlpch.h>

#include <Rish/Renderer/Camera/OrthographicCameraController.h>

namespace rl {

class RL_API EditorGrid
{
public:
    void onAttach() {}
    void onDetach() {}
    void onUpdate(const OrthographicCameraController &cameraController);

    void onImGuiRender();
private:
    void drawGrid(float offset, const glm::vec4 &color);
    //
    OrthographicCameraBounds m_currentBound{0.f, 0.f, 0.f, 0.f};
    glm::vec4 startColor{0.7f, 0.7f, 0.7f, 1.0f}, endColor{0.1f, 0.1f, 0.1f, 1.0f};
    float limit{20.f}, preLimit{limit/10.f};
    //
    float currentOffset = 1.5f;
    float preOffset = currentOffset / 10.f;
    float nextOffset = currentOffset * 10.f;
    //
};

} // end of namespace rl
