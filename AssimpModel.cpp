#include "stdafx.h"
#include <future>
#include "AssimpModel.h"

#include "Usefulheaders.h"

#pragma comment(lib, "assimp.lib")

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


vertexbufferobject CAssimpModel::vboModelData;
vertexbufferobject CAssimpModel::vboBumpMapData;
UINT CAssimpModel::uiVAO;
vector<CTexture> CAssimpModel::tTextures;
vector<CTexture> CAssimpModel::tNormalMap;
/*-----------------------------------------------

Name:	GetDirectoryPath

Params:	sFilePath - guess ^^

Result: Returns directory name only from filepath.

/*---------------------------------------------*/

std::string GetDirectoryPath(std::string sFilePath)
{
	// Get directory path
	std::string sDirectory = "";
	RFOR(i, ESZ(sFilePath) - 1)if (sFilePath[i] == '\\' || sFilePath[i] == '/')
	{
		sDirectory = sFilePath.substr(0, i + 1);
		break;
	}
	return sDirectory;
}

CAssimpModel::CAssimpModel()
{
	bLoaded = false;
}

inline glm::vec3 aiToGlm(aiVector3D v)
{
	return glm::vec3(v.x, v.y, v.z);
}

bool CAssimpModel::LoadModelFromFileWithNormalMapping(char* sFilePath, char* sTexturePath, char* sNormalTexturePath)
{
	sTexturePath == NULL;
	sNormalTexturePath == NULL;
	if (vboModelData.getBuffer() == 0)
	{
		vboModelData.createVBO();
		vboBumpMapData.createVBO();
		tTextures.reserve(50);
	}
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(sFilePath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FlipUVs |
		aiProcess_SortByPType);

	if (!scene)
	{
		Logger::Log("Cannot import asset " + std::string(sFilePath));
		return false;
	}

	const int iVertexTotalSize = sizeof(aiVector3D)* 2 + sizeof(aiVector2D);

	int iTotalVertices = 0;

	FOR(i, scene->mNumMeshes)
	{
		aiMesh* mesh = scene->mMeshes[i];
		int iMeshFaces = mesh->mNumFaces;
		iMaterialIndices.push_back(mesh->mMaterialIndex);
		int iSizeBefore = vboModelData.GetCurrentSize();
		iMeshStartIndices.push_back(iSizeBefore / iVertexTotalSize);
		FOR(j, iMeshFaces)
		{
			const aiFace& face = mesh->mFaces[j];

			glm::vec3 tangent;
			glm::vec3 bitangent;

			if (hasnormalmap)
			{
				glm::vec3 & v0 = aiToGlm(mesh->mVertices[face.mIndices[0]]);
				glm::vec3 & v1 = aiToGlm(mesh->mVertices[face.mIndices[1]]);
				glm::vec3 & v2 = aiToGlm(mesh->mVertices[face.mIndices[2]]);

				// Shortcuts for UVs
				glm::vec3 & uv0 = aiToGlm(mesh->mTextureCoords[0][face.mIndices[0]]);
				glm::vec3 & uv1 = aiToGlm(mesh->mTextureCoords[0][face.mIndices[1]]);
				glm::vec3 & uv2 = aiToGlm(mesh->mTextureCoords[0][face.mIndices[2]]);

				// Edges of the triangle : position delta
				glm::vec3 vPosDif1 = v1 - v0;
				glm::vec3 vPosDif2 = v2 - v0;

				// UV delta
				glm::vec3 vTexCoordDif1 = uv1 - uv0;
				glm::vec3 vTexCoordDif2 = uv2 - uv0;

				float r = 1.0f / (vTexCoordDif1.x * vTexCoordDif2.y - vTexCoordDif1.y * vTexCoordDif2.x);
				tangent = glm::normalize((vPosDif1 * vTexCoordDif2.y - vPosDif2 * vTexCoordDif1.y)*r);
				bitangent = glm::normalize((vPosDif2 * vTexCoordDif1.x - vPosDif1 * vTexCoordDif2.x)*r);
			}

			FOR(k, 3)
			{
				aiVector3D pos = mesh->mVertices[face.mIndices[k]];
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
				vboModelData.addData(&pos, sizeof(aiVector3D));
				vboModelData.addData(&uv, sizeof(aiVector2D));
				vboModelData.addData(&normal, sizeof(aiVector3D));
				if (hasnormalmap)
				{
					vboBumpMapData.addData(&tangent, sizeof(glm::vec3));
					vboBumpMapData.addData(&bitangent, sizeof(glm::vec3));
				}
			}
		}
		int iMeshVertices = mesh->mNumVertices;
		iTotalVertices += iMeshVertices;
		iMeshSizes.push_back((vboModelData.GetCurrentSize() - iSizeBefore) / iVertexTotalSize);
	}
	iNumMaterials = scene->mNumMaterials;

	vector<int> materialRemap(iNumMaterials);


	FOR(i, iNumMaterials)
	{
		const aiMaterial* material = scene->mMaterials[i];
		int a = 5;
		int texIndex = 0;
		aiString path;  // filename

		string sDir = GetDirectoryPath(sFilePath);
		string sFullPath;
		bool normalok = false;
		if (strlen(sTexturePath) < 4 || strlen(sNormalTexturePath) < 4)
		{
			bool normalok = false;
			bool ok = material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS;
			if (!ok)ok = material->GetTexture(aiTextureType_AMBIENT, texIndex, &path) == AI_SUCCESS;
			if (!ok)ok = material->GetTexture(aiTextureType_UNKNOWN, texIndex, &path) == AI_SUCCESS;
			if (!ok)ok = material->GetTexture(aiTextureType_EMISSIVE, texIndex, &path) == AI_SUCCESS;
			//if (!ok)ok = material->GetTexture(aiTextureType_HEIGHT, texIndex, &path) == AI_SUCCESS; normalok = ok;
			if (!ok)
			{
				string sPutToPath = sFilePath;
				sPutToPath = sPutToPath.substr(sPutToPath.find_last_of("/\\") + 1);
				sPutToPath = sPutToPath.substr(0, sPutToPath.find_last_of("."));
				sPutToPath += ".jpg";
				path = sPutToPath.c_str();
			}
			string sTextureName = path.data;
			sFullPath = sDir + sTextureName;
			if (ok)
			{
				CTexture tNew;
				tNew.loadTexture2D(sFullPath, true);
				materialRemap[i] = ESZ(tTextures);
				tNew.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
				tTextures.push_back(tNew);
			}
	
		}
		else sFullPath = sDir + string(sTexturePath);
		int iTexFound = -1;
		FOR(j, ESZ(tTextures))if (sFullPath == tTextures[j].GetPath())
		{
			iTexFound = j;
			break;
		}
		if (iTexFound != -1)materialRemap[i] = iTexFound;
		else
		{
			
			CTexture tNew;
			tNew.loadTexture2D(sFullPath, true);
			materialRemap[i] = ESZ(tTextures);
			tNew.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
			tTextures.push_back(tNew);
		}
		
	}
	FOR(i, ESZ(iMeshSizes))
	{
		int iOldIndex = iMaterialIndices[i];
		iMaterialIndices[i] = materialRemap[iOldIndex];
	}
	int textureCountThisModel = tTextures.size();
	

	FOR(i, iNumMaterials)
	{
		const aiMaterial* material = scene->mMaterials[i];
		int a = 5;
		int texIndex = 0;
		aiString path;  // filename
		bool ok = material->GetTexture(aiTextureType_HEIGHT, texIndex, &path) == AI_SUCCESS;
		if (ok)
		{
			string sDir = GetDirectoryPath(sFilePath);
			string sTextureName = path.data;
			string sFullPath = sDir + sTextureName;

			CTexture tNew;
			tNew.loadTexture2D(sFullPath, true);
			tNew.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST);
			tNormalMap.push_back(tNew);
		}
		else
		{
			continue;
		}
	}

	

	return bLoaded = true;
}

/*-----------------------------------------------

Name:	LoadModelFromFile

Params:	sFilePath - guess ^^

Result: Loads model using Assimp library.

/*---------------------------------------------*/

bool CAssimpModel::LoadModelFromFile(char* sFilePath, char* sTexturePath, char* sNormalTexturePath)
{
	if (vboModelData.getBuffer() == 0)
	{
		vboModelData.createVBO();
		tTextures.reserve(50);
	}
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(sFilePath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene)
	{
		Logger::Log("Could not load model " + std::string(sFilePath));
		return false;
	}

	const int iVertexTotalSize = sizeof(aiVector3D)* 2 + sizeof(aiVector2D);

	int iTotalVertices = 0;

	FOR(i, scene->mNumMeshes)
	{
		aiMesh* mesh = scene->mMeshes[i];
		int iMeshFaces = mesh->mNumFaces;
		iMaterialIndices.push_back(mesh->mMaterialIndex);
		int iSizeBefore = vboModelData.GetCurrentSize();
		iMeshStartIndices.push_back(iSizeBefore / iVertexTotalSize);
		FOR(j, iMeshFaces)
		{
			const aiFace& face = mesh->mFaces[j];
			FOR(k, 3)
			{
				aiVector3D pos = mesh->mVertices[face.mIndices[k]];
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
				vboModelData.addData(&pos, sizeof(aiVector3D));
				vboModelData.addData(&uv, sizeof(aiVector2D));
				vboModelData.addData(&normal, sizeof(aiVector3D));
			}
		}
		int iMeshVertices = mesh->mNumVertices;
		iTotalVertices += iMeshVertices;
		iMeshSizes.push_back((vboModelData.GetCurrentSize() - iSizeBefore) / iVertexTotalSize);
	}
	iNumMaterials = scene->mNumMaterials;

	vector<int> materialRemap(iNumMaterials);

	FOR(i, iNumMaterials)
	{
		const aiMaterial* material = scene->mMaterials[i];
		int a = 5;
		int texIndex = 0;
		aiString path;  // filename
		bool ok = material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS;
		if (!ok)ok = material->GetTexture(aiTextureType_AMBIENT, texIndex, &path) == AI_SUCCESS;
		if (!ok)ok = material->GetTexture(aiTextureType_UNKNOWN, texIndex, &path) == AI_SUCCESS;
		if (!ok)ok = material->GetTexture(aiTextureType_EMISSIVE, texIndex, &path) == AI_SUCCESS;
		if (!ok)
		{
			string sPutToPath = sFilePath;
			sPutToPath = sPutToPath.substr(sPutToPath.find_last_of("/\\") + 1);
			sPutToPath = sPutToPath.substr(0, sPutToPath.find_last_of("."));
			sPutToPath += ".jpg";
			path = sPutToPath.c_str();
		}

		string sDir = GetDirectoryPath(sFilePath);
		string sTextureName = path.data;
		string sFullPath = sDir + sTextureName;
		int iTexFound = -1;
		FOR(j, ESZ(tTextures))if (sFullPath == tTextures[j].GetPath())
		{
			iTexFound = j;
			break;
		}
		if (iTexFound != -1)materialRemap[i] = iTexFound;
		else
		{
			CTexture tNew;
			tNew.loadTexture2D(sFullPath, true);
			materialRemap[i] = ESZ(tTextures);
			tTextures.push_back(tNew);
		}
	}

	FOR(i, ESZ(iMeshSizes))
	{
		int iOldIndex = iMaterialIndices[i];
		iMaterialIndices[i] = materialRemap[iOldIndex];
	}

	if (tTextures.size() >= 1)
	{
		this->hasMaterials = true;
	}

	return bLoaded = true;
}


/*-----------------------------------------------

Name:	FinalizeVBO

Params: none

Result: Uploads all loaded model data in one global
models' VBO.

/*---------------------------------------------*/


int CAssimpModel::GetModelTex()
{
	return tTextures[0].GetTextureID();
}


void CAssimpModel::FinalizeVBO()
{
	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);
	vboModelData.bindVBO();
	vboModelData.uploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D)+sizeof(aiVector2D), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D)+sizeof(aiVector2D), (void*)sizeof(aiVector3D));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D)+sizeof(aiVector2D), (void*)(sizeof(aiVector3D)+sizeof(aiVector2D)));

	if (vboBumpMapData.GetCurrentSize() > 1)
	{
		vboBumpMapData.bindVBO();
		vboBumpMapData.uploadDataToGPU(GL_STATIC_DRAW);

		// Tangent vector
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D), 0);
		// Bitangent vector
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D), (void*)(sizeof(aiVector3D)));
	}

}

/*-----------------------------------------------

Name:	BindModelsVAO

Params: none

Result: Binds VAO of models with their VBO.

/*---------------------------------------------*/

void CAssimpModel::BindModelsVAO()
{
	glBindVertexArray(uiVAO);
}
void CAssimpModel::UnBindModelsVAO()
{
	glBindVertexArray(0);
}

/*-----------------------------------------------

Name:	RenderModel

Params: none

Result: Guess what it does ^^.

/*---------------------------------------------*/

void CAssimpModel::BeginRender(CShaderProgram sProgram)
{
	
}

void CAssimpModel::RenderWithNormalMaps(GLenum RenderMode)
{
	
	if (!bLoaded)return;
	int iNumMeshes = ESZ(iMeshSizes);
	FOR(i, iNumMeshes)
	{
		int mapSizeDif = tTextures.size() - tNormalMap.size();
		int iMatIndex = iMaterialIndices[i];
		tTextures[iMatIndex].textureHint = GL_TEXTURE_2D;
		tTextures[iMatIndex].bindTexture(0);
		
		//tNormalMap[iMatIndex - mapSizeDif].textureHint = GL_TEXTURE_2D;
		//tNormalMap[iMatIndex - mapSizeDif].bindTexture(1);
		


		glDrawArrays(RenderMode, iMeshStartIndices[i], iMeshSizes[i]);
	}
}

void CAssimpModel::RenderModel(GLenum RenderMode)
{

	if (!bLoaded)return;
	int iNumMeshes = ESZ(iMeshSizes);
	FOR(i, iNumMeshes)
	{
		int mapSizeDif = tTextures.size() - tNormalMap.size();
		int iMatIndex = iMaterialIndices[i];
		tTextures[iMatIndex].textureHint = GL_TEXTURE_2D;
		tTextures[iMatIndex].bindTexture(0);
		//tNormalMap[iMatIndex - mapSizeDif].textureHint = GL_TEXTURE_2D;
		//tNormalMap[iMatIndex - mapSizeDif].bindTexture(1);

		glDrawArrays(RenderMode, iMeshStartIndices[i], iMeshSizes[i]);
	}
}

/*
if (!bLoaded)return;
	int iNumMeshes = ESZ(iMeshSizes);
	FOR(i, iNumMeshes)
	{
		int mapSizeDif = tTextures.size() - tNormalMap.size();
		int iMatIndex = iMaterialIndices[i];
		tTextures[iMatIndex].textureHint = GL_TEXTURE_2D;
		tTextures[iMatIndex].bindTexture();
		glDrawArrays(RenderMode, iMeshStartIndices[i], iMeshSizes[i]);
	}
*/

/*-----------------------------------------------

Name:	ReleaseAllModelData

Params: none

Result: Releases all vertices data, VAO and VBO of
all Assimp models.

/*---------------------------------------------*/

void CAssimpModel::ReleaseAllModelData()
{
	vboModelData.releaseVBO();
	glDeleteVertexArrays(1, &uiVAO);
	FOR(i, ESZ(tTextures))tTextures[i].releaseTexture();
}