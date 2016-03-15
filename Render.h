#pragma once


#ifndef RENDERER_H
#define RENDERER_H
#include <glew.h>
#include <wglew.h>
#include "Renderables.h"
#include "FreeTypeFont.h"
#include "SkyBox.h"
#include "FrameBuffer.h"
#include "PrimitiveShape.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "FlyingCamera.h"
#include "FreeTypeFont.h"
#include "Sun.h"


class Render
{
public:
	Render();
	void addRenderObject(Renderables lpObject);
	void addPrimitiveRenderObject(PrimitiveShape lpObject);
	void addSkyBox(SkyBox sb);
	void RenderAll(LPVOID lpParam);
	void SetFps(float fps);
	void ShaderPrograms();
	void addPointLight(Light &plight);
	


	~Render();
	std::vector<Renderables> Objects;
	std::vector<PrimitiveShape> Primitives;
	std::vector<Light> PointLights;
	void DestroyRender();
	glm::mat4 GetModelMatrix();
	std::string fps;
	



private: //private members
	CFreeTypeFont ftFont;
	Renderables *m_pickedObject;
	PrimitiveShape *m_pickedPrimitive;
	SkyBox skybox;

	glm::mat4 mProjection;
	glm::mat4 mOrtho;
	glm::mat4 mModelMat;

	Light plight;

	int xviewwidth = 900;
	int yviewwidth = 900;
	bool bShadowsOn = true;
	bool bDisplayShadowMap = true;
	bool ShowLogger = false;

	float skyboxrotator = 0;
	float _fps = 0;
	float sunY = 1000;

	int logtimecounter = 0;
	int coolslider = 0;
	int msx = 0, msy = 0;
	int iShadowMapTextureSize = 2056;

	std::string pickedObject = "none";
	

private: // Private functions
	void InitPhysics();
	void InitShadowMapping();
	void RayCast3DPicking(glm::mat4 projMat, RECT vprect);
	void FrameBufferMultiSampling();
	void StartMultiSampling();
	void PostProcessMultiSampling(RECT viewportrec);
	void SetLightUniforms();

	void ScreenPosToWorldRay(
		int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
		int screenWidth, int screenHeight,  // Window size, in pixels
		glm::mat4 ViewMatrix,               // Camera position and orientation
		glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
		glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
		glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
		);
	int RenderText(LPVOID lpParam);
	bool enableAntiAliasing = true;
	int fresneluse = 1;
	int bumbMaps = 1;
	GLuint generateMultiSampleTexture(GLuint samples);
	GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil);



private: //More private members
#define PI 3.14159265
	Logger logs1;

	CShader shader[20];
	CShaderProgram sProgram, sFontProgram, sSkyboxProgram, spShadowMapper, 
		spShadowMapRender, sOrtho2D, spHdr;


	
	FlyingCam cam;


	//Physics members
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;


	CFramebuffer fboShadowMap;
	CFramebuffer *MultiSampler;


	vertexbufferobject vboShadowMapQuad;
	UINT uiVAOShadowMapQuad;
	CTexture rotationTexture;
};

#endif