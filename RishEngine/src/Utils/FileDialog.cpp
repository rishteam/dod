#include <nfd.h>

#include "Rish/Core/Log.h"
#include "Rish/Utils/FileDialog.h"

namespace rl {

bool FileDialog::SelectSingleFile(const char *filter, const char *defaultPath, std::string &outPath)
{
    nfdchar_t *nfdchar_outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(filter, defaultPath, &nfdchar_outPath);

    if (result == NFD_ERROR)
    {
        RL_ERROR("FileDialog Error: {}", std::string(NFD_GetError()));
        return false;
    }

    if(result == NFD_OKAY)
    {
        outPath.assign(nfdchar_outPath);
        free(nfdchar_outPath);
        return true;
    }
    else
        return false;
}

bool FileDialog::SelectMultipleFile(const char *filter, const char *defaultPath, std::vector<std::string> &outPathList)
{
    nfdpathset_t pathSet;
    nfdresult_t result = NFD_OpenDialogMultiple(filter, defaultPath, &pathSet);

    if(result == NFD_ERROR)
    {
        RL_ERROR("FileDialog Error: {}", std::string(NFD_GetError()));
        return false;
    }

    if (result == NFD_OKAY)
    {
        size_t count = NFD_PathSet_GetCount(&pathSet);
        for (size_t i = 0; i < count; ++i)
        {
            nfdchar_t *path = NFD_PathSet_GetPath(&pathSet, i);
            outPathList.emplace_back(path);
        }
        NFD_PathSet_Free(&pathSet);
        return true;
    }
    else
        return false;
}

bool FileDialog::SelectSaveFile(const char *filter, const char *defaultPath, std::string &outPath)
{
    nfdchar_t *savePath = nullptr;
    nfdresult_t result = NFD_SaveDialog(filter, defaultPath, &savePath);

    if(result == NFD_ERROR)
    {
        RL_ERROR("FileDialog Error: {}", std::string(NFD_GetError()));
        return false;
    }

    if(result == NFD_OKAY)
    {
        outPath.assign(savePath);
        free(savePath);
        return true;
    }
    else
        return false;
}

bool FileDialog::SelectFolder(const char *defaultPath, std::string &outPath)
{
    nfdchar_t *nfdchar_outPath = nullptr;
    nfdresult_t result = NFD_PickFolder(defaultPath, &nfdchar_outPath);

    if(result == NFD_ERROR)
    {
        RL_ERROR("FileDialog Error: {}", std::string(NFD_GetError()));
        return false;
    }

    if(result == NFD_OKAY)
    {
        outPath.assign(nfdchar_outPath);
        free(nfdchar_outPath);
        return true;
    }
    else
        return false;
}


} // end namespace rl