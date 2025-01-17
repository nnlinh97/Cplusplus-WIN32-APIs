

#include <windows.h>
#include <Windows.h>
#include <windowsX.h>

#include "stdafx.h"
#include "QuickLaunch.h"
#include <Commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include "Strsafe.h"

#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <shellapi.h>
#include <msi.h>
#include <Shlobj.h>
#pragma comment(lib, "Msi.lib")
using namespace std;
#define MAX_LOADSTRING 100
#define COLOR_B			RGB(255, 128, 0)
#define COLOR_A			RGB(244,194,13)
#define COLOR_C			RGB(156,39,176)
#define COLOR_D			RGB(219,40,54)
#define COLOR_E			RGB(60,176,84)
#define COLOR_F			RGB(141,119,99)
#define COLOR_G			RGB(72,123,237)
#define COLOR_TOOLBAR	RGB(44, 144, 244)
#define COLOR_BACKGROUND	RGB(240, 240, 240)
#define COLOR_WHITE		RGB(255,255,255)
#define COLOR_BLACK		RGB(0, 0, 0)
#define DEFAULT_COLOR	RGB(255,255,255)
COLORREF colorList[] = { COLOR_A, COLOR_B, COLOR_C, COLOR_D, COLOR_E, COLOR_F, COLOR_G };

UINT EXIT = 100;
UINT SCAN = 200;
UINT VIEW_STATITISTIC = 300;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	MyHookProc(int, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ViewStatitisticDialog(HWND, UINT, WPARAM, LPARAM);


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
    LoadStringW(hInstance, IDC_QUICKLAUNCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QUICKLAUNCH));

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

HWND SearchBox, lblSearchBox;
HWND ListView;
HWND lblQuickLaunch;
HWND hToolbarText, hAnnoTextA, hAnnoTextB, hAnnoTextC, hAnnoTextD, hAnnoTextE, hAnnoTextF, hAnnoTextG;
HINSTANCE ghInstance;
WCHAR keyword[100];
NOTIFYICONDATA notify;
RECT rcClient;
HHOOK hHook = NULL;
HINSTANCE hinstLib;
bool hide = false;
HWND g_hWnd;
float *Per;
struct Program
{
	int index;
	wstring Name;
	wstring Icon;
	wstring Location;
	int frequency = 0;
};
struct FiletoUseDraw
{
	vector<int> FUDProgramList;
	vector<wstring> FUDProgramNameList;
};
vector<int> FreqProgramList;
vector<wstring> FreqProgramNameList;
FiletoUseDraw UsedAppList;
vector<Program> AppNameList;

vector<Program> ShowList;

void deepPathSearch(TCHAR* applicationPath, int depthLevel);
void LoadDataToListView(vector<Program> list);
void ScanProgramFileX86();
void ScanProgramFile();
void ReadProgramFrequency(wstring filePath);
bool isExecutableFile(WCHAR file[]);
void deepPathSearch(TCHAR* applicationPath, int depthLevel);
void InsertFrequencyToDatabase();
void InsertColLV();
wstring fixExecutablePath(wstring path);
void RunProgram(int index);
void SearchList(wstring keyword);
void ScanDatabase();
void WriteProgramFrequency(wstring filePath);
void _doInstallHook(HWND hWnd);
void _doRemoveHook(HWND hWnd);
void AddNotificationIcon(HWND hWnd);
void fillRect(HDC hdc, int x1, int y1, int x2, int y2, COLORREF clr);
void ReadProgramFrequency1(wstring filePath);






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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUICKLAUNCH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_QUICKLAUNCH);
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
      CW_USEDEFAULT, 0, 500, 680, nullptr, nullptr, hInstance, nullptr);

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
	g_hWnd = hWnd;
    switch (message)
    {
	case WM_CREATE:
	{
		HFONT hFont;
		_doInstallHook(hWnd);
		AddNotificationIcon(hWnd);
		hFont = CreateFont(30, 0, 0, 0, FW_BOLD, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");

		lblQuickLaunch = CreateWindowEx(0, L"STATIC", L"Final Project - QuickLaunch", WS_CHILD | WS_VISIBLE, 70, 30, 350, 30, hWnd, (HMENU)NULL, ghInstance, NULL);
		SendMessage(lblQuickLaunch, WM_SETFONT, WPARAM(hFont), true);

		hFont = CreateFont(18, 0, 0, 0, FW_BOLD, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");

		SearchBox = CreateWindowEx(0, L"EDIT", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE, 40, 100, 400, 25, hWnd, NULL, hInst, NULL);
		SendMessage(SearchBox, WM_SETFONT, WPARAM(hFont), TRUE);

		lblSearchBox = CreateWindowEx(0, L"STATIC", L"Search: ",
			WS_CHILD | WS_VISIBLE, 40, 80, 50, 20, hWnd, NULL, hInst, NULL);
		SendMessage(lblSearchBox, WM_SETFONT, WPARAM(hFont), TRUE);

		hFont = CreateFont(13, 0, 0, 0, FW_BOLD, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");

		

		ListView = CreateWindow(WC_LISTVIEW, L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL | LVS_REPORT, 40, 140, 400, 450, hWnd, (HMENU)IDM_LISTVIEW, ghInstance, NULL);
		SendMessage(ListView, WM_SETFONT, WPARAM(hFont), TRUE);
		InsertColLV();

		ScanProgramFileX86();
		ScanProgramFile();

		ReadProgramFrequency(L"program_frequency.txt");
		InsertFrequencyToDatabase();

		//Load data
		ShowList = AppNameList;
		LoadDataToListView(AppNameList);
	}
	break;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkColor(hdcStatic, RGB(255, 255, 255));
		return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
	}break;
	case WM_KEYDOWN:
	{
		_doInstallHook(hWnd);
	}
	break;
	case WM_USER + 1:
	{
		switch (lParam)
		{
		case WM_RBUTTONDOWN:
		{
			HMENU hMenu = CreatePopupMenu();

			if (hMenu) {
				InsertMenu(hMenu, -1, MF_BYPOSITION, EXIT, L"Exit");
				InsertMenu(hMenu, -1, MF_BYPOSITION, VIEW_STATITISTIC, L"View statitistics");
				InsertMenu(hMenu, -1, MF_BYPOSITION, SCAN, L"Scan to build database");

				POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow(hWnd);
				UINT clicked = TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL);

				if (clicked == VIEW_STATITISTIC) {
					
					DialogBox(hInst, MAKEINTRESOURCE(IDD_STATISTIC), hWnd, ViewStatitisticDialog);
				}
				else if (clicked == SCAN) //scan database
				{
					ShowList.clear();
					AppNameList.clear();
					ScanDatabase();
					LoadDataToListView(AppNameList);
					ShowList = AppNameList;
				}
				else if (clicked == EXIT)
				{
					DestroyWindow(hWnd);
				}
				PostMessage(hWnd, WM_NULL, 0, 0);
				DestroyMenu(hMenu);
			}

		}
			break;
		}
	}

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
			int wmEvent = HIWORD(wParam);
            // Parse the menu selections:
			if ((HWND)lParam == SearchBox)
			{
				if (wmEvent == EN_CHANGE)
				{
					GetWindowText(SearchBox, keyword, 100);
					SearchList(keyword);
					LoadDataToListView(ShowList);
				}
			}
            switch (wmId)
            {
			case IDM_STATISTIC:
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_STATISTIC),
					hWnd, ViewStatitisticDialog);
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

	case WM_NOTIFY:
	{
		int nCurSelIndex;
		NMHDR* notifyMess = (NMHDR*)lParam; 
		LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)notifyMess;
		HTREEITEM currSelected;

		switch (notifyMess->code)
		{
		case NM_DBLCLK:
			if (notifyMess->hwndFrom == ListView)
			{
				int i = ListView_GetSelectionMark(ListView);
				RunProgram(i);
			}
			break;
		case NM_RETURN:
			if (notifyMess->hwndFrom == ListView)
			{
				int i = ListView_GetSelectionMark(ListView);
				RunProgram(i);
			}
			break;
		}
	}
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			
			EndPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		break;
    case WM_DESTROY:
		WriteProgramFrequency(L"program_frequency.txt");
		_doRemoveHook(hWnd);
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
		hToolbarText = CreateWindowEx(0, L"STATIC", L"Statistic Chart", WS_CHILD | WS_VISIBLE, 20, 10, 100, 24, hDlg, NULL, NULL, NULL);
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
void _doInstallHook(HWND hWnd)
{
	if (hHook != NULL) return;
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)MyHookProc, hinstLib, 0);

}
void _doRemoveHook(HWND hWnd)
{
	if (hHook == NULL) return;
	UnhookWindowsHookEx(hHook);
	hHook = NULL;
}
LRESULT CALLBACK MyHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode < 0)
	{
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	}

	if ((GetAsyncKeyState(VK_CONTROL) < 0) && (GetAsyncKeyState('L') < 0))
	{
		if (hide)
		{
			ShowWindow(g_hWnd, SW_SHOWDEFAULT);
			hide = false;
		}
		else
		{
			hide = true;
			ShowWindow(g_hWnd, SW_MINIMIZE);

		}
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);

}
void ScanProgramFileX86()
{
	
	WIN32_FIND_DATA  wfd; // Contains information about the file that is found by Find first file and Find next file
	TCHAR* programFileRoot = new TCHAR[wcslen(L"C:\\Program Files (x86)") + 2];
	StrCpy(programFileRoot, L"C:\\Program Files (x86)");
	StrCat(programFileRoot, _T("\\*"));

	HANDLE hFile = FindFirstFileW(programFileRoot, &wfd);
	bool found = true;

	//If the function fails or fails to locate files from the search string
	if (hFile == INVALID_HANDLE_VALUE) {
		found = false;
		return;
	}
	while (found)
	{
		TCHAR* applicationPath;
		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //Get only directory and folder
			&& ((wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& ((wfd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
			&& (StrCmp(wfd.cFileName, _T(".")) != 0) && (StrCmp(wfd.cFileName, _T("..")) != 0))
		{
			applicationPath = new TCHAR[wcslen(L"C:\\Program Files (x86)") + wcslen(wfd.cFileName) + 2];
			StrCpy(applicationPath, L"C:\\Program Files (x86)");
			StrCat(applicationPath, L"\\");
			StrCat(applicationPath, wfd.cFileName);
			deepPathSearch(applicationPath, 0);
		}
		found = FindNextFileW(hFile, &wfd);
	}
}
void ScanProgramFile()
{
	//Init find_data struct
	WIN32_FIND_DATA  wfd; //Contains information about the file that is found by Find first file and Find next file
	TCHAR* programFileRoot = new TCHAR[wcslen(L"C:\\Program Files") + 2];
	StrCpy(programFileRoot, L"C:\\Program Files");
	StrCat(programFileRoot, _T("\\*"));

	HANDLE hFile = FindFirstFileW(programFileRoot, &wfd);
	bool found = true;

	//If the function fails or fails to locate files from the search string
	if (hFile == INVALID_HANDLE_VALUE) {
		found = false;
		return;
	}

	while (found)
	{
		TCHAR* applicationPath;
		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //Get only directory and folder
			&& ((wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& ((wfd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
			&& (StrCmp(wfd.cFileName, _T(".")) != 0) 
			&& (StrCmp(wfd.cFileName, _T("..")) != 0))
		{
			applicationPath = new TCHAR[wcslen(L"C:\\Program Files") + wcslen(wfd.cFileName) + 2];
			StrCpy(applicationPath, L"C:\\Program Files");
			StrCat(applicationPath, L"\\");
			StrCat(applicationPath, wfd.cFileName);

			deepPathSearch(applicationPath, 0);
		}
		found = FindNextFileW(hFile, &wfd);
	}
}

bool isExecutableFile(WCHAR file[])
{
	if (wcsstr(file, L".exe") != nullptr || wcsstr(file, L".EXE") != nullptr)
	{
		//Contain .exe
		return true;
	}

	return false;
}
void deepPathSearch(TCHAR* applicationPath, int depthLevel)
{
	//Determine depth search level
	if (depthLevel >= 2)
		return;
	if (applicationPath == NULL || wcslen(applicationPath) == 0) {
		return;
	}

	TCHAR* appContent = new TCHAR[wcslen(applicationPath) + 2];
	StrCpy(appContent, applicationPath);
	StrCat(appContent, L"\\*");

	WIN32_FIND_DATA wfd; // Contains information about the file that is found by Find first file and Find next file
	HANDLE hFile = FindFirstFileW(appContent, &wfd);
	bool isFound = true;
	if (hFile == INVALID_HANDLE_VALUE) {
		isFound = false;
	}
	while (isFound) {
		if (depthLevel == 1) {
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) //Get only directory and folder
			{
				isFound = FindNextFileW(hFile, &wfd);
				continue;
			}
		}
		if ((StrCmp(wfd.cFileName, _T(".")) != 0) && (StrCmp(wfd.cFileName, _T("..")) != 0)) {
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //Get only directory and folder
			{
				//Recursion to dive to next depth level
				TCHAR* childDir;
				childDir = new TCHAR[wcslen(applicationPath) + wcslen(wfd.cFileName) + 2];
				StrCpy(childDir, applicationPath);
				StrCat(childDir, L"\\");
				StrCat(childDir, wfd.cFileName);
				deepPathSearch(childDir, depthLevel + 1);

				//Make GC happy
				delete[] childDir;
			}
			else {
				if (isExecutableFile(wfd.cFileName))
				{
					//Gotcha!
					TCHAR* childFile;
					childFile = new TCHAR[wcslen(applicationPath) + wcslen(wfd.cFileName) + 2];
					StrCpy(childFile, applicationPath);
					StrCat(childFile, L"\\");
					StrCat(childFile, wfd.cFileName);

					//Save data
					Program p;
					p.Location = applicationPath;
					p.Icon = childFile;
					p.Name = wfd.cFileName;
					AppNameList.push_back(p);
				}
			}
		}
		isFound = FindNextFileW(hFile, &wfd);
	}
}
void ReadProgramFrequency1(wstring filePath)
{
	wfstream f;
	f.open(filePath, ios::in);

	wstring buffer;
	if (f.is_open() == true)
	{
		//Get items
		int i = -1;
		while (getline(f, buffer))
		{
			FreqProgramNameList.push_back(buffer);
			getline(f, buffer);
			FreqProgramList.push_back(_wtoi64(buffer.c_str()));
		}
	}
	//Close file
	f.close();
}
void ReadProgramFrequency(wstring filePath)
{
	wfstream f;
	f.open(filePath, ios::in);

	wstring buffer;
	if (f.is_open() == true)
	{
		//Get items
		int i = -1;
		while (getline(f, buffer))
		{
			i++;
			UsedAppList.FUDProgramNameList.push_back(buffer);
			getline(f, buffer);
			UsedAppList.FUDProgramList.push_back(_wtoi64(buffer.c_str()));
		}
	}
	//Close file
	f.close();
}
void InsertFrequencyToDatabase()
{
	for (int i = 0; i < AppNameList.size(); i++)
	{
		AppNameList.at(i).index = i;
		for (int j = 0; j < FreqProgramNameList.size(); j++)
		{
			if (AppNameList.at(i).Name == FreqProgramNameList.at(j))
			{
				AppNameList.at(i).frequency = FreqProgramList.at(j);
				break;
			}
		}
	}
}
void LoadDataToListView(vector<Program> list)
{
	ListView_DeleteAllItems(ListView);
	for (int i = 0; i < list.size(); i++)
	{
		//Add to ListView
		LV_ITEM lv;

		lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;

		//Insert type to first column
		lv.iItem = i;
		lv.iSubItem = 0;
		lv.pszText = (LPWSTR)list.at(i).Name.c_str();
		//lv.iImage
		//lv.lParam
		ListView_InsertItem(ListView, &lv);

		
	}
}
void InsertColLV()
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 450;
	lvCol.pszText = (LPWSTR)L"Name";
	ListView_InsertColumn(ListView, 0, &lvCol);
}
wstring fixExecutablePath(wstring path)
{
	int index = path.find(L"exe");
	return path.substr(0, index + 3);
}
void RunProgram(int index)
{
	wstring exeString = ShowList.at(index).Icon;
	if (!exeString.empty())
	{
		exeString = fixExecutablePath(exeString);
	}
	else
	{
		exeString = ShowList.at(index).Location;
	}

	//Increase frequency
	AppNameList.at(ShowList.at(index).index).frequency++;

	ShellExecute(NULL, _T("open"), exeString.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
void SearchList(wstring keyword)
{
	ShowList.clear();
	for (int i = 0; i < AppNameList.size(); i++)
	{
		if (AppNameList.at(i).Name.find(keyword) != -1)
		{
			ShowList.push_back(AppNameList.at(i));
		}
	}
}
void AddNotificationIcon(HWND hWnd)
{
	//add icon to notification area 
	InitCommonControls();

	ZeroMemory(&notify, sizeof(NOTIFYICONDATA));
	notify.cbSize = sizeof(NOTIFYICONDATA);
	notify.uID = IDI_ICON1;
	notify.hBalloonIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	notify.hIcon = (HICON)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	StringCchCopy(notify.szInfoTitle, ARRAYSIZE(notify.szInfoTitle), L"QuickLaunch is running.");
	StringCchCopy(notify.szInfo, ARRAYSIZE(notify.szInfo), L" ");
	notify.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_INFO;
	notify.hWnd = hWnd;
	notify.uCallbackMessage = WM_USER + 1;
	StringCchCopy(notify.szTip, ARRAYSIZE(notify.szTip), L"Quick Launch");
	Shell_NotifyIcon(NIM_MODIFY, &notify); 
	Shell_NotifyIcon(NIM_ADD, &notify); 
}
void ScanDatabase()
{
	ScanProgramFile();
	ScanProgramFileX86();
	InsertFrequencyToDatabase();
}
void WriteProgramFrequency(wstring filePath)
{
	wofstream f(filePath);

	for (int i = 0; i < AppNameList.size(); i++)
	{
		if (AppNameList.at(i).frequency > 0)
		{
			f << AppNameList.at(i).Name << endl;
			f << AppNameList.at(i).frequency << endl;
		}
	}

	//Close file
	f.close();

}
INT_PTR CALLBACK ViewStatitisticDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH dlgTextBackground;
	HFONT hFont = CreateFont(30, 0, 0, 0, FW_SEMIBOLD, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		case WM_INITDIALOG:
			
			hToolbarText = CreateWindowEx(0, L"STATIC", L"Statistic Chart", WS_CHILD | WS_VISIBLE, 320, 5, 300, 40, hDlg, NULL, NULL, NULL);
			SendMessage(hToolbarText, WM_SETFONT, (WPARAM)hFont, true);
			dlgTextBackground = CreateSolidBrush(RGB(255, 255, 255));
			break;
			return (INT_PTR)TRUE;
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, RGB(255, 255, 255));
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
		}break;
		case WM_CTLCOLORDLG:
			return (INT_PTR)dlgTextBackground;
			break;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hDlg, &ps);
			// TODO: Add any drawing code that uses hdc here...
			WriteProgramFrequency(L"program_frequency.txt");
			ReadProgramFrequency(L"program_frequency.txt");
			if (UsedAppList.FUDProgramNameList.size() > 0)
			{
				int SumFreqOfUsedApp = 0;
				for (int i = 0; i < UsedAppList.FUDProgramNameList.size(); i++)
				{
					SumFreqOfUsedApp += UsedAppList.FUDProgramList[i];
				}
				//sort
				for (int i = 0; i < UsedAppList.FUDProgramNameList.size() - 1; i++)
				{
					for (int j = i + 1; j < UsedAppList.FUDProgramNameList.size(); j++)
					{
						if (UsedAppList.FUDProgramList[i] < UsedAppList.FUDProgramList[j])
						{
							swap(UsedAppList.FUDProgramList[i], UsedAppList.FUDProgramList[j]);
							swap(UsedAppList.FUDProgramNameList[i], UsedAppList.FUDProgramNameList[j]);
						}
					}
				}
				//pick to draw
				
				wstringstream wss;
				int x = 100;
				int y = 120;
				int width = 200;
				int height = 30;
				int margin = 0;
				if (UsedAppList.FUDProgramNameList.size() < 3)
				{
					for (int i = 0; i < UsedAppList.FUDProgramNameList.size(); i++)
					{
						
						Per = new float[UsedAppList.FUDProgramNameList.size()];
						for (int i = 0; i < UsedAppList.FUDProgramNameList.size(); i++)
						{
							Per[i] = 0;
						}
						for (int i = 0; i < UsedAppList.FUDProgramNameList.size(); i++)
						{
							Per[i] = 1.0 * UsedAppList.FUDProgramList[i]*100 / (1.0* SumFreqOfUsedApp);
						}
						wss.str(L"");
						wss << UsedAppList.FUDProgramNameList[i] << " (" << int(Per[i]) << "%)";
						HFONT hFont = CreateFont(20, 0, 0, 0, FW_SEMIBOLD, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");

						HWND hWndName = CreateWindowEx(0, L"Static", wss.str().c_str(), WS_VISIBLE | WS_CHILD, x, y, width, height, hDlg, NULL, NULL, NULL);
						SendMessage(hWndName, WM_SETFONT, (WPARAM)hFont, true);
						fillRect(hdc, 400, 117 + margin, 400 + Per[i] * 3, 117 + height + margin, colorList[i]);
						y += 45;
						margin += 45;
					}
				}
				else
				{
					for (int i = 0; i < 3; i++)
					{
						Per = new float[3];
						for (int i = 0; i < 3; i++)
						{
							Per[i] = 0;
						}
						for (int i = 0; i < 3; i++)
						{
							Per[i] = 1.0 * UsedAppList.FUDProgramList[i]*100 / (1.0* SumFreqOfUsedApp);
						}
						wss.str(L"");
						wss << UsedAppList.FUDProgramNameList[i] << " (" << int(Per[i]) << "%)";
						HFONT hFont = CreateFont(20, 0, 0, 0, FW_SEMIBOLD, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");

						HWND hWndName = CreateWindowEx(0, L"Static", wss.str().c_str(), WS_VISIBLE | WS_CHILD, x, y, width, height, hDlg, NULL, NULL, NULL);
						y += 45;
						SendMessage(hWndName, WM_SETFONT, (WPARAM)hFont, true);
						fillRect(hdc, 400, 117 + margin, 400 + Per[i] * 5, 117 + height + margin, colorList[i]);
						margin += 45;
					}
				}


			}
			else
			{
				HFONT hFont = CreateFont(35, 0, 0, 0, FW_SEMIBOLD, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Times New Roman");

				HWND notify = CreateWindowEx(0, L"STATIC", L"Chưa có ứng dụng nào chạy cả!!!", WS_CHILD | WS_VISIBLE, 200, 150, 450, 40, hDlg, NULL, NULL, NULL);
				SendMessage(notify, WM_SETFONT, (WPARAM)hFont, true);
			}
			EndPaint(hDlg, &ps);
			EndPaint(hDlg, &ps);

		}break;
	}


	return (INT_PTR)FALSE;
}
void fillRect(HDC hdc, int x1, int y1, int x2, int y2, COLORREF clr)
{
	RECT* rect = new RECT;
	rect->left = x1;
	rect->top = y1;
	rect->right = x2;
	rect->bottom = y2;

	HBRUSH hbrush = CreateSolidBrush(clr);
	FillRect(hdc, rect, hbrush);
}
