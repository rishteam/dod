#pragma once

#include <Rish/rlpch.h>
#include <Rish/Events/Event.h>

#include "Panels/Common/SceneMultiTargetPanel.h"
#include "Edit/EditorGrid.h"
#include "Edit/Gizmo.h"

namespace rl {

/**
 * @brief Edit Controller
 * @details Control the editor view panel
 */
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

    /// Change the Gizmo mode
    void changeGizmoMode(Gizmo::GizmoMode mode);

    /// Toggle the grid
    void toggleShowGrid();

    Ref<OrthographicCameraController> getCameraController() const { return m_cameraController; }
private:
    bool onMouseScrolled(MouseScrolledEvent &e);

    EditorGrid m_editorGrid;                              ///< Editor grid
    Ref<OrthographicCameraController> m_cameraController; ///< Editor camera

    Gizmo m_gizmo; ///< Gizmo

    // Camera pane
    glm::vec2 m_preMousePos{0.f};        ///< The mouse position in game world in the previous frame
    bool m_isNowMovingCamera = false;    ///< Is now moving camera (pane)
    bool m_isNowScrollingCamera = false; ///< Is now scrollint the camera (zoom)

    // mouse
    glm::vec2 sceneMousePosNormalize{0.f};       ///< Mouse position relative to the scene window (origin is left upper)
    glm::vec2 sceneMousePosCenterNormalize{0.f}; ///< Mouse position relative to the scene window (origin is center)
    glm::vec2 mousePosInWorld;                   ///< Mouse position in the game world

    // States
    bool m_sceneWindowFocused        = false; ///< Is Scene window focused?
    bool m_sceneWindowHovered        = false; ///< Is Scene window hovered?
    bool m_scenePrevLeftMouseDown    = false; ///< Is the mouse down in the previous frame
    bool m_scenePrevLeftMouseClicked = false; ///< Is the mouse clicked in the previous frame

    /// Draw camera icon and border
    void drawCameraIconAndBorder(const Ref<Scene> &scene) const;

    bool m_showGrid = true; ///< Show Grid

    void arrangeSelectedEntity();
    void arrangeSelectedGroup(std::set<Entity> &delTarget, Entity entity);

    // init group
    std::set <Entity> m_initGroupSet;
    std::set <Entity> m_movingGroupSet;
    void movingGroupEntity(Entity targetEntity);
    void initGroupEntityTransform(Entity groupEntity);

public:
    // Debug Window
    bool m_debugEditorGrid       = false;
    bool m_debugCameraController = false;
    bool m_debugEditorController = false;
    bool m_debugShowIcon         = true;
    bool m_debugSimulateParticle = true;

};

} // end of namespace rl


