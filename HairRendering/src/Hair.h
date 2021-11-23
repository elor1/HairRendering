#pragma once
#include <glew.h>
#include <vector>

class Simulation;
class Strand;

class Hair
{
public:
	Hair(int numGuides, Simulation* simulation);

	void Update(float time);
	void Draw(GLuint program);

	std::vector<Strand*> mGuideHairs;
	Simulation* mSimulation;
};