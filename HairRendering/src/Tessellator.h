#pragma once
#include "glew.h"
class ShaderProgram;

class Tesselator
{
public:
	Tesselator();
	virtual ~Tesselator();

	void Initialise(int numLines);
	void Begin();
	void End();
	void Draw();

private:
	ShaderProgram* mProgram;
	int mNumLines;
	GLuint mQuery;
	GLuint mVAO;
	GLuint mVBO;

	void SetNumLines(int numLines);
};