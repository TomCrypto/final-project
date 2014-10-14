#ifndef CORE_LIGHT_H
#define CORE_LIGHT_H

#include <glm/glm.hpp>

// meta-representation of a light source in the world
// use this to communicate light sources between different
// subsystems

enum light_type
{
    LIGHT_TINY,
    LIGHT_SMALL,
    LIGHT_NORMAL,
    LIGHT_LARGE,
    LIGHT_HUGE
};

struct light
{
    public:
        light(const glm::vec4& position,
              const glm::vec3& intensity,
              const glm::vec3& attenuation,
              const float& radius,
              const light_type& type,
              bool ghosts)
            : position(position),
              intensity(intensity),
              attenuation(attenuation),
              radius(radius),
              type(type),
              ghosts(ghosts)
        {

        }

        glm::vec4 position; // Position of light source (WORLD SPACE)
        glm::vec3 intensity; // Total RGB intensity of light source
        glm::vec3 attenuation; // Constant/linear/quadratic (xyz)

        float radius; // World radius of light source sphere
        light_type type; // Type of light (for lens flare)
        bool ghosts; // Whether to draw ghosts or not
};

#endif
