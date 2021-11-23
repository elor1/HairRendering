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

	void Run();

private:
	void Initialise();
	void Draw();
	void Update();

	Mesh* mMesh;
	Hair* mHair;
	Simulation* mSimulation;
	ShaderProgram* mMeshProgram;
	ShaderProgram* mHairProgram;

	GLFWwindow* mWindow;
	int mWidth;
	int mHeight;

	double mPrevTime;
	double mCurrentTime;
	double mDeltaTime;
};