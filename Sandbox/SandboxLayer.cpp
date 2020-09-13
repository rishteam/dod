#include "SandboxLayer.h"

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("ExampleSandboxLayer")
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    rl::VFS::Mount("shader", "assets");
    rl::VFS::Mount("texture", "assets");
}

void ExampleSandboxLayer::onAttach()
{
    auto cur = rl::FileSystem::GetCurrentDirectoryPath();
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
