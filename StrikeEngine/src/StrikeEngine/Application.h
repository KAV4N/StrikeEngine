#pragma once

#include "Core.h"


namespace StrikeEngine {


	class STRIKE_API Application {
	
	public: 
		Application();
		virtual ~Application();

		void Run();
	
	};

	Application* CreateApplication();
}



