#pragma once

#ifdef PLATFORM_WINDOWS

extern StrikeEngine::Application* StrikeEngine::CreateApplication();

int main(int argc, char** argv) 
{

	StrikeEngine::Log::Init();

	auto app = StrikeEngine::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif
