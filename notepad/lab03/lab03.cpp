// lab03.cpp : Defines the entry point for the application.
//
#include <Windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include "stdafx.h"
#include "lab03.h"
#include<string>
#include<iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif
using namespace std;
#define MAX_LOADSTRING 100
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND h;

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
    LoadStringW(hInstance, IDC_LAB03, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {

        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB03));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
		
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
			if (msg.message == WM_KEYDOWN)
				SendMessage(h, WM_KEYDOWN, msg.wParam, msg.lParam);
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB03));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB03);
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
      CW_USEDEFAULT, 0, 800, 500, nullptr, nullptr, hInstance, nullptr);
   h = hWnd;
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

static HWND box;

const int BUFFER_SIZE = MAX_PATH;
CHOOSEFONT cf;
static LOGFONT lf;
HFONT hfont;
COLORREF g_editcolor = RGB(0, 0, 0);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static OPENFILENAME ofn;
	static WCHAR link[260] = { 0 };
	ofn.lpstrFile = link;
	
    switch (message)
    {
	case WM_CREATE:
		box = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE| WS_HSCROLL | WS_VSCROLL, 0, 0, 800, 500, hWnd, NULL, hInst, NULL);
		break;
	case WM_SIZE:
		MoveWindow(box, 0, 0, LOWORD(lParam), HIWORD(lParam), 1);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_EDIT_COPY:
				SendMessage(box, WM_COPY, 0, 0);
				break;
			case ID_EDIT_CUT:
				SendMessage(box, WM_CUT, 0, 0);
				break;
			case ID_EDIT_PASTE:
				SendMessage(box, WM_PASTE, 0, 0);
				break;
			case ID_EDIT_SELECTALL:
				SendMessage(box, EM_SETSEL, 0, -1);
				break;
			case ID_EDIT_UNDO:
				SendMessage(box, WM_UNDO, 0, 0);
				break;
			case ID_FILE_OPEN:
			{
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.nMaxFile = BUFFER_SIZE;
				ofn.lpstrFilter = L"Text document (*.txt) \0*.txt\0All file (*.*)\0*.*\0";
				ofn.nFilterIndex = 2;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST ;
				if (GetOpenFileName(&ofn) == TRUE)
				{
					ifstream file(ofn.lpstrFile, ifstream::binary);
					file.seekg(0, file.end);
					long size = file.tellg();
					file.seekg(0, file.beg);
					char * buf = new char[size + 1];
					memset(buf, 0, size);
					file.read(buf, size);
					buf[size] = '\0';
					SetWindowTextA(box, buf);
					file.close();
					delete[] buf;

					//ifstream file(ofn.lpstrFile, ifstream::binary);
					//file.imbue(std::locale(locale::empty(), new codecvt_utf16<wchar_t, 0x10ffff, consume_header>));
					//file.seekg(0, file.end);
					//long size;
					//size = file.tellg();
					//file.seekg(0, file.beg);
					//wchar_t * buf = new wchar_t[size / 2 + 1];
					//memset(buf, 0, size / 2 + 1);
					//file.read((char*)buf, size + 1);
					////buf[size] = L'\r\0';
					//SetWindowText(box, buf);
					//file.close();
					//delete[] buf;
					/*
					fstream f("x.txt", ios::in|ios::binary);
					int n;
					f.seekg(0, ios::end);
					n = f.tellg();
					f.seekg(0, ios::beg);

					wchar_t *str = new wchar_t[n / 2];
					f.read((char*)str, n);

					f.close();
					fstream ff("y.txt", ios::out | ios::binary);
					ff.write((char*)str, n);
					ff.close();
					*/
					
				}
			}
			break;
			case ID_FILE_SAVE:
			{
				long size = GetWindowTextLength(box);
				char *buf = new char[size + 1];
				//ZeroMemory(&ofn, sizeof(ofn));
				if (link[0] != 0)
				{
						ofstream file(ofn.lpstrFile, ofstream::binary);
						GetWindowTextA(box, buf, size + 1);
						buf[size] = '\0';
						file.write(buf, size);
						delete[] buf;
						file.close();
				}
				else
				{
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hWnd;
					ofn.nMaxFile = BUFFER_SIZE;
					ofn.lpstrFilter = L"Text document (*.txt) \0*.txt\0All file (*.*)\0*.*\0";
					ofn.nFilterIndex = 2;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
					if (GetSaveFileName(&ofn) == TRUE)
					{
						ofstream file(ofn.lpstrFile, ofstream::binary);
						GetWindowTextA(box, buf, size + 1);
						buf[size] = '\0';
						file.write(buf, size + 1);
						delete[] buf;
						file.close();
					}
				}
			}
				break;
			case ID_FILE_SAVEAS:
			{
				long size = GetWindowTextLength(box);
				char *buf = new char[size + 1];

				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.nMaxFile = BUFFER_SIZE;
				ofn.lpstrFilter = L"Text document (*.txt) \0*.txt\0All file (*.*)\0*.*\0";
				ofn.nFilterIndex = 2;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if (GetSaveFileName(&ofn) == TRUE)
				{
					ofstream file(ofn.lpstrFile, ofstream::binary);
					GetWindowTextA(box, buf, size);
					buf[size] = '\0';
					file.write(buf, size);
					delete[] buf;
					file.close();
				}
			}
				break;
			case ID_FORMAT_FONT:
			{
				ZeroMemory(&cf, sizeof(cf));
				cf.lStructSize = sizeof(cf);
				cf.hwndOwner = hWnd;
				cf.lpLogFont = &lf;
				cf.rgbColors = g_editcolor;
				cf.Flags = CF_SCREENFONTS | CF_EFFECTS;

				if (ChooseFont(&cf) == TRUE)
				{
					hfont = CreateFontIndirect(cf.lpLogFont);
					SendMessage(box, WM_SETFONT, (WPARAM)hfont, TRUE);
				}
			}
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
	case WM_KEYDOWN:
		if (GetKeyState(VK_CONTROL))
		{
			if (wParam == 'A')
				SendMessage(box, EM_SETSEL, 0, -1);
		}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
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
