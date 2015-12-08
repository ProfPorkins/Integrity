/*
Copyright (c) 2015 James Dean Mathias

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <windows.h>

#include <cstdio>
#include <fcntl.h>
#include <io.h>

#include "ScalabilityApp.hpp"
#include "ThreadPool.hpp"

#include <memory>

const int SIZE_X = 500;
const int SIZE_Y = 500;

//
// This is the windows message callback
LRESULT CALLBACK WinMessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//
// O crud, a global variable!!
std::shared_ptr<ScalabilityApp> g_app;

// ------------------------------------------------------------------
//
// @details This is the entry point for the Win32 application.
//
// ------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	//
	// Create a console window for debugging and other output
	AllocConsole();

	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	int hcrt = _open_osfhandle((long) hout, _O_TEXT);
	FILE* fout = _fdopen(hcrt, "w");
	setvbuf(fout, NULL, _IONBF, 1);
	*stdout = *fout;

	HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
	hcrt = _open_osfhandle((long) hin, _O_TEXT);
	FILE* fin = _fdopen(hcrt, "r");
	setvbuf(fin, NULL, _IONBF, 128);
	*stdin = *fin;

	//
	// Define a window class
	WNDCLASS wcl;
	wcl.hInstance = hInstance;
	wcl.lpszClassName = L"Scalability";
	wcl.lpfnWndProc = WinMessageHandler;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;

	wcl.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcl.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcl.lpszMenuName=NULL;

	//
	// Register with Windows
	if (!RegisterClass(&wcl)) 
	{
		return 0;
	}

	DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	//
	// Create the window
	HWND hwnd = CreateWindowEx(
		dwExStyle,
		L"Scalability",
		L"Scalability - Demo",
		dwStyle,
		CW_USEDEFAULT,CW_USEDEFAULT,
		SIZE_X, SIZE_Y,
		NULL,
		NULL,
		hInstance,
		NULL);
	SetFocus(hwnd);

	//
	// Obtain the client size of the window
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);

	//
	// Initialize the application code
	g_app = std::make_shared<ScalabilityApp>(hwnd, static_cast<uint16_t>(rcClient.right), static_cast<uint16_t>(rcClient.bottom));

	//
	// Display the window
	ShowWindow(hwnd, nShowCmd);

	//
	// Enter the Windows message loop
	MSG winMessage;
	while (GetMessage(&winMessage,NULL,0,0))
	{
		TranslateMessage(&winMessage);
		DispatchMessage(&winMessage);

		g_app->pulse();
	}

	//
	// Play nice and clean up the thread pool now that the app in done
	ThreadPool::terminate();

	return static_cast<int>(winMessage.wParam);
}

// ------------------------------------------------------------------
//
// @details This is the message handler callback for the registered window class.
//
// ------------------------------------------------------------------
LRESULT CALLBACK WinMessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_UP:
					g_app->moveUp();
					break;
				case VK_DOWN:
					g_app->moveDown();
					break;
				case VK_LEFT:
					g_app->moveLeft();
					break;
				case VK_RIGHT:
					g_app->moveRight();
					break;
				case VK_ADD:
					g_app->zoomIn();
					break;
				case VK_SUBTRACT:
					g_app->zoomOut();
					break;
				case VK_ESCAPE:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					break;
			}
			break;
		case WM_CHAR:
			switch (wParam)
			{
				case 'w':
					g_app->moveUp();
					break;
				case 's':
					g_app->moveDown();
					break;
				case 'a':
					g_app->moveLeft();
					break;
				case 'd':
					g_app->moveRight();
					break;
				case 'q':
					g_app->zoomIn();
					break;
				case 'e':
					g_app->zoomOut();
					break;
			}
			break;
		case WM_PAINT:
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}

return 0;
}
