#pragma once

#ifndef RENDERABLE_H
#define RENDERABLE_H
#include <glew.h>
#include "vertexbufferobject.h"
#include "Shader.h"
#include "TTexture.h"
#include "AssimpModel.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "AssimpModelI.h"



class Renderables
{
public:

	
	Renderables();
	void ReleaseTextureData();
	void TestImprovedAssimp(char* path);

	void InitializeObject(int objectType, float Tiling);
	void InitializeObject(char* path, char* bumpMapFilepath, char* matfilepath);

	void SetObjectLocation(glm::vec3 loc);
	void SetObjectTexture(char *TexPath);
	void SetModelObjectTexture(char *TexPath);
	void SetObjectBumpMapTexture(char *TexPath);
	void SetObjectCubeMapTexture(std::vector<std::string> texPaths);
	void SetObjectColor(glm::vec3 col);
	void SkyObject(int val);
	void MakeGlow();
	void rotateObject(float angle, glm::vec3 axis);

	void SetPhysics(btRigidBody* rgbody, int objectindex);
	static void FinalizeVBOS(); // Obsolete with new assimploader

	void Selected();

	void InitNewAssimp(CShaderProgram *sProgram);
	void DrawNewAssimp(CShaderProgram *sProgram);

	Model *model;

	btRigidBody *getRigidbody();
	void SetObjectId(char* oid);
	char* GetObjectId();
	void SetMaterialPath();
	void SetObjectSpecular(float shinines);
	void SetObjectSize(glm::vec3 size);
	void SetObjectTextureFromData(BYTE* data, int32 a_iWidth, int32 a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps);
	void RenderObjectInit(CShaderProgram *sProgram, btDynamicsWorld *dyn);
	void RenderObjectGL();
	void DestroyObject();
	void SetModelMatrix(glm::mat4 mModelMat);
	glm::mat4 GetModelMatrix();
	~Renderables();
	int HasTexture = 0;
	CAssimpModel ObjectModel;
	glm::vec3 GetLocation();

	glm::vec3 GetScale();
	bool IsTransParent = false;
	void SetRoughness(float froughness);
	bool isInShadowMap = true;
	bool usesphysics = true;
	float rotateAngle = 0.0f;
	float getRotationAngle();

	void setBumpMaponOff(bool onoff);

	void SetObjectMaterialProperties(vec3 ambient, vec3 diffuse, vec3 specular, float shininess, int ismetal);

private:
	int objectindex_;
	float TextureTiling;
	void RenderForThread();
	
protected:
	btRigidBody* rigidbodys;

	bool hasCubeMap = false;

	glm::mat4 mModelMat;
	GLuint VertexArrayID;
	vertexbufferobject vbo;
	vertexbufferobject vbobumb;
	int32 objType = 0;
	
	
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	

	glm::vec3 location = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotateAxis = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 scaleSize = glm::vec3(1.0f, 1.0f, 1.0f);
	btDefaultMotionState* motionstate;
	btCollisionShape* boxCollisionShape;
	
	char* objectId = "n/a";
	
	struct TexturePaths
	{
		char *TexturePath;
		char *BumpMapTexturePath;
		char *CubeMapTexturePath;

	};

	CTexture texture;
	CTexture bumpMapTexture;
	CTexture CubeMapTexture;

	//Material properties in struct for cleaner code etc
	struct MaterialProperties
	{
		glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 specProto = glm::vec3(1.0f, 1.0f, 1.0f);

		float Shininess = 0.0005f;
		
		float specular = 0.0005f; 
		float specI = 0.04f;
		float Roughness = 100.0f;
		int isMetallic = 0;
	};
	
	void SetMaterialUniformData(CShaderProgram *sProgram);

public:
	MaterialProperties matProp;
	TexturePaths texturepaths;
	int bumbMaps = 0;
	int cubeMaps = 0;
private:
	
	bool glowingtex = false;
	bool PartOfSky;
	

};

#endif