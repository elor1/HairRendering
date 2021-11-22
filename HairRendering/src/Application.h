#pragma once
#include <glew.h>
#include <glfw3.h>
#include "Shape.h"

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

	Shape mTriangle;
	GLuint mBaseShader;
	GLuint mFullShader;

	GLFWwindow* mWindow;
	int mWidth;
	int mHeight;

	double mPrevTime;
	double mCurrentTime;
	float mFPS;
};