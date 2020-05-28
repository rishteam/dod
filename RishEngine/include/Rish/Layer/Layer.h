#pragma once

#include "Rish/Core/Core.h"

#include "Rish/Events/Event.h"
#include "Rish/rlpch.h"

namespace rl {


class RL_API Layer
{
public:
	Layer(const std::string& debugname = "Layer");
	virtual ~Layer();

	virtual void onAttach() {};
	virtual void onDetach() {};
	virtual void onUpdate() {};
	virtual void onEvent(Event& event) {};

	inline const std::string& GetName() const { return m_DebugName; }
protected:
	std::string m_DebugName;

};


}
