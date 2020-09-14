#pragma once

#include <Rish/rlpch.h>

namespace rl {

class RL_API ErrorModal
{
public:
    ErrorModal() = default;
    ~ErrorModal() = default;

    // TODO: make format
    void setMessage(const std::string &msg) { m_message = msg; m_showErrorModal = true; }
    std::string getMessage() const { return m_message; }

    void onImGuiRender();
private:
    std::string m_message;
    bool m_showErrorModal = false;
};

} // end of namespace rl


