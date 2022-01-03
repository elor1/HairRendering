#pragma once
#include <glew.h>
#include "Quad.h"
#include "ShaderProgram.h"

class Texture
{
public:
	Texture();
	virtual ~Texture();

	void Create(const char* filename, GLint magFilter, GLint minFilter);
	void Create(int width, int height, GLint magFilter, GLint minFilter);
	void CreateDepthTexture(int width, int height);
	void Bind(GLenum textureUnit);
	void Unbind(GLenum textureUnit);
	void RenderFullScreen();
	GLuint GetID();
	GLuint GetWidth();
	GLuint GetHeight();

private:
	void Create(unsigned char* image, GLint format, int width, int height, GLenum format1, GLenum type, GLint magFilter, GLint minFilter);
	Quad* mQuad;
	ShaderProgram* mProgram;
	GLuint mID;
	int mWidth;
	int mHeight;
};