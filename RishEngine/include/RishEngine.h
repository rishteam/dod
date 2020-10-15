/**
 * @file RishEngine.h
 * @author roy4801 (me@roy4801.tw), icejj (icejj@rish.com.tw)
 * @brief This header should only be include by the CLIENT code
 * @warning This header should only be include by the CLIENT code
 * @date 2020-05-21
 */
#pragma once

/**
 * @defgroup core Core Module
 * @brief Core functions of RishEngine
 */
#include <Rish/Core/Core.h>
#include <Rish/Core/Log.h>
#include <Rish/Core/ResHolder.h>
#include <Rish/Core/Application.h>
#include <Rish/Core/Time.h>
#include <Rish/Core/FileSystem.h>
#include <Rish/Core/VFS.h>

// Event module
#include <Rish/Events/Event.h>

// Input Module
#include <Rish/Input/Input.h>

// Utils Module
#include <Rish/Utils/FileDialog.h>

/**
 * @defgroup animation Animation Module
 * @brief Animation
 */
#include <Rish/Animation/Animation.h>
#include <Rish/Animation/Loader.h>
#include <Rish/Animation/Editor/EditorApp.h>

// Layer
#include <Rish/Layer/Layer.h>
#include <Rish/Layer/ImGuiLayer.h>

/**
 * @defgroup renderer Renderer module
 * @brief Renderer module
 */
#include <Rish/Renderer/Buffer.h>
#include <Rish/Renderer/Shader.h>
#include <Rish/Renderer/VertexArray.h>
#include <Rish/Renderer/Texture2D.h>
#include <Rish/Renderer/SubTexture2D.h>
#include <Rish/Renderer/Framebuffer.h>
#include <Rish/Renderer/Renderer.h>
#include <Rish/Renderer/Renderer2D.h>
#include <Rish/Renderer/RendererCommand.h>
#include <Rish/Renderer/Camera/OrthographicCamera.h>
#include <Rish/Renderer/Camera/OrthographicCameraController.h>

/**
 * @defgroup physics Physics Module
 * @brief Physics Module
 */
#include <Rish/Physics/PhysicsWorld.h>
#include <Rish/Physics/RigidBody2D.h>
#include <Rish/Physics/Joint.h>

/**
 * @defgroup math Math Module
 * @brief Math related
 */
#include <Rish/Math/AABB.h>

/**
 * @defgroup scene Scene Module
 * @brief Scene
 */
#include "Rish/Scene/Scene.h"
#include "Rish/Scene/Entity.h"
#include "Rish/Scene/Component.h"

// Debug
#include <Rish/Debug/Instrumentor.h>
#include <Rish/Debug/debug-trap.h>
#include <Rish/Debug/ImGuiLogWindow.h>
