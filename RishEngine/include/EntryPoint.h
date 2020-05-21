#pragma once

extern rl::Application* rl::CreateApplication();

int main(int argc, char **argv)
{
    auto app = rl::CreateApplication();
    app->run();
    delete app;
}

