#include <iostream>
#include <string>

#include "resManager.h"
#include "log.h"
#include "animation/animation.h"
#include "animation/loader.h"

namespace rl
{

Animation AnimationLoader::loadFromString(const std::string &str)
{
    nlo::json json = nlo::json::parse(str);
    return loadFromJson(json);
}

Animation AnimationLoader::loadFromJson(const nlo::json &json)
{
    Animation ani, empty;
    bool isLoop;
    // load the animation name
    auto name = json["name"].get<std::string>();
    ani.m_texName = name;
    // load number of frames
    int number = json["number"].get<int>();
    ani.m_count = number;
    // load texture path
    std::string texPath = json["rootPath"].get<std::string>();
    // load all frames
    std::vector<std::string> fileList;
    auto frames = json["frames"];
    std::string frameType = frames["type"];
    if(frameType == "separate")
    {
        fileList = frames["separate"].get<std::vector<std::string>>();

        int goodCnt = 0;
        for(int i = 0; i < fileList.size(); i++)
        {
            std::string targetName = fmt::format("{}_{}", ani.m_texName, i);
            std::string filePath = fmt::format("{}{}", texPath, fileList[i]);
            // success to load
            if (ResManager::loadRes(ResType::ResTexture, targetName, filePath))
            {
                auto &tex = ResManager::getTexture(targetName);
                ani.addFrame(tex);
                goodCnt++;
            }
        }
        // if failed, release the textures
        if(goodCnt != ani.m_count)
        {
            for (int i = 0; i < fileList.size(); i++)
            {
                std::string targetName = fmt::format("{}_{}", ani.m_texName, i);
                if (!ResManager::releaseRes(ResType::ResTexture, targetName))
                {
                    RL_ERROR("Failed to release texture {}\n", targetName);
                }
            }
            goto FAIL;
        }
    }
    // load m_loop
    isLoop = json["loop"].get<bool>();
    ani.m_loop = isLoop;

    ani.ready = true;

    std::cout << ani << '\n';

    return ani;
FAIL:
    return empty;
}

std::string AnimationLoader::dumpFromObject(const Animation &obj)
{

}

}