/**
 * @file Texture2D.h
 * @author icejj (lpc0503@gmail.com), roy4801 (roy@nisra.net)
 * @brief Header of Texture2D
 * @version 0.1
 * @date 2020-07-22
 */
#pragma once

#include <glm/glm.hpp>

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>

#include <Rish/Renderer/Image.h>

namespace rl {

// TODO: Support other format (RGB, RGBA, ...) of textures. Now only support RGBA format

/**
 * @brief 2D Texture class
 */
class RL_API Texture2D
{
public:
    /**
     * @brief Create a empty texture
     * @param width Width
     * @param height Height
     */
    Texture2D(uint32_t width, uint32_t height);
    /**
     * @brief ctor
     * @param path Image file path
     */
    Texture2D(const std::string &path, bool flip=true);
    /**
     * @brief ctor
     * @param image Image Ref
     */
    Texture2D(Ref<Image> image);
	~Texture2D();

	void setData(void *data, uint32_t size);
	// TODO: void setRegion() ?
	void setPixel(uint32_t x, uint32_t y, const glm::vec4 &color);

	uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height;}
    float getAspectRatio() const { return (float)m_width / (float)m_height; }

    void bind(uint32_t slot=0) const;
	void unbind() const;

	/**
	 * @brief Load a Texture with rl::FileSystem
	 * @param path Path to the file
	 * @param flip Flip the image?
	 * @return Reference to the texture
	 */
	static Ref<Texture2D> LoadTexture(const std::string &path, bool flip=true);
	/**
	 * @brief Load a Texture with rl::VFS
	 * @param path Path to the file
	 * @param flip Flip the image?
	 * @return Reference to the texture
	 */
	static Ref<Texture2D> LoadTextureVFS(const std::string &virtualPath, bool flip=true);
    /**
     * @brief Create a empty Texture
     * @param width Width
     * @param height Height
     * @return Reference to the texture
     */
	static Ref<Texture2D> Create(uint32_t width, uint32_t height);

    bool operator==(const Texture2D &rhs) const;
    bool operator!=(const Texture2D &rhs) const;

private:
    void createTexture();
    void setSize(uint32_t width, uint32_t height);
    void setTexture(const void * imagePtr);

    /// Path to the image file
	std::string m_path;
    /// Width
	uint32_t m_width;
	/// Height
	uint32_t m_height;
	/// Texture ID
	uint32_t m_textureID;
};

}

/**
 * @class rl::Texture2D
 * A 2D Texture. You can use rl::Texture2D::LoadTexture() or rl::Texture2D::LoadTextureVFS() to load a texture.
 *
 * @code{.cpp}
 * // Load the texture with VFS
 * auto texture = rl::Texture2D::LoadTextureVFS("/texture/1.png");
 * // Load the texture with FileSystem
 * auto texture2 = rl::Texture2D::LoadTexture("C:/Users/Rish/Desktop/rish/dod/assets/1.png");
 * @endcode
 *
 * Or you can load the image first and then construct a Texture2D with the image.
 * @code{.cpp}
 * auto image = rl::Image::LoadImageVFS("/texture/1.png");
 * auto texture = rl::MakeRef<rl::Texture2D>(image);
 * @endcode
 *
 */
