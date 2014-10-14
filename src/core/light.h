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
        light(const glm::vec4& pos,
              const float& radius,
              const light_type& type)
            : pos(pos), radius(radius), type(type)
        {

        }

        const glm::vec4& pos; // world coordinates
        const float& radius; // in world space, approximate extent of light source
        light_type type;
};

#endif
