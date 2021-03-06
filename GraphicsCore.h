#pragma once
#ifndef GL_CORE
#define GL_CORE
#include "stdafx.h"
#include "Renderables.h"
#include "Usefulheaders.h"
#include "Shader.h"
#include "Render.h"
#include "SoundClass.h"
#include "GorillaAudio.h"
class GLcore
{
public:
	
	void InitGraphics(HWND hWnd, LPCTSTR clsname, LPCTSTR wndname, HINSTANCE hInstance, int XWP, int YWP, int width, int height, bool Fullscreen);
	void Destroy();
	void SwapBuffersm();
	void RenderLoop();
	HWND GetViewPort();
	void setProjection3D(float fFOV, float fAspectRatio, float fNear, float fFar);
	void setOrtho2d(int width, int height);
	glm::mat4* getProjectionMatrix();
	glm::mat4* getOrthoMatrix();
	void InitRender();
	int GetFps();
	
	GLcore();
private:   //Private scenes. Makes testing easier and a little bit cleaner
	//globaalit oliotyms



	void ShaderTester();
	void SponzaScene();
private:

	bool InitGLEW(HINSTANCE hInstance);
	bool GlContext(HWND hWnd, HWND MainWindow);
	bool OldContext(HWND hWnd, HDC *hDC, HGLRC *hRC);
	bool NewContext(HWND hWnd, HDC *hDC, HGLRC *hRC);
	bool bClassRegistered;
	bool bGlewInitialized;
	// Matrix for perspective projection
	glm::mat4 mProjection;
	
	bool EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC);
	void DisableOpenGl(HWND hWnd, HDC hDC, HGLRC hRC);
	
	

};


#endif
