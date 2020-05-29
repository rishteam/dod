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

	/// Push Layer from m_LayerInsertIndex
	void pushLayer(Layer* layer);
	/// Push Layer from last
	void pushOverlay(Layer* overlay);
	/// Pop Layer
	void popLayer(Layer* layer);
	/// Pop Overlay
	void popOverlay(Layer* overlay);
	// typedefs
	typedef std::vector<Layer*>::iterator iterator;
	typedef std::vector<Layer*>::reverse_iterator reverse_iterator;
	typedef std::vector<Layer*>::const_iterator const_iterator;
	typedef std::vector<Layer*>::const_reverse_iterator const_reverse_iterator;
	// iterator
	iterator begin() { return m_Layers.begin(); }
	iterator end() { return m_Layers.end();   }
	reverse_iterator rbegin() { return m_Layers.rbegin(); }
	reverse_iterator rend() { return m_Layers.rend(); }
	// const iterator
	const_iterator begin() const { return m_Layers.begin(); }
	const_iterator end() const { return m_Layers.end();   }
	const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
	const_reverse_iterator rend() const { return m_Layers.rend(); }
private:
	/// a vector of layer*
	std::vector<Layer*> m_Layers;
	/// start index of inserting a layer
	unsigned int m_LayerInsertIndex;
};

}
