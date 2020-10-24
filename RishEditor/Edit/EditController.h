#pragma once

#include <Rish/rlpch.h>
#include <Rish/Events/Event.h>

#include "Panels/SceneMultiTargetPanel.h"
#include "Edit/EditorGrid.h"

namespace rl {

class RL_API EditController : public SceneMultiTargetPanel
{
public:
    EditController();
    EditController(const Ref<Scene> &scene) : SceneMultiTargetPanel(scene) {}
    ~EditController() override = default;
    //
    void onAttach(const Ref<Scene> &scene) override;
    void onDetach() override;
    void onUpdate(Time dt) override;
    void onImGuiRender() override;
    void onEvent(Event &e);

    void gizmoModeChange(int k);

    Ref<OrthographicCameraController> getCameraController() const { return m_cameraController; }
private:
    bool onMouseScrolled(MouseScrolledEvent &e);

    EditorGrid m_editorGrid;                              ///< Editor grid
    Ref<OrthographicCameraController> m_cameraController; ///< Editor camera

    // Attributes of current selected entity
    glm::vec3 m_curEntPos{}, m_curSize{};

    // Entity move
    std::unordered_map<Entity, bool> m_isNowMovingEntity{};
    std::unordered_map<Entity, glm::vec3> m_oriEntityPosition{};
    std::unordered_map<Entity, glm::vec3> m_oriEntitySize{};
    std::unordered_map<Entity, float> m_oriEntityRotate{};
    std::unordered_map<Entity, glm::vec3> m_oriMousePosition{};
    std::unordered_map<Entity, float> m_oriMouseRotate{};
    std::unordered_map<Entity, glm::vec3> m_moveEntityWeight{};
    std::unordered_map<Entity, glm::vec3> m_zoomEntityWeight{};

    // Camera pane
    glm::vec3 m_moveCameraDiff{0.f};
    glm::vec2 m_preMPos{0.f};
    bool m_isNowMovingCamera = false; ///< Is now moving camera

    //
    glm::vec2 sceneMousePosNormalize{0.f};
    glm::vec2 sceneMousePosCenterNormalize{0.f};

    bool m_sceneWindowFocused        = false; ///< Is Scene window focused?
    bool m_sceneWindowHovered        = false; ///< Is Scene window hovered?
    bool m_scenePrevLeftMouseDown    = false;
    bool m_scenePrevLeftMouseClicked = false;

    // Helper functions
    void drawCameraIconAndBorder(const Ref<Scene> &scene) const;

    // Gizmo
    enum class Gizmo{
        ZoomMode,
        MoveMode,
        RotationMode
    }m_gizmoMode = Gizmo::ZoomMode;

public:
    bool m_debugEditorGrid       = false;
    bool m_debugCameraController = false;
    bool m_debugEditorController = false;
    bool m_debugShowIcon         = true;
    bool m_debugSimulateParticle = true;

};

} // end of namespace rl


