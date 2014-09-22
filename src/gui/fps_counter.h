// simple class that takes in frametimes and computes fps

#ifndef GUI_FPS_COUNTER_H
#define GUI_FPS_COUNTER_H

#include <cstddef>
#include <vector>

class fps_counter
{
public:
    fps_counter(size_t backlog);

    // a new frame has just been presented
    void add_frame();

    // false if not enough frametimes
    bool average_ready();

    // returns average frametime in seconds
    double get_average();

private:
    std::vector<double> m_history;
    size_t m_counter;
};

#endif
