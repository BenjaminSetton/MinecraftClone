#ifndef UTILITY_H
#define UTILITY_H

#include "ScopeTimer.h"
#include "Log.h"

#include <assert.h>

#ifdef _DEBUG

	// Misc defines
#define OG_CONCAT_(a, b) a ## b
#define OG_CONCAT(a, b) OG_CONCAT_(a, b)
#define OG_VARNAME(var) OG_CONCAT(var, __LINE__)

#define OG_BIT(x) (1 << x)

// Asserting
#define OG_ASSERT(cond) assert(cond)
#define OG_ASSERT_MSG(cond, ...) assert(cond && printf(__VA_ARGS__))

// Profiling
#define OG_PROFILE_FUNC() auto OG_VARNAME(var) = ScopeTimer(std::string(__FUNCTION__))
#define OG_PROFILE_FUNC_MODE(mode) auto OG_VARNAME(var) = ScopeTimer(std::string(__FUNCTION__), mode)
#define OG_PROFILE_SCOPE(msg) auto OG_VARNAME(var) = ScopeTimer(std::string(msg))
#define OG_PROFILE_SCOPE_MODE(msg, mode) auto OG_VARNAME(var) = ScopeTimer(std::string(msg), mode)
#define OG_PROFILE_OUT(outVar) auto OG_VARNAME(var) = ScopeTimer(std::string(__FUNCTION__), 0, outVar)

#define OG_COOLDOWN(numberOfSeconds, dt)\
static auto OG_VARNAME(var) = numberOfSeconds;\
if(OG_VARNAME(var) <= 0) OG_VARNAME(var) += numberOfSeconds;\
OG_VARNAME(var) -= dt;\
if(OG_VARNAME(var) <= 0)

// Logging

#define OG_LOG_ERROR(...) \
Log OG_VARNAME(log);\
OG_VARNAME(log).SetConsoleColors(FOREGROUND_RED);\
OG_VARNAME(log).PrintTimestamp();\
printf(__VA_ARGS__);\
OG_VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);\
OG_VARNAME(log).End();

#define OG_LOG_WARNING(...) \
Log OG_VARNAME(log);\
OG_VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN);\
OG_VARNAME(log).PrintTimestamp();\
printf(__VA_ARGS__);\
OG_VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);\
OG_VARNAME(log).End();

#define OG_LOG_INFO(...) \
Log OG_VARNAME(log);\
OG_VARNAME(log).SetConsoleColors(FOREGROUND_GREEN);\
OG_VARNAME(log).PrintTimestamp();\
printf(__VA_ARGS__);\
OG_VARNAME(log).SetConsoleColors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);\
OG_VARNAME(log).End();

#define OG_LOG(...) \
Log OG_VARNAME(log);\
OG_VARNAME(log).PrintTimestamp();\
printf(__VA_ARGS__);\
OG_VARNAME(log).End();

#else

	// Misc defines
#define OG_CONCAT_(a, b)
#define OG_CONCAT(a, b)
#define OG_VARNAME(var)

#define OG_BIT(x) (1 << x)

// Assert defines
#define OG_ASSERT(cond)
#define OG_ASSERT_MSG(cond, ...)

// Log defines
#define OG_PROFILE_FUNC()
#define OG_PROFILE_FUNC_MODE(mode)
#define OG_PROFILE_SCOPE()
#define OG_PROFILE_SCOPE_MODE(msg, mode)
#define OG_PROFILE_OUT(outVar)

#define OG_COOLDOWN(numberOfSeconds, dt)

#define OG_LOG_ERROR(...)
#define OG_LOG_WARNING(...)
#define OG_LOG_INFO(...);
#define OG_LOG(...);

#endif

#endif