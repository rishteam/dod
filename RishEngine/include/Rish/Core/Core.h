/**
 * @file Core.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of core utilities
 * @date 2020-05-28
 */
#pragma once

#include <Rish/Debug/debug-trap.h>
#include <Rish/Core/Log.h>

#define RL_DEBUG
#define RL_ATTACH 0

#ifdef RL_DEBUG
    #define RL_ENABLE_ASSERT
    #define RL_PROFILE
#endif

// Profile options
#ifdef RL_PROFILE
    #define RL_PROFILE_RENDERER 0
#endif

#ifdef RL_ENABLE_ASSERT
  #define RL_ASSERT(x, fmt, ...)                                 \
      {                                                          \
          if (!(x))                                              \
          {                                                      \
              RL_ERROR("Assertion Failed: " fmt, ##__VA_ARGS__); \
              psnip_trap();                                      \
          }                                                      \
      }
  #define RL_CORE_ASSERT(x, fmt, ...)                                 \
      {                                                               \
          if(!(x))                                                    \
          {                                                           \
              RL_CORE_ERROR("Assertion Failed: " fmt, ##__VA_ARGS__); \
              psnip_trap();                                           \
          }                                                           \
      }
#else
  #define RL_ASSERT(x, fmt, ...)
  #define RL_CORE_ASSERT(x, fmt, ...)
#endif

#define RL_UNUSED(x) ((void)(x))

#define BIT(x) (1<<(x))

#if RL_SHARED_LIB
    #if defined(_WIN32) && defined(_MSC_VER)
        #ifdef RL_EXPORTS
            #define RL_API __declspec(dllexport)
        #else
            #define RL_API __declspec(dllimport)
        #endif
    #elif __GNUC__ >= 4
        #define RL_API __attribute__ ((visibility("default")))
    #else
        #define RL_API
    #endif
#else
    #define RL_API
#endif

/**
 * @brief Bind Event function macro
 * @details For EventDispatcher
 */
#define RL_BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

namespace rl {

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> MakeScope(Args&& ... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> MakeRef(Args&& ... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

}

#include <Rish/Debug/Instrumentor.h>
