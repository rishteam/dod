#include <RishEngine.h>
#include <EntryPoint.h>

#include "EditorLayer.h"

namespace rl {

class Editor : public rl::Application
{
public:
	Editor() : rl::Application("RishEditor", 1920, 1000)
	{
		pushLayer(new EditorLayer());
	}

	virtual ~Editor() override
	{
	}
};

rl::Application* CreateApplication()
{
	return new Editor();
}

}