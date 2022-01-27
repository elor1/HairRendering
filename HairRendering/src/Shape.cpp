#include "Shape.h"
#include <iostream>
#include "Mesh.h"

Shape::Shape()
{
	mIsCreated = false;
}

void Shape::Create()
{
	glGenBuffers(1, &mVBO);
	glGenVertexArrays(1, &mVAO);

	mIsCreated = true;
}

void Shape::Destroy()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

void Shape::SetVertexData(float* data, int size, int numVertices)
{
	if (!mIsCreated)
	{
		std::cout << "ERROR: Can't use a shape before it is created" << std::endl;
		return;
	}

	mNumVertices = numVertices;

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Shape::SetAttribute(GLuint index, GLint size, GLenum type, GLboolean normalised, GLsizei step, int offset)
{
	if (!mIsCreated)
	{
		std::cout << "ERROR: Can't use a shape before it is created" << std::endl;
		return;
	}

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, normalised, step, (void*)offset);

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Shape::Draw(GLenum mode)
{
	if (!mIsCreated)
	{
		std::cout << "ERROR: Can't use a shape before it is created" << std::endl;
		return;
	}

	glBindVertexArray(mVAO);
	glDrawArrays(mode, 0, mNumVertices);
	glBindVertexArray(0);
}
