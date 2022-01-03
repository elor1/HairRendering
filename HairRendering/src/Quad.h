#pragma once
#include "Shape.h"

class Quad
{
public:
	Quad() = default;
	virtual ~Quad() = default;

	void Initialise();
	void Draw();

private:
	Shape mShape;
};