#pragma once

#include "Application.h"
#include "StrikeEngine/Core/Log.h"

extern StrikeEngine::Application* StrikeEngine::createApplication();

int main(int argc, char** argv) {
    StrikeEngine::Log::init();

    auto app = StrikeEngine::createApplication();
    app->run();
    delete app;
    return 0;
}
