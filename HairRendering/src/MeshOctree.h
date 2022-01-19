#pragma once
#include "Mesh.h"

struct OctreeNode
{
	int maxDepthDistance;
	OctreeNode** children;
	std::vector<Triangle> triangles;
	glm::vec3 min;
	glm::vec3 max;

	OctreeNode(glm::vec3 minimum, glm::vec3 maximum, int distance)
	{
		min = minimum;
		max = maximum;
		maxDepthDistance = distance;
		children = NULL;
	}

	~OctreeNode()
	{
		delete[] children;
	}

	bool ContainsPoint(glm::vec3 point)
	{
		if (point.x < min.x)
		{
			return false;
		}

		if (point.y < min.y)
		{
			return false;
		}

		if (point.z < min.z)
		{
			return false;
		}

		if (point.x > max.x)
		{
			return false;
		}

		if (point.y > max.y)
		{
			return false;
		}

		if (point.z > max.z)
		{
			return false;
		}

		return true;
	}

	void Split()
	{
		if (maxDepthDistance <= 0)
		{
			return;
		}

		if (children)
		{
			return;
		}

		children = new OctreeNode * [8];
		glm::vec3 half = (min + max) / 2.0f;

		int dist = maxDepthDistance - 1;
		children[0] = new OctreeNode(glm::vec3(min.x, min.y, min.z), glm::vec3(half.x, half.y, half.z), dist);
		children[1] = new OctreeNode(glm::vec3(min.x, min.y, half.z), glm::vec3(half.x, half.y, max.z), dist);
		children[2] = new OctreeNode(glm::vec3(min.x, half.y, min.z), glm::vec3(half.x, max.y, half.z), dist);
		children[3] = new OctreeNode(glm::vec3(half.x, min.y, min.z), glm::vec3(max.x, half.y, half.z), dist);
		children[4] = new OctreeNode(glm::vec3(half.x, half.y, min.z), glm::vec3(max.x, max.y, half.z), dist);
		children[5] = new OctreeNode(glm::vec3(min.x, half.y, half.z), glm::vec3(half.x, max.y, max.z), dist);
		children[6] = new OctreeNode(glm::vec3(half.x, min.y, half.z), glm::vec3(max.x, half.y, max.z), dist);
		children[7] = new OctreeNode(glm::vec3(half.x, half.y, half.z), glm::vec3(max.x, max.y, max.z), dist);

		for (auto& t : triangles)
		{
			for (int i = 0; i < 8; i++)
			{
				if (children[i]->ContainsPoint(t.vertex1.position) || children[i]->ContainsPoint(t.vertex2.position) || children[i]->ContainsPoint(t.vertex3.position))
				{
					children[i]->triangles.push_back(t);
				}
			}
		}
	}
};

class MeshOctree
{
public:
	MeshOctree(Mesh* mesh);
	~MeshOctree();

	void GetTriangles(std::vector<Triangle>& triangles, glm::vec3 point, glm::vec3 direction);

private:
	void CalculateBounds();
	bool IsIntersecting(glm::vec3 point, glm::vec3 direction, glm::vec3 minCorner, glm::vec3 maxCorner);
	void AddTriangle(Triangle t);

	Mesh* mMesh;
	OctreeNode* mRoot;

	glm::vec3 mMin;
	glm::vec3 mMax;

	int maxDepth;
	int minTrianglesForSplit;
};