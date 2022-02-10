#include "Mesh.h"
#include <glew.h>
#include <glm.hpp>
#include <gtc/random.hpp>
#include "MeshOctree.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, float scale)
{
	mVertices = vertices;
	mIndices = indices;

	mMin = glm::vec3(std::numeric_limits<float>::max());
	mMax = glm::vec3(std::numeric_limits<float>::min());

	mScale = scale;

	for (auto& vertex : vertices)
	{
		mMin = glm::min(mMin, vertex.position /** mScale*/);
		mMax = glm::max(mMax, vertex.position /** mScale*/);
	}

	SetupMesh();
}

void Mesh::SetupMesh()
{
	/*for (unsigned int i = 0; i < mVertices.size() / 3; i++)
	{
		unsigned int index = i * 3;
		Triangle triangle(mVertices[index], mVertices[index + 1], mVertices[index + 2]);
		triangles.push_back(triangle);
	}*/

	//Create triangles
	for (int i = 0; i * 3 < mIndices.size(); i++)
	{
		Vertex v1 = mVertices[mIndices[3 * i]];
		Vertex v2 = mVertices[mIndices[3 * i + 1]];
		Vertex v3 = mVertices[mIndices[3 * i + 2]];

		v1.position *= mScale;
		v2.position *= mScale;
		v3.position *= mScale;

		triangles.push_back(Triangle(v1, v2, v3));
	}

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void Mesh::Draw()
{
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

std::vector<Vertex> Mesh::GetVertices()
{
	return mVertices;
}

bool Mesh::Contains(glm::vec3 &normal, glm::vec3 position)
{
	//Ellipsoid collisions
	/*normal = glm::normalize(position / glm::pow(mMax, glm::vec3(2.0f)));
	return glm::dot(glm::pow(position / mMax, glm::vec3(2.0f)), glm::vec3(1.0f)) < 1;*/

	if (glm::any(glm::lessThan(position, mMin)) || glm::any(glm::greaterThan(position, mMax)))
	{
		return false;
	}

	Vector3 direction = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 pos = Vector3(position.x, position.y, position.z);
	int numIntersections = 0;

	for (auto& triangle : triangles)
	{
		//glm::vec3 intersectionPoint = glm::vec3(0.0f);
		//if (triangle.IsIntersecting(intersectionPoint, position, glm::normalize(position)))
		//{
		//	//normal = (triangle.vertex1.normal + triangle.vertex2.normal + triangle.vertex3.normal) / 3.0f;
		//	normal = (triangle.vertex1.normal + triangle.vertex2.normal + triangle.vertex3.normal) / 3.0f;
		//	numIntersections++;
		//}

		if (triangle.Intersects(direction, pos))
		{
			normal = (triangle.vertex1.normal + triangle.vertex2.normal + triangle.vertex3.normal) / 3.0f;
			numIntersections++;
		}
	}

	if (numIntersections % 2 == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
