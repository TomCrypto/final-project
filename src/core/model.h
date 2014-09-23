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
#include "utils/gl_utils.h"

#define G308_SHADE_POLYGON 0            
#define G308_SHADE_WIREFRAME 1
struct G308_Point {
	float x;
	float y;
	float z;
};
typedef G308_Point G308_Normal;
struct G308_UVcoord {
	float u;
	float v;
};
struct Triangle {
	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
	unsigned int n1;
	unsigned int n2;
	unsigned int n3;
	unsigned int t1;
	unsigned int t2;
	unsigned int t3;
	/*unsigned int v[3];
	unsigned int n[3];
	unsigned int t[3];*/
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
	void display();
private:
	void readMTL(std::string filename);
	void useMTL(std::string mtl);
	void CreateGLPolyGeometry();
	void CreateGLWireGeometry();
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

	int m_glGeomListPoly;   // Display List for Polygon
	int m_glGeomListWire;   // Display List for Wireframe
};

#endif
