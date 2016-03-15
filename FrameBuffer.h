#pragma once

#ifndef FRAME_BUFFER
#define FRAME_BUFFER

#include "TTexture.h"
#include "Shader.h"
class CFramebuffer
{
public:

	//Multisampling framebuffers
	void StartMultiSampling();
	void PostProcessMultiSampling();
	void FrameBufferMSAACreate(int width, int height, GLuint samplecount);
	void PostProcessMultiSampling(RECT viewportrec);

	void CreateHDRBuffer();


	//Shadowmap framebuffers
	bool CreateFrameBufferForDepthShadow(int a_iWidth, int a_iHeight);
	bool CreateFramebufferWithTexture(int a_iWidth, int a_iHeight);

	bool AddDepthBuffer();
	void BindFramebuffer(bool bSetFullViewport = true);
	void BindFramebufferShadowMap();

	void SetFramebufferTextureFiltering(int a_tfMagnification, int a_tfMinification);
	void BindFramebufferTexture(int iTextureUnit = 0, bool bRegenMipMaps = false);

	glm::mat4 CalculateProjectionMatrix(float fFOV, float fNear, float fFar);
	glm::mat4 CalculateOrthoMatrix();

	void DeleteFramebuffer();

	int GetWidth();
	int GetHeight();

	CFramebuffer();

private:
	GLuint generateMSAATexture(GLuint samples);
	GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil);

private:

	// Framebuffers
	GLuint hdrfbo;
	GLuint colorBufferhdr;
	GLuint rboDepth;


	//MSAA +shadowmapping
	GLuint rbo;
	GLuint intermediateFBO;
	GLuint textureColorBufferMultiSampled;
	GLuint MSaaSamples;

	int iWidth, iHeight;

	GLuint uiFramebuffer;
	UINT uiDepthRenderbuffer;
	CTexture tFramebufferTex;
	UINT depthTexture;
};

#endif