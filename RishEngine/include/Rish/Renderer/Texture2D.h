/**
 * @file Texture2D.h
 * @author icejj (lpc0503@gmail.com), roy4801 (roy@nisra.net)
 * @brief Header of Texture2D
 * @version 0.1
 * @date 2020-07-22
 */
#pragma once

#include <Rish/rlpch.h>

namespace rl {

/**
 * @brief 2D Texture class
 */
class Texture2D
{
public:
    /**
     * @brief ctor
     * @param path Image file path
     */
	Texture2D(const std::string& path);
	~Texture2D();

	uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height;}

    void bind(uint32_t slot = 0) const;
	void unbind() const;

private:
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