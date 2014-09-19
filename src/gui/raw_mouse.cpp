#include "gui/raw_mouse.h"

raw_mouse::raw_mouse()
    : m_last(glm::vec2(0, 0)),
      m_first(true)
{

}

glm::vec2 raw_mouse::delta(const glm::vec2& pos)
{
    return m_first ? glm::vec2(0, 0) : pos - m_last;
}

void raw_mouse::set_pos(const glm::vec2& pos)
{
    m_first = false;
    m_last = pos;
}

glm::vec2 raw_mouse::get_pos()
{
    return m_first ? glm::vec2(0, 0) : m_last;
}