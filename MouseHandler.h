#pragma once

#ifndef MOUSEH_H
#define MOUSEH_H
#include <Windows.h>
#include "Input.h"

class MouseHandler
{
public:
	MouseHandler();
	~MouseHandler();
	
	static void SetCurrentWindow(HWND hwnd);
	static bool MouseRightClick();
	static bool MouseLeftClick();
	static POINT CurrentPosition();
	static void SetCursorPosInWindow(int x, int y);
private:

	
};
static HWND curWindow;
static int cursorlocationX;
static int cursorlocationY;

#endif