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
#include "Texture.h"
#include "Framebuffer.h"

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

Application::~Application()
{
	delete mMesh;
	delete mSimulation;
	delete mHair;
	delete mHairProgram;
	delete mMeshProgram;
	delete mNoiseTexture;
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

	//Camera
	mCamera = new Camera(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 180.0f, 0.0f);

	//Shaders
	mMeshProgram = new ShaderProgram("../HairRendering/src/shaders/mesh.vert", "../HairRendering/src/shaders/mesh.frag");
	mHairProgram = new ShaderProgram("../HairRendering/src/shaders/hair.vert", "../HairRendering/src/shaders/hair.frag", "../HairRendering/src/shaders/hair.geom", "../HairRendering/src/shaders/hair.tcs", "../HairRendering/src/shaders/hair.tes");

	//Textures
	mNoiseTexture = new Texture();
	mNoiseTexture->Create("../images/Noise.jpg", GL_LINEAR, GL_LINEAR);
	mShadowDepthTexture = new Texture();
	mShadowDepthTexture->CreateDepthTexture(1024, 1024);

	//Framebuffers
	mShadowFramebuffer = new Framebuffer();
	mShadowFramebuffer->Create();
	mShadowFramebuffer->AttachDepthTexture(mShadowDepthTexture->GetID());

	InitSimulation();
}

void Application::InitSimulation()
{
	delete mMesh;
	delete mSimulation;

	//Head model
	Model* model = new Model("../models/Head2.obj");
	mMesh = model->GetFirstMesh();

	//Collision model
	Model* collisionModel = new Model("../models/LowPoly.obj", 1.25f);
	mCollider = collisionModel->GetFirstMesh();
	/*cube->triangles = { Triangle(Vertex(glm::vec3(-5.0f, -0.7f, -5.0f), glm::vec2(0.875f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f)), Vertex(glm::vec3(5.0f, -0.3f, 5.0f), glm::vec2(0.625, 0.75), glm::vec3(0.0f, 1.0f, 0.0f)), Vertex(glm::vec3(5.0f, -0.3f, -5.0f), glm::vec2(0.625, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f))),
	Triangle(Vertex(glm::vec3(5.0f, -0.3f, 5.0f), glm::vec2(0.625f, 0.75f), glm::vec3(0.0f, 0.0f, 1.0f)), Vertex(glm::vec3(-5.0f, -0.7f, 5.0f), glm::vec2(0.375, 1.0), glm::vec3(0.0f, 0.0f, 1.0f)), Vertex(glm::vec3(-5.0f, -0.3f, -5.0f), glm::vec2(0.375, 0.75f), glm::vec3(0.0f, 0.0f, 1.0f))),
	Triangle(Vertex(glm::vec3(-5.0f, -0.3f, 5.0f), glm::vec2(0.625f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f)), Vertex(glm::vec3(-5.0f, -0.7f, -5.0f), glm::vec2(0.375, 0.25), glm::vec3(-1.0f, 0.0f, 0.0f)), Vertex(glm::vec3(-5.0f, -0.7f, 5.0f), glm::vec2(0.375, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f))),
	Triangle(Vertex(glm::vec3(5.0f, -0.7f, -5.0f), glm::vec2(0.375f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f)), Vertex(glm::vec3(-5.0f, -0.7f, 5.0f), glm::vec2(0.125, 0.75), glm::vec3(0.0f, -1.0f, 0.0f)), Vertex(glm::vec3(-5.0f, -0.7f, -5.0f), glm::vec2(0.125, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f))),
	Triangle(Vertex(glm::vec3(5.0f, -0.3f, -5.0f), glm::vec2(0.625f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f)), Vertex(glm::vec3(5.0f, -0.7f, 5.0f), glm::vec2(0.375, 0.75), glm::vec3(1.0f, 0.0f, 0.0f)), Vertex(glm::vec3(5.0f, -0.7f, -5.0f), glm::vec2(0.375, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f))),
	Triangle(Vertex(glm::vec3(-5.0f, -0.3f, -5.0f), glm::vec2(0.625f, 0.25f), glm::vec3(0.0f, 0.0f, -1.0f)), Vertex(glm::vec3(5.0f, -0.7f, -5.0f), glm::vec2(0.375, 0.5), glm::vec3(0.0f, 0.0f, -1.0f)), Vertex(glm::vec3(-5.0f, -0.7f, -5.0f), glm::vec2(0.375, 0.25f), glm::vec3(0.0f, 0.0f, -1.0f))),
	Triangle(Vertex(glm::vec3(-5.0f, -0.3f, -5.0f), glm::vec2(0.875f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f)), Vertex(glm::vec3(-5.0f, -0.3f, 5.0f), glm::vec2(0.875, 0.75), glm::vec3(0.0f, 1.0f, 0.0f)), Vertex(glm::vec3(5.0f, -0.3f, 5.0f), glm::vec2(0.625, 0.75f), glm::vec3(0.0f, 1.0f, 0.0f))),
	Triangle(Vertex(glm::vec3(5.0f, -0.3f, 5.0f), glm::vec2(0.625f, 0.75f), glm::vec3(0.0f, 0.0f, 1.0f)), Vertex(glm::vec3(-5.0f, -0.3f, 5.0f), glm::vec2(0.625, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)), Vertex(glm::vec3(-5.0f, -0.7f, 5.0f), glm::vec2(0.375, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
	Triangle(Vertex(glm::vec3(-5.0f, -0.3f, 5.0f), glm::vec2(0.625f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)), Vertex(glm::vec3(-5.0f, -0.3f, -5.0f), glm::vec2(0.625, 0.25), glm::vec3(-1.0f, 0.0f, 0.0f)), Vertex(glm::vec3(-5.0f, -0.7f, -5.0f), glm::vec2(0.375, 0.25f), glm::vec3(-1.0f, 0.0f, 0.0f))),
	Triangle(Vertex(glm::vec3(5.0f, -0.7f, -5.0f), glm::vec2(0.375f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f)), Vertex(glm::vec3(5.0f, -0.7f, 5.0f), glm::vec2(0.375, 0.75), glm::vec3(0.0f, -1.0f, 0.0f)), Vertex(glm::vec3(-5.0f, -0.7f, 5.0f), glm::vec2(0.125, 0.75f), glm::vec3(0.0f, -1.0f, 0.0f))),
	Triangle(Vertex(glm::vec3(5.0f, -0.3f, -5.0f), glm::vec2(0.625f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f)), Vertex(glm::vec3(5.0f, -0.3f, 5.0f), glm::vec2(0.625, 0.75), glm::vec3(1.0f, 0.0f, 0.0f)), Vertex(glm::vec3(5.0f, -0.7f, 5.0f), glm::vec2(0.375, 0.75f), glm::vec3(1.0f, 0.0f, 0.0f))),
	Triangle(Vertex(glm::vec3(-5.0f, -0.3f, -5.0f), glm::vec2(0.625f, 0.25f), glm::vec3(0.0f, 0.0f, -1.0f)), Vertex(glm::vec3(5.0f, -0.3f, -5.0f), glm::vec2(0.625, 0.5), glm::vec3(0.0f, 0.0f, -1.0f)), Vertex(glm::vec3(5.0f, -0.7f, -5.0f), glm::vec2(0.375, 0.5f), glm::vec3(0.0f, 0.0f, -1.0f))) };*/
	mSimulation = new Simulation(mCollider);

	//Scalp model
	Model* scalp = new Model("../models/ScalpLow.obj");
	mHair = new Hair(scalp->GetFirstMesh(), mHairDensity, mSimulation, mHair);
}

void Application::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float time = mFrame++ / 60.0f;
	
	//Update hair
	mSimulation->Update(time);
	mHair->Update(time);
	
	glm::vec3 lightPosition = glm::vec3(2.0f, 1.0f, 2.0f);
	glm::mat4 lightProjection = glm::perspective(1.5f, 1.0f, 1.0f, 100.0f);
	glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 eyeToLight = lightProjection * lightView * glm::inverse(mCamera->GetViewMatrix());

	//Shadow map
	glViewport(0, 0, mShadowDepthTexture->GetWidth(), mShadowDepthTexture->GetHeight());
	mShadowFramebuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mMeshProgram->Bind();
	mMeshProgram->uniforms.projection = lightProjection;
	mMeshProgram->uniforms.view = lightView;
	mMeshProgram->uniforms.model = glm::mat4(1.0f);
	mMeshProgram->SetGlobalUniforms();
	mMeshProgram->SetObjectUniforms();
	mMesh->Draw();
	mMeshProgram->Unbind();

	mHairProgram->Bind();
	mNoiseTexture->Bind(GL_TEXTURE0);
	mShadowDepthTexture->Bind(GL_TEXTURE1);
	mHairProgram->uniforms.projection = lightProjection;
	mHairProgram->uniforms.view = lightView;
	mHairProgram->uniforms.model = glm::mat4(1.0f);
	mHairProgram->uniforms.dirToLight = eyeToLight;
	mHairProgram->uniforms.lightPosition = lightPosition;
	mHairProgram->uniforms.noiseTexture = 0;
	mHairProgram->uniforms.shadowMap = 1;
	mHairProgram->SetGlobalUniforms();
	mHair->Draw(mHairProgram);
	mShadowDepthTexture->Unbind(GL_TEXTURE1);
	mNoiseTexture->Unbind(GL_TEXTURE0);
	mHairProgram->Unbind();

	mShadowFramebuffer->Unbind();

	//Render hair
	glViewport(0, 0, mWidth, mHeight);
	mHairProgram->Bind();
	mNoiseTexture->Bind(GL_TEXTURE0);
	mShadowDepthTexture->Bind(GL_TEXTURE1);
	mHairProgram->uniforms.projection = glm::perspective(0.8f, (float)mWidth / mHeight, 0.1f, 100.0f);
	mHairProgram->uniforms.view = mCamera->GetViewMatrix();
	mHairProgram->uniforms.model = glm::mat4(1.0f);
	mHairProgram->uniforms.dirToLight = eyeToLight;
	mHairProgram->uniforms.lightPosition = lightPosition;
	mHairProgram->uniforms.noiseTexture = 0;
	mHairProgram->uniforms.shadowMap = 1;
	mHairProgram->SetGlobalUniforms();
	mHair->Draw(mHairProgram);

	mShadowDepthTexture->Unbind(GL_TEXTURE1);
	mNoiseTexture->Unbind(GL_TEXTURE0);
	mHairProgram->Unbind();
	
	//Render mesh
	mMeshProgram->Bind();

	mMeshProgram->uniforms.projection = mHairProgram->uniforms.projection;
	mMeshProgram->uniforms.view = mHairProgram->uniforms.view;
	mMeshProgram->uniforms.model = mHairProgram->uniforms.model;
	mMeshProgram->uniforms.lightPosition = mHairProgram->uniforms.lightPosition;
	mMeshProgram->SetGlobalUniforms();
	mMeshProgram->SetObjectUniforms();

	mMesh->Draw();
	//mCollider->Draw();
	mMeshProgram->Unbind();
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
		mCamera->Rotate(0.0f, mDeltaTime * 1);
	}

	if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mCamera->Rotate(0.0f, -mDeltaTime * 1);
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
