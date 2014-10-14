#ifndef CORE_SKYBOX_H
#define CORE_SKYBOX_H

#include <GL/glew.h>
#include <GL/glu.h>

#include "utils/shader.h"
#include "core/camera.h"

struct atmos {
	glm::vec3 sunColor;
    float theta, phi, ray, mie; //sun
};

class skybox
{
public:
    skybox();
	static glm::vec3 calcSunColor(float theta);
	static glm::vec3 calcSunDir(float theta, float phi);
	void display(const camera& cam, atmos vars);

private:
    gl::shader m_shader;
    gl::shader m_sun;
    GLUquadric* quad;
};

#endif
