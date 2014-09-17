#include "core/camera.h"

#include <cmath>

camera::camera(int width, int height, float fov,
               const glm::vec3& position,
               const glm::vec3& direction)
    : m_width(width), m_height(height),
      m_fov(fov), m_pos(position),
      m_dir(direction)
{

}

void camera::resize(int width, int height)
{
    m_width = width;
    m_height = height;
}

void camera::move_forward(float amount)
{
    m_pos += m_dir * amount;
}

void camera::move_left(float amount)
{
    auto left = glm::cross(m_dir, glm::vec3(0, 1, 0));
    m_pos -= left * amount;
}

void camera::move_right(float amount)
{
    auto left = glm::cross(m_dir, glm::vec3(0, 1, 0));
    m_pos += left * amount;
}

void camera::turn_horizontal(float d)
{
    float phi = atan2(m_dir.z, m_dir.x);
    float theta = acos(m_dir.y);

    phi += d;

    m_dir = glm::vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
}

#include <cstdio>

void camera::turn_vertical(float d)
{
    float phi = atan2(m_dir.z, m_dir.x);
    float theta = acos(m_dir.y);

    theta += d;
    printf("theta = %.2f\n", theta);
    if (theta > M_PI - 1e-4) theta = M_PI - 1e-4;
    if (theta < 0 + 1e-4) theta = 0 + 1e-4;

    m_dir = glm::vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
}

glm::mat4 camera::view() const
{
    return glm::lookAt(m_pos, m_pos + m_dir, glm::vec3(0, 1, 0));
}

glm::mat4 camera::proj() const
{
    const float near_plane = 0.1;
    const float far_plane = 1000;

    return glm::perspectiveFov(m_fov, (float)m_width, (float)m_height,
                               near_plane, far_plane);
}
