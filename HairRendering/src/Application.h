#pragma once
#include <glew.h>
#include <glfw3.h>
#include "HairPatch.h"
#include "ShaderProgram.h"

class Hair;
class Simulation;
class Mesh;

class Application
{
public:
	Application(int width, int height);
	Application();
	~Application();

	void Run();

private:
	void Initialise();
	void InitSimulation();
	void Draw();
	void Update();
	void SetPatchHair(int numHairs);
	void SetNumSplineVertices(int numVertices);
	void SetHairColour(glm::vec3 colour);

	Mesh* mMesh;
	Hair* mHair;
	Simulation* mSimulation;
	ShaderProgram* mMeshProgram;
	ShaderProgram* mHairProgram;
	float mHairDensity;

	GLFWwindow* mWindow;
	int mWidth;
	int mHeight;

	double mPrevTime;
	double mCurrentTime;
	double mDeltaTime;
};