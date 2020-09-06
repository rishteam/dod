#include "SandboxLayer.h"

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("ExampleSandboxLayer")
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");
}

void ExampleSandboxLayer::onAttach()
{
    auto cur = rl::FileSystem::GetCurrentDirectoryPath();

    rl::FileSystem::CreateFile("assets/test.1");
    rl::FileSystem::CreateFile((cur / "assets/test.2").string());

    rl::FileSystem::CreateFile("assets/");

    rl::FileSystem::DeleteFile("assets/test.1");
    rl::FileSystem::DeleteFile("assets/test.3");

    size_t size;
    auto buf = rl::FileSystem::ReadWholeFile("assets/test.bin", size);
    for(int i = 0; i < size; i++)
        printf("%hhx ", buf[i]);

    std::vector<rl::File> fileList;
    rl::FileSystem::List("assets/", fileList);

    for(auto &i : fileList)
        printf("%s\n", i.getFilename().c_str());
}

void ExampleSandboxLayer::onDetach()
{

}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{

}

void ExampleSandboxLayer::onImGuiRender()
{

}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
}
