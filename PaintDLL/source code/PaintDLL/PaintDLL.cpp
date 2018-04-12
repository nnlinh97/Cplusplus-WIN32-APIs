// PaintDLL.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PaintDLL.h"
#include "DLL.h"
#include <iostream >
#include <vector>
#include <windowsx.h>
using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PAINTDLL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINTDLL));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//-------------------------------------------------------------------------------
bool isDown = false;
PaintLibrary::Point p1, p2;
vector<PaintLibrary::SHAPE*> shapes;
vector<PaintLibrary::SHAPE*> prototypes;
PaintLibrary::ShapeMode curShapeMode;
void ShiftDown();
int x, y;

//-------------------------------------------------------------------------------



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINTDLL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PAINTDLL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
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
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HMENU hMenu = GetMenu(hWnd);

    switch (message)
    {
	case WM_CREATE:
		prototypes.push_back(new PaintLibrary::LINE);
		prototypes.push_back(new PaintLibrary::RECTANGLE);
		prototypes.push_back(new PaintLibrary::ELLIPSE);
		break;
	case WM_LBUTTONDOWN:
		p1 = PaintLibrary::newPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if (isDown == false) {
			isDown = true;
			SetCapture(hWnd);
		}
		break;
	case WM_MOUSEMOVE:
	{
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		if (isDown == true)
		{
			//WCHAR buffer[200];
			//wsprintf(buffer, L"%d %d", x, y);
			//SetWindowText(hWnd, buffer);
			p2 = PaintLibrary::newPoint(x, y);
			if (wParam & MK_SHIFT)
				ShiftDown();
		}

		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		p2 = PaintLibrary::newPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		isDown = false;
		if (wParam & MK_SHIFT)
			ShiftDown();
		switch (curShapeMode)
		{
		case PaintLibrary::SM_LINE: {
			PaintLibrary::LINE* line = new PaintLibrary::LINE;
			line->setData(p1.x, p1.y, p2.x, p2.y);
			shapes.push_back(line);
		}
									break;
		case PaintLibrary::SM_RECTANGLE: {
			PaintLibrary::RECTANGLE* rect = new PaintLibrary::RECTANGLE;
			rect->setData(p1.x, p1.y, p2.x, p2.y);
			shapes.push_back(rect);
		}
										 break;
		case PaintLibrary::SM_ELLIPSE:
		{
			PaintLibrary::ELLIPSE* elp = new PaintLibrary::ELLIPSE;
			elp->setData(p1.x, p1.y, p2.x, p2.y);
			shapes.push_back(elp);
		}
		break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
			HDC hdc;
			PaintLibrary::SHAPE* shape;
            switch (wmId)
            {
			case ID_DRAW_LINE:
				CheckMenuItem(hMenu, ID_DRAW_LINE, MF_CHECKED);
				CheckMenuItem(hMenu, ID_DRAW_RECTANGLE, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_DRAW_ELLIPSE, MF_UNCHECKED);
				curShapeMode = PaintLibrary::SM_LINE;
				shape = prototypes[0]->Create();
				shape->setData(10, 10, 20, 20);
				shapes.push_back(shape);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case ID_DRAW_RECTANGLE:
				CheckMenuItem(hMenu, ID_DRAW_RECTANGLE, MF_CHECKED);
				CheckMenuItem(hMenu, ID_DRAW_LINE, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_DRAW_ELLIPSE, MF_UNCHECKED);
				curShapeMode = PaintLibrary::SM_RECTANGLE;
				shape = prototypes[1]->Create();
				shape->setData(10, 10, 20, 20);
				shapes.push_back(shape);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case ID_DRAW_ELLIPSE:
				CheckMenuItem(hMenu, ID_DRAW_ELLIPSE, MF_CHECKED);
				CheckMenuItem(hMenu, ID_DRAW_LINE, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_DRAW_RECTANGLE, MF_UNCHECKED);
				curShapeMode = PaintLibrary::SM_ELLIPSE;
				shape = prototypes[2]->Create();
				shape->setData(10, 10, 20, 20);
				shapes.push_back(shape);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			int sz = shapes.size();
			for (int i = 0; i < sz; i++)
			{
				shapes[i]->Draw(hdc);
			}
			if (isDown == true)
			{

				switch (curShapeMode)
				{
				case PaintLibrary::SM_LINE:
					MoveToEx(hdc, p1.x, p1.y, NULL);
					LineTo(hdc, p2.x, p2.y);
					break;
				case PaintLibrary::SM_RECTANGLE:
					Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);

					break;
				case PaintLibrary::SM_ELLIPSE:
					Ellipse(hdc, p1.x, p1.y, p2.x, p2.y);
					break;
				}
			}
            EndPaint(hWnd, &ps);
        }
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
void ShiftDown()
{
	int subX = abs(p1.x - p2.x);
	int subY = abs(p1.y - p2.y);
	if (subX < subY)
	{
		if (p2.y > p1.y)
			p2.y = p1.y + subX;
		else
			p2.y = p1.y - subX;
	}
	else
	{
		if (p2.x > p1.x)
			p2.x = p1.x + subY;
		else
			p2.x = p1.x - subY;
	}
}