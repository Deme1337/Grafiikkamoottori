#pragma once


#ifndef OBJ_MANIPULATOR
#define OBJ_MANIPULATOR

#include "Renderables.h"
#include "PrimitiveShape.h"
#include "TextBox.h"



class ObjectManipulator
{
public:
	ObjectManipulator();
	~ObjectManipulator();


private:
	HWND ObjViewer;
	TextBox tbs[10];


};

#endif