#pragma once
#include <glew.h>
#include <vector>
#include "ShaderProgram.h"

class Simulation;
class Strand;

class Hair
{
public:
	Hair(int numGuides, Simulation* simulation);

	void Update(float time);
	void Draw(ShaderProgram &program);

	std::vector<Strand*> mGuideHairs;
	Simulation* mSimulation;
};