#include <Rish/Script/SpriteRotate.h>

namespace rl {

void SpriteRoatate::onUpdate(rl::Time dt)
{
    auto &trans = GetComponent<TransformComponent>();
    trans.rotate += 100.f * dt.asSeconds();
    trans.rotate = std::fmod(trans.rotate, 360.f);
}

}