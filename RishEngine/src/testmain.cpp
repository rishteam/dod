#include <iostream>
#include "config.h"

#include "Rish/Core/Log.h"
//#include <spdlog/spdlog.h>
//#include <spdlog/sinks/stdout_color_sinks.h>

using namespace std;
using namespace rl;

int main()
{
     Logger::Init();

     RL_ERROR("shit");
     RL_CORE_CRITICAL("Fuck me");

}