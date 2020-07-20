#include <RishEngine.h>

namespace rl {

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
	std::shared_ptr<rl::Shader> m_shader;
	std::shared_ptr<rl::VertexArray> m_vertexArray;

	std::shared_ptr<rl::Shader> testShader;
	std::shared_ptr<rl::VertexArray> testVA;

	std::shared_ptr<rl::Texture2D> m_texture;

	std::shared_ptr<rl::Framebuffer> m_framebuffer;
};

}



