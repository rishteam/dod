#include <RishEngine.h>
#include "EditorLayer.h"

#include <EntryPoint.h>

namespace rl {

class Editor : public rl::Application
{
public:
	Editor() : rl::Application("Editor", 1920, 1000)
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