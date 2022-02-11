#pragma once
#include <glew.h>
#include <vector>
#include "shaderPrograms/ShaderProgram.h"
#include "Mesh.h"
#include "Strand.h"

class Texture;
class Simulation;

class Hair
{
public:
	friend class GuiWindow;
	Hair(int numGuides, Simulation* simulation);
	Hair(Mesh* mesh, float hairDensity, Simulation* simulation, Hair* oldHair = nullptr);
	Hair(Mesh* mesh, float hairDensity, const char* hairMap, Simulation* simulation, Hair* oldHair = nullptr);
	~Hair();

	void Update(float time);
	void Draw(ShaderProgram* program);

	void SetAttributes(Hair* oldHair);
	void SetAttributes(glm::vec3 colour = glm::vec3(0.25f, 0.17f, 0.15f), int numGroupHairs = 64, float groupSpread = 0.3f, float hairRadius = 0.003f, float noiseAmplitude = 0.3f, float noiseFrequency =  0.2f, int numSplineVertices = 20);

	std::vector<Strand*> GetGuideHairs();
	int GetNumGroupHairs();
	void SetNumGroupHairs(int num);
	void SetNumSplineVertices(int num);
	void SetColour(glm::vec3 colour);
	Texture* GetHairMap();
	glm::vec3 GetColour();
	float GetShadowIntensity();
	float GetDiffuseIntensity();
	float GetSpecularIntensity();
	float GetOpacity();
	float GetColourChange();

private:
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
	float mNoiseFrequency;
	int mNumSplineVertices;
	Texture* mHairMap;
	float mShadowIntensity;
	float mDiffuseIntensity;
	float mSpecularIntensity;
	float mMaxLength = 0.45f;
	float mOpacity = 0.75f;
	float mColourChange;
};