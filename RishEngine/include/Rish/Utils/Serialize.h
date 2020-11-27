#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/FileSystem.h>
#include <Rish/Core/VFS.h>

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