#pragma once

// ------------------------------
// PROFILER_* facade over Tracy
// ------------------------------

// Usage example:
// #include <Profiler/Profiler.h>
// PROFILER_ZONE;
// PROFILER_ZONE_NAMED("MyZone");
// PROFILER_FRAME_MARK;
// PROFILER_SET_THREAD_NAME("MyThread");

// clang-format off

#if PROFILER_ENABLED
    #include <tracy/Tracy.hpp>
#endif

// Helpers (stringify)
#define PROFILER_STR_IMPL(x) #x
#define PROFILER_STR(x) PROFILER_STR_IMPL(x)

// -----------------
// Zones
// -----------------

#if PROFILER_ENABLED
    #define PROFILER_ZONE                ZoneScoped
    #define PROFILER_ZONE_NAMED(name)    ZoneScopedN(name)
#else
    #define PROFILER_ZONE                do {} while (0)
    #define PROFILER_ZONE_NAMED(name)    do {} while (0)
#endif

// -------------
// Frame marks
// -------------

#if PROFILER_ENABLED
    #define PROFILER_FRAME_MARK    FrameMark
#else
    #define PROFILER_FRAME_MARK    do {} while (0)
#endif

// ---------
// Threads
// ---------

#if PROFILER_ENABLED
    #define PROFILER_SET_THREAD_NAME(name)   ::tracy::SetThreadName(name)
#else
    #define PROFILER_SET_THREAD_NAME(name)   do { (void)(name); } while (0)
#endif

// clang-format on