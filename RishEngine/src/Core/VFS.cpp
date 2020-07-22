#include "Rish/Core/VFS.h"
#include "Rish/Core/fileSystem.h"

namespace rl {

VFS *VFS::vfs_Instance = nullptr;

void VFS::Init()
{
	vfs_Instance = new VFS();
	RL_CORE_INFO("VFS::Initialized");
}

void VFS::ShutDown()
{
	delete vfs_Instance;
	RL_CORE_INFO("VFS::Shutdown");
}

void VFS::Mount(const std::string &virtualPath, const std::string &physicalPath)
{
	m_MountPoints[virtualPath].push_back(physicalPath);
}

void VFS::Unmount(const std::string &path)
{
	m_MountPoints[path].clear();
}

bool VFS::ResolvePhysicalPath(const std::string &path, std::string &outphysicalPath)
{
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

	const std::string &virtualDir = dirs.front();
	// If the virtual directory doesn't exist
	if (!m_MountPoints.count(virtualDir))
	{
		RL_CORE_ERROR("VFSError: Mapping entry is not found {}", virtualDir);
		return false;
	}
	// If the virtual directory doesn't have any mapping
	if (m_MountPoints[virtualDir].empty())
	{
		RL_CORE_ERROR("VFSError: Mapping list is empty {}", virtualDir);
	}
	// e.g. Mount("res", "fuck/res")
	// v: res/test.txt p:fuck/res/test.txt
	// remainder = test.txt
	std::string remainder = path.substr(virtualDir.size() + 1, path.size() - virtualDir.size());
	// RL_CORE_TRACE("{}", remainder);
	//Search the file in the mapping list
	for (const std::string &physicalPath : m_MountPoints[virtualDir])
	{
		std::string p = physicalPath + remainder;
		if (FileSystem::FileExists(p))
		{
			outphysicalPath = p;
			return true;
		}
	}

	RL_CORE_ERROR("VFSError: File is not found {}", path);
	return true;
}

char *VFS::ReadFile(const std::string &path)
{
	std::string physicalPath;
	return ResolvePhysicalPath(path, physicalPath) ? FileSystem::ReadFile(physicalPath) : nullptr;
}

std::string VFS::ReadTextFile(const std::string &path)
{
	std::string physicalPath;
	return ResolvePhysicalPath(path, physicalPath) ? FileSystem::ReadTextFile(physicalPath) : nullptr;
}

bool VFS::WriteFile(const std::string &path, const char *buffer, const int size)
{
	std::string physicalPath;
	return ResolvePhysicalPath(path, physicalPath) ? FileSystem::WriteFile(physicalPath, buffer, size) : false;
}

bool VFS::WriteTextFile(const std::string &path, const std::string &text, const int size)
{
	std::string physicalPath;
	return ResolvePhysicalPath(path, physicalPath) ? FileSystem::WriteTextFile(physicalPath, text, size) : false;
}

}