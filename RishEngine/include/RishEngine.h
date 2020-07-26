/**
 * @file RishEngine.h
 * @author roy4801 (roy@rish.com.tw), icejj (icejj@rish.com.tw)
 * @brief This header should only be include by the CLIENT code
 * @date 2020-05-21
 */
#pragma once

/**
 * @warning This header should only be include by the CLIENT code
 */

// Core Module
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
#include <Rish/Debug/debug-trap.h>
#include <Rish/Utils/FileDialog.h>
#include <Rish/Debug/ImGuiLogWindow.h>

// Animation Module
#include <Rish/Animation/Animation.h>
#include <Rish/Animation/Loader.h>
#include <Rish/Animation/Editor/Editor.h>

// Layers
#include <Rish/Layer/Layer.h>
#include <Rish/Layer/ImGuiLayer.h>

// Renderer
#include <Rish/Renderer/Buffer.h>
#include <Rish/Renderer/Shader.h>
#include <Rish/Renderer/VertexArray.h>
#include <Rish/Renderer/Texture2D.h>
#include <Rish/Renderer/Framebuffer.h>
#include <Rish/Renderer/Renderer.h>
#include <Rish/Renderer/RendererCommand.h>
#include <Rish/Renderer/Camera/OrthographicCamera.h>

/// @defgroup core Core Module
/// Core functions