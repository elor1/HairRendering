#include "Application.h"
#include <iostream>
#include "ShaderProgram.h"
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Simulation.h"
#include "Hair.h"
#include "Mesh.h"

#define USE_MESH false

Application::Application(int width, int height)
{
	mPrevTime = glfwGetTime();
	mCurrentTime = glfwGetTime();
	mWidth = width;
	mHeight = height;

	Initialise();
}

Application::Application()
{
	delete mMesh;
	delete mSimulation;
	delete mHair;
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
	mMeshProgram = new ShaderProgram("../HairRendering/src/shaders/base.vert", "../HairRendering/src/shaders/base.frag");
	mHairProgram = new ShaderProgram("../HairRendering/src/shaders/full.vert", "../HairRendering/src/shaders/full.frag", "../HairRendering/src/shaders/full.geom", "../HairRendering/src/shaders/full.tcs", "../HairRendering/src/shaders/full.tes");

	mSimulation = new Simulation();

	if (USE_MESH)
	{
		mMesh = new Mesh("../models/Head.obj");
		mHair = new Hair(mMesh, mSimulation);
	}
	else
	{
		mHair = new Hair(1, mSimulation);
	}
	
	glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void Application::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	mSimulation->Update(mDeltaTime);
	mHair->Update(mDeltaTime);
	
	mHairProgram->Bind();
	mHairProgram->uniforms.projection = glm::perspective(0.8f, (float)mWidth / mHeight, 0.1f, 100.0f);
	mHairProgram->uniforms.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mHairProgram->SetGlobalUniforms();

	mHairProgram->uniforms.model = glm::mat4(1.0f);
	mHair->Draw(*mHairProgram);
	mHairProgram->Unbind();

	if (USE_MESH)
	{
		mMeshProgram->Bind();
		glUniformMatrix4fv(glGetUniformLocation(mMeshProgram->GetID(), "projection"), 1, GL_FALSE, glm::value_ptr(mHairProgram->uniforms.projection));
		glUniformMatrix4fv(glGetUniformLocation(mMeshProgram->GetID(), "view"), 1, GL_FALSE, glm::value_ptr(mHairProgram->uniforms.view));
		glUniformMatrix4fv(glGetUniformLocation(mMeshProgram->GetID(), "model"), 1, GL_FALSE, glm::value_ptr(mHairProgram->uniforms.model));
		mMesh->Draw();
		mMeshProgram->Unbind();
	}
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
