#pragma once
#include <vector>
#include <glm.hpp>
#include "Shape.h"
#include <random>
#include <limits>

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

	//Triangle-ray intersection
	bool IsIntersecting(glm::vec3& intersection, glm::vec3 position, glm::vec3 randomDir)
	{
		glm::vec3 edge1 = vertex2.position - vertex1.position;
		glm::vec3 edge2 = vertex3.position - vertex1.position;
		glm::vec3 pVector = glm::cross(randomDir, edge2);

		float det = glm::dot(edge1, pVector);
		if (det == 0)
		{
			return false;
		}
		float inverseDet = 1.0f / det;

		glm::vec3 tVector = position - vertex1.position;
		float u = glm::dot(tVector, pVector) * inverseDet;
		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		glm::vec3 qVector = glm::cross(tVector, edge1);
		float v = glm::dot(randomDir, qVector) * inverseDet;
		if (v < 0.0f || u + v > 1.0f)
		{
			return false;
		}

		glm::vec3 point = vertex1.position + u * edge1 + v * edge2;
		float t = (point.x - position.x) / randomDir.x;
		if (t < 0)
		{
			return false;
		}

		intersection = point;
		return true;
	}
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);


	void Draw();
	std::vector<Vertex> GetVertices();
	std::vector<Triangle> triangles;
	bool Contains(glm::vec3 &normal, glm::vec3 position);

private:
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	unsigned int mVAO, mVBO, mEBO;

	void SetupMesh();
	Shape mShape;
	glm::vec3 mMin;
	glm::vec3 mMax;
};