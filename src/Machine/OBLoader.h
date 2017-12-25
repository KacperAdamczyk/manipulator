#pragma once
#include "main.h"
#include "shapes.h"

namespace objl {
	class Loader;
}

class OBLoader
{
	struct Mesh
	{
		std::vector<GLfloat> vertices;
		std::vector<GLfloat> textureCoords;
		std::vector<GLfloat> normals;
		std::vector<GLfloat> indices;
		GLfloat ambient[3];
		GLfloat diffuse[3];
		GLfloat specular[3];
		GLfloat color[4];
		GLfloat Ni;
	};

	std::string file;
	objl::Loader *loader;
	std::vector<Mesh> meshes;

public:
	OBLoader(std::string file);
	~OBLoader();

private:
	void prepareData();
	draw::Point3D parseColor(std::string s, GLint i);
public:
	void setPointers(GLint i);
	void setMaterial(GLint i);
	void draw(GLboolean material = true);
};

