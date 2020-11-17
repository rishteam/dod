#pragma once

#include <Rish/rlpch.h>
//
#include <Rish/Scene/Component.h>
#include <Rish/Physics/Component.h>
#include <Rish/Physics/PhysicsWorld.h>

namespace rl
{

// Forward declaration
class Entity;
class Time;
class SceneCamera;
class UUID;

class NativeScriptComponent;
class RigidBody2DComponent;

class PhysicsWorld;
class RigidBody2D;
class Box;
class Joint;

/**
 * @brief Scene object
 * @details A Scene is used to create and destroy entity, also update system
 */
class Scene
{
public:
    // TODO: Scene name
	Scene();
	~Scene();

	/**
	 * @brief Create a Entity object
	 * @param name Name
	 * @return Entity
	 */
	Entity createEntity(const std::string& name=std::string(), const glm::vec3 &pos=glm::vec3{0.f, 0.f, 0.f});

	/**
	 * @brief Destroy a Entity object
	 * @param entity Target
	 */
	void destroyEntity(Entity entity);

	/**
	 * @brief Duplicate a Entity
	 * @param src Target
	 * @return Duplicate entity
	 */
	Entity duplicateEntity(Entity src);

	Entity getEntityByUUID(UUID uuid);

	/**
	 * @brief Update Entity behavior
	 * @param camera Camera
	 * @param dt Delta t
	 */
	void onUpdate(Time dt);

	void onEditorInit();
	void onRuntimeInit();

	void onScenePlay();
	void onScenePause();
	void onSceneStop();

	void copySceneTo(Ref<Scene> &target);

	void onViewportResize(uint32_t width, uint32_t height);

	void onImGuiRender();

    enum class SceneState
    {
        Editor = 0,
        Play,
        Pause
    };
	SceneState getSceneState() const { return m_sceneState; }
	void setSceneState(SceneState state) { m_sceneState = state; }

	const SceneCamera& getMainCamera() const { return m_mainCamera; }

	// Debug flags
	bool m_debugCamera          = false;
	bool m_debugPhysics         = false;
	bool m_debugScene           = false;

// TODO: Remove me (because ParticleSystem)
//private:
    ////////////////////////////////////////////////////////////////
    // Scene Camera
    ////////////////////////////////////////////////////////////////
    bool findPrimaryCamera();
    SceneCamera m_mainCamera{};
    glm::mat4 m_mainCameraTransform{};

    uint32_t m_viewportWidth = 0, m_viewportHeight = 0;

    ////////////////////////////////////////////////////////////////
    // Scene
    ////////////////////////////////////////////////////////////////
	static int entityNumber;
	entt::registry m_registry;

    SceneState m_sceneState = SceneState::Editor;

	Entity createEntity(const UUID &id, const std::string &name);

	// TODO: refactor in to class
	// Entity map
	std::unordered_map<UUID, Ref<Entity>> m_UUIDToEntityMap{};
	void addEntityToMap(const UUID& id, Entity entity);
	void removeEntityFromMap(const UUID& id);
	bool isEntityInMap(Entity entity);
	bool isValidUUID(const UUID &id) const;

	/**
	 * @brief Manager of EntityNames
	 */
	struct EntityNameManager
    {
        void incrementName(const std::string &name);
        void decrementName(const std::string &name);
        std::string getName(const std::string &name);

        static std::string stripNumber(const std::string &str);

        std::unordered_map<std::string, size_t> m_entNameToNumMap{};
    };
	EntityNameManager m_nameManager;

    ////////////////////////////////////////////////////////////////
    // Physics
    ////////////////////////////////////////////////////////////////
    PhysicsWorld physicsWorld;
    // physics engine management
    std::unordered_map<UUID, Ref<RigidBody2D>> mapPhysicsObj;
    std::unordered_map<UUID, Ref<Box>> mapBoxColliderObj;
    std::unordered_map<UUID, Ref<Joint>> mapJointObj;
    // physics state management
    std::vector<std::pair<UUID, bool>> StatePhysicsObj;
    std::vector<std::pair<UUID, bool>> StateBoxColliderObj;
    std::vector<std::pair<UUID, bool>> StateJointObj;

    ////////////////////////////////////////////////////////////////
	// friend class
    ////////////////////////////////////////////////////////////////
    friend class Entity;
    // Editor
    friend class EditorLayer;
    friend class SceneHierarchyPanel;
    friend class ComponentSelectionPanel;
    friend class ComponentEditPanel;
    friend class EditController;

    friend class ParticleSystem;
    friend class PhysicsSystem;
    friend class NativeScriptSystem;
    friend class ExampleSandboxLayer;

    // Debug functions
    friend void DrawSceneDebugWindow(const char *name, Ref<Scene> scene);
    friend void DrawSceneCameraDebugWindow(const SceneCamera & camera, const glm::mat4 &trans);

    ////////////////////////////////////////////////////////////////
    // Serialization functions
    ////////////////////////////////////////////////////////////////
	friend class cereal::access;
	// TODO: Auto reflect the components of a entity?
	template <class Archive>
	void save(Archive &ar) const
	{
		 entt::snapshot{m_registry}
		 	.component<
                TagComponent,
                TransformComponent,
                SpriteRenderComponent,
                CameraComponent,
                ParticleComponent,
                NativeScriptComponent,
                RigidBody2DComponent,
                BoxCollider2DComponent,
                Joint2DComponent
            >(ar);
	}

	template <class Archive>
	void load(Archive &ar)
	{
		entt::snapshot_loader{m_registry}
			.component<
                TagComponent,
			    TransformComponent,
			    SpriteRenderComponent,
			    CameraComponent,
			    ParticleComponent,
			    NativeScriptComponent,
                RigidBody2DComponent,
                BoxCollider2DComponent,
                Joint2DComponent
            >(ar);
	}
};

} // namespace rl



/**
 * @class rl::Scene
 * @ingroup scene
 */
