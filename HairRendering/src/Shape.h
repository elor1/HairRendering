#pragma once
#include <glew.h>
#include <vector>

struct Vertex;

class Shape
{
public:
	Shape();
	virtual ~Shape() = default;

	void Create();
	void Destroy();
	void SetVertexData(float* data, int size, int numVertices);
	void SetAttribute(GLuint index, GLint size, GLenum type, GLboolean normalised, GLsizei step, int offset);
	void Draw(GLenum mode);

private:
	bool mIsCreated;
	GLuint mVBO;
	GLuint mVAO;
	int mNumVertices;
};