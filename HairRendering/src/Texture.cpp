#include "Texture.h"
#include <SOIL2.h>
#include <iostream>
#include "Quad.h"
#include "shaderPrograms/ShaderProgram.h"

Texture::Texture()
{
	mQuad = nullptr;
	mProgram = nullptr;
}

Texture::~Texture()
{
	delete mQuad;
	delete mProgram;
}

void Texture::Create(const char* filename, GLint magFilter, GLint minFilter)
{
	int width;
	int height;
	unsigned char* image = SOIL_load_image(filename, &width, &height, NULL, SOIL_LOAD_RGBA);

	Create(image, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, magFilter, minFilter);
}

void Texture::Create(int width, int height, GLint magFilter, GLint minFilter)
{
	Create(0, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, magFilter, minFilter);
}

void Texture::CreateDepthTexture(int width, int height, GLint magFilter, GLint minFilter)
{
	Create(0, GL_DEPTH_COMPONENT16, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, magFilter, minFilter);
	Bind(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	Unbind(GL_TEXTURE0);
}

void Texture::Resize(int width, int height)
{
	mWidth = width;
	mHeight = height;
	Bind(GL_TEXTURE0);
	glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, width, height, 0, mFormat, mType, 0);
	Unbind(GL_TEXTURE0);
}

void Texture::Bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, mID);
	glActiveTexture(GL_TEXTURE0);
}

void Texture::Unbind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}

void Texture::RenderFullScreen()
{
	if (!mQuad)
	{
		mQuad = new Quad();
		mQuad->Initialise();
	}

	if (!mProgram)
	{
		mProgram = new ShaderProgram("src/shaders/quad.vert", "src/shaders/quad.frag");
	}

	mProgram->Bind();
	Bind(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(mProgram->GetID(), "tex"), 0);
	mQuad->Draw();
	Unbind(GL_TEXTURE0);
	mProgram->Unbind();
}

GLuint Texture::GetID()
{
	return mID;
}

GLuint Texture::GetWidth()
{
	return mWidth;
}

GLuint Texture::GetHeight()
{
	return mHeight;
}

void Texture::Create(unsigned char* image, GLint internalFormat, int width, int height, GLenum format, GLenum type, GLint magFilter, GLint minFilter)
{
	mInternalFormat = internalFormat;
	mWidth = width;
	mHeight = height;
	mFormat = format;
	mType = type;
	glGenTextures(1, &mID);
	Bind(GL_TEXTURE0);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	Unbind(GL_TEXTURE0);
}
