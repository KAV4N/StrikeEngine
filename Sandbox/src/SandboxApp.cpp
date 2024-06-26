#include <StrikeEngine.h>

class Sandbox : public StrikeEngine::Application {

public:
	Sandbox() {

	}
	~Sandbox() {

	}
};

StrikeEngine::Application* StrikeEngine::CreateApplication() {
	return new Sandbox();
}