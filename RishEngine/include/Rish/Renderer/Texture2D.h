#pragma once

#include "Rish/rlpch.h"

namespace rl {

class Texture2D {

public:

	Texture2D(const std::string& path);
	~Texture2D();

	uint32_t getWidth() const;
	uint32_t getHeight() const;

	void bind(uint32_t slot = 0) const;
	void unbind() const;

private:

	std::string m_path;
	uint32_t m_width, m_height;
	uint32_t m_rendererID;
};

}