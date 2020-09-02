#include <RishEngine.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ComponentEditPanel.h"
#include "Panels/ComponentSelectionPanel.h"

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

	static void BeginDockspace();
	static void EndDockspace();

private:
	OrthographicCameraController m_cameraController;

	Ref<rl::Framebuffer> m_framebuffer;
	Ref<rl::Scene> m_scene;
	std::string m_scenePath;
	bool m_sceneLoaded = false;

	//
	bool m_showErrorModal = false;
	std::string m_showErrorModalErrorMessage;

	bool m_sceneWindowHovered = false;
	bool m_sceneWindowFocused = false;

	std::vector<Entity> m_entityList;

	SceneHierarchyPanel m_sceneHierarchyPanel;
	ComponentEditPanel m_componentEditPanel;
};

}
