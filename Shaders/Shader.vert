#version 410

uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;                                                                           
	mat4 normalMatrix;
	mat4 depthBiasMVP;
} matrices;

#include "DirectionalLight.frag"

uniform DirectionalLight sunLight;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;


uniform float changingBias;

uniform int bEnableBumpMap; 
out vec3 vLightDirTanSpace; 

out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;


smooth out float biasCalc;
smooth out vec3 vNormal;
smooth out vec2 vTexCoord;
smooth out vec3 vWorldPos;

smooth out vec3 normalMapNormal;

smooth out vec4 vEyeSpacePos;
smooth out vec4 ShadowCoord;

out vec3 cubeMapTexCoords;

void main()
{

 
  mat4 mMV = matrices.viewMatrix*matrices.modelMatrix;  
  mat4 mMVP = matrices.projMatrix*matrices.viewMatrix*matrices.modelMatrix;
 
  vTexCoord = inCoord;

  biasCalc = changingBias;
  vEyeSpacePos = mMV*vec4(inPosition, 1.0);
  gl_Position = mMVP*vec4(inPosition, 1.0);



  vNormal = (matrices.normalMatrix*vec4(inNormal, 1.0)).xyz;
  vWorldPos = (matrices.modelMatrix*vec4(inPosition, 1.0)).xyz;
  
   
  cubeMapTexCoords = inPosition;

  ShadowCoord = matrices.depthBiasMVP  * vec4(vWorldPos, 1.0);




    if(bEnableBumpMap == 1)
	{
		vec3 vTangent = inTangent;
        vec3 vBitangent = inBitangent;
		
		mat3 mTBN = transpose(mat3(vTangent, vBitangent, vNormal));
		vLightDirTanSpace = normalize(mTBN * sunLight.vDirection).xyz;
	}
	else vLightDirTanSpace = vec3(0, 0, 0);
   
}