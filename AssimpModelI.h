#pragma once

#ifndef ASSIMP_I_H
#define ASSIMP_I_H
#include <glew.h>
#include <wglew.h>
// Std. Includes
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "TTexture.h"
#include "Mesh.h"

class Model
{
public:
	Model(GLchar* path);
	void Draw(CShaderProgram shader);
	bool HasTexture;

private:

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	CTexture LoadTexturesForAssimp(const char* pathforfile, std::string directorys);
	/*  Model Data  */
	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	void loadModel(std::string path);
};

#endif