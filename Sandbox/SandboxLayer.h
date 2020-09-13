#pragma once

#include <Rish/rlpch.h>
#include <RishEngine.h>

class ExampleSandboxLayer : public rl::Layer
{
public:
    ExampleSandboxLayer();

    void onAttach() override;
    void onDetach() override;
    void onUpdate(rl::Time dt) override;
    void onImGuiRender() override;
    void onEvent(rl::Event& event) override;

private:

};
