#pragma once
#include <glew.h>
#include <string>
#include "Shader.h"
#include "TTexture.h"
#include "vertexbufferobject.h"



class SkyBox
{
public:
	void loadSkybox(std::string a_sDirectory, std::string a_sFront, std::string a_sBack, std::string a_sLeft, std::string a_sRight, std::string a_sTop, std::string a_sBottom);
	void renderSkybox(CShaderProgram *sProgram);
	void CenterSkybox(glm::vec3 pos);
	void releaseSkybox();
	CTexture tTextures[6];
	void RotateSkybox(float fAngle, vec3 rotatedirection);
	
private:

	glm::vec3 Rotator = glm::vec3(1.0f,1.0f,1.0f);
	float fRangle = 0.0f;
	GLuint uiVAO;
	glm::vec3 sbloc;
	vertexbufferobject vboRenderData;
	std::vector<std::string> cubeMapTextures;
	std::string sDirectory;
	std::string sFront, sBack, sLeft, sRight, sTop, sBottom;
};

