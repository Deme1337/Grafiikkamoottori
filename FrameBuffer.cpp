#include "stdafx.h"
#include "Framebuffer.h"
#include "Usefulheaders.h"

#include <gtc/matrix_transform.hpp>

CFramebuffer::CFramebuffer()
{
	uiFramebuffer = 0;
	uiDepthRenderbuffer = 0;
	rbo = 0;

}

GLuint CFramebuffer::generateMSAATexture(GLuint samples)
{
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, 1924, 1080, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	return texture;
}

GLuint CFramebuffer::generateAttachmentTexture(GLboolean depth, GLboolean stencil)
{
	// What enum to use?
	GLenum attachment_type;
	if (!depth && !stencil)
		attachment_type = GL_RGB;
	else if (depth && !stencil)
		attachment_type = GL_DEPTH_COMPONENT;
	else if (!depth && stencil)
		attachment_type = GL_STENCIL_INDEX;

	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (!depth && !stencil)
		glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, 1924, 1080, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
	else // Using both a stencil and depth test, needs special format arguments
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 1924, 1080, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

void CFramebuffer::FrameBufferMSAACreate(int width, int height, GLuint samplecount)
{
	MSaaSamples = samplecount;
	iWidth = width;
	iHeight = height;

	glGenFramebuffers(1, &uiFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);
	// Create a multisampled color attachment texture
	GLuint textureColorBufferMultiSampled = generateMSAATexture(MSaaSamples);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
	// Create a renderbuffer object for depth and stencil attachments

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, 1924, 1080);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Logger::Log("ERROR::FRAMEBUFFER:: framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// second framebuffer
	GLuint screenTexture = generateAttachmentTexture(false, false);
	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// We only need a color buffer

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Logger::Log("ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CFramebuffer::StartMultiSampling()
{
	glEnable(GL_MULTISAMPLE);
	// 1. Draw scene as normal in multisampled buffers
	glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);
}

void CFramebuffer::PostProcessMultiSampling(RECT viewportrec)
{
	// 2. Now blit multisampled buffer(s) to default framebuffers
	glBindFramebuffer(GL_READ_FRAMEBUFFER, uiFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, viewportrec.right, viewportrec.bottom, 0, 0, viewportrec.right, viewportrec.bottom, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

bool CFramebuffer::CreateFrameBufferForDepthShadow(int a_iWidth, int a_iHeight)
{
	if (uiFramebuffer != 0)
	{
		return false;
	}

	glGenFramebuffers(1, &uiFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);

	tFramebufferTex.CreateDepthTexture(a_iWidth, a_iHeight);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tFramebufferTex.GetTextureID(), 0);

	glDrawBuffers(0, NULL); glReadBuffer(GL_NONE);

	iWidth = a_iWidth;
	iHeight = a_iHeight;

	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

/*-----------------------------------------------

Name:	CreateFramebufferWithTexture

Params:	a_iWidth - framebuffer width
a_iHeight - framebuffer height

Result:	Creates a framebuffer and a texture to
render to.

/*---------------------------------------------*/

bool CFramebuffer::CreateFramebufferWithTexture(int a_iWidth, int a_iHeight)
{
	if (uiFramebuffer != 0)return false;

	glGenFramebuffers(1, &uiFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);

	tFramebufferTex.CreateEmptyTexture(a_iWidth, a_iHeight, GL_RGB);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tFramebufferTex.GetTextureID(), 0);

	iWidth = a_iWidth;
	iHeight = a_iHeight;

	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

/*-----------------------------------------------

Name:	AddDepthBuffer

Params:	none

Result:	Adds depth renderbuffer to framebuffer,
so rendering can perform depth testing.

/*---------------------------------------------*/

bool CFramebuffer::AddDepthBuffer()
{
	if (uiFramebuffer == 0)return false;

	glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);

	glGenRenderbuffers(1, &uiDepthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, uiDepthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, iWidth, iHeight);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, uiDepthRenderbuffer);

	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

/*-----------------------------------------------

Name:	BindFramebuffer

Params:	bSetFullViewport - set full framebuffer
viewport, default is true

Result:	Binds this framebuffer.

/*---------------------------------------------*/

void CFramebuffer::BindFramebuffer(bool bSetFullViewport)
{
	glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);
	if (bSetFullViewport)glViewport(0, 0, iWidth, iHeight);
}

void CFramebuffer::BindFramebufferShadowMap()
{
	glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);
	glViewport(0, 0, 1024, 1024);
}

/*-----------------------------------------------

Name:	BindFramebufferTexture

Params:	iTextureUnit - texture unit to bind to (defaul 0)
bRegenMipMaps - regenerate mipmaps? (default false)

Result:	Binds framebuffer texture, where renderings
are stored.

/*---------------------------------------------*/

void CFramebuffer::BindFramebufferTexture(int iTextureUnit, bool bRegenMipMaps)
{
	tFramebufferTex.isShadow = 1;
	tFramebufferTex.bindTexture(iTextureUnit);
	if (bRegenMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
}

/*-----------------------------------------------

Name:	SetFramebufferTextureFiltering

Params:	a_tfMagnification - magnification filter
a_tfMinification - miniification filter

Result:	Sets filtering of framebuffer texture,
poor texturing here results in greater
performance.

/*---------------------------------------------*/

void CFramebuffer::SetFramebufferTextureFiltering(int a_tfMagnification, int a_tfMinification)
{
	tFramebufferTex.setFiltering(a_tfMagnification, a_tfMinification);
	tFramebufferTex.SetWrap();
}

/*-----------------------------------------------

Name:	DeleteFramebuffer

Params:	none

Result:	Deletes framebuffer and frees memory.

/*---------------------------------------------*/

void CFramebuffer::DeleteFramebuffer()
{
	if (uiFramebuffer)
	{
		glDeleteFramebuffers(1, &uiFramebuffer);
		uiFramebuffer = 0;
	}
	if (uiDepthRenderbuffer)
	{
		glDeleteRenderbuffers(1, &uiDepthRenderbuffer);
		uiDepthRenderbuffer = 0;
	}
	tFramebufferTex.releaseTexture();
}

/*-----------------------------------------------

Name:	CalculateProjectionMatrix

Params:	fFOV - field of view angle
fNear, fFar - distance of near and far clipping plane

Result:	Calculates projection matrix.

/*---------------------------------------------*/

glm::mat4 CFramebuffer::CalculateProjectionMatrix(float fFOV, float fNear, float fFar)
{
	return glm::perspective(fFOV, float(iWidth) / float(iHeight), fNear, fFar = 1500);
}

/*-----------------------------------------------

Name:	CalculateOrthoMatrix

Params:	none

Result:	Calculates ortho 2D projection matrix.

/*---------------------------------------------*/

glm::mat4 CFramebuffer::CalculateOrthoMatrix()
{
	return glm::ortho(0.0f, float(iWidth), 0.0f, float(iHeight));
}

/*-----------------------------------------------

Name:	Getters

Params:	none

Result:	... They get something :D

/*---------------------------------------------*/

int CFramebuffer::GetWidth()
{
	return iWidth;
}

int CFramebuffer::GetHeight()
{
	return iHeight;
}