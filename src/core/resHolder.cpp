#include "core/resHolder.h"

namespace rl {

ResHolder& ResHolder::Get()
{
    static ResHolder holder;
    return holder;
}

ResHolder::ResHolder()
    :   texture(),
        image(),
        font(),
        music(),
        sound()
{

}

} // end namespace rl
