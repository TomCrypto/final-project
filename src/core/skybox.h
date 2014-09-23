#ifndef CORE_SKYBOX_H
#define CORE_SKYBOX_H

#include <GL/glew.h>
#include <GL/glu.h>

#include "utils/gl_utils.h"
#include "core/camera.h"

class skybox
{
public:
    skybox();

    void display(const camera& cam);

private:
    gl::shader m_shader;
    GLUquadric* quad;
};

#endif
