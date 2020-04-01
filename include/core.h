#pragma once

#include <log.h>

#include <debug-trap.h>

#define RL_DEBUG_DEFINE

#ifdef RL_DEBUG_DEFINE
    #define RL_ENABLE_ASSERT
#endif

// TODO: implment other platform's debugbreak

#ifdef RL_ENABLE_ASSERT
#define RL_ASSERT(x, fmt, ...)                                 \
    {                                                          \
        if (!(x))                                              \
        {                                                      \
            RL_ERROR("Assertion Failed: " fmt, ##__VA_ARGS__); \
            psnip_trap();                                      \
        }                                                      \
    }
#else
#define RL_ASSERT(x, fmt, ...)
#endif
