#include "Quad.h"

void Quad::Initialise()
{
	GLfloat data[] = { -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
					   -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
					   1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
					   1.0f, -1.0f, 0.0f, 1.0f, 0.0f };

	mShape.Create();
	mShape.SetVertexData(data, sizeof(data), 4);
	mShape.SetAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	mShape.SetAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
}

void Quad::Draw()
{
	mShape.Draw(GL_TRIANGLE_STRIP);
}
