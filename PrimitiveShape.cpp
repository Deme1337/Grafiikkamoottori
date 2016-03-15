#include "stdafx.h"
#include "PrimitiveShape.h"


/*
Here is declared data for cube 

this is messy

*/

#ifndef PRIMITIVE_SHAPES
#define PRIMITIVE_SHAPES

glm::vec3 vCubeVertices[36] =
{
	// Front face
	glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f),
	// Back face
	glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f),
	// Left face
	glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f),
	// Right face
	glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f),
	// Top face
	glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, -0.5f),
	// Bottom face
	glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, 0.5f),
};


glm::vec2 vCubeTexCoords[6] = { glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f) };

glm::vec3 vCubeNormals[6] =
{
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f)
};
#endif

PrimitiveShape::PrimitiveShape()
{
}

void PrimitiveShape::initPrimitiveShape(int objectType, float Tiling)
{
	objType = objectType;
	HasTexture = 0;
	TextureTiling = Tiling;


	vbo.createVBO();
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	vbo.bindVBO();

	glm::vec3 tangent;
	glm::vec3 bitangent;


	if (objectType == 0)
	{
		for (int i = 0; i < 36; i++)
		{
			vbo.addData(&vCubeVertices[i], sizeof(glm::vec3));
			vbo.addData(&vCubeTexCoords[i % 6], sizeof(glm::vec2));
		    vbo.addData(&vCubeNormals[i / 6], sizeof(glm::vec3));
			
		}
		Logger::Log("Done primitive cube");
	}



	if (objectType == 2)
	{
		bool obj_load = m_objectloader.loadOBJ("Models\\sphere.obj", vertices, uvs, normals);
		for (int j = 0; j < vertices.size(); j++)
		{

			vbo.addData(&vertices[j], sizeof(glm::vec3));
		    vbo.addData(&uvs[j], sizeof(glm::vec2));
			vbo.addData(&normals[j], sizeof(glm::vec3));

		}
		Logger::Log("Done sphere");
	}

	vbo.uploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	

}

void PrimitiveShape::initRenderPrimitiveShape(CShaderProgram *sProgram, btDynamicsWorld *dyn)
{
	glBindVertexArray(VertexArrayID);
	sProgram->setUniform("cMap", 0);
	glEnable(GL_TEXTURE_2D);

	if (hasCubeMap)
	{
		sProgram->setUniform("usesCubemap", 1);
		sProgram->setUniform("hasTex", 0);
		CubeMapTexture.textureHint = GL_TEXTURE_CUBE_MAP;
		CubeMapTexture.bindTexture(0);	
	}

	if (HasTexture == 1 && bumbMaps == 1)
	{
		sProgram->setUniform("bEnableBumpMap", 1);
		sProgram->setUniform("hasTex", 1);
		texture.bindTexture(0);
		bumpMapTexture.bindTexture(1);
		sProgram->setUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	if (HasTexture == 1 && bumbMaps == 0 && cubeMaps == 0)
	{
		CubeMapTexture.textureHint = GL_TEXTURE_2D;
		texture.bindTexture(0);
		sProgram->setUniform("bEnableBumpMap", 0);
		sProgram->setUniform("hasTex", 1);
		sProgram->setUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		sProgram->setUniform("hasTex", 0);
		sProgram->setUniform("bEnableBumpMap", 0);
		sProgram->setUniform("hasTex", 0);
		sProgram->setUniform("vColor", glm::vec4(matProp.color, 1.0f));
	}
	
	mModelMat = glm::mat4(1.0);

	if (usesphysics)
	{
		btTransform trans;

		rigidbodys->getMotionState()->getWorldTransform(trans);
		//this->SetObjectLocation(glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
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

void PrimitiveShape::renderPrimitiveShape()
{
	
	if (IsTransParent)
	{
	
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	}
	//Real rendering
	if (objType == 0)
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else if (objType > 0)
	{
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBindVertexArray(0);
}

void PrimitiveShape::destroyPrimitiveShape()
{
	vbo.releaseVBO();
	if (texturepaths.TexturePath != NULL)
	{
		texture.releaseTexture();
	}
}

PrimitiveShape::~PrimitiveShape()
{
}
