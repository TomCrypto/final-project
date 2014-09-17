#ifndef GUI_FPS_COUNTER_H
#define GUI_FPS_COUNTER_H

// simple rolling FPS counter

#include <vector>

class fps_counter
{
public:
    fps_counter(size_t backlog);

    // a new frame has just started
    void add_frame();

    // false if not enough frame times
    bool average_ready();
    
    // returns average frametime in seconds
    double get_average();

private:
    std::vector<double> m_history;
    size_t m_counter;
};

#endif