#pragma  once

#include <Rish/rlpch.h>
#include <Rish/Scene/Entity.h>
#include <Rish/Math/BoundingBox.h>

namespace rl{

class Gizmo {
public:

    enum class GizmoMode{
        MoveMode,
        ZoomMode,
        ScaleMode,
        RotationMode
    }m_gizmoMode = GizmoMode::MoveMode;

public:

    Gizmo();
    ~Gizmo() = default;
    //
    void onUpdate();
    void onImGuiRender(bool isvalid, glm::vec2 mposInWorld);
    bool isMovingEntity();
    bool isMouseOnGizmo(glm::vec2 mposInWorld);
    //
    void setGizmoMode(GizmoMode mode);
    void setMousePosBegin(glm::vec2 pos);
    void setMousePosEnd(glm::vec2 pos);
    void setClickSize(glm::vec2 size);
    void setClickSize(glm::vec3 size);
    void setSelectedEntity(std::set<Entity>& ent);
    //
    BoundingBox2D getMouseBound();

private:

    glm::vec4 colorY = glm::vec4(1.f, 1.f, 0.f, 1.f);
    glm::vec4 colorR = glm::vec4(1.f, 0.f, 0.f, 1.f);
    glm::vec4 colorB = glm::vec4(0.f, 0.f, 1.f, 1.f);

    glm::vec2 m_clickSize2;
    glm::vec3 m_clickSize3;

    // Mouse position
    glm::vec2 m_mousePosBegin;
    glm::vec2 m_mousePosEnd;

    // Entity
    std::set<Entity> m_entitySet;

    std::unordered_map<Entity, bool>      m_isNowMovingEntity{};
    std::unordered_map<Entity, glm::vec3> m_oriEntityPosition{};
    std::unordered_map<Entity, glm::vec3> m_oriEntitySize{};
    std::unordered_map<Entity, glm::vec3> m_oriEntityNegative{};
    std::unordered_map<Entity, float>     m_oriEntityRotate{};
    std::unordered_map<Entity, glm::vec3> m_oriMousePosition{};
    std::unordered_map<Entity, float>     m_oriMouseRotate{};
    std::unordered_map<Entity, glm::vec3> m_moveEntityWeight{};
    std::unordered_map<Entity, glm::vec3> m_zoomEntityWeight{};

};

}

