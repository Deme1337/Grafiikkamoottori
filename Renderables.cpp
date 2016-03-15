#include "stdafx.h"
#include "AssimpModelI.h"
#include "BulletCollision\CollisionShapes\btConvexHullShape.h"
#include "BulletCollision\CollisionShapes\btConvexShape.h"
#include "Renderables.h"
#include <thread>
#include <glm.hpp>





Renderables::Renderables()
{
}

void Renderables::SetObjectLocation(glm::vec3 loc)
{
	location = loc;
}

void Renderables::SetObjectColor(glm::vec3 col)
{
	matProp.color = col;
}

void Renderables::SetObjectSize(glm::vec3 Size)
{
	scaleSize = Size;
}
void Renderables::rotateObject(float angle, glm::vec3 rotAxis)
{
	rotateAngle = angle;
	rotateAxis = rotAxis;
}
void Renderables::SetObjectMaterialProperties(vec3 ambient, vec3 diffuse, vec3 specular,
	float shininess, int ismetal)
{
	matProp.ambient = glm::vec3(1.0f) * ambient;
	matProp.diffuse = glm::vec3(1.0f) *diffuse;
	matProp.specProto = glm::vec3(1.0f) *specular;
	matProp.Shininess = shininess;
	matProp.isMetallic = ismetal;
}

void Renderables::SetObjectTexture(char *TexPath)
{
	this->texture.releaseTexture();
	texturepaths.TexturePath = TexPath;
	texture.loadTexture2D(texturepaths.TexturePath, true);
	texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	HasTexture = 1;
}

void Renderables::SetModelObjectTexture(char *TexPath)
{
	texturepaths.TexturePath = TexPath;
	texture.loadTexture2D(texturepaths.TexturePath, true);
	texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	HasTexture = 1;
}
void Renderables::SetObjectBumpMapTexture(char *TexPath)
{
	texturepaths.BumpMapTexturePath = TexPath;
	bumpMapTexture.loadTexture2D(texturepaths.BumpMapTexturePath, true);
	bumpMapTexture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	bumbMaps = 1;
}

void Renderables::SetObjectCubeMapTexture(std::vector<std::string> texPaths)
{
	CubeMapTexture.textureHint = GL_TEXTURE_CUBE_MAP;
	CubeMapTexture.LoadCubeMap(texPaths);
	hasCubeMap = true;
}

void Renderables::SetObjectTextureFromData(BYTE* data, int32 a_iWidth, int32 a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps)
{
	texture.createFromData(data, a_iWidth, a_iHeight, 0, GL_BGR, bGenerateMipMaps);
	texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	HasTexture = 1;
}

void Renderables::SkyObject(int val)
{
	if (val == 1)
	{
		PartOfSky = true;
	}
	else
	{
		PartOfSky = false;
	}
}

void Renderables::SetObjectSpecular(float specPower)
{
	matProp.Shininess = specPower;
}

void Renderables::MakeGlow()
{
	glowingtex = true;
}

glm::mat4 Renderables::GetModelMatrix()
{
	return mModelMat;
}

void Renderables::Selected()
{
	
}

void Renderables::SetModelMatrix(glm::mat4 mModelMat)
{
	this->mModelMat = mModelMat;
}

void Renderables::TestImprovedAssimp(char* path)
{
	model = new Model(path);
}

void Renderables::InitializeObject(char* path, char* bumpMapFilepath, char* matfilepath)
{
	HasTexture = 0;
	this->texturepaths.BumpMapTexturePath = bumpMapFilepath;
	this->texturepaths.TexturePath = matfilepath;
	if (bumbMaps == 1)
	{
		HasTexture = 1;
		bumbMaps = 1;
		this->ObjectModel.hasnormalmap = true;
		this->ObjectModel.MaterialFilename = matfilepath;
		this->ObjectModel.LoadModelFromFileWithNormalMapping(path, matfilepath, bumpMapFilepath);
	}
	else
	{
		this->ObjectModel.MaterialFilename = matfilepath;
		this->ObjectModel.LoadModelFromFile(path, matfilepath, bumpMapFilepath);
	}



	std::string patharr(path);
	Logger::Log("Loaded: " + patharr);
	objectId = path;
}

void Renderables::setBumpMaponOff(bool onoff)
{
	if (onoff)
	{
		this->bumbMaps = 1;
	}
	else
	{
		this->bumbMaps = 0;
	}
}

void Renderables::InitializeObject(int objectType, float Tiling)
{

}

//Finalize all object model vertexbuffers
void Renderables::FinalizeVBOS()
{
	CAssimpModel::FinalizeVBO();
}

void Renderables::RenderForThread()
{
	
}

void Renderables::RenderObjectGL()
{
	glEnable(GL_TEXTURE_2D);
	if (IsTransParent)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
	}
	if (bumbMaps == 1)
	{
		this->ObjectModel.RenderWithNormalMaps(GL_TRIANGLES);
	}
	else
	{
		this->ObjectModel.RenderModel();
	}
	
	glDisable(GL_BLEND);
	
	CAssimpModel::UnBindModelsVAO();
}

glm::vec3 Renderables::GetLocation()
{
	return this->location;
}

glm::vec3 Renderables::GetScale()
{
	return this->scaleSize;
}

void Renderables::SetObjectId(char* oid)
{
	this->objectId = oid;
}

char* Renderables::GetObjectId()
{
	return this->objectId;
}

void Renderables::SetMaterialUniformData(CShaderProgram *sProgram)
{
	sProgram->setUniform("matActive.ambient", matProp.ambient);
	sProgram->setUniform("matActive.diffuse", matProp.diffuse);
	sProgram->setUniform("matActive.specular", matProp.specProto);

	sProgram->setUniform("matActive.Shininess", matProp.Shininess);
	sProgram->setUniform("matActive.isMetallic", matProp.isMetallic);
	sProgram->setUniform("roughness", matProp.Roughness);
	sProgram->setUniform("gNormalMap", 1);

	
	if (this->ObjectModel.hasMaterials)
	{
		this->HasTexture = 1;
	}
	if (bumbMaps == 1)
	{
		sProgram->setUniform("bEnableBumpMap", 1);
		sProgram->setUniform("hasTex", 1);
		sProgram->setUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	if (HasTexture == 1 && bumbMaps == 0)
	{
		sProgram->setUniform("hasTex", 1);
		sProgram->setUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		sProgram->setUniform("bEnableBumpMap", 0);
		sProgram->setUniform("hasTex", 0);
		sProgram->setUniform("vColor", glm::vec4(matProp.color, 1.0f));
	}
}

void Renderables::InitNewAssimp(CShaderProgram *sProgram)
{
	glEnable(GL_TEXTURE_2D);
	mModelMat = glm::mat4(1.0);


	if (usesphysics)
	{
		btTransform trans;

		rigidbodys->getMotionState()->getWorldTransform(trans);
		this->location.x = trans.getOrigin().getX();
		this->location.y = trans.getOrigin().getY();
		this->location.z = trans.getOrigin().getZ();
		rotateObject(trans.getRotation().getAngle(), glm::vec3(trans.getRotation().x(), trans.getRotation().y(), trans.getRotation().z()));

	}

	mModelMat = glm::translate(mModelMat, location);
	mModelMat = glm::rotate(mModelMat, rotateAngle, rotateAxis);
	mModelMat = glm::scale(mModelMat, scaleSize);
	this->SetMaterialUniformData(sProgram);
	sProgram->SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", &mModelMat);
	sProgram->setUniform("shadowMap", 50);
	sProgram->setUniform("bShadowsOn", 1);
}

void Renderables::DrawNewAssimp(CShaderProgram *sProgram)
{
	model->Draw(*sProgram);
}

void Renderables::RenderObjectInit(CShaderProgram *sProgram, btDynamicsWorld *dyn)
{
	glEnable(GL_TEXTURE_2D);
	CAssimpModel::BindModelsVAO();
	


	
	mModelMat = glm::mat4(1.0);
	

	if (usesphysics)
	{
		btTransform trans;
		
		rigidbodys->getMotionState()->getWorldTransform(trans);
		this->location.x = trans.getOrigin().getX();
		this->location.y = trans.getOrigin().getY();
		this->location.z = trans.getOrigin().getZ();
		rotateObject(trans.getRotation().getAngle(), glm::vec3(trans.getRotation().x(), trans.getRotation().y(), trans.getRotation().z()));
		
	}
	
	mModelMat = glm::translate(mModelMat, location);
	mModelMat = glm::rotate(mModelMat, rotateAngle, rotateAxis);
	mModelMat = glm::scale(mModelMat, scaleSize);
	this->SetMaterialUniformData(sProgram);
	sProgram->SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", &mModelMat);
	sProgram->setUniform("shadowMap", 50);
	sProgram->setUniform("bShadowsOn", 1);
}

void Renderables::SetRoughness(float froughness)
{
	this->matProp.Roughness = froughness;
}

btRigidBody *Renderables::getRigidbody()
{
	return rigidbodys;
}

void Renderables::SetPhysics(btRigidBody* rgbody, int objectindex)
{
	 this->objectindex_ = objectindex;
	 this->rigidbodys = rgbody;
	 if (usesphysics)
	  {
		 if (objType == 2)
		 {
			 boxCollisionShape = new btSphereShape(this->scaleSize.y+3);
			 
			 motionstate = new btDefaultMotionState(btTransform(btQuaternion(this->rotateAxis.x, this->rotateAxis.y,
				 this->rotateAxis.z, 0.0f), btVector3(this->location.x, this->location.y, this->location.z)));
			 btScalar mass = 10;
			 btVector3 fallInertia(0, 0, 1);
			 //boxCollisionShape->calculateLocalInertia(mass, fallInertia);
			 btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionstate, boxCollisionShape,btVector3(0,0,0));
		     
			 this->rigidbodys = new btRigidBody(rigidBodyCI);
			 this->rigidbodys->setUserPointer((char*)objectId);
		 }
		 else
		 {
			 if (scaleSize.x < 50.0f && scaleSize.y < 50.0f && scaleSize.z < 50.0f)
			 {
				 boxCollisionShape = new btBoxShape(btVector3(50.0f, 50.0f, 50.0f));

			 }
			 else
			 {
				 boxCollisionShape = new btBoxShape(btVector3(this->scaleSize.x, this->scaleSize.y , this->scaleSize.z));
			 }
			 motionstate = new btDefaultMotionState(btTransform(btQuaternion(this->rotateAxis.x, this->rotateAxis.y,
				 this->rotateAxis.z, 0.0f), btVector3(this->location.x, this->location.y, this->location.z)));
			 btScalar mass = 1;
			 btVector3 fallInertia(0, 0, 0);
			 //boxCollisionShape->calculateLocalInertia(mass, fallInertia);
			 btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionstate, boxCollisionShape, btVector3(0,0,0));

			 this->rigidbodys = new btRigidBody(rigidBodyCI);
			 this->rigidbodys->setUserPointer((char*)objectId);
		 }
	
	   }

	   else
	   {
		   if (scaleSize.x < 50.0f && scaleSize.y < 50.0f && scaleSize.z < 50.0f)
		   {
			   boxCollisionShape = new btBoxShape(btVector3(100.0f, 100.0f, 100.0f));

		   }
		   else
		   {
			   boxCollisionShape = new btBoxShape(btVector3(this->scaleSize.x+10, this->scaleSize.y+10, this->scaleSize.z+10));
		   }

		   motionstate = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1.0f), btVector3(this->location.x,this->location.y,this->location.z)));

		   btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionstate, boxCollisionShape, btVector3(0, 0, 0));

		   this->rigidbodys = new btRigidBody(rigidBodyCI);
		   this->rigidbodys->setUserPointer((char*)objectId);
	   }


	
}

float Renderables::getRotationAngle()
{
	return rotateAngle;
}

void Renderables::DestroyObject()
{
	//for (int i = 0; i < 2; i++)
	//{
	//	vbos[i]->~VertexBufferObject();
	//}
	
	vbo.releaseVBO();
	if (texturepaths.TexturePath != NULL)
	{
		texture.releaseTexture();
	}
	if(cubeMaps == 1)
	{
		CubeMapTexture.releaseTexture();
	}
	if (bumbMaps == 1)
	{
		bumpMapTexture.releaseTexture();
	}
	this->ObjectModel.ReleaseAllModelData();
	//vao->~VertexArrayObject();
}
void Renderables::ReleaseTextureData()
{
	texture.releaseTexture();
}

Renderables::~Renderables()
{

}


