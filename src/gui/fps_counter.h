/* Purpose:
 *
 *  - acts as a very simple framerate counter
 *
 * This class simply keeps a rolling history of the last N frametimes in order
 * to average them when requested (the window class then derives the FPS).
*/

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
