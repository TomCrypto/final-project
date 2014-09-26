#ifndef CORE_CAMERA_H
#define CORE_CAMERA_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

class camera
{
public:
    camera(const glm::ivec2& dims,
           const glm::vec3& position,
           const glm::vec3& direction,
           float fov);
    camera() {};

    void resize(const glm::ivec2& dims);

    // move relative to current direction
    // e.g. (0, 0, 1) moves forward
    void move(const glm::vec3& direction);

    // change in euler angles
    void turn(const glm::vec2& delta);

    // change the field of view (radians)
    void set_fov(float fov);

    glm::mat4 view(bool translate = true) const;
    glm::mat4 proj() const;

private:
    glm::ivec2 m_dims;
    glm::vec3 m_pos;
    glm::vec3 m_dir;
    float m_fov;
};

#endif
