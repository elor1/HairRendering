#pragma once
#include "Shape.h"

class HairPatch
{
public:
	HairPatch() = default;
	virtual ~HairPatch();

	void Initialise();
	void Draw();
	void TestData(GLfloat* data, int numVertices, float time);

private:
	Shape mPatch;
};