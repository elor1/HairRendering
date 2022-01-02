#include "Application.h"
#include <iostream>
#include "ShaderProgram.h"
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Simulation.h"
#include "Hair.h"
#include "Model.h"

#define USE_MESH true
#define USE_TEXTURE false

Application::Application(int width, int height)
{
	mPrevTime = glfwGetTime();
	mCurrentTime = glfwGetTime();
	mWidth = width;
	mHeight = height;
	mHairDensity = 40;
	mFirstMouse = true;
	mLastX = width / 2.0;
	mLastY = width / 2.f;
	mFrame = 0;

	Initialise();
}

Application::Application()
{
	if (USE_MESH)
	{
		delete mMesh;
	}
	delete mSimulation;
	delete mHair;
}

Application::~Application()
{
	delete mMesh;
	delete mSimulation;
	delete mHair;
	delete mHairProgram;
	delete mMeshProgram;
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
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	//Callback functions
	glfwSetFramebufferSizeCallback(mWindow, FrameBufferCallback);

	glfwSetWindowUserPointer(mWindow, this);
	auto CursorCallback = [](GLFWwindow* window, double xPos, double yPos)
	{
		Application* app = (Application*)glfwGetWindowUserPointer(window);
		app->MouseCallback(window, xPos, yPos);
	};

	glfwSetCursorPosCallback(mWindow, CursorCallback);

	//Initialise GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR: Failed to initialise GLEW" << std::endl;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	mCamera = new Camera(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 180.0f, 0.0f);

	//Shaders
	mMeshProgram = new ShaderProgram("../HairRendering/src/shaders/mesh.vert", "../HairRendering/src/shaders/mesh.frag");
	mHairProgram = new ShaderProgram("../HairRendering/src/shaders/hair.vert", "../HairRendering/src/shaders/hair.frag", "../HairRendering/src/shaders/hair.geom", "../HairRendering/src/shaders/hair.tcs", "../HairRendering/src/shaders/hair.tes");

	InitSimulation();
}

void Application::InitSimulation()
{
	delete mMesh;
	delete mSimulation;
	Hair* oldHair = mHair;

	mSimulation = new Simulation();
	if (USE_MESH)
	{
		if (USE_TEXTURE)
		{
			Model* model = new Model("../models/Head2.obj");
			mMesh = model->GetFirstMesh();
			mHair = new Hair(mMesh, mHairDensity, "../images/hair.jpg", mSimulation, mHair);
		}
		else
		{
			Model* model = new Model("../models/Head2.obj");
			mMesh = model->GetFirstMesh();
			Model* scalp = new Model("../models/ScalpLow.obj");
			mHair = new Hair(scalp->GetFirstMesh(), mHairDensity, mSimulation, mHair);
		}
	}
	else
	{
		if (oldHair)
		{
			mHair = new Hair(oldHair, mSimulation);
		}
		else
		{
			mHair = new Hair(1, mSimulation);
		}
	}

	delete oldHair;
}

void Application::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float time = mFrame++ / 60.0f;
	
	mSimulation->Update(time);
	mHair->Update(time);
	
	mHairProgram->Bind();
	mHairProgram->uniforms.projection = glm::perspective(0.8f, (float)mWidth / mHeight, 0.1f, 100.0f);
	//mHairProgram->uniforms.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mHairProgram->uniforms.view = mCamera->GetViewMatrix();
	mHairProgram->uniforms.model = glm::mat4(1.0f);
	mHairProgram->uniforms.lightPosition = glm::vec3(2.0f, 1.0f, 2.0f);
	mHairProgram->SetGlobalUniforms();
	
	mHair->Draw(mHairProgram);
	mHairProgram->Unbind();
	
	if (USE_MESH)
	{
		mMeshProgram->Bind();

		mMeshProgram->uniforms.projection = mHairProgram->uniforms.projection;
		mMeshProgram->uniforms.view = mHairProgram->uniforms.view;
		mMeshProgram->uniforms.model = mHairProgram->uniforms.model;
		mMeshProgram->uniforms.lightPosition = mHairProgram->uniforms.lightPosition;
		mMeshProgram->SetGlobalUniforms();
		mMeshProgram->SetObjectUniforms();

		mMesh->Draw();
		mMeshProgram->Unbind();
	}
}

void Application::Update()
{
	mCurrentTime = glfwGetTime();
	mDeltaTime = mCurrentTime - mPrevTime;

	ProcessInput();

	//Draw 60 times per second
	if (mDeltaTime >= 1.0f / 60.0f)
	{
		std::cout << 1 / mDeltaTime << " FPS" << std::endl;
		Draw();

		//Swap front and back buffers
		glfwSwapBuffers(mWindow);

		mPrevTime = mCurrentTime;
	}
	
	//Poll events
	glfwPollEvents();
}

void Application::SetPatchHair(int numHairs)
{
	mHair->mNumGroupHairs = numHairs;
}

void Application::SetNumSplineVertices(int numVertices)
{
	mHair->mNumSplineVertices = numVertices;
}

void Application::SetHairColour(glm::vec3 colour)
{
	mHair->mColour = colour / 2550.0f;
}

void Application::ProcessInput()
{
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(mWindow, true);
	}

	if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		mCamera->Move(EMovementDirection::Forward, mDeltaTime);
	}

	if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		mCamera->Move(EMovementDirection::Backward, mDeltaTime);
	}

	if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		mCamera->Move(EMovementDirection::Left, mDeltaTime);
	}

	if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		mCamera->Move(EMovementDirection::Right, mDeltaTime);
	}

	if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mCamera->Rotate(0.0f, mDeltaTime);
	}

	if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mCamera->Rotate(0.0f, -mDeltaTime);
	}

	if (glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		mCamera->Rotate(-mDeltaTime, 0.0f);
	}

	if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		mCamera->Rotate(mDeltaTime, 0.0f);
	}
}

void Application::FrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
}

void Application::MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (mFirstMouse)
	{
		mLastX = xPos;
		mLastY = yPos;
		mFirstMouse = false;
	}

	float xoffset = xPos - mLastX;
	float yoffset = mLastY - yPos;

	mLastX = xPos;
	mLastY = yPos;

	mCamera->Rotate(xoffset, yoffset);
}
