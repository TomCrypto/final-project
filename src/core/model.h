#ifndef MODELH
#define MODELH

#include <cstdio>
#include <GL/glew.h>
#include <glm/glm.hpp>

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
struct G308_Triangle2 {
	unsigned int v1;
	unsigned int v2;
	unsigned int v3;
	unsigned int n1;
	unsigned int n2;
	unsigned int n3;
	unsigned int t1;
	unsigned int t2;
	unsigned int t3;
};

class Model {
public:
	Model(const char *filename);
	void display();
private:
	void CreateGLPolyGeometry();
	void CreateGLWireGeometry();
	void addToList(G308_Point v, G308_Normal n, G308_UVcoord u);

	int m_nNumPoint;
	int m_nNumUV;
	int m_nNumNormal;
	int m_nNumPolygon;

	int mode; // Which mode to display

	G308_Point* m_pVertexArray;		// Vertex Array
	G308_Normal* m_pNormalArray;	// Normal Array
	G308_Triangle2* m_pTriangles;	// Triangle Array
	G308_UVcoord* m_pUVArray;	    // Texture Coordinate Array

	int m_glGeomListPoly;	// Display List for Polygon
	int m_glGeomListWire;	// Display List for Wireframe
};

#endif
