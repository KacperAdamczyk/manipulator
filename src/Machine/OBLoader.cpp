#include "OBLoader.h"
#include "OBJ_Loader.h"

OBLoader::OBLoader(std::string file) : file(file)
{
	loader = new objl::Loader();
	loader->LoadFile(file.c_str());
	prepareData();
}


OBLoader::~OBLoader()
{
}

void OBLoader::prepareData()
{
	auto mesh = loader->LoadedMeshes;

	for (int i = 0; i < mesh.size(); i++) {
		Mesh nm;
		for (int j = 0; j < mesh[i].Vertices.size(); j++) {
			auto v = mesh[i].Vertices[j];

			nm.vertices.push_back(v.Position.X);
			nm.vertices.push_back(v.Position.Y);
			nm.vertices.push_back(v.Position.Z);

			nm.normals.push_back(v.Normal.X);
			nm.normals.push_back(v.Normal.Y);
			nm.normals.push_back(v.Normal.Z);

			nm.indices.push_back(mesh[i].Indices[j]);

			nm.textureCoords.push_back(v.TextureCoordinate.X);
			nm.textureCoords.push_back(v.TextureCoordinate.Y);
		}
		nm.ambient[0] = mesh[i].MeshMaterial.Ka.X;
		nm.ambient[1] = mesh[i].MeshMaterial.Ka.Y;
		nm.ambient[2] = mesh[i].MeshMaterial.Ka.Z;

		nm.diffuse[0] = mesh[i].MeshMaterial.Kd.X;
		nm.diffuse[1] = mesh[i].MeshMaterial.Kd.Y;
		nm.diffuse[2] = mesh[i].MeshMaterial.Kd.Z;

		nm.specular[0] = mesh[i].MeshMaterial.Ks.X;
		nm.specular[1] = mesh[i].MeshMaterial.Ks.Y;
		nm.specular[2] = mesh[i].MeshMaterial.Ks.Z;

		draw::Point3D c = parseColor(mesh[i].MeshMaterial.name, i);
		nm.color[0] = c.x;
		nm.color[1] = c.y;
		nm.color[2] = c.z;
		nm.color[3] = mesh[i].MeshMaterial.d == 0 ? 1.0f : mesh[i].MeshMaterial.d;

		nm.Ni = mesh[i].MeshMaterial.Ni + 128;

		meshes.push_back(nm);
	}
}

draw::Point3D OBLoader::parseColor(std::string s, GLint i)
{
	if (s.length() == 0) {
		if (meshes.size() != 0) {
			return draw::Point3D(meshes[i - 1].color[0], meshes[i - 1].color[1], meshes[i - 1].color[2]);
		}
		else {
			return draw::Point3D(0.5f, 0.5f, 0.5f);
		}
	}

	std::string color[3];
	GLint ind = 0;
	for (char c : s) {
		if (c == ',') {
			if (++ind >= 3)
				break;
			continue;
		}
		color[ind] += c;
	}
	return draw::Point3D(S_RGB(atoi(color[0].c_str())), S_RGB(atoi(color[1].c_str())), S_RGB(atoi(color[2].c_str())));
}

void OBLoader::setPointers(GLint i)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, meshes[i].vertices.data());
	glNormalPointer(GL_FLOAT, 0, meshes[i].normals.data());
	glIndexPointer(GL_FLOAT, 0, meshes[i].indices.data());
	glTexCoordPointer(2, GL_FLOAT, 0, meshes[i].textureCoords.data());
}

void OBLoader::setMaterial(GLint i)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, meshes[i].ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, meshes[i].diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, meshes[i].specular);
	glMaterialf(GL_FRONT, GL_SHININESS, meshes[i].Ni);

	glColor4fv(meshes[i].color);
}

void OBLoader::draw(GLboolean material)
{
	//glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < meshes.size(); i++) {
		setPointers(i);
		if (material)
			setMaterial(i);
		glDrawArrays(GL_TRIANGLES, 0, loader->LoadedMeshes[i].Vertices.size());
	}
	//glDisable(GL_TEXTURE_2D);
}
