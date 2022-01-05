#pragma once
#include <glew.h>
#include <vector>
#include "ShaderProgram.h"
#include "Mesh.h"

class Simulation;
class Strand;

class Hair
{
public:
	Hair(int numGuides, Simulation* simulation);
	Hair(Mesh* mesh, float hairDensity, Simulation* simulation, Hair* oldHair = nullptr);

	void Update(float time);
	void Draw(ShaderProgram* program);
	void SetAttributes(Hair* oldHair);
	void SetAttributes(glm::vec3 colour = glm::vec3(0.6f, 0.4f, 0.3f), int numGroupHairs = 15, float groupSpread = 0.15f, float hairRadius = 0.005f, float noiseAmplitude = 0.1f, int numSplineVertices = 20);

	std::vector<Strand*> mGuideHairs;
	Simulation* mSimulation;
	int mNumGuideHairs;
	int mNumHairVertices;
	int mNumPatchHair;
	glm::vec3 mColour;
	int mNumGroupHairs;
	float mGroupSpread;
	float mHairRadius;
	float mNoiseAmplitude;
	int mNumSplineVertices;
};