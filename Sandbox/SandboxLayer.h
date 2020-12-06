#pragma once

#include <Rish/rlpch.h>
#include <RishEngine.h>

#include <Rish/Sound/Component.h>

using namespace rl;

struct TestBox
{
    TestBox(const glm::vec2 &pos, const glm::vec2 &siz)
        : position(pos), size(siz)
    {}
    glm::vec2 position;
    glm::vec2 size;
};

class ExampleSandboxLayer : public rl::Layer
{
public:
    ExampleSandboxLayer();
    ~ExampleSandboxLayer() override;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(rl::Time dt) override;
    void onImGuiRender() override;
    void onEvent(rl::Event& event) override;

private:
    bool onKeyPressEvent(KeyPressedEvent &event);
    //
    std::vector<TestBox> m_boxList;
    OrthographicCameraController m_cameraController;
    Ref<Texture2D> m_testTexture;
    Ref<Scene> m_scene;
    //
#ifdef TEST_FPS
    Clock clk;
    std::vector<float> m_frameTime;
    std::vector<float> m_fps;
    int dataSetSize = 100;
    int spriteNumber = 100000;
#endif
    //
    Ref<Sound> m_testSound;
    SoundComponent m_testSoundCompoennt;
};
