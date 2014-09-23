#include "gui/mouse_tracker.h"

mouse_tracker::mouse_tracker()
    : m_last(glm::vec2(0, 0)),
      m_first(true)
{

}

glm::vec2 mouse_tracker::delta(const glm::vec2& pos)
{
    return m_first ? glm::vec2(0, 0) : pos - m_last;
}

void mouse_tracker::set_pos(const glm::vec2& pos)
{
    m_first = false;
    m_last = pos;
}

glm::vec2 mouse_tracker::get_pos()
{
    return m_first ? glm::vec2(0, 0) : m_last;
}
