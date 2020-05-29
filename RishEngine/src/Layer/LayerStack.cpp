#include "Rish/Layer/LayerStack.h"

namespace rl {

LayerStack::LayerStack()
{
	m_LayerInsertIndex = 0;
}

LayerStack::~LayerStack()
{
	for(Layer* layer: m_Layers)
	{
		layer->onDetach();
		delete layer;
	}
}

void LayerStack::pushLayer(Layer* layer)
{
	m_Layers.emplace(begin() + m_LayerInsertIndex, layer);
	layer->onAttach();
	m_LayerInsertIndex++;
}

void LayerStack::pushOverlay(Layer* overlay)
{
	m_Layers.emplace_back(overlay);
	overlay->onAttach();
}

void LayerStack::popLayer(Layer* layer)
{
	auto it = std::find(begin(), begin() + m_LayerInsertIndex, layer);
	if(it != begin() + m_LayerInsertIndex)
	{
		layer->onDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
}

void LayerStack::popOverlay(Layer* overlay)
{
	auto it = std::find(begin() + m_LayerInsertIndex, end(), overlay);
	if(it != end())
	{
		overlay->onDetach();
		m_Layers.erase(it);
	}
}

}
