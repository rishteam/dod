#include <RishEngine.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ComponentEditPanel.h"
#include "Panels/ComponentSelectionPanel.h"

#include "Panels/ErrorModal.h"

namespace rl {

class EditorLayer : public Layer
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
    void onImGuiMainMenuRender();

	Ref<Framebuffer> m_framebuffer;
	Ref<Scene> m_scene;
	std::string m_scenePath;
	bool m_sceneLoaded = false;
	//
	bool m_sceneWindowHovered = false;
	bool m_sceneWindowFocused = false;
	// Scene window camera
	OrthographicCameraController m_cameraController;

	SceneHierarchyPanel m_sceneHierarchyPanel;
	ComponentEditPanel m_componentEditPanel;
	ErrorModal m_errorModal;
};

}
