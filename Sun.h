#pragma once

#ifndef SUN_H
#define SUN_H

#include "stdafx.h"
#include <glm.hpp>
class Light
{
public:
	Light();
	~Light();
	void CreateSun(glm::vec3 sunlocation);

	void AnimateSun();

	void CreatePointLight(glm::vec3 location);
	
	
private:
	struct DLightProperties
	{
		glm::vec3 Specular = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 Ambient = glm::vec3(1.0f, 1.0f, 1.0f);

	};

	struct PLightProperties
	{
		glm::vec3 Position = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 Specular = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 Ambient = glm::vec3(0.2f, 0.2f, 0.2f);

		float constant = 1.0f;
		float linear = 0.027f;
		float quadratic = 0.0028f;

	};

	
	float fSunAngle = 45.0f;
	glm::vec3 shineDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 sunPosition = glm::vec3(1.0f, 1.0f, 1.0f);

public:
	PLightProperties pointlight;
};

#endif