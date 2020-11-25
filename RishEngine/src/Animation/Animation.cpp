#include <fstream>

#include "Rish/Animation/Animation.h"

#include "Rish/Core/ResManager.h"
#include "Rish/Core/Log.h"

namespace rl {

// TODO: change the end point to loop points
bool Animation::isEnd()
{
    RL_ASSERT(m_ready, "Animation is not ready");
    if(loop) return false;

    if(reverse)
        return m_nowFrame == 0;
    else
        return m_nowFrame == m_count-1;
}

}