#include "Strand.h"
#include <iostream>

Strand::Strand(int numSegments, double length, glm::vec3 position, glm::vec3 direction)
{
	if (numSegments < 1)
	{
		std::cout << "ERROR: Must have at least 1 hair segment" << std::endl;
	}

	if (length <= 0)
	{
		std::cout << "ERROR: Hair length must be higher than 0" << std::endl;
	}

	direction = glm::normalize(direction);
	mNumSegments = numSegments;
	mLength = length;

	double step = (double)length / numSegments;
	for (int i = 0; i < numSegments; i++)
	{
		HairVertex* newVertex = new HairVertex(position + direction * (float)(step * i));
		if (i > 0)
		{
			HairVertex* oldVertex = mVertices[i - 1];
			newVertex->theta = acos(glm::clamp(glm::dot(oldVertex->position - newVertex->position, glm::vec3(0.0f, -1.0f, 0.0f)), -1.0f, 1.0f));
		}
		newVertex->segmentLength = step;
		mVertices.push_back(newVertex);
	}

	GLfloat data[] = { -0.5f, 0.5f, 0.0f,
					  0.5f, 0.5f, 0.0f,
					  -0.5f, -0.5f, 0.0f,
					  0.5f, -0.5f, 0.0f };
	mPatch.Create();
	mPatch.SetVertexData(data, sizeof(data), 4);
	mPatch.SetAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

Strand::~Strand()
{
	mPatch.Destroy();
}

void Strand::Update(float time)
{
}

void Strand::Draw(ShaderProgram &program)
{
	program.uniforms.numHairVertices = glm::min((int)mVertices.size(), 64);
	for (int i = 0; i < program.uniforms.numHairVertices; i++)
	{
		program.uniforms.vertexData[i] = mVertices[i]->position;
	}
	program.uniforms.colour = glm::vec3(0.6f, 0.4f, 0.3f);
	program.uniforms.numGroupHairs = 10;
	program.uniforms.groupWidth = 0.2f;
	program.uniforms.hairRadius = 0.005f;
	program.uniforms.numSplineVertices = 20;//program.uniforms.numHairVertices;

	program.SetUniforms();

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	mPatch.Draw(GL_PATCHES);
}



