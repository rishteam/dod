#include <Rish/Animation/Component.h>

namespace rl {

bool Animation2DComponent::isTimerUp()
{
    if(!textureList.empty())
        return clk.getElapsedTime() > duration / (float)textureList.size();
    else
        return false;
}

void Animation2DComponent::resetTimer()
{
    clk.restart();
}

void Animation2DComponent::playNextFrame()
{
    if(reverse)
        currentFrame--;
    else
        currentFrame++;

    if(loop)
    {
        if (reverse)
        {
            if (currentFrame < 0)
                currentFrame = !textureList.empty() ? textureList.size() - 1 : 0;
        }
        else
        {
            if (currentFrame >= textureList.size())
                currentFrame = 0;
        }
    }
    else
    {
        if (reverse)
        {
            if (currentFrame < 0)
                currentFrame = 0;
        }
        else
        {
            if (currentFrame >= textureList.size())
                currentFrame = !textureList.empty() ? textureList.size() - 1 : 0;
        }
    }
}

Ref<Texture2D> Animation2DComponent::getCurrentFrame()
{
    return !textureList.empty() ? textureList[currentFrame] : nullptr;
}

void Animation2DComponent::play()
{
    playing = true;
    stopped = false;
}

void Animation2DComponent::stop()
{
    playing = false;
    stopped = true;

    if(reverse)
        currentFrame = !textureList.empty() ? textureList.size() - 1 : 0;
    else
        currentFrame = 0;
}

void Animation2DComponent::pause()
{
    playing = false;
    stopped = false;
}

void Animation2DComponent::toggle()
{
    if(isPlay())
        stop();
    else if(isStop())
        play();
}

bool Animation2DComponent::isPlay()
{
    return playing && !stopped;
}

bool Animation2DComponent::isStop()
{
    return !playing && stopped;
}

bool Animation2DComponent::isPause()
{
    return !playing && !stopped;
}


} // end of namespace rl