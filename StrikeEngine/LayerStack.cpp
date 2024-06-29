#include "strikepch.h"
#include "LayerStack.h"

namespace StrikeEngine {
	LayerStack::LayerStack() 
	{
		m_LayerInsert = m_Layers.begin();
	};

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			delete layer;
		}
	};

	void LayerStack::PushLayer(Layer* layer)
	{
	};


	void  LayerStack::PushOverlay(Layer* layer)
	{
	};
	
	void  LayerStack::PopLayer(Layer* layer)
	{
	};
	
	void  LayerStack::PopOverlay(Layer* layer) 
	{
	};

}