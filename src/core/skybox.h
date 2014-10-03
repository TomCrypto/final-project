#ifndef CORE_SKYBOX_H
#define CORE_SKYBOX_H

#include <GL/glew.h>
#include <GL/glu.h>

#include "utils/gl_utils.h"
#include "core/camera.h"

struct atmos {
	glm::vec3 ray, mie;
	float RayMult, MieMult, InMult;
	glm::vec3 light;
};

class skybox
{
public:
    skybox();

	void display(const camera& cam, atmos vars);

private:
    gl::shader m_shader;
    GLUquadric* quad;
};

#endif
