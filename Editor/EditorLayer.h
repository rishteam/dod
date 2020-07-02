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

};

}



