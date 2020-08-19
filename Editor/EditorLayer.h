#include <RishEngine.h>

namespace rl {

#define MAXENTITES 1000

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
	std::shared_ptr<rl::Framebuffer> m_framebuffer;
	std::shared_ptr<rl::Scene> m_scene;

	std::vector<Entity> m_entityList;
};

}
