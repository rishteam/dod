/**
 * @file LayerStack.h
 * @author icejj (icejj@rish.com.tw)
 * @brief Header of LayerStack
 * @date 2020-05-29
 */
#pragma once

#include "Rish/Core/Core.h"

#include "Rish/Layer/Layer.h"
#include "Rish/rlpch.h"

namespace rl {

/**
 * @brief Layer Stack class
 * @details A list of layer for drawing on screen and interactive with
 */
class RL_API LayerStack
{
public:
	LayerStack();
	~LayerStack();

	void pushLayer(Layer* layer);
	void pushOverlay(Layer* overlay);
	void popLayer(Layer* layer);
	void popOverlay(Layer* overlay);

	std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	std::vector<Layer*>::iterator end()   { return m_Layers.end();   }
private:
	std::vector<Layer*> m_Layers;
	std::vector<Layer*>::iterator m_LayerInsert;

};

}
