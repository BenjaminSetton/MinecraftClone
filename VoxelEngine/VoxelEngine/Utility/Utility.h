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

// Assert defines
#define VX_ASSERT(cond) assert(cond)
#define VX_ASSERT_MSG(cond, msg) assert(cond && "msg")

// Profiling defines
#define VX_PROFILE_FUNC() auto VARNAME(var) = ScopeTimer(std::string(__FUNCTION__));
#define VX_PROFILE_SCOPE() auto VARNAME(var) = ScopeTimer();
#define VX_PROFILE_SCOPE(msg) auto VARNAME(var) = ScopeTimer(std::string(msg));

// Log defines
#define VX_LOG_ERROR(msg) \
Log VARNAME(log);\
VARNAME(log).SetConsoleColors(FOREGROUND_RED);\
VARNAME(log) << msg;\
VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);\
VARNAME(log).End();

#define VX_LOG_WARN(msg) \
Log VARNAME(log);\
VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN);\
VARNAME(log) << msg;\
VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);\
VARNAME(log).End();

#define VX_LOG_PRINT(msg) \
Log VARNAME(log);\
VARNAME(log) << msg;\
VARNAME(log).End();

#else

// Misc defines
#define CONCAT_(a, b)
#define CONCAT(a, b)
#define VARNAME(var)

// Assert defines
#define VX_ASSERT(cond)
#define VX_ASSERT_MSG(cond, msg)

// Log defines
#define VX_PROFILE_FUNC()
#define VX_PROFILE_SCOPE()
#define VX_PROFILE_SCOPE(msg)

#define VX_LOG_ERROR(msg)
#define VX_LOG_WARN(msg)
#define VX_LOG_PRINT(msg);

#endif


#endif