#include "ShaderProgram.h"
#include <fstream>
#include <iostream>
#include <glfw3.h>
#include <string>
#include <gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram(const char* vertex, const char* fragment, const char* geometry, const char* tessControl, const char* tessEval)
{
	//Load(vertex, fragment, geometry, tessControl, tessEval);

	mVertex = vertex;
	mFragment = fragment;
	mGeometry = geometry;
	mTessControl = tessControl;
	mTessEval = tessEval;

	mID = Load();

	//Setup uniforms
	std::vector<GLchar const*> uniformNames;
	uniformNames.push_back("model");
	uniformNames.push_back("view");
	uniformNames.push_back("projection");
	uniformNames.push_back("numHairPatch");
	uniformNames.push_back("numHairSegments");
	uniformNames.push_back("vertexData");
	uniformNames.push_back("colour");

	for (auto name : uniformNames)
	{
		mUniformLocations[name] = glGetUniformLocation(mID, name);
	}
}

GLuint ShaderProgram::GetID()
{
	return mID;
}

void ShaderProgram::SetUniforms()
{
	glUniformMatrix4fv(mUniformLocations["model"], 1, GL_FALSE, glm::value_ptr(uniforms.model));
	glUniformMatrix4fv(mUniformLocations["view"], 1, GL_FALSE, glm::value_ptr(uniforms.view));
	glUniformMatrix4fv(mUniformLocations["projection"], 1, GL_FALSE, glm::value_ptr(uniforms.projection));
	glUniform1i(mUniformLocations["numHairPatch"], uniforms.numGroupHairs);
	glUniform1i(mUniformLocations["numHairSegments"], uniforms.numHairVertices - 1);
	glUniform3fv(mUniformLocations["vertexData"], uniforms.numHairVertices, &uniforms.vertexData[0]);
	glUniform3fv(mUniformLocations["colour"], 1, glm::value_ptr(uniforms.colour));
}

void ShaderProgram::Bind()
{
	glUseProgram(mID);
}

void ShaderProgram::Unbind()
{
	glUseProgram(mID);
}

GLuint ShaderProgram::Load()
{
	std::vector<GLuint> shaders;
	shaders.push_back(CreateShader(GL_VERTEX_SHADER, mVertex));
	shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, mFragment));
	if (mGeometry != "")
	{
		shaders.push_back(CreateShader(GL_GEOMETRY_SHADER, mGeometry));
	}
	if (mTessControl != "" && mTessEval != "")
	{
		shaders.push_back(CreateShader(GL_TESS_CONTROL_SHADER, mTessControl));
		shaders.push_back(CreateShader(GL_TESS_EVALUATION_SHADER, mTessEval));
	}

	GLuint programID = CreateProgram(shaders);

	for (auto shader : shaders)
	{
		glDeleteShader(shader);
	}

	return programID;
}

GLuint ShaderProgram::CreateShader(GLenum type, const char* path)
{
	GLuint shaderID = glCreateShader(type);

	//Read from file
	std::ifstream file(path);
	std::string out;

	if (file.is_open())
	{
		std::string str;
		while (std::getline(file, str))
		{
			out += str + "\n";
		}
	}
	else
	{
		std::cout << "ERROR: Could not open file " << path << std::endl;
	}

	file.close();

	//Compile shader
	const char* code = out.c_str();
	glShaderSource(shaderID, 1, &code, NULL);
	glCompileShader(shaderID);

	GLint success;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		std::string log(length, ' ');
		glGetShaderInfoLog(shaderID, length, &length, &log[0]);
		std::cout << "ERROR Failed to compile " << path << std::endl << log << std::endl;
		glDeleteShader(shaderID);
		glfwTerminate();
	}

	return shaderID;
}

GLuint ShaderProgram::CreateProgram(std::vector<GLuint>& shaders)
{
	GLuint programID = glCreateProgram();

	for (auto shader : shaders)
	{
		glAttachShader(programID, shader);
	}

	glLinkProgram(programID);

	GLint result = GL_FALSE;
	int logLength;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		std::vector<char> infoLog(logLength);
		glGetProgramInfoLog(programID, logLength, NULL, &infoLog[0]);
		std::cout << "ERROR: Failed to link program" << std::endl << &infoLog << std::endl;
	}

	return programID;
}
