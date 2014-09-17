#include "core/model.h"

Model::Model(const char *filename) {
	mode = G308_SHADE_POLYGON;

	FILE* fp;
	char mode, vmode;
	char str[200];
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;
	int numVert, numNorm, numUV, numFace;
	float x, y, z;
	float u, v;

	numVert = numNorm = numUV = numFace = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
		printf("Error reading %s file\n", filename);
	else
		printf("Reading %s file\n", filename);

	// Check number of vertex, normal, uvCoord, and Face
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
				numUV++;
			else if (vmode == 'n') // normal
				numNorm++;
			else if (vmode == ' ') // vertex
				numVert++;
			break;
		case 'f': /* faces */
			numFace++;
			break;
		}
	}

	m_nNumPoint = numVert;
	m_nNumUV = numUV;
	m_nNumPolygon = numFace;
	m_nNumNormal = numNorm;

    m_pVertexArray = 0;
    m_pNormalArray = 0;
    m_pUVArray = 0;
    m_pTriangles = 0;

	printf("Number of Point %d, UV %d, Normal %d, Face %d\n", numVert, numUV,
		numNorm, numFace);
	//-------------------------------------------------------------
	//	Allocate memory for array
	//-------------------------------------------------------------

	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	m_pVertexArray = new G308_Point[m_nNumPoint];

	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	m_pNormalArray = new G308_Normal[m_nNumNormal];

	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	m_pUVArray = new G308_UVcoord[m_nNumUV];

	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
	m_pTriangles = new G308_Triangle2[m_nNumPolygon];

	//-----------------------------------------------------------
	//	Read obj file
	//-----------------------------------------------------------
	numVert = numNorm = numUV = numFace = 0;

	fseek(fp, 0L, SEEK_SET);
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
			{
				sscanf(str, "vt %f %f", &u, &v);
				m_pUVArray[numUV].u = u;
				m_pUVArray[numUV].v = v;
				numUV++;
			}
			else if (vmode == 'n') // normal
			{
				sscanf(str, "vn %f %f %f", &x, &y, &z);
				m_pNormalArray[numNorm].x = x;
				m_pNormalArray[numNorm].y = y;
				m_pNormalArray[numNorm].z = z;
				numNorm++;
			}
			else if (vmode == ' ') // vertex
			{
				sscanf(str, "v %f %f %f", &x, &y, &z);
				m_pVertexArray[numVert].x = x;
				m_pVertexArray[numVert].y = y;
				m_pVertexArray[numVert].z = z;
				numVert++;
			}
			break;
		case 'f': /* faces : stored value is index - 1 since our index starts from 0, but obj starts from 1 */
			if (numNorm > 0 && numUV > 0) {
				sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			}
			else if (numNorm > 0 && numUV == 0){
				sscanf(str, "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
			}
			else if (numUV > 0 && numNorm == 0){
				sscanf(str, "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3);
			}
			else if (numUV == 0 && numNorm == 0){
				sscanf(str, "f %d %d %d", &v1, &v2, &v3);
			}
			// Vertex indicies for triangle:
			if (numVert != 0) {
				m_pTriangles[numFace].v1 = v1 - 1;
				m_pTriangles[numFace].v2 = v2 - 1;
				m_pTriangles[numFace].v3 = v3 - 1;
			}

			// Normal indicies for triangle
			if (numNorm != 0) {
				m_pTriangles[numFace].n1 = n1 - 1;
				m_pTriangles[numFace].n2 = n2 - 1;
				m_pTriangles[numFace].n3 = n3 - 1;
			}

			// UV indicies for triangle
			if (numUV != 0) {
				m_pTriangles[numFace].t1 = t1 - 1;
				m_pTriangles[numFace].t2 = t2 - 1;
				m_pTriangles[numFace].t3 = t3 - 1;
			}

			numFace++;
			break;
		default:
			break;
		}
	}

	fclose(fp);
	printf("Reading OBJ file is DONE.\n");
	CreateGLWireGeometry();
	CreateGLPolyGeometry();
}

void Model::display() {
	if (mode == G308_SHADE_POLYGON) {
		glCallList(m_glGeomListPoly);
	}
	else if (mode == G308_SHADE_WIREFRAME) {
		glCallList(m_glGeomListWire);
	}
	else {
		printf("Warning: Wrong Shading Mode. \n");
	}
}
void Model::addToList(G308_Point v, G308_Normal n, G308_UVcoord u = G308_UVcoord()) {
	if (m_nNumUV>0) glTexCoord2f(u.u, u.v);
	glNormal3f(n.x, n.y, n.z); //Add the normal
	glVertex3f(v.x, v.y, v.z); //Add the vertex
}
void Model::CreateGLPolyGeometry() {
	if (m_glGeomListPoly != 0)
		glDeleteLists(m_glGeomListPoly, 1);

	// Assign a display list; return 0 if err
	m_glGeomListPoly = glGenLists(1);
	glNewList(m_glGeomListPoly, GL_COMPILE);

	glBegin(GL_TRIANGLES); //Begin drawing triangles
	for (int i = 0; i < m_nNumPolygon; i++) { //Add the 3 vertex, normal and UV to the list for each face
		if (m_nNumUV > 0) {
			addToList(m_pVertexArray[m_pTriangles[i].v1], m_pNormalArray[m_pTriangles[i].n1], m_pUVArray[m_pTriangles[i].t1]);
			addToList(m_pVertexArray[m_pTriangles[i].v2], m_pNormalArray[m_pTriangles[i].n2], m_pUVArray[m_pTriangles[i].t2]);
			addToList(m_pVertexArray[m_pTriangles[i].v3], m_pNormalArray[m_pTriangles[i].n3], m_pUVArray[m_pTriangles[i].t3]);
		}
		else {
			addToList(m_pVertexArray[m_pTriangles[i].v1], m_pNormalArray[m_pTriangles[i].n1]);
			addToList(m_pVertexArray[m_pTriangles[i].v2], m_pNormalArray[m_pTriangles[i].n2]);
			addToList(m_pVertexArray[m_pTriangles[i].v3], m_pNormalArray[m_pTriangles[i].n3]);
		}
	}
	glEnd();

	glEndList();
}
void Model::CreateGLWireGeometry() {
	if (m_glGeomListWire != 0)
		glDeleteLists(m_glGeomListWire, 1);

	// Assign a display list; return 0 if err
	m_glGeomListWire = glGenLists(1);
	glNewList(m_glGeomListWire, GL_COMPILE);

	for (int i = 0; i < m_nNumPolygon; i++) {
		glBegin(GL_LINE_LOOP);
		glNormal3f(m_pNormalArray[m_pTriangles[i].n1].x,
			m_pNormalArray[m_pTriangles[i].n1].y,
			m_pNormalArray[m_pTriangles[i].n1].z);
		glVertex3f(m_pVertexArray[m_pTriangles[i].v1].x,
			m_pVertexArray[m_pTriangles[i].v1].y,
			m_pVertexArray[m_pTriangles[i].v1].z);
		glNormal3f(m_pNormalArray[m_pTriangles[i].n2].x,
			m_pNormalArray[m_pTriangles[i].n2].y,
			m_pNormalArray[m_pTriangles[i].n2].z);
		glVertex3f(m_pVertexArray[m_pTriangles[i].v2].x,
			m_pVertexArray[m_pTriangles[i].v2].y,
			m_pVertexArray[m_pTriangles[i].v2].z);
		glNormal3f(m_pNormalArray[m_pTriangles[i].n3].x,
			m_pNormalArray[m_pTriangles[i].n3].y,
			m_pNormalArray[m_pTriangles[i].n3].z);
		glVertex3f(m_pVertexArray[m_pTriangles[i].v3].x,
			m_pVertexArray[m_pTriangles[i].v3].y,
			m_pVertexArray[m_pTriangles[i].v3].z);
		glEnd();
	}

	glEndList();
}
