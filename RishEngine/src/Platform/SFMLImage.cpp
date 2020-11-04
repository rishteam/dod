#include <Rish/rlpch.h>
#include <Rish/Platform/SFMLImage.h>
#include <Rish/Core/ResHolder.h>

#include <Rish/Utils/uuid.h>

namespace rl {

std::unordered_map<std::string, std::string> SFMLImage::s_pathToId;

Ref<Image> Image::LoadImage(const std::string &path, bool flip)
{
    return std::make_shared<SFMLImage>(path, flip);
}

Ref<Image> Image::LoadImageVFS(const std::string &path, bool flip)
{
    std::string pp;
    VFS::ResolvePhysicalPath(path, pp);
    return std::make_shared<SFMLImage>(pp, flip);
}

SFMLImage::SFMLImage(const std::string &path, bool flip)
{
    // Cache path
    if(!s_pathToId.count(path))
    {
        m_id = uuid::generate_uuid_v4();
        s_pathToId[path] = m_id;
        ResHolder::Image().load(m_id, path);
        if(!flip)
            ResHolder::Image().get(m_id).flipVertically();
    }
    else // if exists, don't load
        m_id = s_pathToId[path];
    //
    m_image = &ResHolder::Image().get(m_id);

    m_width = m_image->getSize().x;
    m_height = m_image->getSize().y;
}

SFMLImage::~SFMLImage()
{
//    ResHolder::Image().release(m_id);
}

const void *SFMLImage::getPixelPtr()
{
    return m_image ? m_image->getPixelsPtr() : nullptr;
}

} // end of namespace rl
