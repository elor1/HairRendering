#include "Strand.h"
#include <iostream>

Strand::Strand(int numSegments, double length, glm::vec3 position)
{
	if (numSegments < 2)
	{
		std::cout << "ERROR: Must have at least 2 hair segments" << std::endl;
	}

	if (length <= 0)
	{
		std::cout << "ERROR: Hair length must be higher than 0" << std::endl;
	}

	mNumSegments = numSegments;
	mLength = length;

	double step = (double)length / numSegments;
	for (int i = 0; i < numSegments; i++)
	{
		mVertices.push_back(new HairVertex(glm::vec3(position.x, position.y - step * i, position.z)));

		if (i != 0)
		{
			glm::vec3 joint = (mVertices[i]->position + mVertices[i - 1]->position) / 2.0f;
			mJoints.push_back(new Joint(joint));
		}
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
	
	program.uniforms.colour = glm::vec3(0.6f, 0.4f, 0.3f);
	program.uniforms.numGroupHairs = 20;
	program.uniforms.numHairVertices = glm::min((int)mVertices.size(), 64);

	for (int i = 0; i < program.uniforms.numHairVertices; i++)
	{
		program.uniforms.vertexData[3 * i] = mVertices[i]->position.x;
		program.uniforms.vertexData[3 * i + 1] = mVertices[i]->position.y;
		program.uniforms.vertexData[3 * i + 2] = mVertices[i]->position.z;
	}

	program.SetUniforms();

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	mPatch.Draw(GL_PATCHES);
}



