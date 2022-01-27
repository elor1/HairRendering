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

#define SHADOWS true

Application::Application(int width, int height)
{
	mPrevTime = glfwGetTime();
	mCurrentTime = glfwGetTime();
	mWidth = width;
	mHeight = height;
	mHairDensity = 50;
	mFirstMouse = true;
	mLastX = width / 2.0;
	mLastY = width / 2.f;
	mFrame = 0;
	mIsPaused = false;
	mIsSpaceDown = false;

	Initialise();
}

Application::~Application()
{
	for (auto& program : mPrograms)
	{
		delete program;
	}

	for (auto& texture : mTextures)
	{
		delete texture;
	}

	for (auto& fb : mFramebuffers)
	{
		delete fb;
	}

	delete mMesh;
	delete mCollider;
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
	mPrograms = {
		mMeshProgram = new ShaderProgram(Type::Mesh),
		mHairProgram = new ShaderProgram(Type::Hair),
		mHairOpacityProgram = new ShaderProgram(Type::HairOpacity),
		mWhiteHairProgram = new ShaderProgram(Type::WhiteHair),
		mWhiteMeshProgram = new ShaderProgram(Type::WhiteMesh),
	};

	//Textures
	mTextures = {
		mNoiseTexture = new Texture(),
		mHairDepthTexture = new Texture(),
		mMeshDepthTexture = new Texture(),
		mOpacityMapTexture = new Texture(),
		mFinalTexture = new Texture(),
	};

	int shadowMapSize = 2048;
	mNoiseTexture->Create("../images/Noise.jpg", GL_LINEAR, GL_LINEAR);
	mHairDepthTexture->CreateDepthTexture(shadowMapSize, shadowMapSize, GL_NEAREST, GL_NEAREST);
	mMeshDepthTexture->CreateDepthTexture(shadowMapSize, shadowMapSize, GL_LINEAR, GL_LINEAR);
	mOpacityMapTexture->Create(shadowMapSize, shadowMapSize, GL_NEAREST, GL_NEAREST);
	mFinalTexture->Create(2 * mWidth, 2 * mHeight, GL_LINEAR, GL_LINEAR);

	//Framebuffers
	mFramebuffers = {
		mHairShadowFramebuffer = new Framebuffer(),
		mMeshShadowFramebuffer = new Framebuffer(),
		mOpacityMapFramebuffer = new Framebuffer(),
		mFinalFramebuffer = new Framebuffer(),
	};

	for (auto& fb : mFramebuffers)
	{
		fb->Create();
	}

	mHairShadowFramebuffer->AttachDepthTexture(mHairDepthTexture->GetID());
	mMeshShadowFramebuffer->AttachDepthTexture(mMeshDepthTexture->GetID());

	mOpacityMapFramebuffer->AttachTexture(mOpacityMapTexture->GetID());
	mOpacityMapFramebuffer->GenerateDepthBuffer(shadowMapSize, shadowMapSize);
	mFinalFramebuffer->AttachTexture(mFinalTexture->GetID());
	mFinalFramebuffer->GenerateDepthBuffer(mFinalTexture->GetWidth(), mFinalTexture->GetHeight());

	InitSimulation();
}

void Application::InitSimulation()
{
	delete mMesh;
	delete mCollider;
	delete mSimulation;

	//Head model
	Model* model = new Model("../models/Head3.obj");
	mMesh = model->GetFirstMesh();

	//Collision model
	Model* collisionModel = new Model("../models/Collider.obj", 1.1f);
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

	delete scalp;
}

#define SUPERSAMPLING false
void Application::Draw()
{
	float time = mFrame++ / 60.0f;
	
	//Update hair
	if (!mIsPaused)
	{
		mSimulation->Update(time);
		mHair->Update(time);
	}
	
	glm::mat4 model = mSimulation->GetTransform();
	glm::vec3 lightPosition = glm::vec3(1.0f, 2.0f, 4.0f);
	glm::mat4 lightProjection = glm::perspective(1.3f, 1.0f, 1.0f, 100.0f);
	glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 eyeToLight = lightProjection * lightView * glm::inverse(mCamera->GetViewMatrix());

	//Bind textures
	mNoiseTexture->Bind(GL_TEXTURE0);
	mHairDepthTexture->Bind(GL_TEXTURE1);
	mOpacityMapTexture->Bind(GL_TEXTURE2);
	mMeshDepthTexture->Bind(GL_TEXTURE3);
	mFinalTexture->Bind(GL_TEXTURE4);

	//Shadow map
	if (SHADOWS)
	{
		//Hair shadows
		glViewport(0, 0, mHairDepthTexture->GetWidth(), mHairDepthTexture->GetHeight());
		mHairShadowFramebuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mWhiteHairProgram->Bind();
		mWhiteHairProgram->uniforms.noiseTexture = 0;
		mWhiteHairProgram->uniforms.hairShadowMap = 1;
		mWhiteHairProgram->uniforms.projection = lightProjection;
		mWhiteHairProgram->uniforms.view = lightView;
		mWhiteHairProgram->uniforms.model = model;
		mWhiteHairProgram->uniforms.dirToLight = eyeToLight;
		mWhiteHairProgram->uniforms.lightPosition = lightPosition;
		mWhiteHairProgram->SetGlobalUniforms();
		mHair->Draw(mWhiteHairProgram);
		mWhiteHairProgram->Unbind();
		mHairShadowFramebuffer->Unbind();

		//Mesh shadows
		mMeshShadowFramebuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mWhiteMeshProgram->Bind();
		mWhiteMeshProgram->uniforms.projection = lightProjection;
		mWhiteMeshProgram->uniforms.view = lightView;
		mWhiteMeshProgram->uniforms.model = model;
		mWhiteMeshProgram->SetGlobalUniforms();
		mWhiteMeshProgram->SetObjectUniforms();
		mMesh->Draw();
		mWhiteMeshProgram->Unbind();
		mMeshShadowFramebuffer->Unbind();

		//Opacity map
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
		glViewport(0, 0, mHairDepthTexture->GetWidth(), mHairDepthTexture->GetHeight());

		mOpacityMapFramebuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mHairOpacityProgram->Bind();
		mHairOpacityProgram->uniforms.noiseTexture = 0;
		mHairOpacityProgram->uniforms.hairShadowMap = 1;
		mHairOpacityProgram->uniforms.projection = lightProjection;
		mHairOpacityProgram->uniforms.view = lightView;
		mHairOpacityProgram->uniforms.model = model;
		mHairOpacityProgram->uniforms.dirToLight = eyeToLight;
		mHairOpacityProgram->uniforms.lightPosition = lightPosition;
		mHairOpacityProgram->SetGlobalUniforms();
		mHair->Draw(mHairOpacityProgram);
		mHairOpacityProgram->Unbind();
		mOpacityMapFramebuffer->Unbind();
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}

	if (SUPERSAMPLING)
	{
		mFinalFramebuffer->Bind();
		glViewport(0, 0, mFinalTexture->GetWidth(), mFinalTexture->GetHeight());
	}
	else
	{
		glViewport(0, 0, mWidth, mHeight);
	}

	//Render hair
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mHairProgram->Bind();
	mHairProgram->uniforms.noiseTexture = 0;
	mHairProgram->uniforms.hairShadowMap = 1;
	mHairProgram->uniforms.opacityMap = 2;
	mHairProgram->uniforms.meshShadowMap = 3;
	mHairProgram->uniforms.projection = glm::perspective(0.8f, (float)mWidth / mHeight, 0.1f, 100.0f);
	mHairProgram->uniforms.view = mCamera->GetViewMatrix();
	mHairProgram->uniforms.model = model;
	mHairProgram->uniforms.dirToLight = eyeToLight;
	mHairProgram->uniforms.lightPosition = lightPosition;
	mHairProgram->uniforms.shadowIntensity = 15.0f;
	mHairProgram->uniforms.useShadows = SHADOWS;
	mHairProgram->SetGlobalUniforms();
	mHair->Draw(mHairProgram);
	mHairProgram->Unbind();
	
	//Render mesh
	mMeshProgram->Bind();
	mMeshProgram->uniforms.hairShadowMap = 1;
	mMeshProgram->uniforms.opacityMap = 2;
	mMeshProgram->uniforms.meshShadowMap = 3;
	mMeshProgram->uniforms.projection = mHairProgram->uniforms.projection;
	mMeshProgram->uniforms.view = mCamera->GetViewMatrix();
	mMeshProgram->uniforms.model = model;
	mMeshProgram->uniforms.lightPosition = lightPosition;
	mMeshProgram->uniforms.dirToLight = eyeToLight;
	mMeshProgram->uniforms.shadowIntensity = 15.0f;
	mMeshProgram->uniforms.useShadows = SHADOWS;
	mMeshProgram->SetGlobalUniforms();
	mMeshProgram->SetObjectUniforms();
	mMesh->Draw();
	//mCollider->Draw();
	mMeshProgram->Unbind();

	if (SUPERSAMPLING)
	{
		mFinalFramebuffer->Unbind();
		glViewport(0, 0, mWidth, mHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mFinalTexture->RenderFullScreen();
	}

	mNoiseTexture->Unbind(GL_TEXTURE0);
	mHairDepthTexture->Unbind(GL_TEXTURE1);
	mOpacityMapTexture->Unbind(GL_TEXTURE2);
	mMeshDepthTexture->Unbind(GL_TEXTURE3);
	mFinalTexture->Unbind(GL_TEXTURE4);
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

	if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (!mIsSpaceDown)
		{
			mIsPaused = !mIsPaused;
		}

		mIsSpaceDown = true;
	}
	else
	{
		mIsSpaceDown = false;
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

	if (glfwGetKey(mWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		mCamera->Move(EMovementDirection::Up, mDeltaTime);
	}

	if (glfwGetKey(mWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		mCamera->Move(EMovementDirection::Down, mDeltaTime);
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
