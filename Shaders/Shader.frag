#version 410

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec4 vEyeSpacePos;
smooth in vec3 vWorldPos;



in vec3 normalMapNormal;
in vec3 cubeMapTexCoords;

float when_gt(float x, float y);

in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 outputColor;
smooth in float biasCalc;

uniform samplerCube cMap;
uniform sampler2D gSampler;
uniform sampler2D gNormalMap; 
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform vec4 vColor;


uniform int NumPLights;
uniform int bEnableBumpMap; 
uniform int usesCubemap;
uniform int hasTex;

#include "DirectionalLight.frag"

#define MAX_POINT_LIGHTS 3

uniform DirectionalLight sunLight;
uniform PointLight pLight[MAX_POINT_LIGHTS];

uniform vec3 vEyePosition;

in vec3 vLightDirTanSpace;



uniform Material matActive;

#include "shadows.frag"

uniform sampler2D shadowMap;
smooth in vec4 ShadowCoord;

vec4 vTexDiffuseColor;
vec4 vMixedColor;
vec4 TextureSpecular;

vec4 calculatePointLight(PointLight plight, vec3 vEyePos, Material mat, vec3 vWorldPos,
	 vec3 vNormal);


float when_gt(float x, float y){
	return max(sign(x-y),0.0);
}


void main()
{

	
	vec4 ambientLight = vec4(sunLight.fAmbient,sunLight.fAmbient,sunLight.fAmbient,1.0);
	if(usesCubemap == 1)
	{
		vTexDiffuseColor = texture(cMap, cubeMapTexCoords);
		vMixedColor = vTexDiffuseColor * vec4(matActive.diffuse,1.0);
	}
	if(hasTex == 1)
	{
		vTexDiffuseColor = texture(texture_diffuse1, vTexCoord); 
		TextureSpecular = texture(texture_specular1,vTexCoord);
	}
	else
	{
		vTexDiffuseColor = vColor;
	}
	
	

    vec3 vNormalized = normalize(vNormal);

	DirectionalLight sunlight = sunLight;
	

  float biasc = 0.0029;
  vec3 l = normalize(sunLight.vDirection);
  vec3 vVeye = normalize(vEyePosition-vWorldPos);
  float cosTheta = clamp(dot(vNormalized,l),0,1);

  biasc = 0.002*tan(acos(cosTheta));
  biasc = clamp(biasc,0,0.01);

  vec3 HalfWayDir = normalize(sunLight.vDirection + vVeye);

  float visibility = GetVisibility(shadowMap, ShadowCoord, biasc);


  float fspecularInt = 128.0; 
  if(bEnableBumpMap == 0)
	{
		vec4 vDiffuseColor = GetDirectionalLightColor(sunLight,matActive, vNormalized,visibility);
		vec4 vSpecularColor = GetSpecularColor(vWorldPos, vEyePosition, matActive, sunLight, vNormalized,visibility,fspecularInt);
		vec4 FinalAmbient = ambientLight * vTexDiffuseColor;
		vec4 FinalDiffuse = vDiffuseColor * vTexDiffuseColor;
		vec4 FinalSpecular = vSpecularColor * TextureSpecular;
		vec4 colors;
		vec4 dirlightcol = (FinalAmbient + FinalDiffuse + FinalSpecular);
		colors = dirlightcol;
		vec4 plightColor;
		for(int i = 0; i < 3; i++)
			colors += calculatePointLight(pLight[i], vEyePosition.xyz, matActive,vWorldPos, vNormalized);
		outputColor = colors;
		
	}
	else
	{
		sunlight.vDirection = -vLightDirTanSpace;
		vec3 vNormalExtr = normalize(texture(gNormalMap, vTexCoord).rgb*2.0 - 1.0);
		float fDiffuseIntensity = max(0.0, dot(vNormalExtr, -vLightDirTanSpace));
		vec3 DiffuseColor = ((sunlight.vColor + matActive.ambient) * (fDiffuseIntensity * matActive.diffuse)) * visibility;
		vec4 vSpecularColor = GetSpecularColor(vWorldPos, vEyePosition, matActive, sunLight, vNormalized, visibility, fspecularInt);
		
		vec4 FinalAmbient = ambientLight * vTexDiffuseColor;
		vec4 FinalDiffuse = vec4(DiffuseColor,1.0) * vTexDiffuseColor;
		vec4 FinalSpecular = vSpecularColor * TextureSpecular;
		vec4 dirlightcol = (FinalAmbient + FinalDiffuse + FinalSpecular);
		vec4 colors;
		colors = dirlightcol;
		vec4 plightColor;
		for(int i = 0; i < 3; i++)
			colors += calculatePointLight(pLight[i], vEyePosition.xyz, matActive,vWorldPos, vNormalized);
		
		outputColor = colors;
	 }

}

vec4 calculatePointLight(PointLight plight, vec3 vEyePos, Material mat,vec3 vWorldPos,
		 vec3 vNormal)
{
	if(hasTex == 1)
	{
		vTexDiffuseColor = texture(texture_diffuse1, vTexCoord); 
		TextureSpecular = texture(texture_specular1,vTexCoord);
	}
	else
	{
		vTexDiffuseColor = vColor;
	}
	
	vec4 ambient = vec4(plight.ambient,1.0);

	//diffuselight
	vec3 norm = normalize(vNormal);
	vec3 lightDir = normalize(plight.position - vWorldPos);
	float DiffuseP = max(dot(norm, lightDir),0.0);


	vec4 diffuse = vec4(plight.diffuse,1.0) * DiffuseP * vTexDiffuseColor;

	//specular
	vec3 viewDir = normalize(vEyePos - vWorldPos);
	vec3 HalfVector = normalize(viewDir + lightDir);



	float fSpecularFactor =  pow(max(dot(norm, HalfVector),0.0),mat.Shininess) * DiffuseP;



	vec4 specular = vec4(plight.specular,1.0) * fSpecularFactor * TextureSpecular;

	float distance = length(plight.position - vWorldPos);
	
	float VdotN = max(dot(viewDir, norm), 0.0);
	float fr = CalculateFresnel(viewDir,HalfVector);
    float Fresnel = pow(1.0-VdotN,fr);


	float attenuation = 10.0f / (plight.constant + plight.linear * distance + plight.quadratic * (distance * distance));    

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec4 plightColor = ambient + diffuse + (specular * Fresnel);
	//if(dot(norm, lightDir) < 0.0)
	//{
		//plightColor = vec4(0.0, 0.0, 0.0, 0.0);
	//}

	return plightColor;
}