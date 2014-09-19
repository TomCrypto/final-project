#ifndef GUI_RAW_MOUSE_H
#define GUI_RAW_MOUSE_H

#include <glm/glm.hpp>

class raw_mouse
{
public:
    raw_mouse();

    // get the delta between this mouse position and the
    // last one (if there is no last one, returns (0,0))
    glm::vec2 delta(const glm::vec2& pos);

    // define the current mouse position
    void set_pos(const glm::vec2& pos);

    // gets the last mouse position
    glm::vec2 get_pos();

private:
    glm::vec2 m_last;
    bool m_first;
};

#endif