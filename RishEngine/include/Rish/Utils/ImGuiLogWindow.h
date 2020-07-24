#pragma once

#include <Rish/rlpch.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>

// TODO: Add Color support
// TODO: Add text filter

/**
 * @brief ImGui Log Window
 */
class ImGuiLogWindow
{
public:
    ImGuiLogWindow(uint32_t logLines=16384)
    {
        logLinesLimit = logLines;
    }
    void onImGuiRender();
    void clearLog() { msgList.clear(); }

    std::deque<std::string> msgList;
    uint32_t logLinesLimit;
};

extern ImGuiLogWindow defaultLogWindow;

/**
 * @brief ImGui Log Sink for spdlog
 * @tparam Mutex
 */
template<typename Mutex>
class ImGuiLogSink : public spdlog::sinks::base_sink <Mutex>
{
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
        defaultLogWindow.msgList.push_front(fmt::to_string(formatted));
    }

    void flush_() override
    {
    }
};

#include <spdlog/details/null_mutex.h>
using ImGuiLogSink_mt = ImGuiLogSink<std::mutex>;
using ImGuiLogSink_st = ImGuiLogSink<spdlog::details::null_mutex>;
