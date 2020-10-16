#pragma once

#include <Rish/rlpch.h>
#include <RishEngine.h>

using namespace rl;

class ScriptTestLayer : public rl::Layer
{
public:
    ScriptTestLayer();
    ~ScriptTestLayer() override;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(rl::Time dt) override;
    void onImGuiRender() override;
    void onEvent(rl::Event& event) override;

private:
    Ref<Scene> m_scene;
    Ref<Texture2D> m_testSprite;

    bool onKeyPressEvent(KeyPressedEvent &event);
};
