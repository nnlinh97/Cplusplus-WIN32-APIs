// PaintGDIPlusRibbon.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PaintGDIPlusRibbon.h"
#include <Objbase.h>
#pragma comment(lib, "Ole32.lib")
#include "RibbonFramework.h"
#include "RibbonIDs.h"
#include <iostream>>
#include <vector>
#include <windowsx.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
using namespace std;


#define MAX_LOADSTRING 100
//--------------------------------------------------------------------------------
int CShape::index = 0;

// ---------------------------------------------------------------------------- -
// ---------------------------------------------------------------------------- -
enum ShapeMode
{
	SM_LINE,
	SM_RECTANGLE,
	SM_ELLIPSE
};
//-----------------------------------------------------------------------------
class SHAPE {
protected:
	int x1, y1, x2, y2;
public:
	virtual void Draw(HDC hdc) = 0;
	virtual SHAPE* Create() = 0;
	virtual void setData(int a, int b, int c, int d) = 0;
};

//-----------------------------------------------------------------------------
struct Point
{
	int x, y;
};
Point newPoint(int x, int y)
{
	Point p;
	p.x = x;
	p.y = y;
	return p;
}

//-------------------------------------------------------------------------
class LINE :public SHAPE {
public:
	void Draw(HDC hdc);
	SHAPE* Create();
	void setData(int a, int b, int c, int d);
};
void LINE::Draw(HDC hdc) {
	Gdiplus::Graphics* graphics = new Gdiplus::Graphics(hdc);
	int lineWidth = 2;
	Gdiplus::Pen* pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), lineWidth);
	graphics->DrawLine(pen, x1, y1, x2, y2);
	delete pen;
	delete graphics;
}
SHAPE* LINE::Create()
{
	return new LINE;
}
void LINE::setData(int a, int b, int c, int d)
{
	x1 = a;
	y1 = b;
	x2 = c;
	y2 = d;
}

//--------------------------------------------------------------------------
class RECTANGLE :public SHAPE
{
public:
	void Draw(HDC hdc);
	SHAPE* Create();
	void setData(int left, int top, int right, int bottom);
};
SHAPE* RECTANGLE::Create()
{
	return new RECTANGLE;
}
void RECTANGLE::Draw(HDC hdc)
{
	Gdiplus::Graphics* graphics = new Gdiplus::Graphics(hdc);
	int lineWidth = 2;
	Gdiplus::Pen* pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), lineWidth);
	if (x1 > x2 && y1 > y2)
		graphics->DrawRectangle(pen, x2, y2, -(x2 - x1), -(y2 - y1));
	else if ((x1 > x2 && y1 < y2))
		graphics->DrawRectangle(pen, x2, y1, (x1 - x2), (y2 - y1));
	else if ((x1 < x2 && y1 > y2))
		graphics->DrawRectangle(pen, x1, y2, (x2 - x1), (y1 - y2));
	else graphics->DrawRectangle(pen, x1, y1, (x2 - x1), (y2 - y1));
	delete pen;
	delete graphics;
}
void RECTANGLE::setData(int left, int top, int right, int bottom)
{
	x1 = left;
	y1 = top;
	x2 = right;
	y2 = bottom;
}

//-----------------------------------------------------------------------------
class ELLIPSE :public SHAPE
{
public:
	void Draw(HDC hdc);
	SHAPE* Create();
	void setData(int left, int top, int right, int bottom);
};

SHAPE* ELLIPSE::Create()
{
	return new ELLIPSE;
}
void ELLIPSE::setData(int left, int top, int right, int bottom)
{
	x1 = left;
	y1 = top;
	x2 = right;
	y2 = bottom;
}
void ELLIPSE::Draw(HDC hdc)
{
	Gdiplus::Graphics* graphics = new Gdiplus::Graphics(hdc);
	int lineWidth = 2;
	Gdiplus::Pen* pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), lineWidth);
	if (x1 > x2 && y1 > y2)
		graphics->DrawEllipse(pen, x2, y2, -(x2 - x1), -(y2 - y1));
	else if ((x1 > x2 && y1 < y2))
		graphics->DrawEllipse(pen, x2, y1, (x1 - x2), (y2 - y1));
	else if ((x1 < x2 && y1 > y2))
		graphics->DrawEllipse(pen, x1, y2, (x2 - x1), (y1 - y2));
	else graphics->DrawEllipse(pen, x1, y1, x2 - x1, y2 - y1);
	delete pen;
	delete graphics;
}

//-------------------------------------------------------------------------------

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//------------------------------------------------------
bool isDown = false;
Point p1, p2;
vector<SHAPE*> shapes;
vector<SHAPE*> prototypes;
Gdiplus::GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;
int x, y;
//ShapeMode curShapeMode;
void ShiftDown();
//--------------------------------------

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	CoInitialize(NULL);
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PAINTGDIPLUSRIBBON, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINTGDIPLUSRIBBON));

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
	CoUninitialize();
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = 0;// CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINTGDIPLUSRIBBON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PAINTGDIPLUSRIBBON);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,
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
    switch (message)
    {
		bool init;
	case WM_CREATE:
		init = InitializeFramework(hWnd);
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		prototypes.push_back(new LINE);
		prototypes.push_back(new RECTANGLE);
		prototypes.push_back(new ELLIPSE);
		break;
	case WM_LBUTTONDOWN:
	{
		p1 = newPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if (isDown == false)
			isDown = true;
	}break;
	case WM_MOUSEMOVE: {
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		if (isDown == true)
		{
			p2 = newPoint(x, y);
			if (wParam & MK_SHIFT)
				ShiftDown();
		}
		InvalidateRect(hWnd, NULL, TRUE);

	}break;
	case WM_LBUTTONUP: {
		p2 = newPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		isDown = false;
		if (wParam & MK_SHIFT)
			ShiftDown();
		switch (CShape::index)
		{
		case SM_LINE: {
			LINE* line = new LINE;
			line->setData(p1.x, p1.y, p2.x, p2.y);
			shapes.push_back(line);
			InvalidateRect(hWnd, NULL, TRUE);

		}break;
		case SM_RECTANGLE: {
			RECTANGLE* rect = new RECTANGLE;
			rect->setData(p1.x, p1.y, p2.x, p2.y);
			shapes.push_back(rect);
			InvalidateRect(hWnd, NULL, TRUE);

		}break;
		case SM_ELLIPSE:
		{
			ELLIPSE* elp = new ELLIPSE;
			elp->setData(p1.x, p1.y, p2.x, p2.y);
			shapes.push_back(elp);
			InvalidateRect(hWnd, NULL, TRUE);

		}break;
		}
	}break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
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
			Gdiplus::Graphics* graphics = new Gdiplus::Graphics(hdc);
			int lineWidth = 2;
			Gdiplus::Pen* pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), lineWidth);
			if (isDown == true)
			{

				switch (CShape::index)
				{
				case SM_LINE:
				{
					graphics->DrawLine(pen, p1.x, p1.y, p2.x, p2.y);

				}break;

				case SM_RECTANGLE:
				{
					if (p1.x >= p2.x && p1.y >= p2.y)
						graphics->DrawRectangle(pen, p2.x, p2.y, -p2.x + p1.x, -p2.y + p1.y);
					else if (p1.x > p2.x && p1.y < p2.y)
						graphics->DrawRectangle(pen, p2.x, p1.y, -p2.x + p1.x, p2.y - p1.y);
					else if (p1.x < p2.x && p1.y > p2.y)
						graphics->DrawRectangle(pen, p1.x, p2.y, p2.x - p1.x, -p2.y + p1.y);
					else graphics->DrawRectangle(pen, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);

				}break;

				case SM_ELLIPSE:
				{
					if (p1.x >= p2.x && p1.y >= p2.y)
						graphics->DrawEllipse(pen, p2.x, p2.y, -p2.x + p1.x, -p2.y + p1.y);
					else if (p1.x > p2.x && p1.y < p2.y)
						graphics->DrawEllipse(pen, p2.x, p1.y, -p2.x + p1.x, p2.y - p1.y);
					else if (p1.x < p2.x && p1.y > p2.y)
						graphics->DrawEllipse(pen, p1.x, p2.y, p2.x - p1.x, -p2.y + p1.y);
					else graphics->DrawEllipse(pen, p1.x, p1.y, (p2.x - p1.x), (p2.y - p1.y));

				}break;
				}
			}
			delete pen;
			delete graphics;
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