#include "core/camera.h"

#include <cmath>

camera::camera(const glm::ivec2& dims,
               const glm::vec3& position,
               const glm::vec3& direction,
               float fov)
    : m_dims(dims), m_pos(position),
      m_dir(glm::normalize(direction)),
      m_fov(fov)
{

}

void camera::resize(const glm::ivec2& dims)
{
    m_dims = dims;
}

void camera::move(const glm::vec3& direction)
{
    m_pos += glm::vec3(glm::vec4(direction, 0) * view());
}

void camera::turn(const glm::vec2& delta)
{
    float phi = atan2(m_dir.z, m_dir.x);
    float theta = acos(m_dir.y);

    phi += delta.x;
    theta += delta.y;

    if (theta > glm::pi<float>() - 1e-1f)
        theta = glm::pi<float>() - 1e-1f;
    if (theta < 1e-1f) theta = 1e-1f;

    m_dir = glm::vec3(sin(theta) * cos(phi),
                      cos(theta),
                      sin(theta) * sin(phi));
}

float camera::aspect_ratio() const
{
    return (float)m_dims.x / m_dims.y;
}

void camera::set_fov(float fov)
{
    m_fov = fov;
}

glm::vec3 camera::pos() const
{
    return m_pos;
}

glm::vec3 camera::dir() const
{
    return m_dir;
}

glm::ivec2 camera::dims() const
{
    return m_dims;
}

glm::mat4 camera::view(bool translate) const
{
    if (translate)
        return glm::lookAt(m_pos, m_pos + m_dir, glm::vec3(0, 1, 0));
    else
        return glm::lookAt(glm::vec3(0), m_dir, glm::vec3(0, 1, 0));
}

glm::mat4 camera::proj() const
{
    const float near_plane = 0.1f;
    const float far_plane = 1000.0f;

    return glm::perspectiveFov(m_fov,
                               (float)m_dims.x,
                               (float)m_dims.y,
                               near_plane, far_plane);
}
