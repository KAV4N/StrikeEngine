#pragma once

#ifdef PLATFORM_WINDOWS

extern StrikeEngine::Application* StrikeEngine::CreateApplication();

int main(int argc, char** argv) 
{

	StrikeEngine::Log::Init();
	STRIKE_CORE_WARN("INIT");
	int a = 5;
	STRIKE_INFO("TEST Var={0}", a);

	auto app = StrikeEngine::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif
