#include <Rish/Core/Core.h>
#include <Rish/Core/VFS.h>
#include <Rish/Core/FileSystem.h>

namespace rl {

VFS *VFS::instance = nullptr;

std::unordered_map<std::string, std::vector<std::string>> VFS::m_MountPoints;

void VFS::Init()
{
    instance = new VFS();
	RL_CORE_INFO("[VFS] Initialized");
}

void VFS::ShutDown()
{
    instance = nullptr;
	delete instance;
	RL_CORE_INFO("[VFS] Shutdown");
}

void VFS::Mount(const std::string &virtualPath, const std::string &physicalPath)
{
    RL_CORE_ASSERT(instance, "[VFS] The VFS is not initialized.");
	m_MountPoints[virtualPath].push_back(physicalPath);
}

void VFS::Unmount(const std::string &path)
{
    RL_CORE_ASSERT(instance, "[VFS] The VFS is not initialized.");
	m_MountPoints[path].clear();
}

bool VFS::ResolvePhysicalPath(const std::string &path, std::string &outphysicalPath)
{
    RL_CORE_ASSERT(instance, "[VFS] The VFS is not initialized.");
	// TODO: ???? WTF
	if (path[0] != '/')
	{
		outphysicalPath = path;
		return FileSystem::FileExists(path);
	}
	std::vector<std::string> dirs;
	// split path
	// e.g. split "aaa/bbb/ccc" to [aaa, bbb, ccc]
	// (stored in std::vector)
	size_t start = 0;
	size_t end = path.find_first_of('/');
	while (end <= std::string::npos)
	{
		std::string token = path.substr(start, end - start);
		if (!token.empty())
			dirs.push_back(token);

		if (end == std::string::npos)
			break;

		start = end + 1;
		end = path.find_first_of('/', start);
	}

	// The Virtual Directory (first level). e.g. virtual = /shader/a/b/c/d, virtualDir = shader
	const std::string &virtualDir = dirs.front();
	// If the virtual directory doesn't exist
	if (!m_MountPoints.count(virtualDir))
	{
		RL_CORE_ERROR("[VFS]: Mapping entry is not found {}", virtualDir);
		return false;
	}
	// If the virtual directory doesn't have any mapping
	if (m_MountPoints[virtualDir].empty())
	{
		RL_CORE_ERROR("[VFS]: Mapping list is empty {}", virtualDir);
	}
	// e.g. Mount("res", "fuck/res")
	// v: res/test.txt p:fuck/res/test.txt
	// remainder = test.txt
	std::string remainder = path.substr(virtualDir.size() + 1, path.size() - virtualDir.size());
	// RL_CORE_TRACE("{}", remainder);
	// Search the file in the mapping list
	for (const std::string &physicalPath : m_MountPoints[virtualDir])
	{
		std::string p = physicalPath + remainder;
		if (FileSystem::FileExists(p))
		{
			outphysicalPath = p;
			return true;
		}
	}

	RL_CORE_ERROR("[VFS]: File is not found {}", path);
	return true;
}

char *VFS::ReadFile(const std::string &path)
{
    RL_CORE_ASSERT(instance, "[VFS] The VFS is not initialized.");
	std::string physicalPath;
	return ResolvePhysicalPath(path, physicalPath) ? FileSystem::ReadFile(physicalPath) : nullptr;
}

std::string VFS::ReadTextFile(const std::string &path)
{
    RL_CORE_ASSERT(instance, "[VFS] The VFS is not initialized.");
	std::string physicalPath;
	return ResolvePhysicalPath(path, physicalPath) ? FileSystem::ReadTextFile(physicalPath) : nullptr;
}

bool VFS::WriteFile(const std::string &path, const char *buffer, const int size)
{
    RL_CORE_ASSERT(instance, "[VFS] The VFS is not initialized.");
	std::string physicalPath;
	return ResolvePhysicalPath(path, physicalPath) ? FileSystem::WriteFile(physicalPath, buffer, size) : false;
}

bool VFS::WriteTextFile(const std::string &path, const std::string &text, const int size)
{
    RL_CORE_ASSERT(instance, "[VFS] The VFS is not initialized.");
	std::string physicalPath;
	return ResolvePhysicalPath(path, physicalPath) ? FileSystem::WriteTextFile(physicalPath, text, size) : false;
}

bool VFS::FileExists(const std::string &virtualPath)
{
    RL_CORE_ASSERT(instance, "[VFS] The VFS is not initialized.");
    bool succ = false;
    std::string physical;
    if(ResolvePhysicalPath(virtualPath, physical))
    {
        succ = FileSystem::FileExists(physical);
        if(!succ)
            RL_CORE_ERROR("[VFS] The File is noe existed.");
    }
    return succ;
}

}