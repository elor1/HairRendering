#pragma once
#include <glew.h>
#include <vector>
#include <map>
#include <string>
#include <glm.hpp>

enum class Type
{
	Hair,
	Mesh,
	WhiteHair,
	WhiteMesh,
	HairOpacity,
	Quad,
	None
};

struct Uniforms
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	int numGroupHairs;
	int numHairVertices;
	int numSplineVertices;
	glm::vec3 vertexData[64];
	float groupSpread;
	float hairRadius;
	float tapering;
	float noiseAmplitude;
	glm::vec3 triangleFace[2];
	glm::vec3 colour;
	glm::vec3 lightPosition;
	glm::mat4 dirToLight;
	int noiseTexture;
	int hairShadowMap;
	int meshShadowMap;
	int opacityMap;
	bool useShadows;
	float shadowIntensity;
	float occlusionLayerSize = 0.0005f;
};

class ShaderProgram
{
public:
	ShaderProgram(const char* vertex, const char* fragment, const char* geometry = "", const char* tessControl = "", const char* tessEval = "");
	ShaderProgram(Type type);

	GLuint GetID();
	void SetGlobalUniforms();
	void SetObjectUniforms();
	void SetDrawUniforms();
	void Bind();
	void Unbind();

	Uniforms uniforms;

private:
	GLuint CreateShader(GLenum type, const char* path);
	GLuint CreateProgram(std::vector<GLuint>& shaders);
	GLuint Load();
	GLint GetUniformLocation(const GLchar* name);

	const char* mVertex;
	const char* mFragment;
	const char* mGeometry;
	const char* mTessControl;
	const char* mTessEval;
	GLuint mID;
	std::map<std::string, int> mUniformLocations;

	Type mType;
};