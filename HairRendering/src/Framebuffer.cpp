#include "Framebuffer.h"
#include <iostream>
#include "Texture.h"

Framebuffer::~Framebuffer()
{
	if (mColourTexture)
	{
		delete mColourTexture;
	}

	if (mDepthTexture)
	{
		delete mDepthTexture;
	}
}

void Framebuffer::Create()
{
	glGenFramebuffers(1, &mID);
}

void Framebuffer::Bind(GLenum target)
{
	glBindFramebuffer(target, mID);
}

void Framebuffer::Unbind(GLenum target)
{
	glBindFramebuffer(target, 0);
}

void Framebuffer::AttachTexture(GLuint textureID, GLenum target)
{
	std::vector<GLuint> textureIDs = { textureID };
	AttachTextures(textureIDs, target);
}

void Framebuffer::AttachTextures(std::vector<GLuint> textureIDs, GLenum target)
{
	Bind(target);

	int numTextures = textureIDs.size();
	std::vector<GLenum> drawBuffers;
	drawBuffers.resize(numTextures);
	for (int i = 0; i < numTextures; i++)
	{
		drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture(target, GL_COLOR_ATTACHMENT0 + i, textureIDs[i], 0);
	}
	glDrawBuffers(numTextures, &drawBuffers[0]);

	GLenum status = glCheckFramebufferStatus(target);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR: Framebuffer not complete. " << status << std::endl;
	}

	Unbind(target);
}

void Framebuffer::GenerateDepthBuffer(int width, int height)
{
	Bind();
	glGenRenderbuffers(1, &mDepthBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBufferID);
	Unbind();
}

void Framebuffer::AttachDepthTexture(GLuint id, GLenum target)
{
	Bind(target);
	glFramebufferTexture(target, GL_DEPTH_ATTACHMENT, id, 0);
	Unbind(target);
}

void Framebuffer::ResizeDepthBuffer(int width, int height)
{
	Bind();
	glDeleteRenderbuffers(1, &mDepthBufferID);
	GenerateDepthBuffer(width, height);
	Unbind();
}

void Framebuffer::GenerateTexture(int width, int height, GLint magFilter, GLint minFilter)
{
	if (mColourTexture)
	{
		delete mColourTexture;
	}

	mColourTexture = new Texture();
	mColourTexture->Create(width, height, magFilter, minFilter);
	AttachTexture(mColourTexture->GetID());
}

void Framebuffer::GenerateDepthTexture(int width, int height, GLint magFilter, GLint minFilter)
{
	if (mDepthTexture)
	{
		delete mDepthTexture;
	}

	mDepthTexture = new Texture();
	mDepthTexture->CreateDepthTexture(width, height, magFilter, minFilter);
	AttachDepthTexture(mDepthTexture->GetID());
}

Texture* Framebuffer::GetColourTexture()
{
	return mColourTexture;
}

Texture* Framebuffer::GetDepthTexture()
{
	return mDepthTexture;
}
