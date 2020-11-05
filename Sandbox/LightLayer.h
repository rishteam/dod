#include <Rish/rlpch.h>
#include <RishEngine.h>

using rl::Ref;
using rl::MakeRef;
using rl::RigidBody2D;
using rl::Renderer2D;
using rl::OrthographicCameraController;
using rl::Application;

class LightLayer : public rl::Layer
{
public:
    LightLayer();
    ~LightLayer() override;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(rl::Time dt) override;
    void onImGuiRender() override;
    void onEvent(rl::Event &event) override;

private:

    float strength;
    float radius;
    glm::vec4 color;
    glm::vec3 lightPos;

    OrthographicCameraController m_cameraController;
};


