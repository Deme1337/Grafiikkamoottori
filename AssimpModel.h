#pragma once


#ifndef ASSIMP_MOD
#define ASSIMP_MOD
#include "Shader.h"
#include "vertexbufferobject.h"
#include "TTexture.h"




class CAssimpModel
{
public:
	bool LoadModelFromFile(char* sFilePath, char* sTexturePath, char* sNormalTexturePath);
	bool LoadModelFromFileWithNormalMapping(char* sFilePath, char* sTexturePath, char* sNormalTexturePath);
	bool LoadModelFromFileWithMaterials(char* sFilePath);
	static void FinalizeVBO();
	static void BindModelsVAO();
	static void ReleaseAllModelData();
	static void UnBindModelsVAO();
	void RenderModel(GLenum = GL_TRIANGLES);
	void BeginRender(CShaderProgram sProgram);
	int GetModelTex();
	bool hasMaterials = false;
	std::string MaterialFilename;

	void RenderWithNormalMaps(GLenum = GL_TRIANGLES);
	bool hasnormalmap = false;

	CAssimpModel();
private:
	bool bLoaded;
	static vertexbufferobject vboModelData;
	static vertexbufferobject vboBumpMapData;
	static UINT uiVAO;
	static vector<CTexture> tTextures;
	static vector<CTexture> tNormalMap;
	vector<int> iMeshStartIndices;
	vector<int> iMeshSizes;
	vector<int> iMaterialIndices;

	vector<int> iMaterialNormalIndices;
	
	int iNumMaterials;
	int iNumNormalMaterials;
};

#endif