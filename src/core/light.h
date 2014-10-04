#ifndef CORE_LIGHT_H
#define CORE_LIGHT_H

#include <glm/glm.hpp>

// meta-representation of a light source in the world
// use this to communicate light sources between different
// subsystems

struct light
{
    public:
        light(const glm::vec3& pos,
              const glm::vec3& strength,
              const float& radius)
            : pos(pos), strength(strength), radius(radius)
        {
        
        }

        const glm::vec3& pos; // world coordinates
        const glm::vec3& strength; // non-negative
        const float& radius; // in world space, approximate extent of light source
};

#endif
