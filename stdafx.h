// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <stdint.h>
#include <tchar.h>
#include <Commdlg.h>
#include "Logger.h"


namespace Keys
{
	int key(int iKey);
	int onekey(int iKey);
	extern char kp[256];
}
std::wstring s2ws(const std::string& s);



static double engineframeInterval;

static double getEngineFrameInterval() {	return engineframeInterval; }


// wide char to multi byte:
std::string ws2s(const std::wstring& wstr);
