/**
 * @file vfs.h
 * @author icejj (icejj@rish.com.tw), roy4801 (me@roy4801.tw)
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
	static VFS *instance;

	/**
     * @brief Mount points
     * @details The mapping from virtual paths to physical paths
     */
	static std::unordered_map<std::string, std::vector<std::string>> m_MountPoints;
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
     * @brief Mount the path
     *
     * @warning Use Before Initialize the VFS
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
	static void Mount(const std::string &virtualPath, const std::string &physicalPath);

	/**
     * @brief Unmount the path
     *
     * Unmount the virtual path, this will clear all the physical path that mount to the virtual path.
     * @param path Virtual path that will be unmount
     * @warning Use Before Initialize the VFS
     * @code{.cpp}
     * VFS::Unmount("picture");
     * // clear all the path
     * @endcode
     */
    static void Unmount(const std::string &path);

	/**
     * @brief Resolve the physicalPath
     *
     * Get the physical path of the virtual path, and return a bool value to check if success to find the path.
     * Physical path will be store in outphysicalPath
     * @warning Use Before Initialize the VFS
     * @note You can also pass the physical path to the function
     * @warning If you get ‘null’ constantly, please check two things
     *  1. The physical path that mount to the virtual path is correct
     *  2. Check if you have place a ‘/’ when you try to access the virtual path
     *      - Correct: `VFS::ResolvePhysicalPath("/xxxx/xxx.txt", str)`
     *      - Wrong: `VFS::ResolvePhysicalPath(“xxxx/xxx.txt”, str)`
     *
     * @include{'lineno'} snippet/resolve_example.cpp
     *
     * @param[in] path Virtual Path
     * @param[out] outphysicalPath Physical Path
     * @return true Succeeded to resolve the physicalPath
     * @return false Failed to resolve the physicalPath
     */
     // TODO: Remove the file existance check in ResolvePhysicalPath()
     static bool ResolvePhysicalPath(const std::string path, std::string &outphysicalPath);

	/**
	 * @brief Check if the file exists
	 * @param path Virtual Path
	 * @return true Exists
	 * @return false Not exists
	 *
	 * @warning Use Before Initialize the VFS
	 *
	 * @code{.cpp}
	 * if(VFS::FileExists("/virtualDir/path/to/the/file.ext"))
	 * {
	 *     // Do something
	 * }
	 * @endcode
	 */
    static bool FileExists(const std::string &virtualPath);

	/**
     * @brief Read the file in binary mode
     * @warning Use Before Initialize the VFS
     * @warning This will return a new allocated `char*` \n
     *          Remeber to *DELETE* on your own
     * @param path Path to the file in VFS
     * @return char* File content
     */
    static Scope<char[]> ReadWholeFile(const std::string &path, size_t &siz);
	// TODO: return length?

	/**
     * @brief Read the file in text mode
     * @details Read all bytes from the file in text mode
     * @warning Use Before Initialize the VFS
     * @param path Path to the file in VFS
     * @return std::string File content
     */
    static std::string ReadTextFile(const std::string &path);

	/**
     * @brief Write the file in binary mode
     *
     * @warning Use Before Initialize the VFS
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
    static bool WriteFile(const std::string &path, const char *buffer, const int size);

	/**
     * @brief Write the file in text mode
     *
     * @warning Use Before Initialize the VFS
     * @note If the file is not exist, the it won’t generate a file.\n
     *       You should create the file before you access
     *
     * @param path Path to the file in VFS
     * @param text The buffer contains bytes will be written into file
     * @param size The length of data
     * @return true Succeed to write the file
     * @return false Failed to write the file
     */
    static bool WriteTextFile(const std::string &path, const std::string &text, const int size);
};

} // namespace rl