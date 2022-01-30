#include "FeedbackShaderProgram.h"
#include <iostream>

FeedbackShaderProgram::FeedbackShaderProgram(const char** varyings, int numVaryings, const char* vertex, const char* fragment, const char* geometry, const char* tessControl, const char* tessEval)
{
	mVertex = vertex;
	mFragment = fragment;
	mGeometry = geometry;
	mTessControl = tessControl;
	mTessEval = tessEval;

	mID = Load(varyings, numVaryings);
}

GLuint FeedbackShaderProgram::Load(const char** varyings, int numVaryings)
{
	std::vector<GLuint> shaders;
	shaders.push_back(CreateShader(GL_VERTEX_SHADER, mVertex));
	if (mFragment != "")
	{
		shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, mFragment));
	}
	if (mGeometry != "")
	{
		shaders.push_back(CreateShader(GL_GEOMETRY_SHADER, mGeometry));
	}
	if (mTessControl != "" && mTessEval != "")
	{
		shaders.push_back(CreateShader(GL_TESS_CONTROL_SHADER, mTessControl));
		shaders.push_back(CreateShader(GL_TESS_EVALUATION_SHADER, mTessEval));
	}

	GLuint programID = CreateProgram(shaders, varyings, numVaryings);

	for (auto& shader : shaders)
	{
		glDeleteShader(shader);
	}

	return programID;
}

GLuint FeedbackShaderProgram::CreateProgram(std::vector<GLuint>& shaders, const GLchar** varyings, int numVaryings)
{
	GLuint programID = glCreateProgram();

	for (auto& shader : shaders)
	{
		glAttachShader(programID, shader);
	}

	glTransformFeedbackVaryings(programID, numVaryings, varyings, GL_INTERLEAVED_ATTRIBS);
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
