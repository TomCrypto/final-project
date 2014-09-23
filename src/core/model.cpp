#include <easylogging.h>
#include "core/model.h"

Model::Model(std::string filename) {
	mode = G308_SHADE_POLYGON;

	std::string line;
	std::ifstream myfile(filename);
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;
	std::string mtl;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (line.size()<=1 || line[0] == '#') continue;
			std::istringstream iss(line);
			std::vector<std::string> t{ std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{} };
			if (t[0] == "v" && t.size() == 4) {
				vertices.push_back({ std::stof(t[1], nullptr), std::stof(t[2], nullptr), std::stof(t[3], nullptr) });
			}
			else if (t[0] == "vt" && t.size() == 4) {
				uv.push_back({ std::stof(t[1], nullptr), std::stof(t[2], nullptr), std::stof(t[3], nullptr) });
			}
			else if (t[0] == "vt" && t.size() == 3) {
				uv.push_back({ std::stof(t[1], nullptr), std::stof(t[2], nullptr), 0 });
			}
			else if (t[0] == "vn" && t.size() == 4) {
				normals.push_back({ std::stof(t[1], nullptr), std::stof(t[2], nullptr), std::stof(t[3], nullptr) });
			}
			else if (t[0] == "f") {
				int scan = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
				if (scan < 9) {
					scan = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
					if (scan < 6) {
						scan = sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3);
						if (scan < 6) {
							LOG(INFO) << line << "\n\n";
							while (true);
						}
					}
				}
				triangles.push_back({ v1 - 1, v2 - 1, v3 - 1, n1 - 1, n2 - 1, n3 - 1, t1 - 1, t2 - 1, t3 - 1, mtl });
			}
			else if (t[0] == "mtllib") {
				int found = filename.find_last_of("/");
				if (found == std::string::npos) readMTL(t[1]);
				else {
					readMTL(filename.substr(0, found + 1) + t[1]);
				}
			}
			else if (t[0] == "usemtl") {
				mtl = t[1];
				if (materials.find(mtl) == materials.end()) {
					LOG(ERROR) << "missing material " << mtl;
					while (true);
				}
			}
			else if (t[0] == "g" || t[0] == "s") {
				LOG(INFO) << line;
			}
			else {
				LOG(INFO) << line;
				while (true);
			}
		}
		myfile.close();
	}
	LOG(INFO) << "Finished" << "\n\n";
	printf("Number of Point %d, UV %d, Normal %d, Face %d\n", vertices.size(), uv.size(),
		normals.size(), triangles.size());
	//while (true);
}

void Model::readMTL(std::string filename) {
	std::string line;
	std::ifstream myfile(filename);
	int prev = materials.size();
	LOG(INFO) << "reading mtl file" << filename;
	if (myfile.is_open())
	{
		while (getline(myfile, line)) {
			std::istringstream iss(line);
			std::vector<std::string> t{ std::istream_iterator < std::string > {iss}, std::istream_iterator < std::string > {} };
			if (t.size() == 0 || t[0] == "#" || t[0] == "\n");
			else if (t[0] == "newmtl") {
				std::string newline;
				Material m;
				while (getline(myfile, newline)) {
					std::istringstream iss(newline);
					std::vector<std::string> t2{ std::istream_iterator < std::string > {iss}, std::istream_iterator < std::string > {} };

					if (t2.size() == 0) break;
					else if (t2[0] == "illum") m.illum = stoi(t2[1]);
					else if (t2[0] == "Ka") m.Ka = { std::stof(t2[1], nullptr), std::stof(t2[2], nullptr), std::stof(t2[3], nullptr) };
					else if (t2[0] == "Kd") m.Kd = { std::stof(t2[1], nullptr), std::stof(t2[2], nullptr), std::stof(t2[3], nullptr) };
					else if (t2[0] == "Ks") m.Ks = { std::stof(t2[1], nullptr), std::stof(t2[2], nullptr), std::stof(t2[3], nullptr) };
					else if (t2[0] == "Ke") m.Ke = { std::stof(t2[1], nullptr), std::stof(t2[2], nullptr), std::stof(t2[3], nullptr) };
					else if (t2[0] == "Tf") m.Tf = { std::stof(t2[1], nullptr), std::stof(t2[2], nullptr), std::stof(t2[3], nullptr) };
					else if (t2[0] == "Ns") m.Ns = stof(t2[1]);
					else if (t2[0] == "Ni") m.Ni = stof(t2[1]);
					else if (t2[0] == "d") m.d = stof(t2[1]);
					else if (t2[0] == "Tr") m.Tr = stof(t2[1]);
					else if (t2[0] == "map_Kd");
					else {
						LOG(INFO) << t2[0];
					}
				}
				materials[t[1]] = m;
			}
		}
	}
	LOG(INFO) << "finished reading mtl file " << std::to_string(materials.size() - prev);
}

void Model::display() {
	if (mode == G308_SHADE_POLYGON) {
		if (m_glGeomListPoly == 0) CreateGLPolyGeometry();
		glCallList(m_glGeomListPoly);
	}
	else if (mode == G308_SHADE_WIREFRAME) {
		if (m_glGeomListWire == 0) CreateGLWireGeometry();
		glCallList(m_glGeomListWire);
	}
	else {
		printf("Warning: Wrong Shading Mode. \n");
	}
}
float *vec3TofloatArr(glm::vec3 v) {
	return new float[3] {v.x, v.y, v.z};
}
float *specular(glm::vec3 v, float ns) {
	return new float[4] {v.x, v.y, v.z, ns};
}
void Model::useMTL(std::string mtl) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, vec3TofloatArr(materials[mtl].Ka));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, vec3TofloatArr(materials[mtl].Kd));
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular(materials[mtl].Ks, materials[mtl].Ns));
}
void Model::addToList(int v, int n, int u) {
	if (uv.size()>0) glTexCoord2f(uv[u].x, uv[u].y);
	glNormal3f(normals[n].x, normals[n].y, normals[n].z); //Add the normal
	glVertex3f(vertices[v].x, vertices[v].y, vertices[v].z); //Add the vertex
}
void Model::CreateGLPolyGeometry() {
	if (m_glGeomListPoly != 0)
		glDeleteLists(m_glGeomListPoly, 1);

	// Assign a display list; return 0 if err
	m_glGeomListPoly = glGenLists(1);
	glNewList(m_glGeomListPoly, GL_COMPILE);

	glBegin(GL_TRIANGLES); //Begin drawing triangles
	for (Triangle t : triangles) {
		useMTL(t.materialIdx);
		addToList(t.v1, t.n1, t.t1);
		addToList(t.v2, t.n2, t.t2);
		addToList(t.v3, t.n3, t.t3);
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

	for (Triangle t : triangles) {
		glBegin(GL_LINE_LOOP);
		addToList(t.v1, t.n1, t.t1);
		addToList(t.v2, t.n2, t.t2);
		addToList(t.v3, t.n3, t.t3);
		glEnd();
	}
	glEndList();
}
