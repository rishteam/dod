#include "SandboxLayer.h"

ExampleSandboxLayer::ExampleSandboxLayer()
    : Layer("ExampleSandboxLayer"),
      m_cameraController(Application::Get().getWindow().getAspectRatio())
{
    RL_TRACE("Current path is {}", rl::FileSystem::GetCurrentDirectoryPath());
    rl::VFS::Mount("shader", "Sandbox/assets");
    rl::VFS::Mount("texture", "Sandbox/assets");

    ImGui::LoadIniSettingsFromDisk("Sandbox/imgui.ini");

}

void ExampleSandboxLayer::onAttach()
{
    auto cur = rl::FileSystem::GetCurrentDirectoryPath();
    m_scene = rl::MakeRef<rl::Scene>();
    m_world = rl::MakeRef<rl::PhysicsWorld>(Vec2(0.0f, -9.8f), 800.0f, 600.0f);
}

void ExampleSandboxLayer::onDetach()
{
    ImGui::SaveIniSettingsToDisk("Sandbox/imgui.ini");
}

void ExampleSandboxLayer::onUpdate(rl::Time dt)
{
    m_cameraController.onUpdate(dt);
    Renderer2D::BeginScene(m_cameraController.getCamera());
    m_world->timeStep = dt;

    //simulate
    m_world->Step(dt);

    //draw box
    for(auto obj : m_world->bodies)
    {
        Renderer2D::DrawRotatedQuad({obj->position.x, obj->position.y}, { obj->wh.x, obj->wh.y}, {1, 0, 0, 0.5}, glm::degrees(obj->angle));
    }
    for(auto jit : m_world->joints)
    {
        Mat22 R1(jit->b1->angle);
        Mat22 R2(jit->b2->angle);

        Vec2 x1 = jit->b1->position;
        Vec2 p1 = x1 + R1 * jit->localAnchor1;

        Vec2 x2 = jit->b2->position;
        Vec2 p2 = x2 + R2 * jit->localAnchor2;

        Renderer2D::DrawLine({x1.x, x1.y}, {p1.x, p1.y}, {1, 1, 1, 0.5});
        Renderer2D::DrawLine({x2.x, x2.y}, {p2.x, p2.y}, {1, 1, 1, 0.5});
    }

    Renderer2D::EndScene();
}

void ExampleSandboxLayer::onImGuiRender()
{
    ImGui::Begin("Debug");
    ImGui::Text("FPS = %d", rl::Application::Get().getFps());
    ImGui::InputFloat("X", &x, 1.0f, 10.0f, "%.3f");
    ImGui::InputFloat("Y", &y, 1.0f, 10.0f, "%.3f");
    ImGui::InputFloat("W", &w, 1.0f, 10.0f, "%.3f");
    ImGui::InputFloat("H", &h, 1.0f, 10.0f, "%.3f");
    ImGui::InputFloat("Mass", &m, 1.0f, 10.0f, "%.3f");
    if (ImGui::Button("New box"))
    {
        m_world->Add(rl::MakeRef<RigidBody2D>(Vec2(x, y), Vec2(w, h), m));
    }
    if (ImGui::Button("Demo1: A single box")) {
        m_world->demo1();
    }
    if (ImGui::Button("Demo2: Simple Pendulum")) {
        m_world->demo2();
    }
    if (ImGui::Button("Demo3: Varying Friction coefficients")) {
        m_world->demo3();
    }
    if (ImGui::Button("Demo4: Randomized Stacking")) {
        m_world->demo4();
    }
    if (ImGui::Button("Demo5: Pyramid Stacking")){
        m_world->demo5();
    }
    if (ImGui::Button("Demo6: A Teeter")){
        m_world->demo6();
    }
    if (ImGui::Button("Demo7: A Suspension Bridge")){
        m_world->demo7();
    }
    if (ImGui::Button("Demo8: Dominos")){
        m_world->demo8();
    }
    if (ImGui::Button("Demo9: Multi - pendulum")){
        m_world->demo9();
    }
    if (ImGui::Button("Clear")) {
        m_world->Clear();
    }


    ImGui::Separator();
    ImGui::Checkbox("AccumulateImpulses", &m_world->accumulateImpulses);
    ImGui::SameLine();
    ImGui::Checkbox("WarmStarting", &m_world->warmStarting);
    ImGui::SameLine();
    ImGui::Checkbox("positionCorrection", &m_world->positionCorrection);

    if (ImGui::CollapsingHeader("Box's Data"))
    {
        for(int i = 0; i < m_world->bodies.size(); i++)
        {
            Ref<RigidBody2D> box = m_world->bodies.at(i);
            ImGui::Text("Box%d:", i);
            ImGui::Text("[Physics] Center: (%f, %f)", box->getPhysicsData().first.x, box->getPhysicsData().first.y);
            ImGui::Text("[Physics] width:%f, height:%f", box->getwh().x, box->getwh().y);
            ImGui::Text("[Physics] mass: %f", box->getMass());
            ImGui::Text("[Physics] angle: %f", box->getPhysicsData().second );
            ImGui::Text("[Physics] friction: %f", box->getfriction());
            ImGui::Separator();
        }
    }

    if (ImGui::CollapsingHeader("Arbiters"))
    {
        ImGui::Text("Arbiters size: %d", m_world->arbiters.size());
        for(auto arbiter : m_world->arbiters)
        {
            ImGui::Text("Arbiters: Contact %d", arbiter.second.numContacts);
            ImGui::Separator();
        }
    }



    ImGui::End();
    m_cameraController.onImGuiRender();
}

void ExampleSandboxLayer::onEvent(rl::Event &event)
{
    m_cameraController.onEvent(event);
}
