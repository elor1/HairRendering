#include "MeshOctree.h"
#include <queue>

MeshOctree::MeshOctree(Mesh* mesh)
{
	mMesh = mesh;
	maxDepth = 4;
	minTrianglesForSplit = 5;

	CalculateBounds();

	mRoot = new OctreeNode(mMin, mMax, maxDepth - 1);

	for (auto& triangle : mMesh->triangles)
	{
		AddTriangle(triangle);
	}
}

MeshOctree::~MeshOctree()
{
	delete mRoot;
}

void MeshOctree::GetTriangles(std::vector<Triangle>& triangles, glm::vec3 point, glm::vec3 direction)
{
	std::vector<Triangle> tris;
	std::queue<OctreeNode*> queue;
	queue.push(mRoot);

	while (!queue.empty())
	{
		const OctreeNode* current = queue.front();
		queue.pop();

		if (IsIntersecting(point, direction, current->min, current->max))
		{
			if (!current->children)
			{
				for (auto& t : current->triangles)
				{
					tris.push_back(t);
				}
			}
			else
			{
				for (int i = 0; i < 8; i++)
				{
					queue.push(current->children[i]);
				}
			}
		}
	}

	triangles = tris;
}

void MeshOctree::CalculateBounds()
{
	if (mMesh->triangles.size() <= 0)
	{
		return;
	}

	mMin = mMesh->triangles[0].vertex1.position;
	mMax = mMesh->triangles[0].vertex1.position;

	for (auto& t : mMesh->triangles)
	{
		glm::vec3 point = t.vertex1.position;
		for (int i = 0; i < 3; i++)
		{
			if (i == 1)
			{
				point = t.vertex2.position;
			}

			if (i == 2)
			{
				point = t.vertex3.position;
			}

			mMin.x = glm::min(point.x, mMin.x);
			mMax.x = glm::max(point.x, mMax.x);
			mMin.y = glm::min(point.y, mMin.y);
			mMax.y = glm::max(point.y, mMax.y);
			mMin.z = glm::min(point.z, mMin.z);
			mMax.z = glm::max(point.z, mMax.z);
		}
	}
}

bool MeshOctree::IsIntersecting(glm::vec3 point, glm::vec3 direction, glm::vec3 minCorner, glm::vec3 maxCorner)
{
	glm::vec3 invDir = glm::vec3(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z);

	float t1 = (minCorner.x - point.x) * invDir.x;
	float t2 = (maxCorner.x - point.x) * invDir.x;
	float t3 = (minCorner.y - point.y) * invDir.y;
	float t4 = (maxCorner.y - point.y) * invDir.y;
	float t5 = (minCorner.z - point.z) * invDir.z;
	float t6 = (maxCorner.z - point.z) * invDir.z;

	float tMin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
	float tMax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

	if (tMax < 0)
	{
		return false;
	}

	if (tMin > tMax)
	{
		return false;
	}

	return true;
}

void MeshOctree::AddTriangle(Triangle t)
{
	std::queue<OctreeNode*> queue;
	queue.push(mRoot);

	while (!queue.empty())
	{
		OctreeNode* current = queue.front();
		queue.pop();

		if (current->ContainsPoint(t.vertex1.position) || current->ContainsPoint(t.vertex2.position) || current->ContainsPoint(t.vertex3.position))
		{
			current->triangles.push_back(t);

			if (!current->children)
			{
				if (current->maxDepthDistance > 0 && current->triangles.size() >= minTrianglesForSplit)
				{
					current->Split();
				}
			}

			if (!current->children)
			{
				for (int i = 0; i < 8; i++)
				{
					queue.push(current->children[i]);
				}
			}
		}
	}
}
