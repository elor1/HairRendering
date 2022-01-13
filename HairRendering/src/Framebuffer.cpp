#include "Framebuffer.h"
#include <iostream>

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
	GLuint depthbuffer;
	Bind();
	glGenRenderbuffers(1, &depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);
	Unbind();
}

void Framebuffer::AttachDepthTexture(GLuint id, GLenum target)
{
	Bind(target);
	glFramebufferTexture(target, GL_DEPTH_ATTACHMENT, id, 0);
	Unbind(target);
}
