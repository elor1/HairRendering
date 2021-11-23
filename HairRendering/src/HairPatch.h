#pragma once
#include "Shape.h"

class HairPatch
{
public:
	HairPatch() = default;
	virtual ~HairPatch();

	void Initialise();
	void Draw();

private:
	Shape mPatch;
};