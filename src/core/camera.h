#ifndef CAMERA_H
#define CAMERA_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

class camera
{
public:
    camera(int width, int height, float fov,
           const glm::vec3& position,
           const glm::vec3& direction);
    camera() {};

    void resize(int width, int height);

    // move relative to current direction
    // e.g. (0, 0, 1) moves forward
    void move(const glm::vec3& direction);

    // change in euler angles
    void turn(const glm::vec2& delta);

    glm::mat4 view() const;
    glm::mat4 proj() const;

private:
    glm::vec3 m_pos;
    glm::vec3 m_dir;
    int m_width;
    int m_height;
    float m_fov;
};

#endif
