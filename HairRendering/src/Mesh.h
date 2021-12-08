//#pragma once
//#include <string>
//#include "Importer.hpp"
//#include"scene.h"
//#include "postprocess.h"
//#include <vector>
//#include <glm.hpp>
//#include "Shape.h"
//
//struct Vertex
//{
//	glm::vec3 position;
//	glm::vec2 texCoords;
//	glm::vec3 normal;
//};
//
//struct Triangle
//{
//	Vertex vertex1;
//	Vertex vertex2;
//	Vertex vertex3;
//
//	Triangle(Vertex v1, Vertex v2, Vertex v3)
//	{
//		vertex1 = v1;
//		vertex2 = v2;
//		vertex3 = v3;
//	}
//
//	float Area()
//	{
//		return glm::length(glm::cross(vertex3.position - vertex1.position, vertex2.position - vertex1.position)) / 2.0f;
//	}
//
//	void RandomPoint(glm::vec3& position, glm::vec2& texCoords, glm::vec3& normal)
//	{
//		float random = sqrt(rand() / (float)RAND_MAX);
//
//		glm::vec3 point;
//		point.x = 1 - random;
//		point.y = rand() / (float)RAND_MAX;
//		point.z = 1 - point.x - point.y;
//		texCoords = glm::mat3x2(vertex1.texCoords, vertex2.texCoords, vertex3.texCoords) * point;
//		position = glm::mat3(vertex1.position, vertex2.position, vertex3.position) * point;
//		normal = glm::mat3(vertex1.normal, vertex2.normal, vertex3.normal) * point;
//	}
//};
//
//class Mesh
//{
//public:
//	Mesh(std::string filename);
//
//	void Draw();
//
//	std::vector<Triangle> triangles;
//private:
//	void Load(std::string filename);
//	void Process(aiMesh* mesh, const aiScene* scene);
//	void Initialise();
//
//	std::vector<Vertex> mVertices;
//	std::vector<unsigned int> mIndices;
//	Shape mShape;
//	std::string mDirectory;
//};

#pragma once
#include <vector>
#include <glm.hpp>
#include "Shape.h"
#include <random>

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

	float Area()
	{
		return glm::length(glm::cross(vertex3.position - vertex1.position, vertex2.position - vertex1.position)) /*/ 2.0f*/ * 1000.0f;
		/*float a = glm::distance(vertex1.position, vertex2.position);
		float b = glm::distance(vertex2.position, vertex3.position);
		float c = glm::distance(vertex3.position, vertex1.position);

		float s = (a + b + c) / 2.0f;
		return sqrt(s * (s - a) * (s - b) * (s - c));*/
	}

	void RandomPoint(glm::vec3& position, glm::vec2& texCoords, glm::vec3& normal)
	{
		//float random = sqrt((float)rand() / RAND_MAX);

		/*glm::vec3 point;
		point.x = 1 - random;
		point.y = (float)rand() / RAND_MAX;
		point.z = 1 - point.x - point.y;
		texCoords = glm::mat3x2(vertex1.texCoords, vertex2.texCoords, vertex3.texCoords) * point;
		position = glm::mat3(vertex1.position, vertex2.position, vertex3.position) * point;
		normal = glm::mat3(vertex1.normal, vertex2.normal, vertex3.normal) * point;*/

		
		/*std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
		float r1 = distribution(generator);
		float r2 = distribution(generator);
		if (r1 + r2 > 1)
		{
			r1 = 1 - r1;
			r2 = 1 - r2;
		}

		position = (r1 * (vertex2.position - vertex1.position) + r2 * (vertex3.position - vertex1.position)) + vertex1.position;
		texCoords = (r1 * (vertex2.texCoords - vertex1.texCoords) + r2 * (vertex3.texCoords - vertex1.texCoords)) + vertex1.texCoords;
		normal = (r1 * (vertex2.normal - vertex1.normal) + r2 * (vertex3.normal - vertex1.normal)) + vertex1.normal;*/

		std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
		/*float r1 = distribution(generator);
		float r2 = distribution(generator);
		float r3 = distribution(generator);

		glm::vec3 random = glm::vec3(r1, r2, r3);*/

		float random = sqrt(distribution(generator));
		glm::vec3 point;
		point.x = 1 - random;
		point.y = distribution(generator);
		point.z = 1 - point.x - point.y;

		//position = ((vertex1.position + vertex2.position + vertex3.position) / 3.0f)/* + glm::vec3(r1, r2, r3)*/;
		//texCoords = ((vertex1.texCoords + vertex2.texCoords + vertex3.texCoords) / 3.0f)/* + glm::vec2(r1, r2)*/;
		//normal = ((vertex1.normal + vertex2.normal + vertex3.normal) / 3.0f)/* + glm::vec3(r1, r2, r3)*/;
		texCoords = glm::mat3x2(vertex1.texCoords, vertex2.texCoords, vertex3.texCoords) * point;
		position = glm::mat3(vertex1.position, vertex2.position, vertex3.position) * point;
		normal = glm::mat3(vertex1.normal, vertex2.normal, vertex3.normal) * point;
	}
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);


	void Draw();
	std::vector<Vertex> GetVertices();
	std::vector<Triangle> triangles;
private:
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	unsigned int mVAO, mVBO, mEBO;

	void SetupMesh();
	Shape mShape;
};