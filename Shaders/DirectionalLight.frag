#version 410

#include_part

struct DirectionalLight
{
	vec3 vColor;
	vec3 vDirection;

	float fAmbient;
  int iSkybox;
};
uniform float fZero;
uniform int fresneluse;
uniform float roughness;

struct Material
{
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float Shininess;

   float fSpecularIntensity;
   float fSpecularPower;
   int isMetallic;
};


struct PointLight
{
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   vec3 position;

   float constant;
   float linear;
   float quadratic;
};


vec4 GetDirectionalLightColor(DirectionalLight dirLight,Material mat, vec3 vNormal, float Shadow);

vec4 GetSpecularColor(vec3 vWorldPos, vec3 vEyePos, Material mat, DirectionalLight dLight, vec3 vNormal, float Shadow, float tInten);

float CalculateFresnel(vec3 vVtoEye, vec3 vNormal);
float beckmannDistribution(float x, float roughness);

#definition_part

float fDiffuseIntensity;

vec4 GetDirectionalLightColor(DirectionalLight dirLight,Material mat,  vec3 vNormal, float Shadow)
{
	fDiffuseIntensity = max(0.0, dot(vNormal, -dirLight.vDirection));
	vec3 DiffuseColor = (dirLight.vColor + mat.ambient) * (fDiffuseIntensity * mat.diffuse) * Shadow;

	return vec4(DiffuseColor, 1.0);
	
}

float CalculateFresnel(vec3 vVtoEye, vec3 vNormal)
{
 

   float base = 1 - dot(vVtoEye,normalize(vNormal));
   float exp = pow(base,5.0);
   float fresnel = exp * fZero *(1.0-exp);

   return fresnel;
}

float beckmannDistribution(float ndoTh, float roughness1) 
{
  float NdotH = max(ndoTh, 0.0001);
  float cos2Alpha = NdotH * NdotH;
  float tan2Alpha = (cos2Alpha - 1.0) / cos2Alpha;
  float roughness2 = roughness1 * roughness1;
  float denom = 3.141592653589793 * roughness2 * cos2Alpha * cos2Alpha;

  return exp(tan2Alpha / roughness2) / denom;
}

vec4 GetSpecularColor(vec3 vWorldPos, 
	vec3 vEyePos, Material mat, DirectionalLight dLight, vec3 vNormal, float Shadow, float tInten)
{
   vec4 vResult = vec4(0.0, 0.0, 0.0, 0.0);
   vec3 lightDirection = normalize(dLight.vDirection);

   vec3 vReflectedVector = reflect(-lightDirection, vNormal);
   vec3 vVertexToEyeVector = normalize(vEyePos-vWorldPos);
  
   vec3 lightDirFrag = normalize(lightDirection - vWorldPos);
   vec3 EyeToVertex = normalize(vEyePos - vWorldPos);

   vec3 H = normalize(EyeToVertex + lightDirFrag);

   float VdotN = max(dot(EyeToVertex, vNormal), 0.0);
   float LdotN = max(dot(lightDirFrag, vNormal), 0.0);

   //Geometric term just testing.. doesnt work correctly atm
   float NdotH = max(dot(vNormal, H), 0.0);
   float VdotH = max(dot(EyeToVertex, H), 0.000001);
   float LdotH = max(dot(lightDirFrag, H), 0.000001);
   float G1 = (2.0 * NdotH * VdotN) / VdotH;
   float G2 = (2.0 * NdotH * LdotN) / LdotH;
   float G = min(1.0, min(G1, G2));
   float fRoughness = roughness;


   
   float fSpecularFactor =  pow(max(dot(vNormal, H),0.0),mat.Shininess) * fDiffuseIntensity;

   float D = beckmannDistribution(NdotH, fRoughness);

   float fr = CalculateFresnel(EyeToVertex,H);
   float Fresnel = pow(1.0-VdotN,fr);
   float GFD;
 
   GFD  =  G * Fresnel * D / max(3.14159265 * VdotN, 0.000001);
  
 
 
   //Final result. Specular light should be 0 if there's something in front of reflection but still does not seem to work
   vResult =  ((vec4(dLight.vColor, 1.0)  *  (fSpecularFactor*vec4(mat.specular,1.0))) * Fresnel)  * Shadow;

   if(dot(vNormal, lightDirFrag) < 0.0)
   {
	   vResult = vec4(0.0, 0.0, 0.0, 0.0);
   }
   
   return vResult;
}

