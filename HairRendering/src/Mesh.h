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

	bool operator==(const Vertex& other) const
	{
		return (position == other.position && texCoords == other.texCoords && normal == other.normal);
	}
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
		return glm::length(glm::cross(vertex3.position - vertex1.position, vertex2.position - vertex1.position)) / 2.0f;
	}

	void RandomPoint(Vertex& v)
	{
		float t = sqrt(rand() / (float)RAND_MAX);
		glm::vec3 randomPoint;
		randomPoint.x = 1 - t;
		randomPoint.y = rand() / (float)RAND_MAX;
		randomPoint.z = 1 - randomPoint.x - randomPoint.y;
		v.position = glm::mat3(vertex1.position, vertex2.position, vertex3.position) * randomPoint;
		v.texCoords = glm::mat3x2(vertex1.texCoords, vertex2.texCoords, vertex3.texCoords) * randomPoint;
		v.normal = glm::mat3(vertex1.normal, vertex2.normal, vertex3.normal) * randomPoint;
	}

	//https://wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	bool Intersects(glm::vec3 rayVector, glm::vec3 rayOrigin)
	{
		glm::vec3 edge1, edge2, h, s, q;
		float a, f, u, v;

		edge1 = vertex2.position - vertex1.position;
		edge2 = vertex3.position - vertex1.position;
		h = glm::cross(rayVector, edge2);
		a = glm::dot(edge1, h);

		if (a > -0.0000001 && a < 0.0000001)
		{
			return false;
		}
		
		f = 1.0f / a;
		s = rayOrigin - vertex1.position;
		u = f * glm::dot(s, h);
		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		q = glm::cross(s, edge1);
		v = f * glm::dot(rayVector, q);
		if ((v < 0.0f) || (u + v > 1.0f))
		{
			return false;
		}

		float t = f * glm::dot(edge2, q);
		if (t > 0.0000001)
		{
			glm::vec3 intersectionPoint = rayOrigin + rayVector * t;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool operator==(const Triangle& other) const
	{
		return (vertex1 == other.vertex1 && vertex2 == other.vertex2 && vertex3 == other.vertex3);
	}
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, float scale);


	void Draw();
	std::vector<Vertex> GetVertices() const;
	void SetVertices(const std::vector<Vertex> vertices);
	std::vector<Triangle> triangles;
	bool Contains(glm::vec3 &normal, glm::vec3 position);

private:
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	unsigned int mVAO, mVBO, mEBO;

	void SetupMesh();
	glm::vec3 mMin;
	glm::vec3 mMax;
	float mScale;
};