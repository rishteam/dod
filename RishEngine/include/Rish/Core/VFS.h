/**
 * @file vfs.h
 * @author icejj (icejj@rish.com.tw), roy4801 (roy@rish.com.tw)
 * @brief Virtual File System
 * @version 0.1
 * @date 2020-05-02
 */
#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>

namespace rl {

/**
 * @brief Virtual File System class
 */
class RL_API VFS
{
private:
	/**
     * @brief Instance of VFS
     * @details The instance of VFS (singleton pattern)
     */
	static VFS *vfs_Instance;

	/**
     * @brief Mount points
     * @details The mapping from virtual paths to physical paths
     */
	std::unordered_map<std::string, std::vector<std::string>> m_MountPoints;
	// TODO: not unmount whole virtual path: change std::vector to std::list
public:
	/**
     * @brief Initialize the VFS
     */
	static void Init();
	/**
     * @brief Shutdown the VFS
     */
	static void ShutDown();
	/**
     * @brief Get the instance of VFS
     * @details Retrieve the pointer to the instance of VFS
     * @return VFS* Pointer to the VFS object
     */
	inline static VFS *Get() { return vfs_Instance; }

	/**
     * @brief Mount the path
     *
     * @warning After the mount, every time you want to access the file via virtual path should put a '/' front of the virtual path
     *
     * Mount physical path to virtual path, so that you can access the file
     * via virtual path without passing the physical path
     *
     * @param virtualPath Path that will use in VFS.
     * @param physicalPath Path to the real path.
     *
     * @include{'lineno'} snippet/mount_example.cpp
     *
     * @note Remember to Get() the instance before using the function
     *
     */
	void Mount(const std::string &virtualPath, const std::string &physicalPath);

	/**
     * @brief Unmount the path
     *
     * Unmount the virtual path, this will clear all the physical path that mount to the virtual path.
     * @param path Virtual path that will be unmount
     *
     * @code{.cpp}
     * VFS::Get()->Unmount("picture");
     * // clear all the path
     * @endcode
     */
	void Unmount(const std::string &path);

	/**
     * @brief Resolve the physicalPath
     *
     * Get the physical path of the virtual path, and return a bool value to check if success to find the path.
     * Physical path will be store in outphysicalPath
     * @note You can also pass the physical path to the function
     * @warning If you get ‘null’ constanly, please check two things
     *  1. The physical path that mount to the virtual path is correct
     *  2. Check if you have place a ‘/’ when you try to access the virtual path
     *      - Correct: `VFS::Get()->ResolvePhysicalPath("/xxxx/xxx.txt", str)`
     *      - Wrong: `VFS::Get()->ResolvePhysicalPath(“xxxx/xxx.txt”, str)`
     *
     * @include{'lineno'} snippet/resolve_example.cpp
     *
     * @param[in] path Virtual Path
     * @param[out] outphysicalPath Physical Path
     * @return true Succeeded to resolve the physicalPath
     * @return false Failed to resolve the physicalPath
     */
     // TODO: Remove the file existance check in ResolvePhysicalPath()
	bool ResolvePhysicalPath(const std::string &path, std::string &outphysicalPath);

	/**
	 * @brief Check if the file exists
	 * @param path Virtual Path
	 * @return true Exists
	 * @return false Not exists
	 *
	 * @code{.cpp}
	 * if(VFS::Get()->FileExists("/virtualDir/path/to/the/file.ext"))
	 * {
	 *     // Do something
	 * }
	 * @endcode
	 */
	bool FileExists(const std::string &virtualPath);

	/**
     * @brief Read the file in binary mode
     * @warning This will return a new allocated `char*` \n
     *          Remeber to *DELETE* on your own
     * @param path Path to the file in VFS
     * @return char* File content
     */
	char *ReadFile(const std::string &path);
	// TODO: return length?

	/**
     * @brief Read the file in text mode
     * @details Read all bytes from the file in text mode
     * @param path Path to the file in VFS
     * @return std::string File content
     */
	std::string ReadTextFile(const std::string &path);

	/**
     * @brief Write the file in binary mode
     *
     * @note If the file is not exist, the it won’t generate a file.\n
     *        You should create the file before you access
     *
     * @param path Path to the file in VFS
     * @param buffer The buffer contains bytes will be written into file
     * @param size The length of data
     *
     * @return true Succeed to write the file
     * @return false Failed to write the file
     */
	bool WriteFile(const std::string &path, const char *buffer, const int size);

	/**
     * @brief Write the file in text mode
     *
     * @note If the file is not exist, the it won’t generate a file.\n
     *       You should create the file before you access
     *
     * @param path Path to the file in VFS
     * @param text The buffer contains bytes will be written into file
     * @param size The length of data
     * @return true Succeed to write the file
     * @return false Failed to write the file
     */
	bool WriteTextFile(const std::string &path, const std::string &text, const int size);
};

} // namespace rl