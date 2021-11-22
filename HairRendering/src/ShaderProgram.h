#pragma once
#include <glew.h>
#include <vector>

class ShaderProgram
{
public:
	ShaderProgram(const char* vertex, const char* fragment, const char* geometry = "", const char* tessControl = "", const char* tessEval = "");

	GLuint Load();
private:
	GLuint CreateShader(GLenum type, const char* path);
	GLuint CreateProgram(std::vector<GLuint>& shaders);

	const char* mVertex;
	const char* mFragment;
	const char* mGeometry;
	const char* mTessControl;
	const char* mTessEval;
};