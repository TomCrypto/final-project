/* Purpose:
 *
 *  - tracks the mouse movement
 *  - computes difference in mouse position between frames
 *
 * This class is used for the ``first person camera'' feature where the cursor
 * is locked in the center of the window so that the user can move around with
 * only mouse movements without needing to click and drag.
*/

#ifndef GUI_MOUSE_TRACKER_H
#define GUI_MOUSE_TRACKER_H

#include <glm/glm.hpp>

class mouse_tracker
{
public:
    mouse_tracker();

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
