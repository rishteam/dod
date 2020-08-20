#include <RishEngine.h>

namespace rl {

void BeginDockSpace();
void EndDockSpace();

class EditorLayer : public rl::Layer
{
public:
	EditorLayer();

	virtual ~EditorLayer() = default;
	virtual void onAttach() override;
	virtual void onDetach() override;

	virtual void onUpdate(Time dt) override;
	virtual void onImGuiRender() override;
	virtual void onEvent(rl::Event& event) override;

private:
    OrthographicCamera m_camera;

	rl::Ref<rl::Shader> m_shader;
	rl::Ref<rl::Shader> testShader;

	rl::Ref<rl::Texture2D> m_texture;

	rl::Ref<rl::Framebuffer> m_framebuffer;
};

}
