#pragma once
#include "GuiWindow.h"
#include <glew.h>
#include <glfw3.h>
#include "shaderPrograms/ShaderProgram.h"
#include "Camera.h"
#include "Hair.h"
#include "Model.h"

class Simulation;
class Texture;
class Framebuffer;

class Application
{
public:
	Application(int width, int height);
	~Application();

	void Run();

	//Gui
	double GetDeltaTime();
	Hair* GetHair();
	void TogglePause();
	void ResetSimulation();
	bool IsPaused();
	bool useShadows;
	bool useSuperSampling;

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
	
	Model* mHead;
	Model* mCollider;

	Hair* mHair;
	Simulation* mSimulation;

	std::vector<ShaderProgram*> mPrograms;
	ShaderProgram* mMeshProgram;
	ShaderProgram* mHairProgram;
	ShaderProgram* mHairOpacityProgram;
	ShaderProgram* mWhiteHairProgram;
	ShaderProgram* mWhiteMeshProgram;

	float mHairDensity;

	std::vector<Texture*> mTextures;
	Texture* mNoiseTexture;
	Texture* mHairDepthTexture;
	Texture* mMeshDepthTexture;
	Texture* mOpacityMapTexture;
	Texture* mFinalTexture;

	std::vector<Framebuffer*> mFramebuffers;
	Framebuffer* mHairShadowFramebuffer;
	Framebuffer* mMeshShadowFramebuffer;
	Framebuffer* mOpacityMapFramebuffer;
	Framebuffer* mFinalFramebuffer;

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
	bool mIsSpaceDown;

	GuiWindow* mGui;
};