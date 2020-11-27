#include <Rish/rlpch.h>
#include <RishEngine.h>

using namespace rl;

class LightLayer : public Layer
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
    glm::vec4 maskColor;

    Ref<Texture2D> texture;
    Ref<Framebuffer> world, light;

    OrthographicCameraController m_cameraController;
};

