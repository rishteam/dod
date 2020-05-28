/**
 * @file FileDialog.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of FileDialog
 * @date 2020-05-28
 */
#pragma once

#include <string>
#include <vector>

// TODO: Add example for hot to use filter and defaultPath

namespace rl {

/**
 * @brief File Dialog class
 * @details Providing file dialog utilities
 */
class FileDialog
{
public:
    /**
     * @brief Open single file
     * 
     * @param filter 
     * @param defaultPath 
     * @param outPath 
     * @return true 
     * @return false 
     */
    static bool SelectSingleFile(const char *filter, const char *defaultPath, std::string &outPath);

    /**
     * @brief Open multiple files
     * 
     * @param filter 
     * @param defaultPath 
     * @param outPathList 
     * @return true 
     * @return false 
     */
    static bool SelectMultipleFile(const char *filter, const char *defaultPath, std::vector<std::string> &outPathList);

    /**
     * @brief Save single file
     * 
     * @param filter 
     * @param defaultPath 
     * @param outPath 
     * @return true 
     * @return false 
     */
    static bool SelectSaveFile(const char *filter, const char *defaultPath, std::string &outPath);

    /**
     * @brief Open single folder
     * 
     * @param defaultPath 
     * @param outPath 
     * @return true 
     * @return false 
     */
    static bool SelectFolder(const char *defaultPath, std::string &outPath);
};

} // end namespace rl

// Usage of SelectSingleFile()
// std::string path;
// if (FileDialog::SelectSingleFile(nullptr, nullptr, path))
// {
//     cout << path << '\n';
// }

// Usage of SelectMultipleFile()
// std::vector<std::string> pathList;
// if (FileDialog::SelectMultipleFile(nullptr, nullptr, pathList))
// {
//     for (auto &p : pathList)
//         std::cout << p << '\n';
// }

// Usage of SelectSaveFile()
// std::string path;
// if (FileDialog::SelectSaveFile(nullptr, nullptr, path))
// {
//     std::cout << path << '\n';
// }

// Usage of SelectFolder()
// std::string path;
// if (FileDialog::SelectFolder(nullptr, path))
// {
//     std::cout << path << '\n';
// }