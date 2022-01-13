#pragma once
#include <glew.h>
#include <glfw3.h>
#include "ShaderProgram.h"
#include "Camera.h"

class Hair;
class Simulation;
class Mesh;
class Texture;
class Framebuffer;

class Application
{
public:
	Application(int width, int height);
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
	void ProcessInput();
	static void FrameBufferCallback(GLFWwindow* window, int width, int height);
	void MouseCallback(GLFWwindow* window, double xPos, double yPos);
	
	Mesh* mMesh;
	Mesh* mCollider;

	Hair* mHair;
	Simulation* mSimulation;

	ShaderProgram* mMeshProgram;
	ShaderProgram* mHairProgram;
	ShaderProgram* mOpacityMapProgram;

	float mHairDensity;

	Texture* mNoiseTexture;
	Texture* mShadowDepthTexture;
	Texture* mOpacityMapTexture;

	Framebuffer* mShadowFramebuffer;
	Framebuffer* mOpacityMapFramebuffer;

	Camera* mCamera;
	bool mFirstMouse;
	double mLastX;
	double mLastY;

	GLFWwindow* mWindow;
	int mWidth;
	int mHeight;

	double mPrevTime;
	double mCurrentTime;
	double mDeltaTime;
	int mFrame;

	bool mIsPaused;
	bool mSpaceDown;
};