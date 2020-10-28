#include <Rish/Core/FileSystem.h>
#include <Rish/Utils/String.h>

namespace rl {

std::string FileSystem::GetCurrentDirectory()
{
	return fs::current_path().string();
}

fs::path FileSystem::GetCurrentDirectoryPath()
{
	return fs::current_path();
}

void FileSystem::SetCurrentDirectory(const std::string &path)
{
	fs::path p = fs::path(path);
	if (fs::is_directory(p))
		fs::current_path(path);
	else
		RL_CORE_ERROR("FileSystemError: Failed to set currrent directory {}", path);
}

void FileSystem::SetCurrentDirectoryPath(const fs::path &path)
{
	if (fs::is_directory(path))
		fs::current_path(path);
	else
		RL_CORE_ERROR("FileSystemError: Failed to set current directory {}", path.string());
}

bool FileSystem::CreateFile(const std::string &path)
{
    std::ofstream file{path};

    if(!file)
    {
        RL_CORE_ERROR("[FileSystem] failed to create file {}", path);
        return false;
    }
    else
        return true;
}

bool FileSystem::DeleteFile(const std::string &path)
{
    bool res = true;
    fs::path p{path};
    //
    try
    {
        res = fs::remove(p);
    }
    catch(fs::filesystem_error &e)
    {
        RL_CORE_ERROR("[FileSystem] failed to delete {}, reason: {}", path, e.what());
        return false;
    }
    //
    if(!res)
        RL_CORE_WARN("[FileSystem] {} is not exist", path);
    return res;
}

bool FileSystem::FileExists(const std::string &path)
{
	return fs::exists(path);
}

int FileSystem::GetFileSize(const std::string &path)
{
	if (IsFile(path))
	{
		return static_cast<int>(fs::file_size(path));
	}
	else
	{
		RL_CORE_ERROR("{} is not a regular file", path.c_str());
		return 0;
	}
}

bool FileSystem::IsDirectory(const std::string &path)
{
    std::string p{Normalize(path)};
    //
    if(!IsAbsolutePath(p))
        p = AbsolutePath(p);
    //
    if(!FileExists(p))
        RL_CORE_WARN("[FileSystem] {} is not exist", path);
    //
	return fs::is_directory(p);
}

bool FileSystem::IsFile(const std::string &path)
{
    std::string p{Normalize(path)};
    //
    if(!IsAbsolutePath(p))
        p = AbsolutePath(p);
    //
    if(!FileExists(p))
        RL_CORE_WARN("[FileSystem] {} is not exist", path);
    //
	return fs::is_regular_file(p);
}

bool FileSystem::ReadFile(const std::string &path, void *buffer, int size)
{
	if (buffer == nullptr)
		return false;
	//
	std::ifstream fs(path, std::ios::binary);
	if (fs.is_open())
	{
		if (size == 0)
			size = GetFileSize(path);

		fs.read(static_cast<char *>(buffer), size);
		fs.close();
		return true;
	}
	return false;
}

Scope<char[]> FileSystem::ReadWholeFile(const std::string &path, size_t &siz)
{
	std::ifstream fs;
	fs.open(path, std::ios::binary);
	//
    siz = GetFileSize(path);
    Scope<char[]> buffer = MakeScope<char[]>(siz);
    //
	fs.read(buffer.get(), siz);
	fs.close();
    //
	return buffer;
}

std::string FileSystem::ReadTextFile(const std::string &path)
{
	std::ifstream fs;
	std::string context;
	fs.open(path);
	context = {std::istreambuf_iterator<char>(fs), {}};
	fs.close();

	return context;
}

bool FileSystem::WriteFile(const std::string &path, const char *buffer, int size)
{
	std::ofstream fs;
	bool success;
	fs.open(path, std::ios::binary);

	if (fs)
	{
		fs.write(buffer, size);
		success = true;
		fs.close();
	}
	else
	{
		RL_CORE_ERROR("FileSystemError: Error opening file {}", path);
		success = false;
	}

	RL_CORE_INFO("FileSystem: Success to write content to file {}", path);
	return success;
}

bool FileSystem::WriteTextFile(const std::string &path, const std::string &text, const int size)
{
	return WriteFile(path, text.c_str(), size);
}

bool FileSystem::List(const std::string &path, std::vector<File> &vec)
{
	for (const auto &entry : fs::directory_iterator(path))
		vec.emplace_back(entry.path());

	return !vec.empty();
}

std::string FileSystem::Normalize(const std::string &path)
{
    fs::path p;
    if(path.back() == '/' || path.back() == '\\')
        p.assign(path.begin(), path.end()-1);
    else
        p.assign(path.begin(), path.end());
    //
    p = p.lexically_normal();
    return p.string();
}

std::string FileSystem::AbsolutePath(const std::string &path)
{
    return fs::absolute(path).string();;
}

bool FileSystem::IsAbsolutePath(const std::string &path)
{
    return fs::path{path}.is_absolute();
}

std::string FileSystem::RelativePath(const std::string &path)
{
    std::string cur = GetCurrentDirectory() + "\\";
    std::string tar = path;
    return String::replaceFirst(tar, cur, "");
}

}