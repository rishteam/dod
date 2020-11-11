#include <RishEngine.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ComponentEditPanel.h"
#include "Panels/ComponentSelectionPanel.h"
#include "Panels/ErrorModal.h"
#include "Panels/HelpPanel.h"
#include "Panels/AboutPanel.h"

#include "Edit/EditorGrid.h"
#include "Edit/EditController.h"

#include <Rish/ImGui/MenuAction.h>

namespace rl {

struct EditorSetting
{
    bool saveSettingOnExit        = false;

    bool isDefaultOpenSceneLoaded = false;
    bool isDefaultOpenScene       = false;
    std::string path;

    template<typename Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(isDefaultOpenScene));
        ar(CEREAL_NVP(path));
        ar(CEREAL_NVP(saveSettingOnExit));
    }
};

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

private:
    bool onWindowCloseEvent(WindowCloseEvent &event)
    {
        if(m_currentScene->getSceneState() == Scene::SceneState::Play)
        {
            m_currentScene->onSceneStop();
        }
        return false;
    }

    void onImGuiMainMenuRender();

    //////////////////////////////////////////
    // Editor Layer
    //////////////////////////////////////////
    void loadSetting(const std::string &path);
    void saveSetting();

    EditorSetting m_editorSetting;

    void newScene();
    void openScene(const std::string &path);
    void saveScene(const std::string &path);

    //////////////////////////////////////////
    // Scene
    //////////////////////////////////////////
	Ref<Framebuffer> m_editorFramebuffer;
    //
	Ref<Framebuffer> m_sceneFramebuffer;
    Ref<Scene> m_currentScene;        ///< Current Scene
    Ref<Scene> m_editorScene;         ///< Editor Scene
    Ref<Scene> m_runtimeScene;        ///< Runtime Scene
	//
	std::string m_scenePath;
	bool m_sceneLoaded = false;
    glm::vec2 m_sceneViewportPanelSize {0.f, 0.f};

    //////////////////////////////////////////
    // Editor Panels
    //////////////////////////////////////////
	Ref<EditController> m_editController;
    Ref<SceneHierarchyPanel> m_sceneHierarchyPanel;
    Ref<ComponentEditPanel> m_componentEditPanel;

    void switchCurrentScene(const Ref<Scene> &scene);
    void setContextToPanels(const Ref<Scene> &scene);
	std::vector<Ref<ScenePanel>> m_panelList;

    //////////////////////////////////////////
    // Common Widgets
    //////////////////////////////////////////
	ErrorModal m_errorModal;

	// simple panels
	Ref<HelpPanel> m_helpPanel;
	Ref<AboutPanel> m_aboutPanel;
	std::vector<Ref<Panel>> m_simplePanelList;

    //////////////////////////////////////////
    // Editor Menu
    //////////////////////////////////////////

    // TODO: Refactor action menu callback
    ImActionManager m_sceneAction;
    ImAction *m_action_copy = nullptr;
    ImAction *m_action_paste = nullptr;
    ImAction *m_action_delete = nullptr;
    std::vector<Entity> m_copyList;

    //////////////////////////////////////////
    // Debugs
    //////////////////////////////////////////
	bool m_debugNativeScript = false;
};

}
