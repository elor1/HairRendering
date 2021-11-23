#include "Hair.h"
#include <iostream>

Hair::Hair(int numSegments, double length, glm::vec3 position)
{
	if (numSegments < 2)
	{
		std::cout << "ERROR: Must have at least 2 hair segments" << std::endl;
	}

	if (length <= 0)
	{
		std::cout << "ERROR: HAir length must be higher than 0" << std::endl;
	}

	mNumSegments = numSegments;
	mLength = length;
	mVertices = new HairVertex[numSegments + 1];

	double step = (double)numSegments / length;
	for (int i = 0; i < numSegments; i++)
	{
		mVertices[i] = HairVertex(glm::vec3(position.x, position.y * i, position.z));
	}
}

Hair::~Hair()
{
	delete mVertices;
}

int Hair::GetNumSegments()
{
	return mNumSegments;
}

double Hair::GetLength()
{
	return mLength;
}

HairVertex Hair::GetVertex(int index)
{
	return mVertices[index];
}

HairVertex* Hair::GetVertices()
{
	return mVertices;
}
