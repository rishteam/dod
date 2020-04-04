#include <string>
#include <vector>
#include <string>

#include <fmt/format.h>

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
    ani.m_texName = json["name"].get<std::string>();
    // load number of frames
    ani.m_count = json["number"].get<int>();
    // load m_loop
    ani.loop = json["loop"].get<bool>();
    // load durations
    ani.duration = json["duration"].get<float>();
    ani.reverseDuration = json["reverseDuration"].get<float>();
    // load all frames
    std::vector<std::string> fileList;
    auto frames = json["frames"];
    std::string frameType = frames["type"];
    if(frameType == "separate")
    {
        ani.m_loadType = Animation::LoadType::AniLoadSeparate;
        // Load frames
        fileList = frames["separate"].get<std::vector<std::string>>();
        int goodCnt = 0;
        for(int i = 0; i < fileList.size(); i++)
        {
            std::string targetName = fmt::format("{}_{}", ani.m_texName, i);
            std::string filePath = fileList[i];
            // success to load
            if (ResManager::loadRes(ResType::ResTexture, targetName, filePath))
            {
                auto &tex = ResManager::getTexture(targetName);
                ani.addFrame(tex);
                goodCnt++;
            }
        }
        // if failed, release the textures
        if (goodCnt != fileList.size())
        {
            for (int i = 0; i < fileList.size(); i++)
            {
                std::string targetName = fmt::format("{}_{}", ani.m_texName, i);
                if (!ResManager::releaseRes(ResType::ResTexture, targetName))
                    RL_ERROR("Failed to release texture {}\n", targetName);
            }
            goto FAIL;
        }
    }
    else if (frameType == "spritesheet")
    {
        ani.m_loadType = Animation::LoadType::AniLoadSpriteSheet;
        RL_ASSERT(false, "Not implement yet");
    }

    ani.m_ready = true;
    // DEBUG
    RL_DEBUG("{}", dumpFromObject(ani));

    return ani;
FAIL:
    return empty;
}

std::string AnimationLoader::dumpFromObject(const Animation &obj)
{
    nlo::json json;
    json["name"] = obj.m_texName;
    json["number"] = obj.m_count;
    json["loop"] = obj.loop;
    json["reverse"] = obj.reverse;
    json["frames"] = nlo::json::object();

    if(obj.m_loadType == Animation::LoadType::AniLoadSeparate)
    {
        json["frames"]["type"] = "separate";
        std::vector<std::string> pathList;
        for(int i = 0; i < obj.m_count; i++)
            pathList.push_back(ResManager::getResPath(ResType::ResTexture, fmt::format("{}_{}", obj.m_texName, i)));
        json["frames"]["separate"] = pathList;
    }
    else if (obj.m_loadType == Animation::LoadType::AniLoadSpriteSheet)
    {
        RL_ASSERT(false, "Not implement yet");
    }
    else
    {
        RL_ASSERT(false, "Wrong animation load type");
    }

    return json.dump(4);
}

}