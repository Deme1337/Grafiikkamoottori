#include "stdafx.h"
#include <glew.h>
#include "Render.h"
#include <cmath>
#include <stdio.h>
#include <tchar.h>
#include "Usefulheaders.h"
#include "Input.h"
#include "Shader.h"
#include <iostream>
#include <fstream>
#include <gtx\norm.hpp>
#include <string>
#include "MouseHandler.h"
#include "vertexbufferobject.h"
#include "GraphicsCore.h"
#include <thread>
#include "OpenFileDialog.h"
#include "Sun.h"



Render::Render()
{
}


void Render::addSkyBox(SkyBox sb)
{
	skybox = sb;
}


glm::vec3 vShadowMapQuad[] =
{
	glm::vec3(0.75f, 1.0f, 0.0f),
	glm::vec3(0.75f, 0.5f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(1.0f, 0.5f, 0.0f)
};

glm::vec2 vShadowMapQuadTC[] =
{
	glm::vec2(0.0f, 1.0f),
	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(1.0f, 0.0f)
};


//Initializing physics engine
void Render::InitPhysics()
{
	// Build the broadphase
	broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
    collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	solver = new btSequentialImpulseConstraintSolver;

	// The world.
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
}



void Render::ScreenPosToWorldRay(
	int mouseX, int mouseY,             
	int screenWidth, int screenHeight,  
	glm::mat4 ViewMatrix,               
	glm::mat4 ProjectionMatrix,         
	glm::vec3& out_origin,             
	glm::vec3& out_direction           
	)
{
	// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
	glm::vec4 lRayStart_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
		);
	glm::vec4 lRayEnd_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
		0.0,
		1.0f
		);


	// The Projection matrix goes from Camera Space to NDC.
	// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
	glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

	// The View Matrix goes from World Space to Camera Space.
	// So inverse(ViewMatrix) goes from Camera Space to World Space.
	glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

	glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
	glm::vec4 lRayStart_world = InverseViewMatrix       * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
	glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
	glm::vec4 lRayEnd_world = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;


	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);


	out_origin = glm::vec3(lRayStart_world);
	out_direction = glm::normalize(lRayDir_world);
}

void Render::InitShadowMapping()
{

	vboShadowMapQuad.createVBO();
	FOR(i, 4)
	{
		vboShadowMapQuad.addData(&vShadowMapQuad[i], sizeof(glm::vec3));
		vboShadowMapQuad.addData(&vShadowMapQuadTC[i], sizeof(glm::vec2));
	}

	glGenVertexArrays(1, &uiVAOShadowMapQuad);
	glBindVertexArray(uiVAOShadowMapQuad);

	vboShadowMapQuad.bindVBO();
	vboShadowMapQuad.uploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));


	fboShadowMap.CreateFramebufferWithTexture(iShadowMapTextureSize , iShadowMapTextureSize );
	if (fboShadowMap.AddDepthBuffer())
	{
		Logger::Log("Depthbuffer done");
	}


	fboShadowMap.SetFramebufferTextureFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST);
	rotationTexture.CreateRotationTexture(128, 128);
}

void Render::SetLightUniforms()
{
	
	for (int i = 0; i < PointLights.size(); i++)
	{
		std::string index = std::to_string(i);
		sProgram.setUniform("pLight["+index+"].ambient", PointLights[i].pointlight.Ambient);
		sProgram.setUniform("pLight["+index+"].position", PointLights[i].pointlight.Position);
		sProgram.setUniform("pLight["+index+"].diffuse", PointLights[i].pointlight.Diffuse);
		sProgram.setUniform("pLight["+index+"].specular", PointLights[i].pointlight.Specular);
		sProgram.setUniform("pLight["+index+"].constant", PointLights[i].pointlight.constant);
		sProgram.setUniform("pLight["+index+"].linear", PointLights[i].pointlight.linear);
		sProgram.setUniform("pLight["+index+"].quadratic", PointLights[i].pointlight.quadratic);
	}
}


void Render::ShaderPrograms()
{

	std::string ShaderFolder = "Shaders\\";
	shader[0].loadShader(ShaderFolder + "Shader.frag", GL_FRAGMENT_SHADER);
	shader[1].loadShader(ShaderFolder + "Shader.vert", GL_VERTEX_SHADER);
	shader[2].loadShader(ShaderFolder + "font2D.frag", GL_FRAGMENT_SHADER);
	shader[3].loadShader(ShaderFolder + "ortho2D.frag", GL_FRAGMENT_SHADER);
	shader[4].loadShader(ShaderFolder + "ortho2D.vert", GL_VERTEX_SHADER);
	shader[5].loadShader(ShaderFolder + "DirectionalLight.frag", GL_FRAGMENT_SHADER);
	shader[6].loadShader(ShaderFolder + "shadowMapper.vert", GL_VERTEX_SHADER);
	shader[7].loadShader(ShaderFolder + "shadowMapper.frag", GL_FRAGMENT_SHADER);
	shader[8].loadShader(ShaderFolder + "shadowMapRender.frag", GL_FRAGMENT_SHADER);
	shader[9].loadShader(ShaderFolder + "shadowMapRender.vert", GL_VERTEX_SHADER);
	shader[10].loadShader(ShaderFolder + "shadows.frag", GL_FRAGMENT_SHADER);
	shader[11].loadShader(ShaderFolder + "skybox.vert", GL_VERTEX_SHADER);
	shader[12].loadShader(ShaderFolder + "skybox.frag", GL_FRAGMENT_SHADER);
	shader[13].loadShader(ShaderFolder + "hdrshader.vert", GL_VERTEX_SHADER);
	shader[14].loadShader(ShaderFolder + "hdrshader.frag", GL_FRAGMENT_SHADER);


	ShowLogger = false;
	sProgram.setUniform("NumPLights", (int)PointLights.size());
	//shaderit 3d
	sProgram.createProgram();
	sProgram.addShaderToProgram(&shader[5]);
	sProgram.addShaderToProgram(&shader[1]);
	sProgram.addShaderToProgram(&shader[0]);
	sProgram.addShaderToProgram(&shader[10]);
	sProgram.linkProgram();

	//2d shaderit fontteja varten
	sOrtho2D.createProgram();
	sOrtho2D.addShaderToProgram(&shader[4]);
	sOrtho2D.addShaderToProgram(&shader[3]);
	sOrtho2D.linkProgram();
	//2d shaderit fontteja varten
	sFontProgram.createProgram();
	sFontProgram.addShaderToProgram(&shader[2]);
	sFontProgram.addShaderToProgram(&shader[4]);
	sFontProgram.linkProgram();

	//Skyboxille oma shaderi
	sSkyboxProgram.createProgram();
	sSkyboxProgram.addShaderToProgram(&shader[11]);
	sSkyboxProgram.addShaderToProgram(&shader[12]);
	sSkyboxProgram.linkProgram();

	//shader hdr
	spHdr.createProgram();
	spHdr.addShaderToProgram(&shader[13]);
	spHdr.addShaderToProgram(&shader[14]);
	spHdr.linkProgram();


	//shaderit shadow mappingiin
	spShadowMapper.createProgram();
	spShadowMapper.addShaderToProgram(&shader[6]);
	spShadowMapper.addShaderToProgram(&shader[7]);
	spShadowMapper.linkProgram();

	spShadowMapRender.createProgram();
	spShadowMapRender.addShaderToProgram(&shader[8]);
	spShadowMapRender.addShaderToProgram(&shader[9]);
	spShadowMapRender.linkProgram();

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	cam.vEye = glm::vec3(10.0f, 0.0f, 0.0f);

	//Initialize fonts
	Logger::Log("Loading fonts");
	ftFont.SetShaderProgram(&sFontProgram);
	ftFont.LoadFont("arial.ttf", 64, 256);
	mOrtho = glm::ortho(0.0f, float(800), 0.0f, float(800));
    mProjection = glm::perspective(45.0f, float(4) / float(3), 0.1f, 100000.0f);

	glEnable(GL_MULTISAMPLE);
	
	//init shadowmaps
	this->InitShadowMapping();
	//init multisampler 
	MultiSampler = new CFramebuffer();
	MultiSampler->FrameBufferMSAACreate(1924, 1080, 8);
	this->InitPhysics();


	for (size_t i = 0; i < Objects.size(); i++)
	{
		dynamicsWorld->addRigidBody(Objects[i].getRigidbody());
	}
	
	for (size_t i = 0; i < Primitives.size(); i++)
	{
		dynamicsWorld->addRigidBody(Primitives[i].getRigidbody());
	}
	glClearColor(1.0f, 0.2f, 0.5f, 1.0f);
}

void Render::addPointLight(Light &plight)
{
	this->PointLights.push_back(plight);
}

void Render::addRenderObject(Renderables rparam)
{

	this->Objects.push_back(rparam);
}


void Render::addPrimitiveRenderObject(PrimitiveShape rparam)
{

	this->Primitives.push_back(rparam);
}


glm::vec3 sunPos = glm::vec3(0.0f,8000.0f, 0.0f);

void Render::SetFps(float fps)
{
	this->_fps = fps;
}	
float specobjpower = 1.0f;
void Render::RenderAll(LPVOID lpParam)
{
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_MULTISAMPLE);
	// Typecast lpParam to glcore pointer
	GLcore *glcore = (GLcore*)lpParam;
	
	glClearColor(1.0f, 0.2f, 0.5f, 1.0f);
	dynamicsWorld->stepSimulation(100, 5);

	//Count the speed of camera from frameinterval average and clamp it between some suitable values
	cam.fFrameInterval = glm::clamp(1/_fps,0.0001f,0.005f);

	glm::mat4 mDepthBiasMVP;

	bShadowsOn = true;
	glEnable(GL_DEPTH_TEST);



	

	if (bShadowsOn) // So if the shadows are on
	{
		// Render the scene from lights point of view


		fboShadowMap.BindFramebuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		spShadowMapper.useProgram();
		// Because we have a directional light, we just set it high enough (vLightPos) so that it sees all objects on scene
		// We also create orthographics projection matrix for the purposes of rendering shadows
		const float fRangeX = xviewwidth, fRangeY = yviewwidth, fMinZ = 0.1f, fMaxZ = glm::length(glm::vec3(0.0f, 0.0f, 0.0f) - sunPos) + 1500;
		glm::mat4 mPROJ = glm::ortho<float>(-fRangeX, fRangeX, -fRangeY, fRangeY, fMinZ, fMaxZ);

		glm::mat4 mViewFromLight = glm::lookAt(sunPos, glm::vec3(0, 0, 0), glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), glm::normalize(sunPos)));

		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
			);


		// Calculate depth bias matrix to calculate shadow coordinates in shader programs
		mDepthBiasMVP = biasMatrix * mPROJ * mViewFromLight;



		//Render to framebuffer for shadowmaps
		for (unsigned int i = 0; i < Objects.size(); i++)
		{
			if (Objects[i].isInShadowMap)
			{

			    Objects[i].InitNewAssimp(&sProgram);
			    glm::mat4 depthMVP = mPROJ * mViewFromLight * Objects[i].GetModelMatrix();
			    spShadowMapper.setUniform("depthMVP", depthMVP);
			    Objects[i].DrawNewAssimp(&sProgram);

			}
		
		}
		for (unsigned int i = 0; i < Primitives.size(); i++)
		{
			if (Primitives[i].isInShadowMap)
			{
				Primitives[i].initRenderPrimitiveShape(&sProgram, dynamicsWorld);
				glm::mat4 depthMVP = mPROJ * mViewFromLight * Primitives[i].GetModelMatrix();
				spShadowMapper.setUniform("depthMVP", depthMVP);
				Primitives[i].renderPrimitiveShape();
			}
		}

	



	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//Start msaa ALWAYS BEFORE SETTING GLCLEAR AND ENABLES AND STUFF FFS
	if (enableAntiAliasing)
	{
		MultiSampler->StartMultiSampling();
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);

	
	//Get viewport sizes into a RECT tag
	RECT viewportrec;
	GetWindowRect(glcore->GetViewPort(), &viewportrec);
	glViewport(0, 0, viewportrec.right, viewportrec.bottom);


	glm::mat4 mView = cam.look();

	//should it be defined in fragment shader or not?
	float fZero = pow((1.0f - (1.0f / 1.31f)) / (1.0f + (1.0f / 1.31f)), 2);

	//Shaders for rendering objects
	sProgram.useProgram();

	sProgram.setUniform("matActive.Shininess", 0.04f);
	sProgram.setUniform("fZero", (float)fZero);
	sProgram.setUniform("sunLight.vColor", glm::vec3(0.8f, 0.8f, 0.8f));
	sProgram.setUniform("sunLight.fAmbient", 0.001f);

	sProgram.setUniform("sunLight.vDirection", -glm::normalize(sunPos));
	sProgram.setUniform("sunLight.iSkybox", 0);

	sProgram.setUniform("NumPLights", (int)PointLights.size());

	this->SetLightUniforms();

	sProgram.setUniform("texture_diffuse1", 0);


	sProgram.setUniform("matrices.projMatrix", glcore->getProjectionMatrix());
	sProgram.setUniform("matrices.viewMatrix", &cam.look());


	sProgram.setUniform("vEyePosition", cam.vEye);

	sProgram.setUniform("matrices.depthBiasMVP", mDepthBiasMVP);

	


	//Bind shadow map to somewhere safe 
	fboShadowMap.BindFramebufferTexture(50, false);

	sProgram.setUniform("shadowMap", 50);
	sProgram.setUniform("bShadowsOn", bShadowsOn);

	sProgram.setUniform("rotationTexture", 49);
	rotationTexture.bindTexture(49);
	sProgram.setUniform("scale", 1.0f / 128.0f);
	sProgram.setUniform("radius", (iShadowMapTextureSize / 2056.0f) / iShadowMapTextureSize);
	


	//Render objects
	for (unsigned int i = 0; i < Objects.size(); i++)
	{

		Objects[i].InitNewAssimp(&sProgram);
		fboShadowMap.BindFramebufferTexture(54, false);
		Objects[i].DrawNewAssimp(&sProgram);

	}


	for (unsigned int i = 0; i < Primitives.size(); i++)
	{

		Primitives[i].initRenderPrimitiveShape(&sProgram, dynamicsWorld);
		fboShadowMap.BindFramebufferTexture(54, false);
		Primitives[i].renderPrimitiveShape();
	}
	

	
	RayCast3DPicking(*glcore->getProjectionMatrix(),viewportrec);

	if (Keys::onekey(VKEY_N))
	{
		Objects[0].bumbMaps = 0;
	}
	if (Keys::onekey(VKEY_M))
	{
		Objects[0].bumbMaps = 1;
	}


	sProgram.setUniform("fresneluse", 1);
	if (Keys::onekey(VKEY_G))
	{
		if (ShowLogger)
		{
			ShowLogger = false;
		}
		else
		{
			ShowLogger = true;
		}

	}

	if (Keys::onekey(VKEY_F1))
	{
		if (enableAntiAliasing)
		{
			enableAntiAliasing = false;
		}
		else
		{
			enableAntiAliasing = true;
		}
	}


	

	if (Keys::key(VKEY_UP_ARROW))
	{

		sunPos.y = sunPos.y + 10.0f;
	}
	if (Keys::key(VKEY_DOWN_ARROW))
	{

		sunPos.y = sunPos.y - 10.0f;
	}
	if (Keys::key(VKEY_LEFT_ARROW))
	{

		sunPos.x = sunPos.x + 10.0f;
	}
	if (Keys::key(VKEY_LEFT_ARROW))
	{

		sunPos.x = sunPos.x - 10.0f;
	}
	if (Keys::key(VKEY_H))
	{
		cam.vEye = sunPos;
		cam.vView = -normalize(sunPos);
	}
	if (Keys::onekey(VKEY_ESCAPE))
	{
		PostQuitMessage(0);
	}

	if (Keys::onekey(VKEY_F))
	{
		if (bDisplayShadowMap)
		{
			bDisplayShadowMap = false;
		}
		else
		{
			bDisplayShadowMap = true;
		}
	}


	if (Keys::onekey(VKEY_F4))
	{
		OpenFileDialog* openFileDialog1 = new OpenFileDialog();

		if (openFileDialog1->ShowDialog())
		{
			
			std::wstring fn(openFileDialog1->FileName);
			std::string str = ws2s(fn);
			Logger::Log(str);
			char* file_name = new char[str.length() + 1];
			strcpy(file_name, str.c_str());
			if (m_pickedObject->GetObjectId() != "")
			{
				for (size_t i = 0; i < Objects.size(); i++)
				{
					if (m_pickedObject->GetObjectId() == Objects[i].GetObjectId())
					{
						Objects[i].SetModelObjectTexture(file_name);
					}
				}
			}
			if (m_pickedPrimitive->GetObjectId() != "")
			{
				for (size_t i = 0; i < Primitives.size(); i++)
				{
					if (m_pickedPrimitive->GetObjectId() == Primitives[i].GetObjectId())
					{
						Primitives[i].SetObjectTexture(file_name);
					}
				}
			}
		}
		delete openFileDialog1;
	}


	//Render skybox and use skybox shaders
	sSkyboxProgram.useProgram();
	sSkyboxProgram.setUniform("matrices.projMatrix", glcore->getProjectionMatrix());
	sSkyboxProgram.setUniform("matrices.viewMatrix", &cam.look());
	skybox.CenterSkybox(cam.vEye);
	skybox.renderSkybox(&sSkyboxProgram);

	cam.update();

	//Antialiasing
	if (enableAntiAliasing)
	{
		MultiSampler->PostProcessMultiSampling(viewportrec);
	}

	

	//Quad where shadow map is rendered for show
	if (!bDisplayShadowMap)
	{
		
		// Display shadow map
		spShadowMapRender.useProgram();
		glBindVertexArray(uiVAOShadowMapQuad);
		fboShadowMap.BindFramebufferTexture();
		glm::mat4 quadMat = glm::mat4(1.0f);
		spShadowMapRender.setUniform("mMat", &quadMat);
		spShadowMapRender.setUniform("projMat", glcore->getProjectionMatrix());
		spShadowMapRender.setUniform("samplerShadow", 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	this->RenderText(&glcore);

	

}

void Render::RayCast3DPicking(glm::mat4 projMat, RECT viewportrec)
{
	//3d picking test
	if (Keys::onekey(VKEY_LEFT_BUTTON))
	{
		glm::vec3 out_origin;
		glm::vec3 out_direction;
		POINT pos = MouseHandler::CurrentPosition();
		ScreenPosToWorldRay(
			pos.x, pos.y,
			viewportrec.right - viewportrec.left, viewportrec.bottom - viewportrec.top,
			cam.look(),
			projMat,
			out_origin,
			out_direction
			);

		glm::vec3 out_end = out_origin + out_direction*100000.0f;

		btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z));
		dynamicsWorld->rayTest(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z), RayCallback);
		if (RayCallback.hasHit()) {
			pickedObject = std::string((char*)RayCallback.m_collisionObject->getUserPointer());

			m_pickedObject = new Renderables();
			m_pickedPrimitive = new PrimitiveShape();

			//Choose object based on its id
			for (unsigned int i = 0; i < Primitives.size(); i++)
			{
				if (Primitives[i].GetObjectId() == pickedObject)
				{
					delete m_pickedObject;
					*m_pickedPrimitive = Primitives[i];
					pickedObject = Primitives[i].GetObjectId();
					pickedObject += "\n" + std::to_string((int)Primitives[i].GetLocation().x) + " " + std::to_string((int)Primitives[i].GetLocation().y) + " " + std::to_string((int)Primitives[i].GetLocation().z);
					pickedObject += "\n Angle: " + std::to_string(Primitives[i].getRotationAngle());
				}

			}

			for (unsigned int i = 0; i < Objects.size(); i++)
			{
				if (Objects[i].GetObjectId() == pickedObject)
				{
					delete m_pickedPrimitive;
					*m_pickedObject = Objects[i];
					pickedObject = Objects[i].GetObjectId();
					pickedObject += "\n" + std::to_string((int)Objects[i].GetLocation().x) + " " + std::to_string((int)Objects[i].GetLocation().y) + " " + std::to_string((int)Objects[i].GetLocation().z);
					pickedObject += "\n Angle: " + std::to_string(Objects[i].getRotationAngle());
				}
			}

		}
		else{
			pickedObject = "background";
		}
	}

}


int Render::RenderText(LPVOID lpParam)
{
	GLcore *glcore = (GLcore*)lpParam;
	RECT viewportrec;
	GetWindowRect(glcore->GetViewPort(), &viewportrec);
	//2D drawing.. todo: a class for these ples
	UpdateWindow(glcore->GetViewPort());
	GetWindowRect(glcore->GetViewPort(), &viewportrec);
	
 
	

	int vpheight = viewportrec.bottom - viewportrec.top;
	int vpwidth = viewportrec.right - viewportrec.left;
	glViewport(0, 0, vpwidth, vpheight);
	sFontProgram.useProgram();
	glDisable(GL_DEPTH_TEST);
	ftFont.SetShaderProgram(&sFontProgram);
	sFontProgram.setUniform("matrices.projMatrix", glcore->getOrthoMatrix());
	sFontProgram.setUniform("vColor", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));

	if (MouseHandler::MouseLeftClick())
	{
		msx = MouseHandler::CurrentPosition().x;
		msy = MouseHandler::CurrentPosition().y;
	}

	if (glcore->GetFps() > 0)
	{
		ftFont.Print(L"FPS&Frameinterval:  " + std::to_wstring((int)_fps) + L"&" + std::to_wstring((double)(1.0f / glcore->GetFps())) + L" 'F' Shadomap" + L" 'G' Show Log " +
			L"\nEnable/disable AntiAliasing: 'F1'", 5, vpheight - 20, 20);
		
	}
	
	ftFont.Print(s2ws(pickedObject),1500,500, 15);

	//If log texts are shown, show them for a time and fade away
	if (ShowLogger)
	{
		int textsize = 15;
		if (logtimecounter > 1500)
		{
			sFontProgram.setUniform("vColor", glm::vec4(0.9f, 0.9f, 0.9f, (1.0f - ((float)logtimecounter / 2500))));
			coolslider++;

		}
		for (unsigned int i = 0; i < logs1.GetLogListW().size()-10; i++)
		{
			ftFont.Print(logs1.GetLogListW()[i], 5, vpheight-60 - ((i * 15) + (coolslider * 2)), textsize);
		}
		logtimecounter++;

		if (logtimecounter > 2400)
		{
			ShowLogger = false;
			logtimecounter = 0;
			coolslider = 0;
		}

	}

	glEnable(GL_DEPTH_TEST);
	return 0;
}


void Render::DestroyRender()
{
	skybox.releaseSkybox();

	for (unsigned int i = 0; i < Objects.size(); i++)
	{
		Objects[i].DestroyObject();
	}
	for (unsigned int i = 0; i < Primitives.size(); i++)
	{
		Primitives[i].destroyPrimitiveShape();
	}
	fboShadowMap.DeleteFramebuffer();
	MultiSampler->DeleteFramebuffer();
	ftFont.DeleteFont();

	delete MultiSampler;
	delete dynamicsWorld;
	delete solver;
	delete collisionConfiguration;
}

Render::~Render()
{

}
