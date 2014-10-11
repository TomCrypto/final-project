#include <easylogging.h>

#include <glm/gtc/type_ptr.hpp>

#include "core/model.h"

#include <stdexcept>

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void Model::addGroup(std::string g) {
	groups[g];
}

Model::Model(std::string filename)
    : m_shader("model.vert", "model.frag") {
	std::string line;
	std::ifstream myfile(filename);
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;
	std::string g;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			//LOG(INFO) << line;
			if (line.size()<=1 || line[0] == '#') continue;
			std::vector<std::string> t = split(line, ' ');
			if (t[0] == "v" && t.size() == 4) {
				vertices.push_back(glm::vec3(std::stof(t[1]),
                                             std::stof(t[2]),
                                             std::stof(t[3])));
			}
			else if (t[0] == "vt" && t.size() == 4) {
				uv.push_back(glm::vec3(std::stof(t[1]),
                                       std::stof(t[2]),
                                       std::stof(t[3])));
			}
			else if (t[0] == "vt" && t.size() == 3) {
				uv.push_back(glm::vec3(std::stof(t[1]),
                                       std::stof(t[2]),
                                       0));
			}
			else if (t[0] == "vn" && t.size() == 4) {
				normals.push_back(glm::vec3(std::stof(t[1]),
                                            std::stof(t[2]),
                                            std::stof(t[3])));
			}
			else if (t[0] == "f") {
				if (t.size() == 5) {
					int v[4];
					int n[4];
					int u[4];
					for (size_t i = 1; i < t.size(); i++) {
						//LOG(INFO) << t[i];
						v[i-1] = std::stoi(t[i].substr(0, t[i].find_first_of('/')),nullptr);
						u[i - 1] = std::stoi(t[i].substr(t[i].find_first_of('/') + 1, t[i].find_last_of('/') - t[i].find_first_of('/') - 1));
						n[i - 1] = std::stoi(t[i].substr(t[i].find_last_of('/') + 1, t[i].size() - t[i].find_last_of('/') - 1));
					}
					if (g.size() == 0) {
						g = "default";
						addGroup(g);
					}
					groups[g].triangles.push_back(Triangle(glm::ivec3(v[0] - 1, v[1] - 1, v[3] - 1),
                                                           glm::ivec3(n[0] - 1, n[1] - 1, n[3] - 1),
                                                           glm::ivec3(u[0] - 1, u[1] - 1, u[3] - 1)));
					groups[g].triangles.push_back(Triangle(glm::ivec3(v[3] - 1, v[1] - 1, v[2] - 1),
                                                           glm::ivec3(n[3] - 1, n[1] - 1, n[2] - 1),
                                                           glm::ivec3(u[3] - 1, u[1] - 1, u[2] - 1)));
				}
				else if (t[0].size()) {
					int scan = sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
					if (scan < 9) {
						scan = sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
						if (scan < 6) {
							scan = sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3);
							if (scan < 6) {
								LOG(ERROR) << "Failed to parse '" << line << "'.";
								throw std::runtime_error("");
							}
						}
					}
					if (g.size() == 0) {
						g = "default";
						addGroup(g);
					}
					groups[g].triangles.push_back(Triangle(glm::ivec3(v1 - 1, v2 - 1, v3 - 1),
                                                           glm::ivec3(n1 - 1, n2 - 1, n3 - 1),
                                                           glm::ivec3(t1 - 1, t2 - 1, t3 - 1)));
				}
			}
			else if (t[0] == "mtllib") {
				size_t found = filename.find_last_of("/");
				if (found == std::string::npos) readMTL(t[1]);
				else {
					readMTL(filename.substr(0, found + 1) + t[1]);
				}
			}
			else if (t[0] == "usemtl") {
				if (g.size() == 0) {
					g = "default";
					addGroup(g);
				}
				groups[g].materialIdx = t[1];
				if (materials.find(t[1]) == materials.end()) {
					LOG(ERROR) << "missing material " << t[1];
					throw std::runtime_error("");
				}
			}
			else if (t[0] == "g") {
				g = t[1];
				addGroup(g);
			}
			else if (t[0] == "s") {
				groups[g].s = t[1];
			}
			else {
				LOG(ERROR) << "Failed to parse '" << line << "'.";
				throw std::runtime_error("");
			}
		}
		myfile.close();
	}
	LOG(INFO) << "Finished loading '" << filename << "'.";
	LOG(TRACE) << vertices.size() << " vertices, "
	           << uv.size() << " texcoords, "
	           << normals.size() << " normals.";
}

void Model::readMTL(std::string filename) {
	std::string line;
	std::ifstream myfile(filename);
	int prev = materials.size();
	LOG(INFO) << "reading mtl file" << filename;
	if (myfile.is_open())
	{
		std::string mtl;
		while (getline(myfile, line)) {
			std::vector<std::string> t = split(line, ' ');
			if (t.size() == 0 || t[0] == "#" || t[0] == "\n");
			else if (t[0] == "newmtl") {
				mtl = t[1];
				materials[mtl];
			}
			else if (t[0] == "illum") materials[mtl].illum = std::stoi(t[1]);
			else if (t[0] == "Ka") materials[mtl].Ka = glm::vec3(std::stof(t[1]), std::stof(t[2]), std::stof(t[3]));
			else if (t[0] == "Kd") materials[mtl].Kd = glm::vec3(std::stof(t[1]), std::stof(t[2]), std::stof(t[3]));
			else if (t[0] == "Ks") materials[mtl].Ks = glm::vec3(std::stof(t[1]), std::stof(t[2]), std::stof(t[3]));
			else if (t[0] == "Ke") materials[mtl].Ke = glm::vec3(std::stof(t[1]), std::stof(t[2]), std::stof(t[3]));
			else if (t[0] == "Tf") materials[mtl].Tf = glm::vec3(std::stof(t[1]), std::stof(t[2]), std::stof(t[3]));
			else if (t[0] == "Ns") materials[mtl].Ns = std::stof(t[1]);
			else if (t[0] == "Ni") materials[mtl].Ni = std::stof(t[1]);
			else if (t[0] == "d") materials[mtl].d = std::stof(t[1]);
			else if (t[0] == "Tr") materials[mtl].Tr = std::stof(t[1]);
			/*else if (t2[0] == "map_Kd") {
				m.map_Kd = new gl::texture2D(t2[1], GL_UNSIGNED_BYTE);
			}*/
			else {
				LOG(ERROR) << t[0];
			}
		}
	}
	LOG(INFO) << "finished reading mtl file " << std::to_string(materials.size() - prev);
}

void Model::display(const camera& camera, const std::vector<light>& lights) {
    glEnable(GL_DEPTH_TEST);

	if (drawLists.empty()) {
		CreateDrawingLists();
	}
	
	for (auto var : drawLists)
	{
		m_shader.bind();
		m_shader.set("view", camera.view());
        m_shader.set("proj", camera.proj());
		glCallList(var.second);
		m_shader.unbind();
	}
	
    glDisable(GL_DEPTH_TEST);
	
	/*m_shader.bind();
    m_shader.set("view", camera.view());
    m_shader.set("proj", camera.proj());

    m_shader.set("camera_pos", camera.pos());
    m_shader.set("light_count", (int)lights.size());

    for (size_t t = 0; t < lights.size(); ++t) {
        m_shader.set("lights[" + std::to_string(t) + "].pos", lights[t].pos);
        // m_shader.set("lights[" + std::to_string(t) + "].falloff", lights[t].falloff);
        // m_shader.set("lights[" + std::to_string(t) + "].intensity", lights[t].intensity);
    }

    m_shader.set("ks", glm::vec3(0.9, 0.9, 0.9));
    m_shader.set("kd", glm::vec3(0.4, 0.3, 0.2));
    m_shader.set("ka", glm::vec3(0.1, 0.1, 0.1));
    m_shader.set("shininess", 32.0f);

	for (auto var : drawLists)
	{
        // set up material

		glCallList(var.second);
	}

	m_shader.unbind(); */
}

void Model::useMTL(std::string mtl) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, glm::value_ptr(materials[mtl].Ka));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, glm::value_ptr(materials[mtl].Kd));
	glMaterialfv(GL_FRONT, GL_SPECULAR, glm::value_ptr(glm::vec4(materials[mtl].Ks, materials[mtl].Ns)));
	//glEnable(GL_TEXTURE_2D);
	//materials[mtl].map_Kd->bind(0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
void Model::addToList(int v, int n, int u) {
	if (uv.size()>0) glTexCoord2f(uv[u].x, uv[u].y);
	glNormal3f(normals[n].x, normals[n].y, normals[n].z); //Add the normal
	glVertex3f(vertices[v].x, vertices[v].y, vertices[v].z); //Add the vertex
}
void Model::CreateDrawingLists() {
	if (!drawLists.empty()) {
		for (std::pair<std::string,int> var : drawLists)
		{
			glDeleteLists(var.second, 1);
		}
		drawLists.clear();
	}
	
	for (auto& g : groups) {
		int l = glGenLists(1);
		glNewList(l, GL_COMPILE);
		glBegin(GL_TRIANGLES);
		for (Triangle t : g.second.triangles) {
			addToList(t.v[0], t.n[0], t.t[0]);
			addToList(t.v[1], t.n[1], t.t[1]);
			addToList(t.v[2], t.n[2], t.t[2]);
		}
		glEnd();
		glEndList();
		drawLists.push_back(std::pair<std::string, int>(g.second.materialIdx, l));
	}
}
