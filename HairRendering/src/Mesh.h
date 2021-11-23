#pragma once
#include <string>
#include "Importer.hpp"
#include"scene.h"
#include "postprocess.h"
#include <vector>
#include <glm.hpp>
#include "Shape.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
};

struct Triangle
{
	Vertex vertex1;
	Vertex vertex2;
	Vertex vertex3;
	Triangle(Vertex v1, Vertex v2, Vertex v3)
	{
		vertex1 = v1;
		vertex2 = v2;
		vertex3 = v3;
	}
};

class Mesh
{
public:
	Mesh(std::string filename);

	void Draw();

	std::vector<Triangle> triangles;
private:
	void Load(std::string filename);
	void Process(aiMesh* mesh, const aiScene* scene);
	void Initialise();

	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	Shape mShape;
	std::string mDirectory;
};