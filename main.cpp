#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <iostream>
#include <tchar.h>
#include <windows.h>

// Handles
HWND hwnd,hbutton,hlabel;

// Clipping rectangles, default and small
RECT cliprek[2] {0,0,0,0};

// Message strings
const TCHAR* const strings[2]=
{
	_T("Press the button to trap the mouse."),
	_T("Mouse trapped. Press any key to break free.")
};

//  Declare Windows procedure
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

//  Make the class name into a global variable
TCHAR szClassName[]=_T("CodeBlocksWindowsApp");

int WINAPI WinMain(HINSTANCE hThisInstance,
				   HINSTANCE,
				   LPSTR,
				   int nCmdShow)
{
	MSG messages;	// Here messages to the application are saved
	WNDCLASSEX wincl;	// Data structure for the windowclass

	// The Window structure
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc =	WindowProcedure; // This function is called by windows
	wincl.style = CS_DBLCLKS;	// Catch double-clicks
	wincl.cbSize = sizeof(WNDCLASSEX);
	// Use default icon and mouse-pointer
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;	// No menu
	wincl.cbClsExtra = 0;	// No extra bytes after the window class
	wincl.cbWndExtra = 0;	// Structure or the window instance
	// Use Windows's default colour as the background of the window
	wincl.hbrBackground = (HBRUSH) COLOR_WINDOW;

	// Register the window class, and if it fails quit the program
	if (!RegisterClassEx(&wincl))
		return 0;

	// Dimensions of client-rectangle
	const LONG width=512,height=152;
	// Adjust window based on those
	RECT rect{0,0,width,height};
	DWORD style=WS_SYSMENU|(WS_OVERLAPPEDWINDOW^WS_THICKFRAME);
	AdjustWindowRect(&rect,style,false);

	hwnd=CreateWindow(
			 szClassName,         // Classname
			 _T("Mouse Trap"),    // Title Text
			 style,
			 CW_USEDEFAULT,       // Windows decides the position
			 CW_USEDEFAULT,       // where the window ends up on the screen
			 rect.right-rect.left,// The programs width
			 rect.bottom-rect.top,// and height in pixels
			 HWND_DESKTOP,        // The window is a child-window to desktop
			 NULL,                // No menu
			 hThisInstance,       // Program Instance handler
			 NULL                 // No Window Creation data
		 );

	// Make the window visible on the screen
	ShowWindow(hwnd, nCmdShow);

	// Button
	hbutton=CreateWindow(
				"button",
				_T("Capture"),
				WS_VISIBLE|WS_CHILD,
				8,
				8,
				width-16,
				64,
				hwnd,
				(HMENU)100,
				hThisInstance,
				NULL
			);

	// Label
	hlabel=CreateWindow(
			  "static",
			  strings[0],
			  WS_VISIBLE|WS_CHILD,
			  8,
			  8+64+8,
			  width-16,
			  64,
			  hwnd,
			  NULL,
			  hThisInstance,
			  NULL
		  );

	// Set up proper font
	HFONT hFont;
	NONCLIENTMETRICS ncMetrics;
	ncMetrics.cbSize=sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS,sizeof(NONCLIENTMETRICS),
						 &ncMetrics,0);
	hFont=CreateFontIndirect(&ncMetrics.lfMessageFont);
	SendMessage(hbutton,WM_SETFONT,(WPARAM)hFont,TRUE);
	SendMessage(hlabel,WM_SETFONT,(WPARAM)hFont,TRUE);

	// Store default clip rectangle
	GetClipCursor(&cliprek[0]);

	// Run the message loop. It will run until GetMessage() returns 0
	while (GetMessage(&messages, NULL, 0, 0))
	{
		// Translate virtual-key messages into character messages
		TranslateMessage(&messages);
		// Send message to WindowProcedure
		DispatchMessage(&messages);
	}

	// The program return-value is 0 - The value that PostQuitMessage() gave
	return messages.wParam;
}

//  This function is called by the Windows function DispatchMessage()
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam,
								 LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		ClipCursor(&cliprek[0]);
		SetWindowText(hlabel,strings[0]);
		break;
	case WM_ACTIVATE:	// Window deactivated (Alt-Tab etc)
		if (LOWORD(wParam)==WA_INACTIVE)
		{
			SetWindowText(hlabel,strings[0]);
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam)==100)
		{
			SetWindowText(hlabel,strings[1]);
			GetWindowRect(hlabel,&cliprek[1]);
			ClipCursor(&cliprek[1]);
			SetFocus(hwnd);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);	// Send a WM_QUIT to the message queue
		break;
	default:				// For messages that we don't deal with
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
