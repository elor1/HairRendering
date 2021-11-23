#include "Application.h"
#include <iostream>
#include "ShaderProgram.h"
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>

Application::Application(int width, int height)
{
	mPrevTime = glfwGetTime();
	mCurrentTime = glfwGetTime();
	mWidth = width;
	mHeight = height;

	Initialise();
}

Application::~Application()
{
}

void Application::Run()
{
	//Loop until window closes
	while (!glfwWindowShouldClose(mWindow))
	{
		Update();
	}
}


void Application::Initialise()
{
	//Init GLFW
	if (!glfwInit())
	{
		std::cout << "ERROR: Failed to init GLFW" << std::endl;
		glfwTerminate();
	}

	//Create window
	mWindow = glfwCreateWindow(mWidth, mHeight, "Hello World", NULL, NULL);
	if (!mWindow)
	{
		std::cout << "ERROR: Failed to create window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(mWindow);

	//Initialise GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR: Failed to initialise GLEW" << std::endl;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Shaders
	ShaderProgram* baseShader = new ShaderProgram("../HairRendering/src/shaders/base.vert", "../HairRendering/src/shaders/base.frag");
	mBaseShader = baseShader->Load();

	ShaderProgram* fullShader = new ShaderProgram("../HairRendering/src/shaders/full.vert", "../HairRendering/src/shaders/full.frag", "", "../HairRendering/src/shaders/full.tcs", "../HairRendering/src/shaders/full.tes");
	mFullShader = fullShader->Load();

	mHairPatch.Initialise();
	glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void Application::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float fov = 0.8f;
	float aspectRatio = (float)mWidth / mHeight;
	float nearClip = 0.1f;
	float farClip = 100.0f;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 4.0f);
	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 projection = glm::perspective(fov, aspectRatio, nearClip, farClip);
	glm::mat4 view = glm::lookAt(cameraPos, center, up);
	glm::mat4 model(1.0f);

	glUseProgram(mFullShader);
	glUniformMatrix4fv(glGetUniformLocation(mBaseShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(mBaseShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(mBaseShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(glGetUniformLocation(mBaseShader, "colour"), 0.0f, 1.0f, 1.0f);

	mHairPatch.Draw();

	glUseProgram(0);
}

void Application::Update()
{
	mCurrentTime = glfwGetTime();
	mDeltaTime = mCurrentTime - mPrevTime;

	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(mWindow, true);
	}

	//Draw 60 times per second
	if (mDeltaTime >= 1.0f / 60.0f)
	{
		Draw();

		//Swap front and back buffers
		glfwSwapBuffers(mWindow);

		mPrevTime = mCurrentTime;
	}

	//Poll events
	glfwPollEvents();
}
