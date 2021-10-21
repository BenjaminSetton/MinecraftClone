#ifndef UTILITY_H
#define UTILITY_H

#include "ScopeTimer.h"
#include "Log.h"

#include <assert.h>

#ifdef _DEBUG

// Misc defines
#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)
#define VARNAME(var) CONCAT(var, __LINE__)

#define BIT(x) (1 << x)

// Asserting
#define VX_ASSERT(cond) assert(cond)
#define VX_ASSERT_MSG(cond, ...) assert(cond && printf(__VA_ARGS__))

// Profiling
#define VX_PROFILE_FUNC() auto VARNAME(var) = ScopeTimer(std::string(__FUNCTION__))
#define VX_PROFILE_FUNC_MODE(mode) auto VARNAME(var) = ScopeTimer(std::string(__FUNCTION__), mode)
#define VX_PROFILE_SCOPE(msg) auto VARNAME(var) = ScopeTimer(std::string(msg))
#define VX_PROFILE_SCOPE_MODE(msg, mode) auto VARNAME(var) = ScopeTimer(std::string(msg), mode)
#define VX_PROFILE_OUT(outVar) auto VARNAME(var) = ScopeTimer(std::string(__FUNCTION__), 0, outVar)

// Logging

#define VX_LOG_ERROR(...) \
Log VARNAME(log);\
VARNAME(log).SetConsoleColors(FOREGROUND_RED);\
VARNAME(log).PrintTimestamp();\
printf(__VA_ARGS__);\
VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);\
VARNAME(log).End();

#define VX_LOG_WARN(...) \
Log VARNAME(log);\
VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN);\
VARNAME(log).PrintTimestamp();\
printf(__VA_ARGS__);\
VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);\
VARNAME(log).End();

#define VX_LOG_INFO(...) \
Log VARNAME(log);\
VARNAME(log).SetConsoleColors(FOREGROUND_GREEN);\
VARNAME(log).PrintTimestamp();\
printf(__VA_ARGS__);\
VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);\
VARNAME(log).End();

#define VX_LOG(...) \
Log VARNAME(log);\
VARNAME(log).PrintTimestamp();\
printf(__VA_ARGS__);\
VARNAME(log).End();

#else

// Misc defines
#define CONCAT_(a, b)
#define CONCAT(a, b)
#define VARNAME(var)

#define BIT(x) (1 << x)

// Assert defines
#define VX_ASSERT(cond)
#define VX_ASSERT_MSG(cond, ...)

// Log defines
#define VX_PROFILE_FUNC()
#define VX_PROFILE_FUNC_MODE(mode)
#define VX_PROFILE_SCOPE()
#define VX_PROFILE_SCOPE_MODE(msg, mode)
#define VX_PROFILE_OUT(outVar)

#define VX_LOG_ERROR(...)
#define VX_LOG_WARN(...)
#define VX_LOG_INFO(...);
#define VX_LOG(...);

#endif


#endif