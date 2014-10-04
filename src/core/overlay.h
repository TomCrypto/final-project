#ifndef CORE_OVERLAY_H
#define CORE_OVERLAY_H

#include <GL/glew.h>
#include <vector>

#include "core/camera.h"
#include "core/light.h"

#include "utils/gl_utils.h"

// renders a nice overlay on top of the rendered image with
// imperfections and dirt and stuff that respond to lighting

// supports up to 8 lights

class overlay
{
    public:
        overlay(int density);
        
        void regenerate_film(int density);
        
        void render(const std::vector<light>& lights,
                    const camera& camera);

        int get_density() const {
            return m_film.size();
        }

    private:
        gl::shader m_shader;

        std::vector<std::pair<glm::vec2, float>> m_film;
};

#endif
