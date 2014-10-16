#ifndef CORE_SKYBOX_H
#define CORE_SKYBOX_H

#include <GL/glew.h>
#include <GL/glu.h>

#include "utils/shader.h"
#include "core/camera.h"
#include "core/light.h"

struct atmos {
	glm::vec3 sunColor;
	float timeofday, ray, mie, extinction, turbidity, sunBrightness; //sun
};

class skybox
{
public:
    skybox();
    ~skybox();
	static glm::vec3 calcSunColor(float timeofday, float T);
	static glm::vec3 calcSunDir(float timeofday);
	void display(const camera& cam, atmos vars, const std::vector<light>& lights);

private:
    skybox& operator=(const skybox& other);
    skybox(const skybox& other);

    gl::shader m_shader;
    GLUquadric* quad;
};

#endif
