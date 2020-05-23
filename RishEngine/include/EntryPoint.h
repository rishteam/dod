#pragma once

extern rl::Application* rl::CreateApplication();

int main(int argc, char **argv)
{
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
    //
    rl::Logger::Init();

    RL_CORE_INFO("Starting the RishEngine");
    auto app = rl::CreateApplication();

    RL_CORE_INFO("RishEngine is running");
    app->run();

    RL_CORE_INFO("Deleting the RishEngine");
    delete app;
}
