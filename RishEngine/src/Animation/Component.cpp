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
    currentFrame++;
    if(currentFrame >= textureList.size())
        currentFrame %= textureList.size();
}

Ref<Texture2D> Animation2DComponent::getCurrentFrame()
{
    return textureList[currentFrame];
}

} // end of namespace rl