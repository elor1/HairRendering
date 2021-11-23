#pragma once
#include <glew.h>
#include <glfw3.h>
#include "HairPatch.h"

class Application
{
public:
	Application(int width, int height);
	~Application();

	void Run();

private:
	void Initialise();
	void Draw();
	void Update();

	HairPatch mHairPatch;
	GLuint mBaseShader;
	GLuint mFullShader;

	GLFWwindow* mWindow;
	int mWidth;
	int mHeight;

	double mPrevTime;
	double mCurrentTime;
	double mDeltaTime;
};