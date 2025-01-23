#include <StrikeEngine.h>
#include <StrikeEngine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace StrikeEngine {

	class StrikeEditor : public Application
	{
	public:
		StrikeEditor()
		{
			PushLayer(new EditorLayer());
		}

		~StrikeEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new StrikeEditor();
	}

}