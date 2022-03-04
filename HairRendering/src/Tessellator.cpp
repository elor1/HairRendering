#include "Tessellator.h"
#include "shaderPrograms/FeedbackShaderProgram.h"

Tesselator::Tesselator()
{
	const GLchar* varyings[] = { "pos", "tangent", "tessx" };
	mProgram = new FeedbackShaderProgram(varyings, 3, "src/shaders/hair.vert", "", "", "src/shaders/hair.tcs", "src/shaders/hair.tes");

	mNumTriangles = 0;
	mQuery = 0;
	mVAO = 0;
	mVBO = 0;
}

Tesselator::~Tesselator()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	glDeleteQueries(1, &mQuery);
	delete mProgram;
}

void Tesselator::Initialise(int numTriangles)
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenQueries(1, &mQuery);

	SetNumTriangles(numTriangles);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Tesselator::Begin()
{
	mProgram->Bind();
	glEnable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mVBO);

	glBeginTransformFeedback(GL_TRIANGLES);
}

void Tesselator::End()
{
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	mProgram->Unbind();
	glFlush();
}

void Tesselator::Draw()
{
	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, mNumTriangles * 3);
	glBindVertexArray(0);
}

void Tesselator::SetNumTriangles(int numTriangles)
{
	if (numTriangles != mNumTriangles)
	{
		mNumTriangles = numTriangles;
		int bufferSize = numTriangles * 24 * sizeof(GLfloat);

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_COPY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
