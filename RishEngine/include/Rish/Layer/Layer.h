/**
 * @file Layer.h
 * @author icejj (icejj@rish.com.tw)
 * @brief Header of Layer
 * @date 2020-05-29
 */
#pragma once

#include <Rish/rlpch.h>
#include <Rish/Core/Core.h>
#include <Rish/Core/Log.h>
#include <Rish/Events/Event.h>

namespace rl {

/**
 * @brief Layer class
 * @details
 */
class RL_API Layer
{
public:
	Layer(const std::string& debugname="Layer");
	virtual ~Layer();

	/// On attach
	virtual void onAttach() { RL_CORE_TRACE("[{}] onAttach", m_DebugName); };
	/// On Detach
	virtual void onDetach() { RL_CORE_TRACE("[{}] onDetach", m_DebugName); };
	/// On Update
	virtual void onUpdate() { RL_CORE_TRACE("[{}] default onUpdate", m_DebugName); };
	/// On Event
	virtual void onEvent(Event &event) { RL_CORE_TRACE("[{}] default onEvent({})", m_DebugName, event.toString()); };

	inline const std::string& GetName() const { return m_DebugName; }
protected:
	std::string m_DebugName;

};

}
