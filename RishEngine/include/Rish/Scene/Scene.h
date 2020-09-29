#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Time.h>
#include <Rish/Renderer/Framebuffer.h>
//
#include <Rish/Scene/SceneCamera.h>
#include <Rish/Scene/Component.h>
//
#include <Rish/Physics/PhysicsWorld.h>
#include <Rish/Physics/Component.h>

#include <entt/entt.hpp>
#include <cereal/cereal.hpp>

namespace rl
{

// Forward declaration
class Entity;

/**
 * @brief Scene object
 * @details A Scene is used to create and destroy entity, also update system
 */
class Scene
{
public:
	Scene();
	~Scene();

	/**
	 * @brief Create a Entity object
	 * @param name Name
	 * @return Entity
	 */
	Entity createEntity(const std::string& name = std::string());

	/**
	 * @brief Destroy a Entity object
	 * @param entity Target
	 */
	void destroyEntity(const Entity& entity);

	/**
	 * @brief Duplicate a Entity
	 * @param src Target
	 * @return Duplicate entity
	 */
	Entity duplicateEntity(Entity src);

	/**
	 * @brief Update Entity behavior
	 * @param camera Camera
	 * @param dt Delta t
	 */
	void onUpdate(Time dt);

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
	const SceneCamera& getMainCamera() const { return m_mainCamera; }

	//
	bool m_debugCamera          = false;
	bool m_debugPhysics         = true;
	bool m_debugCameraComponent = false;
private:
    ////////////////////////////////////////////////////////////////
    // Scene Camera
    ////////////////////////////////////////////////////////////////
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
	std::unordered_map<std::string, size_t> m_entNameToNumMap{};

    ////////////////////////////////////////////////////////////////
    // Physics
    ////////////////////////////////////////////////////////////////
    PhysicsWorld PhysicsWorld{Vec2(0.0f, -9.8f)};
    std::unordered_map<UUID, Ref<RigidBody2D>> mapPhysics_obj;

    ////////////////////////////////////////////////////////////////
	// friend class
    ////////////////////////////////////////////////////////////////
    friend class Entity;
    // Editor
    friend class EditorLayer;
    friend class SceneHierarchyPanel;
    friend class ComponentSelectionPanel;
    friend class EditController;

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
                RenderComponent,
                CameraComponent
            >(ar);
	}

	template <class Archive>
	void load(Archive &ar)
	{
		entt::snapshot_loader{m_registry}
			.component<
                TagComponent,
			    TransformComponent,
			    RenderComponent,
			    CameraComponent
            >(ar);
	}
};

} // namespace rl

/**
 * @class rl::Scene
 * @ingroup scene
 */