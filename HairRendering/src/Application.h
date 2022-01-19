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

	std::vector<ShaderProgram*> mPrograms;
	ShaderProgram* mMeshProgram;
	ShaderProgram* mHairProgram;
	ShaderProgram* mHairOpacityProgram;

	float mHairDensity;

	std::vector<Texture*> mTextures;
	Texture* mNoiseTexture;
	Texture* mHairDepthTexture;
	Texture* mMeshDepthTexture;
	Texture* mOpacityMapTexture;

	std::vector<Framebuffer*> mFramebuffers;
	Framebuffer* mHairShadowFramebuffer;
	Framebuffer* mMeshShadowFramebuffer;
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