/**
 * @file LayerStack.h
 * @author icejj (icejj@rish.com.tw), roy4801 (roy@rish.com.tw)
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
	// iterator
	std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_Layers.end();   }
	std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
	std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }
	// const iterator
	std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
	std::vector<Layer*>::const_iterator end() const { return m_Layers.end();   }
	std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
	std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }
private:
	std::vector<Layer*> m_Layers;
	std::vector<Layer*>::iterator m_LayerInsert;

};

}
