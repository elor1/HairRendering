#pragma once
#include "glew.h"
class ShaderProgram;

class Tesselator
{
public:
	Tesselator();
	virtual ~Tesselator();

	void Initialise(int numTriangles);
	void Begin();
	void End();
	void Draw();

private:
	ShaderProgram* mProgram;
	int mNumTriangles;
	GLuint mQuery;
	GLuint mVAO;
	GLuint mVBO;

	void SetNumTriangles(int numTriangles);
};