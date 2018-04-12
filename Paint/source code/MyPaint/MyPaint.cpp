// MyPaint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MyPaint.h"
#include <iostream>>
#include <vector>
#include <windowsx.h>
using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


//-----------------------------------------------------------------------------
enum ShapeMode
{
	SM_LINE,
	SM_RECTANGLE,
	SM_ELLIPSE
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

//----------------------------------------------------------------------------
class SHAPE {
protected:
	int x1, y1, x2, y2;
public:
	virtual void Draw(HDC hdc) = 0;
	virtual SHAPE* Create() = 0;
	virtual void setData(int a, int b, int c, int d) = 0;
};


//-------------------------------------------------------------------------
class LINE:public SHAPE {
public:
	void Draw(HDC hdc);
	SHAPE* Create();
	void setData(int a, int b, int c, int d);
};
void LINE::Draw(HDC hdc) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
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
class RECTANGLE:public SHAPE
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
	Rectangle(hdc, x1, y1, x2, y2);
}
void RECTANGLE::setData(int left, int top, int right, int bottom)
{
	x1 = left;
	y1 = top;
	x2 = right;
	y2 = bottom;
}

//-----------------------------------------------------------------------------
class ELLIPSE:public SHAPE
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
	Ellipse(hdc, x1, y1, x2, y2);
}

//-------------------------------------------------------------------------------

bool isDown = false;
Point p1, p2;
vector<SHAPE*> shapes;
vector<SHAPE*> prototypes;
ShapeMode curShapeMode;
void ShiftDown();
int x, y;




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
    LoadStringW(hInstance, IDC_MYPAINT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYPAINT));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYPAINT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYPAINT);
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
	//HMENU hPopurMenu = CreatePopupMenu();
	//InsertMenu(hPopurMenu, 0, MF_BYPOSITION | MF_STRING | MF_UNCHECKED,ID_DRAW_LINE, L"LINE");
	//InsertMenu(hPopurMenu, 1, MF_BYPOSITION | MF_STRING | MF_UNCHECKED, ID_DRAW_RECTANGLE, L"RECTANGLE");
	//InsertMenu(hPopurMenu, 2, MF_BYPOSITION | MF_STRING | MF_UNCHECKED, ID_DRAW_ELLIPSE, L"ELLIPSE");
    switch (message)
    {
	case WM_CREATE:
		prototypes.push_back(new LINE);
		prototypes.push_back(new RECTANGLE);
		prototypes.push_back(new ELLIPSE);
		break;
	case WM_LBUTTONDOWN:
		p1 = newPoint(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
		if (isDown == false){
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
			p2 = newPoint(x, y);
			if (wParam & MK_SHIFT)
				ShiftDown();
		}

		InvalidateRect(hWnd, NULL, TRUE); 
	}
		break;
	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		p2 = newPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		isDown = false;
		if (wParam & MK_SHIFT)
			ShiftDown();
		switch (curShapeMode)
		{
		case SM_LINE: {
			LINE* line = new LINE;
			line->setData(p1.x, p1.y, p2.x, p2.y);
			shapes.push_back(line);
		}break;
		case SM_RECTANGLE: {
			RECTANGLE* rect = new RECTANGLE;
			rect->setData(p1.x, p1.y, p2.x, p2.y);
			shapes.push_back(rect);
		}break;
		case SM_ELLIPSE:
		{
			ELLIPSE* elp = new ELLIPSE;
			elp->setData(p1.x, p1.y, p2.x, p2.y);
			shapes.push_back(elp);
		}break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
			HDC hdc;
			SHAPE* shape;
			LINE* line;
			RECTANGLE* rect;
            switch (wmId)
            {
			case ID_DRAW_LINE:
				CheckMenuItem(hMenu, ID_DRAW_LINE, MF_CHECKED);
				CheckMenuItem(hMenu, ID_DRAW_RECTANGLE, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_DRAW_ELLIPSE, MF_UNCHECKED);
				curShapeMode = SM_LINE;
				shape = prototypes[0]->Create();
				shape->setData(10, 10, 20, 20);
				shapes.push_back(shape);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case ID_DRAW_RECTANGLE:
				CheckMenuItem(hMenu, ID_DRAW_RECTANGLE, MF_CHECKED);
				CheckMenuItem(hMenu, ID_DRAW_LINE, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_DRAW_ELLIPSE, MF_UNCHECKED);
				curShapeMode = SM_RECTANGLE;
				shape = prototypes[1]->Create();
				shape->setData(10, 10, 20, 20);
				shapes.push_back(shape);
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case ID_DRAW_ELLIPSE:
				CheckMenuItem(hMenu, ID_DRAW_ELLIPSE, MF_CHECKED);
				CheckMenuItem(hMenu, ID_DRAW_LINE, MF_UNCHECKED);
				CheckMenuItem(hMenu, ID_DRAW_RECTANGLE, MF_UNCHECKED);
				curShapeMode = SM_ELLIPSE;
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
			int sz = shapes.size();
			for (int i = 0; i < sz; i++)
			{
				shapes[i]->Draw(hdc);
			}
			if (isDown == true)
			{
				
				switch (curShapeMode)
				{
					case SM_LINE:
					{
						MoveToEx(hdc, p1.x, p1.y, NULL);
						LineTo(hdc, p2.x, p2.y);
					}break;
					case SM_RECTANGLE:
					{
						Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);

					}break;
					case SM_ELLIPSE:
					{
						Ellipse(hdc, p1.x, p1.y, p2.x, p2.y);
					}break;
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