#ifndef CORE_SKYBOX_H
#define CORE_SKYBOX_H

#include <GL/glew.h>
#include <GL/glu.h>

#include "utils/gl_utils.h"
#include "core/camera.h"

struct atmos {
	glm::vec3 ray, mie, sunColor;
    float theta, phi; //sun
};

class skybox
{
public:
    skybox();
	static glm::vec3 calcSunColor(float theta);
	void display(const camera& cam, atmos vars);

private:
    gl::shader m_shader;
    gl::shader m_sun;
    GLUquadric* quad;
};

#endif
