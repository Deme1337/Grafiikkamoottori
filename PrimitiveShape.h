#pragma once

#ifndef PRIMITIVE_SHAPE
#define PRIMITIVE_SHAPE

#include "stdafx.h"
#include "Renderables.h"
#include "PrimitiveObjLoader.h"
class PrimitiveShape : public Renderables
{
public:
	PrimitiveShape();
	void initPrimitiveShape(int objectType, float Tiling);
	void renderPrimitiveShape();
	void initRenderPrimitiveShape(CShaderProgram *sProgram, btDynamicsWorld *dyn);
	void destroyPrimitiveShape();
	~PrimitiveShape();

private:
	objLoader m_objectloader;
	int TextureTiling;
};

#endif