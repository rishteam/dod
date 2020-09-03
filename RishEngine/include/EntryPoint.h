/**
 * @file EntryPoint.h
 * @brief The Entry Point of RishEngine Applications
 * @author roy4801 (me@roy4801.tw)
 */
#pragma once

#define LOG_VERBOSE 0

extern rl::Application* rl::CreateApplication();

int main(int argc, char **argv)
{
    RL_UNUSED(argc);
    RL_UNUSED(argv);
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
    //
#if RL_ATTACH
    puts("Attach?");
    getchar();
#endif
    //
#if LOG_VERBOSE
    auto LoggerSetting = rl::Logger::LoggerType::IgnoreDup | rl::Logger::LoggerType::FileInfo | rl::Logger::LoggerType::FunctionInfo;
#else
    auto LoggerSetting = rl::Logger::LoggerType::IgnoreDup;
#endif
    rl::Logger::Init(LoggerSetting);
    rl::VFS::Init();

    RL_CORE_INFO("Initializating the RishEngine");
    RL_PROFILE_BEGIN_SESSION("Create", "rl-create.json");
    auto app = rl::CreateApplication();
    RL_PROFILE_END_SESSION();

    RL_CORE_INFO("RishEngine is running");
    RL_PROFILE_BEGIN_SESSION("Running", "rl-running.json");
    app->run();
    RL_PROFILE_END_SESSION();

    RL_CORE_INFO("Shutdowning the RishEngine");
    RL_PROFILE_BEGIN_SESSION("Delete", "rl-delete.json");
    delete app;
    rl::VFS::ShutDown();
    RL_PROFILE_END_SESSION();
    RL_CORE_INFO("Goodbye");
}
