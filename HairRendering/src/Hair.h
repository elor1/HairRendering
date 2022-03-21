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
	Hair(Mesh* mesh, float hairDensity, const char* hairMap, double maxLength, Simulation* simulation, Hair* oldHair = nullptr);
	~Hair();

	void Update(float time);
	void Draw(ShaderProgram* program);

	void SetAttributes(Hair* oldHair);
	void SetAttributes(glm::vec3 colour = glm::vec3(0.25f, 0.17f, 0.15f), int numGroupHairs = 64, float groupSpread = 0.3f, float hairRadius = 0.003f, float noiseAmplitude = 0.3f, float noiseFrequency =  0.2f, int numSplineVertices = 20, float shadowIntensity = 20.0f, float diffuseIntensity = 1.0f, float specularIntensity = 0.5f, float opacity = 0.25, float colourChange = 1.5f);

	std::vector<Strand*> GetGuideHairs() const;
	int GetNumGroupHairs() const;
	void SetNumGroupHairs(const int num);
	void SetNumSplineVertices(const int num);
	void SetColour(const glm::vec3 colour);
	void SetShadowIntensity(const float intensity);
	void SetGroupSpread(const float spread);
	Texture* GetHairMap() const;
	glm::vec3 GetColour() const;
	float GetShadowIntensity() const;
	float GetDiffuseIntensity() const;
	float GetSpecularIntensity() const;
	float GetOpacity() const;
	float GetColourChange() const;

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
	float mOpacity = 0.75f;
	float mColourChange;
};