#pragma once
#include <vector>
#include <glm.hpp>
#include "Shape.h"
#include <random>
#include <limits>

struct Vector3
{
	float x, y, z;

	Vector3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vector3(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}

	float Dot(Vector3 vec)
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	Vector3 Cross(Vector3 vec)
	{
		return Vector3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
	}

	Vector3 operator+(Vector3 vec)
	{
		return Vector3(x + vec.x, y + vec.y, z + vec.z);
	}

	Vector3 operator-(Vector3 vec)
	{
		return Vector3(x - vec.x, y - vec.y, z - vec.z);
	}

	Vector3 operator*(float scale)
	{
		return Vector3(x * scale, y * scale, z * scale);
	}
};

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

	//Triangle-ray intersection
	/*bool IsIntersecting(glm::vec3& intersection, glm::vec3 position, glm::vec3 randomDir)
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
	}*/

	//https://wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	bool Intersects(Vector3 rayVector, Vector3 rayOrigin)
	{
		Vector3 v1 = Vector3(vertex1.position.x, vertex1.position.y, vertex1.position.z);
		Vector3 v2 = Vector3(vertex2.position.x, vertex2.position.y, vertex2.position.z);
		Vector3 v3 = Vector3(vertex3.position.x, vertex3.position.y, vertex3.position.z);

		Vector3 edge1, edge2, h, s, q;
		float a, f, u, v;

		edge1 = v2 - v1;
		edge2 = v3 - v1;
		h = rayVector.Cross(edge2);
		a = edge1.Dot(h);

		if (a > -0.0000001 && a < 0.0000001)
		{
			return false;
		}
		
		f = 1.0f / a;
		s = rayOrigin - v1;
		u = f * s.Dot(h);
		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		q = s.Cross(edge1);
		v = f * rayVector.Dot(q);
		if ((v < 0.0f) || (u + v > 1.0f))
		{
			return false;
		}

		float t = f * edge2.Dot(q);
		if (t > 0.0000001)
		{
			Vector3 intersectionPoint = rayOrigin + rayVector * t;
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

class MeshOctree;

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, float scale);


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
	float mScale;

	MeshOctree* mOctree;
};