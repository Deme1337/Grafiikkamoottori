#include "stdafx.h"
#include "MouseHandler.h"


MouseHandler::MouseHandler()
{
}

void MouseHandler::SetCurrentWindow(HWND hwnd)
{
	curWindow = hwnd;
}

bool MouseHandler::MouseRightClick()
{
	if (Keys::key(VKEY_RIGHT_BUTTON))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool MouseHandler::MouseLeftClick()
{
	if (Keys::key(VKEY_LEFT_BUTTON))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MouseHandler::SetCursorPosInWindow(int x, int y)
{
	cursorlocationX = x;
	cursorlocationY = y;
}

POINT MouseHandler::CurrentPosition()
{
	POINT cursorpos;

	GetCursorPos(&cursorpos);
	ScreenToClient(curWindow, &cursorpos);

	return cursorpos;
}

MouseHandler::~MouseHandler()
{
}
