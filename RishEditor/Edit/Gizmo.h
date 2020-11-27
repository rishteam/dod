#pragma  once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Math/BoundingBox.h>

namespace rl {

class Gizmo
{
public:

    enum class GizmoMode
    {
        MoveMode,
        ZoomMode,
        ScaleMode,
        RotationMode
    };
    GizmoMode m_gizmoMode = GizmoMode::MoveMode;

public:
    Gizmo();
    ~Gizmo() = default;
    //
    void onUpdate();
    void onImGuiRender(bool isValid, glm::vec2 mousePosInWorld);
    //
    bool isMovingEntity();
    bool isMoving(Entity ent);
    bool isMouseOnGizmo(glm::vec2 mousePosInWorld);
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

    glm::vec2 m_clickSizeVec2; ///< Click size in vec2
    glm::vec3 m_clickSizeVec3; ///< Click size in vec3

    // Mouse position
    glm::vec2 m_mousePosBegin;
    glm::vec2 m_mousePosEnd;

    // Entity
    std::set<Entity> m_entitySet;
    /// Calculate all of the entities' bound and combine them
    BoundingBox2D calculateEntitySetBound();

    std::pair<glm::vec2, glm::vec2> calculateNewBoundVec2()
    {
        auto tmp = calculateEntitySetBound();
        auto boundPos = tmp.getPositionVec2();
        auto boundSize = tmp.getScaleVec2();
        return std::make_pair(boundPos, boundSize);
    }
    std::pair<glm::vec3, glm::vec3> calculateNewBoundVec3()
    {
        auto tmp = calculateEntitySetBound();
        auto boundPos = tmp.getPositionVec3();
        auto boundSize = tmp.getScaleVec3();
        return std::make_pair(boundPos, boundSize);
    }

    //
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
