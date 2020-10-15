#pragma once

#include <Rish/rlpch.h>
#include <Rish/Events/Event.h>

#include "Panels/SceneTargetPanel.h"
#include "Edit/EditorGrid.h"

namespace rl {

class RL_API EditController : public SceneTargetPanel
{
public:
    EditController();
    ~EditController() override = default;
    //
    void onAttach(const Ref<Scene> &scene) override;
    void onDetach() override;
    void onUpdate(Time dt) override;
    void onImGuiRender() override;
    void onEvent(Event &e);

    Ref<OrthographicCameraController> getCameraController() const { return m_cameraController; }
private:
    bool onMouseScrolled(MouseScrolledEvent &e);

    EditorGrid m_editorGrid;                              ///< Editor grid
    Ref<OrthographicCameraController> m_cameraController; ///< Editor camera

    // Attributes of current selected entity
    glm::vec3 m_curEntPos{}, m_curSize{};

    // Entity move
    glm::vec3 m_moveEntityDiff{0.f};
    bool m_isNowMovingEntity = false;

    // Camera pane
    glm::vec3 m_moveCameraDiff{0.f};
    glm::vec2 m_preMPos{0.f};
    bool m_isNowMovingCamera = false; ///< Is now moving camera

    //
    glm::vec2 sceneMousePosNormalize{0.f};
    glm::vec2 sceneMousePosCenterNormalize{0.f};

    bool m_sceneWindowFocused = false; ///< Is Scene window focused?
    bool m_sceneWindowHovered = false; ///< Is Scene window hovered?

    // Helper functions
    void drawCameraIconAndBorder(const Ref<Scene> &scene) const;

public:
    bool m_debugEditorGrid       = false;
    bool m_debugCameraController = false;
    bool m_debugEditorController = false;
    bool m_debugShowIcon         = true;
    bool m_debugSimulateParticle      = true;

};

} // end of namespace rl


