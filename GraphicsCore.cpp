#include "stdafx.h"
#include "Resource.h"
#include <glew.h>
#include <wglew.h>
#include "GraphicsCore.h"
#include "SoundClass.h"
#include "GorillaAudio.h"
#include "PrimitiveShape.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "Input.h"
#include "MouseHandler.h"
#include <thread>
#include "Sun.h"

Logger logs2;

//Metallic color values
struct Metals
{
	vec3 Titanium = vec3(0.541931f, 0.496791f, 0.449419f);
	vec3 Silver = vec3(0.971519f, 0.959915f, 0.915324f);
	vec3 Aluminium = vec3(0.913183f, 0.921494f, 0.924524f);
}metal;

std::string sb1[18] = { "CloudyLightRays\\CloudyLightRaysBack2048.png", "CloudyLightRays\\CloudyLightRaysFront2048.png", "CloudyLightRays\\CloudyLightRaysRight2048.png",
"CloudyLightRays\\CloudyLightRaysLeft2048.png", "CloudyLightRays\\CloudyLightRaysUp2048.png", "CloudyLightRays\\CloudyLightRaysDown2048.png",
"TropicalSunnyDay\\TropicalSunnyDayBack2048.png", "TropicalSunnyDay\\TropicalSunnyDayFront2048.png", "TropicalSunnyDay\\TropicalSunnyDayRight2048.png", "TropicalSunnyDay\\TropicalSunnyDayLeft2048.png",
"TropicalSunnyDay\\TropicalSunnyDayUp2048.png", "TropicalSunnyDay\\TropicalSunnyDayDown2048.png" };

HDC hDC;
HGLRC hRC;
Render render;
glm::mat4 mProjection1;
bool ContextDone = false;
glm::mat4 mOrtho1;

GorillaAudio m_Gaudio;
PrimitiveShape  primShapes[500];
btRigidBody* bodies[300];
Renderables object[200];
HWND ViewPort;
int WindowHeight;
int WindowWidth;
// define jutut
#define iMajorVersion 4
#define iMinorVersion 2

//Tunnistamaan näppäinten painallukset
char Keys::kp[256] = { 0 };
int Keys::key(int iKey)
{
	return (GetAsyncKeyState(iKey) >> 15) & 1;
}

int Keys::onekey(int iKey)
{
	if (key(iKey) && !kp[iKey]){ kp[iKey] = 1; return 1; }
	if (!key(iKey))kp[iKey] = 0;
	return 0;
}




GLcore::GLcore()
{

}

HWND GLcore::GetViewPort()
{
	return ViewPort;
}

void GLcore::setOrtho2d(int width, int height)
{
	mOrtho1 = glm::ortho(0.0f, float(4), 0.0f, float(3));
}
void GLcore::setProjection3D(float fFOV, float fAspectRatio, float fNear, float fFar)
{
	//mProjection = glm::perspective(fFOV, fAspectRatio, fNear, fFar);
}
int iFPSCount, iCurrentFPS;
clock_t tLastSecond;

int GLcore::GetFps()
{
	return iCurrentFPS;
}

glm::mat4* GLcore::getOrthoMatrix()
{
	RECT vpRec;
	GetWindowRect(ViewPort, &vpRec);
	mOrtho1 = glm::ortho(0.0f, float(WindowWidth), 0.0f, (float)WindowHeight);
	return &mOrtho1;
}
glm::mat4* GLcore::getProjectionMatrix()
{
	mProjection1 = glm::perspective(45.0f, (float)WindowWidth / (float)WindowHeight, 0.1f, 100000.0f);
	return &mProjection1;
}

void GLcore::Destroy()
{
	DisableOpenGl(ViewPort, hDC, hRC);
	render.DestroyRender();
	DestroyWindow(ViewPort);
	//m_Audio->Shutdown();
	m_Gaudio.destroy();
}

//Opengl context creation
bool GLcore::GlContext(HWND hWnd, HWND MainWindow)
{
	hDC = GetDC(hWnd); // Get the device context for our window

	PIXELFORMATDESCRIPTOR pfd; // Create a new PIXELFORMATDESCRIPTOR (PFD)
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Clear our  PFD
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD to the size of the class
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window
	pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels
	pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)
	pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)
	pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD

	int nPixelFormat = ChoosePixelFormat(hDC, &pfd); // Check if our PFD is valid and get a pixel format back
	if (nPixelFormat == 0)
	{// If it fails
		MessageBox(MainWindow, L"choose pf error", L"ERROR", MB_OK);
		return false;
	}
	bool bResult = SetPixelFormat(hDC, nPixelFormat, &pfd); // Try and set the pixel format based on our PFD
	if (!bResult) // If it fails
	{

		MessageBox(MainWindow, L"set pf error", L"ERROR", MB_OK);
		return false;
	}
	HGLRC tempOpenGLContext = wglCreateContext(hDC); // Create an OpenGL 2.1 context for our device context
	wglMakeCurrent(hDC, tempOpenGLContext); // Make the OpenGL 2.1 context current and active

	GLenum error = glewInit(); // Enable GLEW
	if (error != GLEW_OK) // If GLEW fails
	{

		MessageBox(MainWindow, L"glewError", L"ERROR", MB_OK);
		return false;
	}
	int attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4.5, // Set the MAJOR version of OpenGL to 4.5
		WGL_CONTEXT_MINOR_VERSION_ARB, 2, // Set the MINOR version of OpenGL to 2
		WGL_CONTEXT_FLAGS_ARB, 
		WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible
		0,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, 2, //Number of buffers (must be 1 at time of writing)
		WGL_SAMPLES_ARB, 16,        //Number of samples
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1) { // If the OpenGL  context creation extension is available
		hRC = wglCreateContextAttribsARB(hDC, NULL, attributes); // Create and OpenGL context based on the given attributes
		wglMakeCurrent(NULL, NULL); // Remove the temporary context from being active
		wglDeleteContext(tempOpenGLContext); // Delete the temporary OpenGL 2.1 context
		wglMakeCurrent(hDC, hRC); // Make our OpenGL context current
	}
	else {
		hRC = tempOpenGLContext; // If we didn't have support for OpenGL 3.x and up, use the OpenGL 2.1 context
	}

	int glVersion[2] = { -1, -1 }; // Set some default values for the version
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); // Get back the OpenGL MAJOR version we are using
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); // Get back the OpenGL MAJOR version we are using


	std::cout << "Using OpenGL: " << glVersion[0] << "." << glVersion[1] << std::endl; // Output which version of OpenGL we are using
	std::string mesg = "Using opengl version: " + std::to_string(glVersion[0]);
	Logger::Log(mesg);
	return true; // We have successfully created a context, return true

}
void GLcore::DisableOpenGl(HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);
}
bool AudioStopped = false;


float avgFpss[9999];
int fpsindex = 0;

float CalcAvgFps()
{
	float totfps = 0;
	for (size_t i = 0; i < 4; i++)
	{
		totfps += avgFpss[i];
	}

    totfps = totfps / 4;
	return totfps;
}
float fpsf1 = 0;
//Render looppi, lasketaan myös frameintervalli ja fps loopin ympärillä
void GLcore::RenderLoop()
{
	
	if (Keys::onekey(VKEY_E))
	{
		m_Gaudio.PlayAudio();
	}
	if (Keys::onekey(VKEY_Q))
	{
		m_Gaudio.StopAudio();
	}

	if (Keys::onekey(VKEY_PLUS))
	{
		m_Gaudio.VolumeUp();
	}

	if (Keys::onekey(VKEY_MINUS))
	{
		m_Gaudio.VolumeDown();
	}

	m_Gaudio.updateMgr();
	clock_t tCurrent = clock();
	double finterval = tCurrent - tLastSecond;
	if (fpsindex > 5)
	{
		fpsindex = 0;
	}
	
	if ((tCurrent - tLastSecond) >= CLOCKS_PER_SEC)
	{
		tLastSecond += CLOCKS_PER_SEC;
		iFPSCount = iCurrentFPS;
	
		fpsindex = 0;
		fpsf1 = CalcAvgFps();
		render.SetFps(fpsf1);
		iCurrentFPS = 0;
		
	}
	avgFpss[fpsindex] = iCurrentFPS;
	
	render.RenderAll((LPVOID)this);
	fpsindex++;
	iCurrentFPS++;
}
//bufferien vaihto
void GLcore::SwapBuffersm()
{
	SwapBuffers(hDC);
}


SkyBox skybox;
Light plights[10];

void GLcore::SponzaScene()
{
	

	object[0].TestImprovedAssimp("Models\\crytek\\sponza.obj");
	object[0].SetObjectId("sponza");
	object[0].HasTexture = true;
	object[0].IsTransParent = false;
	object[0].SetRoughness(0.1f);
	object[0].rotateObject(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	object[0].SetObjectSize(glm::vec3(0.5f, 0.5f, 0.5f));
	object[0].SetObjectLocation(glm::vec3(0.0f, -100.0f, 0.0f));
	object[0].SetObjectColor(metal.Aluminium);
	object[0].usesphysics = false;
	object[0].SetPhysics(bodies[0], 0);
	render.addRenderObject(object[0]);
	

	

	for (size_t i = 0; i < 3; i++)
	{
		plights[i] = Light();
		plights[i].pointlight.Diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		plights[i].pointlight.Position = glm::vec3(-250.0f + i * 200.0f, 260.0f, 200.0f);
		render.addPointLight(plights[i]);

		primShapes[i].initPrimitiveShape(2, 1);
		primShapes[i].SetObjectId("Pallo");
		primShapes[i].SetRoughness(5.0f);
		primShapes[i].SetObjectSize(glm::vec3(10.0f, 10.0f, 10.0f));
		primShapes[i].SetObjectLocation(glm::vec3(-250.0f + i*200.0f, 260.0f, 200.0f));
		primShapes[i].SetObjectColor(metal.Aluminium);
		primShapes[i].SetObjectMaterialProperties(vec3(0.2f, 0.2f, 0.2f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), 150.0f, 1);
		primShapes[i].usesphysics = true;
		primShapes[i].SetPhysics(bodies[i+1], i+1);
		render.addPrimitiveRenderObject(primShapes[i]);
	}
		
	
}

void GLcore::ShaderTester()
{
	primShapes[0].initPrimitiveShape(2, 1);
	primShapes[0].SetObjectId("pallo");
	primShapes[0].SetRoughness(5.0f);
	primShapes[0].SetObjectSize(glm::vec3(100.0f, 100.0f, 100.0f));
	primShapes[0].SetObjectLocation(glm::vec3(0.0f, 5.0f, -900.0f));
	primShapes[0].SetObjectColor(metal.Aluminium);
	primShapes[0].SetObjectMaterialProperties(vec3(0.2f, 0.2f, 0.2f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), 150.0f, 1);
	primShapes[0].usesphysics = true;
	primShapes[0].SetPhysics(bodies[0], 0);
	render.addPrimitiveRenderObject(primShapes[0]);

	primShapes[1].initPrimitiveShape(0, 1);
	primShapes[1].SetObjectId("lattia");
	primShapes[1].SetObjectTexture("Textures\\woodplanks.jpg");
	primShapes[1].SetObjectMaterialProperties(vec3(0.2f, 0.2f, 0.2f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), 45.0f, 1);
	primShapes[1].SetObjectSize(glm::vec3(2500.0f, 5.0f, 2500.0f));
	primShapes[1].SetObjectLocation(glm::vec3(0.0f, -100.0f, 0.0f));
	primShapes[1].SetObjectColor(glm::vec3(1.0f, 1.0f, 1.0f));
	primShapes[1].usesphysics = false;
	primShapes[1].SetPhysics(bodies[8], 8);
	render.addPrimitiveRenderObject(primShapes[1]);


	std::vector<std::string> cubemap;
	cubemap.push_back("Textures\\skyboxes\\" + sb1[0]);
	cubemap.push_back("Textures\\skyboxes\\" + sb1[1]);
	cubemap.push_back("Textures\\skyboxes\\" + sb1[2]);
	cubemap.push_back("Textures\\skyboxes\\" + sb1[3]);
	cubemap.push_back("Textures\\skyboxes\\" + sb1[4]);
	cubemap.push_back("Textures\\skyboxes\\" + sb1[5]);


	primShapes[2].initPrimitiveShape(0, 1);
	primShapes[2].SetObjectId("kuutio");
	primShapes[2].SetObjectTexture("Textures\\crate_1.jpg");
	primShapes[2].SetObjectSize(glm::vec3(100.0f, 100.0f, 100.0f));
	primShapes[2].SetObjectLocation(glm::vec3(500.0f, -50.0f, 300.0f));
	primShapes[2].SetObjectColor(glm::vec3(1.0f, 1.0f, 1.0f));
	primShapes[2].usesphysics = false;
	primShapes[2].SetPhysics(bodies[1], 1);
	render.addPrimitiveRenderObject(primShapes[2]);



	primShapes[3].initPrimitiveShape(0, 1);
	primShapes[3].SetObjectId("kuutio2");
	primShapes[3].SetObjectTexture("Textures\\crate_1.jpg");
	primShapes[3].SetObjectSize(glm::vec3(100.0f, 100.0f, 100.0f));
	primShapes[3].SetObjectLocation(glm::vec3(500.0f, 51.0f, 300.0f));
	primShapes[3].SetObjectColor(glm::vec3(1.0f, 1.0f, 1.0f));
	primShapes[3].usesphysics = false;
	primShapes[3].SetPhysics(bodies[2], 2);
	render.addPrimitiveRenderObject(primShapes[3]);


	object[1].TestImprovedAssimp("Models\\Wellobj\\well.obj");
	object[1].HasTexture = true;
	object[1].IsTransParent = false;
	object[1].SetRoughness(0.1f);
	object[1].rotateObject(6.3f, glm::vec3(1.0f, 0.0f, 0.0f));
	object[1].SetObjectSize(glm::vec3(100.0f, 100.0f, 100.0f));
	object[1].SetObjectLocation(glm::vec3(-500.0f, -50.0f, 500.0f));
	object[1].SetObjectColor(metal.Aluminium);
	object[1].usesphysics = false;
	object[1].SetPhysics(bodies[3], 3);
	render.addRenderObject(object[1]);


	object[2].TestImprovedAssimp("Models\\Crytek\\nanosuit\\nanosuit.obj");
	object[2].SetObjectId("Nanosuit2");
	object[2].HasTexture = true;
	object[2].IsTransParent = true;
	object[2].SetRoughness(0.1f);
	object[2].rotateObject(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	object[2].SetObjectSize(glm::vec3(15.0f, 15.0f, 15.0f));
	object[2].SetObjectLocation(glm::vec3(300.0f, -100.0f, -500.0f));
	object[2].SetObjectColor(metal.Aluminium);
	object[2].usesphysics = false;
	object[2].SetPhysics(bodies[4], 4);
	render.addRenderObject(object[2]);


	primShapes[4].initPrimitiveShape(0, 1);
	primShapes[4].SetRoughness(50.0f);
	primShapes[4].SetObjectId("Wall1");
	primShapes[4].SetObjectTexture("Textures\\brickwall.jpg");
	primShapes[4].SetObjectSize(glm::vec3(200.0f, 500.0f, 1000.0f));
	primShapes[4].SetObjectLocation(glm::vec3(-1100.0f, 160.0f, 700.0f));
	primShapes[4].SetObjectColor(glm::vec3(1.0f, 1.0f, 1.0f));
	primShapes[4].usesphysics = false;
	primShapes[4].SetPhysics(bodies[5], 5);
	render.addPrimitiveRenderObject(primShapes[4]);



	primShapes[5].initPrimitiveShape(2, 1);
	primShapes[5].SetObjectId("pallo2");
	primShapes[5].SetRoughness(0.01f);
	primShapes[5].SetObjectSize(glm::vec3(100.0f, 100.0f, 100.0f));
	primShapes[5].SetObjectLocation(glm::vec3(-600.0f, 5.0f, -600.0f));
	primShapes[5].SetObjectColor(metal.Aluminium);
	primShapes[5].usesphysics = true;
	primShapes[5].SetPhysics(bodies[6], 6);
	render.addPrimitiveRenderObject(primShapes[5]);

	
	for (size_t i = 0; i < 3; i++)
	{
		plights[i] = Light();
		plights[i].pointlight.Position = glm::vec3(-800.0f, 100.0f, 700.0f + i * 120.0f);
		render.addPointLight(plights[i]);
	}
}

void GLcore::InitRender()
{
	
	CShader::SetWindow(ViewPort);

	this->SponzaScene();



	render.ShaderPrograms();



	skybox.loadSkybox("Textures\\skyboxes\\", sb1[6], sb1[7], sb1[8], sb1[9], sb1[10], sb1[11]);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	

	m_Gaudio.initGorillaAudio("Audio\\athmosphere1.wav");
	
	MouseHandler::SetCurrentWindow(ViewPort);
	
	
	

	render.addSkyBox(skybox);
	
	Logger::Log("Initialization complete");

}

void GLcore::InitGraphics(HWND hWnd, LPCTSTR szsWindowClass, LPCTSTR szsTitle, HINSTANCE hInstance, int XWP, int YWP, int width, int height, bool FullScreen)
{
	bool enOGL = false;
	// Initialize GLEW
	//bool glewstuffinit = InitGLEW(hInstance);

	HMONITOR hmon = MonitorFromWindow(hWnd,MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = { sizeof(MONITORINFO) };
	GetMonitorInfo(hmon, &mi);
	
	WindowWidth = mi.rcMonitor.right - mi.rcMonitor.left;
	WindowHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
	ShowWindow(hWnd, false);
	if (FullScreen)
	{
		ViewPort = CreateWindowEx(NULL,szsWindowClass, szsTitle,WS_POPUP | WS_VISIBLE, mi.rcMonitor.left, mi.rcMonitor.top,WindowWidth, WindowHeight, NULL, NULL, hInstance, NULL);
	}
	else
	{
		WindowHeight -= 100;
		WindowWidth -= 200;
		ViewPort = CreateWindowEx(NULL, szsWindowClass, szsTitle, WS_POPUP | WS_VISIBLE, mi.rcMonitor.left + 100, mi.rcMonitor.top + 20, WindowWidth, WindowHeight, NULL, NULL, hInstance, NULL);
		MouseHandler::SetCursorPosInWindow(100,20);
	}
	bool glewstuffinit = GlContext(ViewPort, hWnd);
	if (glewstuffinit)
	{
		Logger::Log("Initialized glew");
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		
	}
	else
	{
		MessageBox(hWnd, L"GLEW INITIALIZATION ERROR!", L"ERROR", MB_OK);
	}


}

