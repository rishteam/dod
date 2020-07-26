#include "Rish/Core/FileSystem.h"

namespace rl {

File::File(const std::string &path) : m_path(path)
{
	m_file = fs::is_regular_file(path);
	m_dir = fs::is_directory(path);
}

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

void FileSystem::CreateFile(const std::string &path)
{
	std::ofstream file{
		path};
	RL_CORE_INFO("FileSystem: File create in {}", path.c_str());
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
	return (FileExists(path) ? fs::is_directory(path) : false);
}

bool FileSystem::IsFile(const std::string &path)
{
	return (FileExists(path) ? fs::is_regular_file(path) : false);
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

char *FileSystem::ReadFile(const std::string &path)
{
	std::ifstream fs;
	fs.open(path, std::ios::binary);
	int size = GetFileSize(path);
	char *buffer = new char[size];
	fs.read(buffer, size);
	fs.close();

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
	{

		File tmp(entry.path().string());
		vec.push_back(tmp);
	}

	if (vec.size())
		return true;
	else
	{
		RL_CORE_INFO("FileSystem: No file in {}");
		return false;
	}
}

}