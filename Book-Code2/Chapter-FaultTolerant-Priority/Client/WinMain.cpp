//
// Disable some compiler warnings that come from boost
// Have to include this first to prevent Windows.h from crying over spilled milk
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)
#include <boost/asio.hpp>
#pragma warning(pop)

#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include <windows.h>

#include "FaultTolerantApp.hpp"

#include <memory>


//
// This is the windows message callback
LRESULT CALLBACK WinMessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//
// O crud, a global variable!!
std::shared_ptr<FaultTolerantApp> g_app;

// -----------------------------------------------------------------
//
// @details This is the entry point for the Win32 application.
//
// -----------------------------------------------------------------
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
	wcl.lpszClassName = L"FaultTolerant";
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
		L"FaultTolerant",
		L"FaultTolerantPriority - Demo",
		dwStyle,
		CW_USEDEFAULT,CW_USEDEFAULT,
		500, 500,
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
	g_app = std::make_shared<FaultTolerantApp>(hwnd, static_cast<uint16_t>(rcClient.right), static_cast<uint16_t>(rcClient.bottom));
	g_app->initialize();
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

	g_app->terminate();

	//
	// Disable a compiler warning complaining about WPARAM to int conversion.
	#pragma warning(push)
	#pragma warning(disable : 4244)
	return winMessage.wParam;
	#pragma warning(pop)
}

// -----------------------------------------------------------------
//
// @details This is the message handler callback for the registered window class.
//
// -----------------------------------------------------------------
LRESULT CALLBACK WinMessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
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
