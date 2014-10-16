#ifndef MODELH
#define MODELH

#include <cstdio>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include "utils/shader.h"
#include "core/camera.h"
#include "core/light.h"

struct Triangle {
    glm::ivec3 v;
    glm::ivec3 n;
    glm::ivec3 t;

    Triangle(const glm::ivec3& v,
             const glm::ivec3& n,
             const glm::ivec3& t)
        : v(v), n(n), t(t) { }
};
struct Material {
	glm::vec3 Ka, Kd, Ks, Ke, Tf;
	int illum;
	float Ni, Ns, d, Tr;
	gl::texture2D *map_Kd;
};
struct Group {
	std::vector<Triangle> triangles;
	std::string materialIdx, s;
};

class Model {
public:
	Model(std::string filename);
	void display(const camera& camera, const std::vector<light>& lights);
	void display(const camera& camera, const std::vector<light>& lights, glm::mat4 transform);
private:
	void readMTL(std::string filename);
	void useMTL(std::string mtl);
	void CreateDrawingLists();
	void addToList(int v, int n, int u);
	void addGroup(std::string g);

	int m_nNumPoint;
	int m_nNumUV;
	int m_nNumNormal;
	int m_nNumPolygon;

	int mode; // Which mode to display

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> uv;
	std::vector<glm::vec3> normals;
	std::map < std::string, Group > groups;
	std::map < std::string, Material > materials;
	std::vector<light> lights;

	gl::shader m_shader;

	std::vector<std::pair<std::string,int>> drawLists;
};

#endif
