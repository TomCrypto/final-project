#ifndef CORE_LIGHT_H
#define CORE_LIGHT_H

#include <glm/glm.hpp>

// meta-representation of a light source in the world
// use this to communicate light sources between different
// subsystems

struct light
{
    public:
        light(const glm::vec4& pos,
              const float& radius)
            : pos(pos), radius(radius)
        {

        }

        const glm::vec4& pos; // world coordinates
        const float& radius; // in world space, approximate extent of light source
};

#endif
