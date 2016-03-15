#include "stdafx.h"
#include "Sun.h"


Light::Light()
{
	this->pointlight = PLightProperties();
}


void Light::AnimateSun()
{
	float fSine = sin(fSunAngle*3.1415 / 180.0);
	glm::vec3 vSunPos(cos(fSunAngle*3.1415 / 180.0) * 7000, sin(fSunAngle*3.1415 / 180.0) * 7000, 20.0);
}

void Light::CreateSun(glm::vec3 sunlocation)
{
	this->sunPosition = sunlocation;
}

void Light::CreatePointLight(glm::vec3 location)
{
	this->pointlight.Position = location;
}

Light::~Light()
{
}
