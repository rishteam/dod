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
    if(loop)
    {
        if(reverse)
        {
            currentFrame--;
            if (currentFrame < 0)
                currentFrame = !textureList.empty() ? textureList.size() - 1 : 0;
        }
        else
        {
            currentFrame++;
            if (currentFrame >= textureList.size())
                currentFrame %= textureList.size();
        }
    }
}

Ref<Texture2D> Animation2DComponent::getCurrentFrame()
{
    return !textureList.empty() ? textureList[currentFrame] : nullptr;
}

} // end of namespace rl