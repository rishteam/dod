#pragma once

#include "Rish/Utils/debug-trap.h"
#include "Rish/Core/Log.h"

#define RL_DEBUG_DEFINE

#ifdef RL_DEBUG_DEFINE
    #define RL_ENABLE_ASSERT
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

#if defined _WIN32 || defined __CYGWIN__
  #ifndef RL_USE_STATIC
    #ifdef RL_BUILD_DLL
      // Exporting...
      #ifdef __GNUC__
        #define RL_API __attribute__ ((dllexport))
      #else
        #define RL_API __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
      #endif
    #else
      #ifdef __GNUC__
        #define RL_API __attribute__ ((dllimport))
      #else
        #define RL_API __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
      #endif
    #endif
  #else
    #define RL_API
  #endif
  #define NOT_EXPORTED
#else
  #if __GNUC__ >= 4
    #define RL_API __attribute__ ((visibility ("default")))
    #define NOT_EXPORTED  __attribute__ ((visibility ("hidden")))
  #else
    #define RL_API
    #define NOT_EXPORTED
  #endif
#endif
