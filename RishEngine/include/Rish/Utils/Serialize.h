#pragma once

#include <Rish/Core/FileSystem.h>
#include <Rish/Core/VFS.h>

#include <cereal/cereal.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/complex.hpp>

#include <cereal/archives/json.hpp>

namespace glm
{
template <class Archive>
void serialize(Archive &archive, glm::vec2 &v) { archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y)); }
template<class Archive> void serialize(Archive& archive, glm::vec3& v) { archive(cereal::make_nvp("x" ,v.x), cereal::make_nvp("y" ,v.y), cereal::make_nvp("z" ,v.z)); }
template<class Archive> void serialize(Archive& archive, glm::vec4& v)
{
    archive(cereal::make_nvp("x" ,v.x),
            cereal::make_nvp("y" ,v.y),
            cereal::make_nvp("z" ,v.z),
            cereal::make_nvp("w" ,v.w)
    );
}

template<class Archive> void serialize(Archive& archive, glm::ivec2& v) { archive(v.x, v.y); }
template<class Archive> void serialize(Archive& archive, glm::ivec3& v) { archive(v.x, v.y, v.z); }
template<class Archive> void serialize(Archive& archive, glm::ivec4& v) { archive(v.x, v.y, v.z, v.w); }
template<class Archive> void serialize(Archive& archive, glm::uvec2& v) { archive(v.x, v.y); }
template<class Archive> void serialize(Archive& archive, glm::uvec3& v) { archive(v.x, v.y, v.z); }
template<class Archive> void serialize(Archive& archive, glm::uvec4& v) { archive(v.x, v.y, v.z, v.w); }
template<class Archive> void serialize(Archive& archive, glm::dvec2& v) { archive(v.x, v.y); }
template<class Archive> void serialize(Archive& archive, glm::dvec3& v) { archive(v.x, v.y, v.z); }
template<class Archive> void serialize(Archive& archive, glm::dvec4& v) { archive(v.x, v.y, v.z, v.w); }

// glm matrices serialization
template<class Archive> void serialize(Archive& archive, glm::mat2& m) { archive(m[0], m[1]); }
template<class Archive> void serialize(Archive& archive, glm::dmat2& m) { archive(m[0], m[1]); }
template<class Archive> void serialize(Archive& archive, glm::mat3& m) { archive(m[0], m[1], m[2]); }
template<class Archive> void serialize(Archive &archive, glm::mat4 &m) { archive(cereal::make_nvp("col0", m[0]), cereal::make_nvp("col1", m[1]), cereal::make_nvp("col2", m[2]), cereal::make_nvp("col3", m[3])); }
template<class Archive> void serialize(Archive& archive, glm::dmat4& m) { archive(m[0], m[1], m[2], m[3]); }

template<class Archive> void serialize(Archive& archive, glm::quat& q) { archive(q.x, q.y, q.z, q.w); }
template<class Archive> void serialize(Archive& archive, glm::dquat& q) { archive(q.x, q.y, q.z, q.w); }
}

namespace rl {

template<typename T>
void saveToFile(const std::string &name, T &obj, const std::string &path)
{
    std::ofstream of(path);
    cereal::JSONOutputArchive outputArchive(of);
    outputArchive(cereal::make_nvp(name, obj));
}

template<typename T>
void loadFromFile(const std::string &name, T &obj, const std::string &path)
{
    std::stringstream oos(FileSystem::ReadTextFile(path));
    cereal::JSONInputArchive inputArchive(oos);
    inputArchive(cereal::make_nvp(name, obj));
}

template<typename T>
void saveToFileVFS(const std::string &name, T &obj, const std::string &path)
{
    std::string ppath;
    VFS::ResolvePhysicalPath(path, ppath);

    saveToFile(name, obj, ppath);
}

template<typename T>
void loadFromFileVFS(const std::string &name, T &obj, const std::string &path)
{
    std::string ppath;
    VFS::ResolvePhysicalPath(path, ppath);

    loadFromFile(name, obj, ppath);
}

} // end of namespace rl