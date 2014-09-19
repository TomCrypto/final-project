#include "gui/fps_counter.h"

#include <cstdint>
#include <ctime>

#if defined(_WIN32)
#include <windows.h>
#endif

static double current_time()
{
    #if defined(_WIN32)
    uint64_t freq, now;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&now);
    return (double)now / freq;
    #else
    timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec + time.tv_nsec * 1e-9;
    #endif
}

fps_counter::fps_counter(size_t backlog)
{
    m_history.resize(backlog);
    m_counter = 0;
}

void fps_counter::add_frame()
{
    m_history[m_counter++ % m_history.size()] = current_time();
}

bool fps_counter::average_ready()
{
    return m_counter >= m_history.size();
}

double fps_counter::get_average()
{
    double avg_time = 0;
    size_t samples = 0;

    for (size_t t = 1; t < m_history.size(); ++t) {
        double dt = m_history[t] - m_history[t - 1];
        if (dt > 0) {
            avg_time += dt;
            ++samples;
        } else break;
    }

    return avg_time / samples;
}