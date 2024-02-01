#pragma once

#ifdef _WIN32
#ifdef SUBPROCESS_STATIC
#define SUBPROCESS_API
#else
#ifdef SUBPROCESS_EXPORTS
#define SUBPROCESS_API __declspec(dllexport)
#else
#define SUBPROCESS_API __declspec(dllimport)
#endif
#endif
#else
#define SUBPROCESS_API
#endif