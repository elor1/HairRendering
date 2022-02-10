#include "Application.h"
#include <iostream>
#include "shaderPrograms/HairShaderProgram.h"
#include "shaderPrograms/MeshShaderProgram.h"
#include "shaderPrograms//HairOpacityShaderProgram.h"
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Simulation.h"
#include "Model.h"
#include "Texture.h"
#include "Framebuffer.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

Application::Application(int width, int height)
{
	mPrevTime = glfwGetTime();
	mCurrentTime = glfwGetTime();
	mWidth = width;
	mHeight = height;
	mHairDensity = 100;
	mFirstMouse = true;
	mLastX = width / 2.0;
	mLastY = width / 2.f;
	mFrame = 0;
	mIsPaused = false;
	mIsSpaceDown = false;
	useShadows = true;
	useSuperSampling = true;

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

	delete mGui;
	delete mHead;
	delete mCollider;
	delete mSimulation;
	delete mHair;
	delete mCamera;
}

void Application::Run()
{
	//Loop until window closes
	while (!glfwWindowShouldClose(mWindow))
	{
		Update();
	}
}

double Application::GetDeltaTime()
{
	return mDeltaTime;
}

Hair* Application::GetHair()
{
	return mHair;
}

void Application::TogglePause()
{
	mIsPaused = !mIsPaused;
}

void Application::ResetSimulation()
{
	InitSimulation();
}

bool Application::IsPaused()
{
	return mIsPaused;
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

	auto MouseCallback = [](GLFWwindow* window, int button, int action, int mods)
	{
		Application* app = (Application*)glfwGetWindowUserPointer(window);
		app->MouseButtonCallback(window, button, action, mods);
	};

	glfwSetMouseButtonCallback(mWindow, MouseCallback);

	auto ScrollCallback = [](GLFWwindow* window, double xOffset, double yOffset)
	{
		Application* app = (Application*)glfwGetWindowUserPointer(window);
		app->MouseScrollCallback(window, xOffset, yOffset);
	};

	glfwSetScrollCallback(mWindow, ScrollCallback);

	//Initialise GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR: Failed to initialise GLEW" << std::endl;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Initialise imgui
	mGui = DBG_NEW GuiWindow(mWindow);
	mGui->SetApplication(this);

	//Camera
	mCamera = DBG_NEW Camera(5.0f, glm::perspective(0.8f, (float)mWidth / mHeight, 0.1f, 100.0f));

	//Shaders
	mPrograms = {
		mMeshProgram = DBG_NEW MeshShaderProgram(),
		mHairProgram = DBG_NEW HairShaderProgram(),
		mHairOpacityProgram = DBG_NEW HairOpacityShaderProgram(),
		mWhiteHairProgram = DBG_NEW HairShaderProgram("src/shaders/hair.vert", "src/shaders/white.frag"),
		mWhiteMeshProgram = DBG_NEW MeshShaderProgram("src/shaders/mesh.vert", "src/shaders/white.frag"),
	};

	//Textures
	mTextures = {
		mNoiseTexture = DBG_NEW Texture(),
		mHairDepthTexture = DBG_NEW Texture(),
		mMeshDepthTexture = DBG_NEW Texture(),
		mOpacityMapTexture = DBG_NEW Texture(),
		mFinalTexture = DBG_NEW Texture(),
	};

	int shadowMapSize = 2048;
	mNoiseTexture->Create("../images/Noise.jpg", GL_LINEAR, GL_LINEAR);
	mHairDepthTexture->CreateDepthTexture(shadowMapSize, shadowMapSize, GL_NEAREST, GL_NEAREST);
	mMeshDepthTexture->CreateDepthTexture(shadowMapSize, shadowMapSize, GL_LINEAR, GL_LINEAR);
	mOpacityMapTexture->Create(shadowMapSize, shadowMapSize, GL_NEAREST, GL_NEAREST);
	mFinalTexture->Create(2 * mWidth, 2 * mHeight, GL_LINEAR, GL_LINEAR);

	//Framebuffers
	mFramebuffers = {
		mHairShadowFramebuffer = DBG_NEW Framebuffer(),
		mMeshShadowFramebuffer = DBG_NEW Framebuffer(),
		mOpacityMapFramebuffer = DBG_NEW Framebuffer(),
		mFinalFramebuffer = DBG_NEW Framebuffer(),
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
	if (mHead)
	{
		mHead = nullptr;
	}
	
	if (mCollider)
	{
		mCollider = nullptr;
	}
	
	if (mSimulation)
	{
		mSimulation = nullptr;
	}

	//Head model
	mHead = DBG_NEW Model("../models/Head3.obj");
	//mHead = model->GetFirstMesh();

	//Collision model
	mCollider = DBG_NEW Model("../models/Collider.obj", 1.1f);
	//mCollider = collisionModel->GetFirstMesh();
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
	mSimulation = DBG_NEW Simulation(mCollider->GetFirstMesh());

	//Scalp model
	Model* scalp = DBG_NEW Model("../models/ScalpLow.obj");
	//mHair = new Hair(scalp->GetFirstMesh(), mHairDensity, mSimulation, mHair);
	mHair = DBG_NEW Hair(mHead->GetFirstMesh(), mHairDensity, "../images/hairmap.png", mSimulation);
	delete scalp;
}


void Application::Draw()
{
	//Update hair
	if (!mIsPaused)
	{
		float time = mFrame++ / 60.0f;
		mSimulation->Update(time);
		mHair->Update(time);
	}
	
	mGui->NewFrame();

	glm::mat4 model = mSimulation->GetTransform();
	mLightPosition = glm::vec3(2.0f, 2.0f, 2.0f);
	glm::mat4 lightProjection = glm::perspective(1.3f, 1.0f, 1.0f, 100.0f);
	glm::mat4 lightView = glm::lookAt(mLightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mDirToLight = lightProjection * lightView * glm::inverse(mCamera->GetView());

	//Bind textures
	mNoiseTexture->Bind(GL_TEXTURE0);
	mHairDepthTexture->Bind(GL_TEXTURE1);
	mOpacityMapTexture->Bind(GL_TEXTURE2);
	mMeshDepthTexture->Bind(GL_TEXTURE3);
	mFinalTexture->Bind(GL_TEXTURE4);
	mHair->GetHairMap()->Bind(GL_TEXTURE5);

	//Shadow map
	if (useShadows)
	{
		//Hair shadow map
		mHairShadowFramebuffer->Bind();
		glViewport(0, 0, mHairDepthTexture->GetWidth(), mHairDepthTexture->GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawHair(mWhiteHairProgram, model, lightView, lightProjection);

		//Mesh shadow map
		mMeshShadowFramebuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawMesh(mWhiteMeshProgram, model, lightView, lightProjection);

		//Opacity map - Enable additive blending
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		mOpacityMapFramebuffer->Bind();
		glViewport(0, 0, mHairDepthTexture->GetWidth(), mHairDepthTexture->GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawHair(mHairOpacityProgram, model, lightView, lightProjection);

		mOpacityMapFramebuffer->Unbind();
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}

	if (useSuperSampling)
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
	DrawHair(mHairProgram, model, mCamera->GetView(), mCamera->GetProjection());
	
	//Render mesh
	DrawMesh(mMeshProgram, model, mCamera->GetView(), mCamera->GetProjection());

	if (useSuperSampling)
	{
		//Render texture
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
	/*if (!mIsPaused)
	{*/
		mCurrentTime = glfwGetTime();
		mDeltaTime = mCurrentTime - mPrevTime;
	//}
	

	if (!ImGui::GetIO().WantCaptureMouse && !ImGui::GetIO().WantCaptureKeyboard)
	{
		ProcessInput();
	}

	//Draw 60 times per second
	if (mDeltaTime >= 1.0f / 60.0f)
	{
		//std::cout << 1 / mDeltaTime << " FPS" << std::endl;
		Draw();

		mGui->Update();

		//Swap front and back buffers
		glfwSwapBuffers(mWindow);

		mPrevTime = mCurrentTime;
	}
	
	//Poll events
	glfwPollEvents();
}

void Application::SetPatchHair(int numHairs)
{
	mHair->SetNumGroupHairs(numHairs);
}

void Application::SetNumSplineVertices(int numVertices)
{
	mHair->SetNumSplineVertices(numVertices);
}

void Application::SetHairColour(glm::vec3 colour)
{
	mHair->SetColour(colour / 2550.0f);
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
			TogglePause();
		}

		mIsSpaceDown = true;
	}
	else
	{
		mIsSpaceDown = false;
	}
}

void Application::FrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
}

void Application::MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glm::mat4 view = mCamera->GetView();
		glm::vec3 up = glm::normalize(glm::vec3(view[2][1], view[2][2], view[2][3]));
		glm::vec3 forward = glm::normalize(glm::vec3(glm::inverse(view) * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)));
		glm::vec3 right = glm::cross(up, forward);
		glm::vec2 delta = glm::vec2(xPos, yPos) - mCamera->GetPreviousPosition();
		glm::vec3 transform = glm::vec3();
		transform += delta.x * 0.005f * right;
		transform += -delta.y * 0.005f * up;

		mSimulation->UpdatePosition(mHair, transform);
		mCamera->SetPreviousPosition(glm::vec2(xPos, yPos));
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		float x = 10 * (xPos - mCamera->GetPrevMousePosition().x) / (float)mWidth;
		float y = 10 * (yPos - mCamera->GetPrevMousePosition().y) / (float)mHeight;
		
		mCamera->SetAngles(x, y);
		mCamera->UpdateViewMatrix();
		mCamera->SetPrevMousePosition(glm::vec2(xPos, yPos));
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		glm::mat4 view = mCamera->GetView();
		glm::vec2 delta = glm::vec2(xPos, yPos) - mCamera->GetPreviousRotation();
		if (fabs(delta.x) > fabs(delta.y))
		{
			//Rotate up
			glm::vec3 up = glm::normalize(glm::vec3(view[2][1], view[2][2], view[2][3]));
			float angle = delta.x * 0.001f;
			mSimulation->UpdateRotation(mHair, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else
		{
			//Rotate right
			float angle = delta.y * 0.001f;
			mSimulation->UpdateRotation(mHair, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		}
	}
}

void Application::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	if (action == GLFW_RELEASE)
	{
		mSimulation->SetHeadMoving(false);
	}

	if (action == GLFW_PRESS)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			mCamera->SetPreviousPosition(glm::vec2(x, y));
			mSimulation->SetHeadMoving(true);
		}

		if (button = GLFW_MOUSE_BUTTON_RIGHT)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			mCamera->SetPrevMousePosition(glm::vec2(x, y));
		}

		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			mCamera->SetPreviousRotation(glm::vec2(x, y));
			mSimulation->SetHeadMoving(true);
		}
	}
}

void Application::MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	mCamera->SetZoom(yOffset / 100.0f);
	mCamera->UpdateViewMatrix();
}

void Application::DrawMesh(ShaderProgram* program, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	program->Bind();
	program->uniforms.hairShadowMap = 1;
	program->uniforms.opacityMap = 2;
	program->uniforms.meshShadowMap = 3;
	program->uniforms.hairMap = 5;
	program->uniforms.projection = projection;
	program->uniforms.view = view;
	program->uniforms.model = model;
	program->uniforms.lightPosition = mLightPosition;
	program->uniforms.dirToLight = mDirToLight;
	program->uniforms.shadowIntensity = mHair->GetShadowIntensity();
	program->uniforms.useShadows = useShadows;
	program->uniforms.colour = 2.0f * mHair->GetColour();
	program->SetGlobalUniforms();
	program->SetObjectUniforms();
	mHead->Draw();
}

void Application::DrawHair(ShaderProgram* program, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	program->Bind();
	program->uniforms.noiseTexture = 0;
	program->uniforms.hairShadowMap = 1;
	program->uniforms.opacityMap = 2;
	program->uniforms.meshShadowMap = 3;
	program->uniforms.projection = projection;
	program->uniforms.view = view;
	program->uniforms.model = model;
	program->uniforms.dirToLight = mDirToLight;
	program->uniforms.lightPosition = mLightPosition;
	program->uniforms.shadowIntensity = mHair->GetShadowIntensity();
	program->uniforms.useShadows = useShadows;
	program->uniforms.specularIntensity = 0.5f;
	program->uniforms.diffuseIntensity = 1.0f;
	program->SetGlobalUniforms();
	mHair->Draw(program);
}
