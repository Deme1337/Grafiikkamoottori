// PeekGraphics.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "PeekGraphics.h"
#include <future>
#include "MouseHandler.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPTSTR lpCmdLine,int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	


	HACCEL hAccelTable;

	// Initialize global std::strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PEEKGRAPHICS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PEEKGRAPHICS));

	
	
	GlCore->InitRender();
	
	//Start the engine loop
	std::future<MSG> ret = std::async(&EngineLoop);
	

	return (int) ret.get().wParam;
}
static int tick = 0;

MSG EngineLoop() 
{
	MSG msg;
	while (!quit)
	{

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{

				if (msg.message == WM_QUIT)
				{
					quit = true;
				}
				else
				{

					GlCore->RenderLoop();
					GlCore->SwapBuffersm();
					UpdateWindow(GlCore->GetViewPort());
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

			}
			else
			{
				if (msg.message == WM_QUIT)
				{
					quit = true;
				}
				else
				{
					
					GlCore->RenderLoop();
					GlCore->SwapBuffersm();
					UpdateWindow(GlCore->GetViewPort());
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

			}
		}


	GlCore->Destroy();
	delete GlCore;
	return msg;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;



	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PEEKGRAPHICS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_DLGFRAME,
      NULL, 0, 1920,1080, NULL, NULL, hInstance, NULL);
   
   if (!hWnd)
   {
      return FALSE;
   }
   
   // Main window is not shown
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   RECT rec;
   GetWindowRect(hWnd, &rec);
   int Xmid = rec.right / 2;
   int viewport_lanchor = Xmid - 500;
   int viewport_banchor = rec.bottom -10;

   
   //Initialising "Graphics engine"
   GlCore->InitGraphics(hWnd, szWindowClass, szTitle, hInstance, 10, 10,rec.right-15,
	   viewport_banchor-15, false);
   ShowWindow(GlCore->GetViewPort(), nCmdShow);
   UpdateWindow(GlCore->GetViewPort());



   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	int x, y;
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		x = LOWORD(wParam);
		y = HIWORD(wParam);
		MouseHandler::SetCursorPosInWindow(x, y);
		break;
	case WM_ACTIVATEAPP:
	
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
