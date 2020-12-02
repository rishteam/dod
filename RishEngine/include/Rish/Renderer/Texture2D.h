/**
 * @file Texture2D.h
 * @author icejj (lpc0503@gmail.com), roy4801 (roy@nisra.net)
 * @brief Header of Texture2D
 * @version 0.1
 * @date 2020-07-22
 */
#pragma once

#include <Rish/rlpch.h>
//
#include <Rish/Core/FileSystem.h>
//
#include <Rish/Renderer/Image.h>

namespace rl {

// TODO: Support other format (RGB, RGBA, ...) of textures. Now only support RGBA format


enum Texture2DFilter
{
    TexNearest = BIT(0),
    TexLinear  = BIT(1),
    MipNearest = BIT(2),
    MipLinear  = BIT(3)
};

enum Texture2DWrap
{
    Repeat = 0,
    MirrorRepeat,
    ClampToEdge,
    ClampToBorder
};

struct Texture2DOption
{
    Texture2DFilter minFilter = Texture2DFilter::TexLinear;
    Texture2DFilter magFilter = Texture2DFilter::TexLinear;
    Texture2DWrap   wrapS     = Texture2DWrap::Repeat;
    Texture2DWrap   wrapT     = Texture2DWrap::Repeat;

    static Texture2DOption DefaultOption;

    template<typename Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(minFilter));
        ar(CEREAL_NVP(magFilter));
        ar(CEREAL_NVP(wrapS));
        ar(CEREAL_NVP(wrapT));
    }
};

/**
 * @brief 2D Texture class
 */
class RL_API Texture2D
{
public:
    Texture2D();
	~Texture2D();

	void setData(void *data, uint32_t size);
	// TODO: void setRegion() ?
	void setPixel(uint32_t x, uint32_t y, const glm::vec4 &color);

	uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height;}
    float getAspectRatio() const { return (float)m_width / (float)m_height; }
    std::string getPath() const {return m_path; }
    void setPath(const std::string &path) { m_path = path; }

    Texture2DOption getOption() const { return m_option; }

    uint32_t getTextureID() const { return m_textureID; }

    void bind(uint32_t slot=0) const;
	void unbind() const;

	/**
	 * @brief Load a Texture with rl::FileSystem
	 * @param path Path to the file
	 * @param flip Flip the image?
	 * @return Reference to the texture
	 */
	static Ref<Texture2D> LoadTexture(const std::string &path, bool flip=false, Texture2DOption option=Texture2DOption{});
	/**
	 * @brief Load a Texture with rl::VFS
	 * @param path Path to the file
	 * @param flip Flip the image?
	 * @return Reference to the texture
	 */
	static Ref<Texture2D> LoadTextureVFS(const std::string &virtualPath, bool flip=false, Texture2DOption option=Texture2DOption{});
    /**
     * @brief Create a empty Texture
     * @param width Width
     * @param height Height
     * @return Reference to the texture
     */
	static Ref<Texture2D> Create(uint32_t width, uint32_t height, Texture2DOption option=Texture2DOption{});

    bool operator==(const Texture2D &rhs) const;
    bool operator!=(const Texture2D &rhs) const;

    operator bool() const
    {
        return m_init;
    }

private:
    void createTexture();
    void setOptions(const Texture2DOption &option);

    void setTexture(const void * imagePtr);
    void setSize(uint32_t width, uint32_t height);

    /// Path to the image file
	std::string m_path{};
    /// Width
	uint32_t m_width{};
	/// Height
	uint32_t m_height{};
	/// Texture ID
	uint32_t m_textureID{};
	/// Is the texture flip?
	bool m_flip = false;
    /// Setting of the texture
    Texture2DOption m_option{};
    /// Is initialized?
    bool m_init = false;

	// Serialization function
	friend class cereal::access;
    template<typename Archive>
    void save(Archive &ar) const
    {
        ar(cereal::make_nvp("path", FileSystem::RelativePath(m_path)));
        ar(cereal::make_nvp("width", m_width));
        ar(cereal::make_nvp("height", m_height));
        ar(cereal::make_nvp("flip", m_flip));
        ar(cereal::make_nvp("option", m_option));
    }

    template<typename Archive>
    void load(Archive &ar)
    {
        ar(cereal::make_nvp("path", m_path));
        ar(cereal::make_nvp("width", m_width));
        ar(cereal::make_nvp("height", m_height));
        ar(cereal::make_nvp("flip", m_flip));
        ar(cereal::make_nvp("option", m_option));
        //
        setOptions(m_option);
        if(m_path != "None")
        {
            Ref<Image> image = Image::LoadImage(m_path, m_flip);
            setSize(image->getWidth(), image->getHeight());
            setTexture(image->getPixelPtr());
        }
        else
        {
            auto image = Image::LoadImage(m_path, m_flip);
            setSize(image->getWidth(), image->getHeight());
            setTexture(image->getPixelPtr());
        }
    }
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

RL_MAKE_HASHABLE(rl::Texture2DOption, t.minFilter, t.magFilter, t.wrapS, t.wrapT)
RL_MAKE_HASHABLE(rl::Texture2D, t.getPath(), t.getWidth(), t.getHeight(), t.getOption())
