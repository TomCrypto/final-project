#ifndef CORE_OCCLUSION_H
#define CORE_OCCLUSION_H

/* Provides occlusion services. It works as follows:
 * first, initialize it with the maximum number of light
 * sources to handle.
 * next, call the query() method with the light data and
 * a copy of the framebuffer contents, it will return the
 * occlusion data texture which can then be bound to other
 * subsystems
*/

#include <GL/glew.h>

#include <vector>

#include "utils/texture2d.h"
#include "utils/shader.h"
#include "core/camera.h"
#include "core/light.h"
#include "gui/framebuffer.h"

const size_t resolution = 8;
const size_t max_lights = 8;

class occlusion
{
public:
    occlusion();
    ~occlusion();

    const gl::texture2D& query(const std::vector<light>& lights,
                               framebuffer& framebuffer,
                               const camera& camera);

private:
    occlusion& operator=(const occlusion& other);
    occlusion(const occlusion& other);

    gl::texture2D m_tex;
    gl::shader m_shader;
};

#endif
