// MyExplorer.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <Windows.h>
#include "stdafx.h"
#include "MyExplorer.h"
#include <Commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;



#define BUFFER_LEN 256
#define MAX_LOADSTRING 256

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void LoadToTV();
LPCWSTR GetPath(HWND hWndTV, HTREEITEM hItem);
void PreLoad(HWND TV, HTREEITEM hItem);
void PreloadExpanding(HWND TV, HTREEITEM hCurSel);
void LoadChild(HWND TreeView, HTREEITEM hParent);
LPWSTR ChangeByte(unsigned long long size);
void InitDriveColumn(HWND ListView);
void InitFolderCol(HWND ListView);
void LoadToLV();
LPWSTR GetType(LPWSTR name);
LPWSTR GetDateModified(const FILETIME &ftLastWrite);
void LoadFile(HWND ListView, LPCWSTR path);
LPCWSTR GetPathLV(HWND ListView, int iItem);
LPCWSTR ToString(int num);
int getlenInt(int num);
static bool flag = false;

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
    LoadStringW(hInstance, IDC_MYEXPLORER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYEXPLORER));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYEXPLORER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYEXPLORER);
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
      CW_USEDEFAULT, 0, 1000, 600, nullptr, nullptr, hInstance, nullptr);

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

struct DRIVE
{
	TCHAR Drive[26][4];
	TCHAR DisPlayName[26][256];
	TCHAR VolName[26][100];
	int Index[128];
	int Count = 0;
};
static DRIVE Dr;
static HWND TreeView;
static HWND ListView;
static LV_COLUMN LvCol;
static RECT rect;
static RECT tv;
static RECT lv;
static RECT srn;


static int wTV;
static int hTV;
static int wLV;
static int hLV;
static int xDOWN;
static int xUP;
static int change;
static int len;
static bool changeH = false;
static bool changeW = false;
static HWND statusbar;
static const int BUFFERSIZE = 260;
static wchar_t istr[10];
static int in = 0;
static WCHAR buffer1[BUFFERSIZE];

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	GetClientRect(hWnd, &srn);

	//WCHAR buffer1[BUFFERSIZE];
	WCHAR curPath[BUFFERSIZE];
	WCHAR configPath[BUFFERSIZE];
	GetCurrentDirectory(BUFFERSIZE, curPath);
	wsprintf(configPath, L"%s\\config.ini", curPath);
	WritePrivateProfileString(L"MyExplorer", L"widthOfhWnd", ToString(srn.right), configPath);
	GetPrivateProfileString(L"MyExplorer", L"widthOfhWnd", L"nothing", buffer1, BUFFERSIZE, configPath);
    switch (message)
    {
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		wTV = 400;
		hTV = rect.bottom;
		hLV = hTV;
		wLV = rect.right - wTV - 2;
		TreeView = CreateWindow( WC_TREEVIEW
			, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT, 0, 0, wTV, hTV, hWnd, (HMENU)IDM_TREEVIEW, hInst, NULL);
		ListView = CreateWindow( WC_LISTVIEW, L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL | LVS_REPORT, wTV + 2, 0, wLV, hLV, hWnd, (HMENU)IDM_LISTVIEW, hInst, NULL);

		InitCommonControls();
		statusbar = CreateWindow(STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hWnd, NULL, hInst, NULL);



		LoadToTV();
		LoadToLV();
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &rect);
		GetClientRect(TreeView, &tv);
		GetClientRect(ListView, &lv);
		hTV = rect.bottom;
		hLV = rect.bottom;
		len = tv.right + lv.right + 2;
		if (rect.right > len){
			change = rect.right - len;
			wTV = tv.right + (int)(change * 2 / 5);
			wLV = lv.right + change - (int)(change * 2 / 5);
		}
		
		else {
			change = -rect.right + len;
			wTV = tv.right - (int)(change * 2 / 5);
			wLV = lv.right - change + (int)(change * 2 / 5);
		}
		if (wTV < 70){
			wTV = 70;
			wLV = rect.right - 70 - 2;
		}
		if (wLV < 70) {
			wLV = 70;
			wTV = rect.right - 70 - 2;
		}
		MoveWindow(TreeView, 0, 0, wTV, hTV, 1);
		MoveWindow(ListView, wTV + 2, 0, wLV, hLV, 1);
		SendMessage(statusbar, WM_SIZE, 0, 0);
		break;
	case WM_LBUTTONUP:
		flag = false;
		xUP = (int)LOWORD(lParam);
		ReleaseCapture();
		break;
	case WM_LBUTTONDOWN:
		int xPos;
		int yPos;
		xPos = (int)LOWORD(lParam);
		yPos = (int)HIWORD(lParam);
		xDOWN = xPos;
		if (xPos >= wTV && xPos <= wTV + 2){
			flag = true;
			SetCapture(hWnd);
			//SetCursor(hcSizeEW);
		}

		break;
	case WM_MOUSEMOVE:
		int x;
		int y;
		x = (int)LOWORD(lParam);
		y = (int)HIWORD(lParam);
		GetClientRect(hWnd, &rect);
		if (x >= wTV && x <= wTV + 2){
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		}
		if (flag == true){
			if (x < 100) {
				wLV = rect.right - 100 - 2;
				wTV = 100;
			}
			else if (x > rect.right - 100){
				wLV = 100 - 2;
				wTV = rect.right - 100;
			}
			else{
				wLV = rect.right - x - 2;
				wTV = x;
			}
			//hTV = rect.bottom;
			//hLV = rect.bottom;
			MoveWindow(TreeView, 0, 0, wTV, hTV, 1);
			MoveWindow(ListView, wTV + 2, 0, wLV, hLV, 1);
		}
		break;
	case WM_NOTIFY:
	{
		UINT code = ((LPNMHDR)lParam)->code;
		//NMHDR *pnm;
		//LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)pnm;

		//switch (pnm->code)
		switch (code){
		case TVN_ITEMEXPANDING:
			PreloadExpanding(TreeView, ((LPNMTREEVIEW)(LPNMHDR)lParam)->itemNew.hItem);
			//PreloadExpanding(TreeView, lpnmTree->itemNew.hItem);
			break;
		case TVN_SELCHANGED:
			{
				ListView_DeleteAllItems(ListView); //xóa list view
				LPCWSTR m_path = (LPCWSTR)((LPNMTREEVIEW)(LPNMHDR)lParam)->itemNew.lParam;
				LoadFile(ListView, m_path);
				break;
			}
		case NM_DBLCLK:
			if (((LPNMHDR)lParam)->hwndFrom == ListView){
				//ListView_DeleteAllItems(ListView);
				if (ListView_GetSelectionMark(ListView) != -1){
					LPCWSTR m_path = GetPathLV(ListView, ListView_GetSelectionMark(ListView));
					ListView_DeleteAllItems(ListView);
					LoadFile(ListView, m_path);
				}
			}
			break;
		case NM_CLICK:
			if (((LPNMHDR)lParam)->hwndFrom == ListView) {
				if (ListView_GetSelectionMark(ListView) != -1) {
					LPCWSTR m_path = GetPathLV(ListView, ListView_GetSelectionMark(ListView));
					//LPCWSTR m_path = GetPathLV(ListView, ((LPNMITEMACTIVATE)lParam)->iItem);
					WIN32_FIND_DATA fd;
					GetFileAttributesEx(m_path, GetFileExInfoStandard, &fd);
					if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)) {
						SendMessage(statusbar, SB_SETTEXT, 0, (LPARAM)ChangeByte(fd.nFileSizeLow));
						SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)buffer1);
					}

				}
			}
			break;
		}
	}
	break;
	
	//GetPrivateProfileString(L"MyExplorer", L"title", L"nothing", buffer, BUFFERSIZE, configPath);

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

void LoadToTV()
{
	HTREEITEM it;
	TV_INSERTSTRUCT tv;
	tv.hParent = TVI_ROOT;
	tv.hInsertAfter = TVI_LAST;
	tv.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tv.item.iImage = 0;
	tv.item.iSelectedImage = 0;
	tv.item.pszText = L"This PC";
	tv.item.lParam = (LPARAM)L"This PC";
	it = TreeView_InsertItem(TreeView, &tv);

	


	TCHAR buffer[BUFFER_LEN];
	int i;

	GetLogicalDriveStrings(BUFFER_LEN, buffer);

	//Đếm số lượng ổ đĩa 
	for (i = 0; !((buffer[i] == 0) && (buffer[i + 1] == 0)); ++i)
		if (buffer[i] == 0)
			Dr.Count++;
	Dr.Count++;
	int j, k;
	i = 0;

	//Lấy từng kí tự ổ đĩa
	for (j = 0; j < Dr.Count; ++j)
	{
		k = 0;
		while (buffer[i] != 0)
			Dr.Drive[j][k++] = buffer[i++];
		Dr.Drive[j][k] = 0; //Kết thúc chuỗi
		++i;
	}

	//Lấy volume cho từng ổ đĩa, kết hợp luôn với việc lấy tên hiển thị ứng với từng ổ
	int nType;



	for (i = 0; i < Dr.Count; i++)
	{
		tv.hParent = it;
		tv.hInsertAfter = TVI_LAST;
		tv.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		nType = GetDriveType(Dr.Drive[i]);
		wcscpy_s(buffer, _T(""));

		if (nType == DRIVE_FIXED)//Nếu là ổ cứng hay USB
		{
				Dr.Index[i] = 0;
				wcscpy_s(Dr.VolName[i], L"Local disk");
				GetVolumeInformation(Dr.Drive[i], buffer, BUFFER_LEN, NULL, NULL, NULL, NULL, 0);
				if (buffer[0] == 0)
					wcscat_s(Dr.DisPlayName[i], L"Local disk");
				else
					wcscpy_s(Dr.DisPlayName[i], buffer);
		}
		else if (nType == DRIVE_CDROM)
		{
				GetVolumeInformation(Dr.Drive[i], buffer, BUFFER_LEN, NULL, NULL, NULL, NULL, 0);
				if (wcslen(buffer) == 0)
					wcscpy_s(Dr.VolName[i], _T("CD Rom"));
				else
					wcscpy_s(Dr.VolName[i], buffer);
				Dr.Index[i] = 0;
			
		}
		else if (((i == 0) || (i == 1)) && (nType == DRIVE_REMOVABLE)) //Nếu là ổ mềm
		{
			wcscpy_s(Dr.VolName[i], _T("3½ Floppy"));
			Dr.Index[i] = 0;
		}
		else
		{
			wcscpy_s(Dr.VolName[i], _T("Removable disk"));
			GetVolumeInformation(Dr.Drive[i], buffer, BUFFER_LEN, NULL, NULL, NULL, NULL, 0);
			if (buffer[0] == 0)
				wcscpy_s(Dr.DisPlayName[i], _T("Removable disk"));
			else
				wcscpy_s(Dr.DisPlayName[i], buffer);
			Dr.Index[i] = 0;
		}
		//Thêm vào phần sau hai dấu mở ngoặc. Ví dụ: (C:) như ta thường thấy WinXp (C:)
		wcscat_s(Dr.DisPlayName[i], _T(" ("));
		wcsncat_s(Dr.DisPlayName[i], Dr.Drive[i], 1);
		wcscat_s(Dr.DisPlayName[i], _T(")"));

		tv.item.iImage = Dr.Index[i];
		tv.item.iSelectedImage = Dr.Index[i];
		tv.item.pszText = Dr.DisPlayName[i];
		tv.item.lParam = (LPARAM)Dr.Drive[i];
		HTREEITEM child = TreeView_InsertItem(TreeView, &tv);

		if (nType == DRIVE_FIXED || (nType == DRIVE_REMOVABLE))
		{
			tv.hParent = child;
			tv.item.pszText = _T("");
			tv.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(TreeView, &tv);
		}

	}
	TreeView_Expand(TreeView, it, TVE_EXPAND);
}

LPCWSTR GetPath(HWND TV, HTREEITEM hItem)
{
	TCHAR * buffer = new TCHAR[1024];
	TVITEM tv;
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(TV, &tv);
	return (LPCWSTR)tv.lParam;
}

void LoadChild(HWND TreeView, HTREEITEM hParent)
{
	LPCWSTR path = GetPath(TreeView, hParent);
	if (path == NULL)
		return;
	TCHAR buffer[1024];
	wcscpy_s(buffer, path);

	wcscat_s(buffer, _T("\\*"));

	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvInsert.item.iImage = 1;
	tvInsert.item.iSelectedImage = 2;

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);
	BOOL bFound = 1;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	TCHAR * folderPath;
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
			&& (wcscmp(fd.cFileName, L".") != 0) && wcscmp(fd.cFileName, L"..") != 0)
		{
			tvInsert.item.pszText = fd.cFileName;
			folderPath = new WCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			wcscpy_s(folderPath, wcslen(path) + wcslen(fd.cFileName) + 2, path);
			if (wcslen(path) != 3)
				wcscat_s(folderPath, wcslen(path) + wcslen(fd.cFileName) + 2, L"\\");
			wcscat_s(folderPath, wcslen(path) + wcslen(fd.cFileName) + 2, fd.cFileName);

			tvInsert.item.lParam = (LPARAM)folderPath;
			HTREEITEM hItem = TreeView_InsertItem(TreeView, &tvInsert);

			PreLoad(TreeView, hItem);
		}

		bFound = FindNextFileW(hFile, &fd);
	}
}

void PreloadExpanding(HWND TV, HTREEITEM hCurSel)
{
	HTREEITEM hCurSelChild = TreeView_GetChild(TV, hCurSel);

	if (!wcscmp(GetPath(TV, hCurSelChild), _T("PreLoad")))
	{
		TreeView_DeleteItem(TV, hCurSelChild);
		LoadChild(TV, hCurSel);
	}
}

void PreLoad(HWND TV, HTREEITEM hItem)
{
	TCHAR buffer[BUFFER_LEN];
	wcscpy_s(buffer, GetPath(TV, hItem));

	if (wcslen(buffer) == 3) 
	{
		if (wcscmp(buffer, _T("A:")) || wcscmp(buffer, _T("B:")))
			return;
	}
	else
		wcscat_s(buffer, _T("\\"));

	wcscat_s(buffer, _T("*"));

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	BOOL bFound = true;

	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& (wcscmp(fd.cFileName, _T(".")) != 0) && (wcscmp(fd.cFileName, _T("..")) != 0))
		{
			TV_INSERTSTRUCT tvInsert;
			tvInsert.hParent = hItem;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
			tvInsert.item.pszText = NULL;
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(TV, &tvInsert);
			bFound = FALSE;
		}
		else
			bFound = FindNextFileW(hFile, &fd);
	}
}

LPCWSTR GetPathLV(HWND ListView, int iItem)
{
	TCHAR * buf = new TCHAR[BUFFER_LEN];
	LVITEM Lv;
	Lv.mask = LVIF_PARAM;
	Lv.iItem = iItem;
	Lv.iSubItem = 0;
	Lv.lParam = (LPARAM)buf;
	ListView_GetItem(ListView, &Lv);
	return (LPCWSTR)Lv.lParam;
}

int getlenInt(int num)
{
	int i = 0;
	while (num != 0) {
		i++;
		num /= 10;
	}
	return i;
}

LPCWSTR ToString(int num) {
	_itow_s(num, istr, 10);
	return istr;
}


void InitDriveColumn(HWND ListView)
{

	LvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	LvCol.fmt = LVCFMT_LEFT;

	LvCol.cx = 120;
	LvCol.pszText = _T("name");
	ListView_InsertColumn(ListView, 0, &LvCol);

	LvCol.cx = 120;
	LvCol.pszText = _T("size");
	ListView_InsertColumn(ListView, 2, &LvCol);

	LvCol.cx = 120;
	LvCol.pszText = _T("type");
	ListView_InsertColumn(ListView, 1, &LvCol);


	LvCol.cx = 125;
	LvCol.pszText = _T("freespace");
	ListView_InsertColumn(ListView, 3, &LvCol);

}

void InitFolderCol(HWND ListView)

{
	LvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	LvCol.fmt = LVCFMT_LEFT;

	LvCol.cx = 180;
	LvCol.pszText = _T("name");
	ListView_InsertColumn(ListView, 0, &LvCol);

	LvCol.cx = 70;
	LvCol.fmt = LVCFMT_RIGHT;

	LvCol.pszText = _T("size");
	ListView_InsertColumn(ListView, 1, &LvCol);

	LvCol.cx = 80;
	LvCol.fmt = LVCFMT_LEFT;
	LvCol.pszText = _T("type");
	ListView_InsertColumn(ListView, 2, &LvCol);


	LvCol.cx = 155;
	LvCol.pszText = _T("time modified");
	ListView_InsertColumn(ListView, 3, &LvCol);

}

void LoadToLV()
{
	for (int i = 0; i < 4; i++)
		ListView_DeleteColumn(ListView, 0);
	InitDriveColumn(ListView);
	for (int i = 0; i < Dr.Count; i++)
	{
		LV_ITEM lv;
		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lv.iItem = i;
		lv.iImage = Dr.Index[i];

		lv.iSubItem = 0;
		lv.pszText = Dr.DisPlayName[i];
		lv.lParam = (LPARAM)Dr.Drive[i];
		ListView_InsertItem(ListView, &lv);
		lv.mask = LVIF_TEXT;

		lv.iSubItem = 1;
		lv.pszText = Dr.VolName[i];
		ListView_SetItem(ListView, &lv);

		if (Dr.Index[i] != 3)
		{
			lv.iSubItem = 2;
			unsigned long long size;
			SHGetDiskFreeSpaceEx(Dr.Drive[i], NULL, (PULARGE_INTEGER)&size, NULL);
			lv.pszText = ChangeByte(size);
			ListView_SetItem(ListView, &lv);

			lv.iSubItem = 3;
			SHGetDiskFreeSpaceEx(Dr.Drive[i], NULL, NULL, (PULARGE_INTEGER)&size);
			lv.pszText = ChangeByte(size);
			ListView_SetItem(ListView, &lv);
		}

	}
}

LPWSTR ChangeByte(unsigned long long size)
{
	//if (size == 0)
		//return L"";

	int type = 0;
	double dsize = (double)size;
	while (dsize >= 1024)
	{
		dsize /= 1024;
		type++;
	}
	if (dsize == 0)
		dsize = 1;
	WCHAR * res = new WCHAR[15];
	if (dsize == 0)
		return L"";
	if (type > 1)
		swprintf_s(res, 15, L"%.1f", dsize);
	else
		swprintf_s(res, 15, L"%.0f", dsize);

	switch (type)
	{
	case 0://Bytes
		wcscat_s(res, 15, _T(" bytes"));
		break;
	case 1:
		wcscat_s(res, 15, _T(" KB"));
		break;
	case 2:
		wcscat_s(res, 15, _T(" MB"));
		break;
	case 3:
		wcscat_s(res, 15, _T(" GB"));
		break;
		wcscat_s(res, 15, _T(" TB"));
	}
	return res;
}

void LoadFile(HWND ListView, LPCWSTR path)
{
	for (int i = 0; i < 4; i++)
		ListView_DeleteColumn(ListView, 0);
	if (path == NULL)
		return;
	if (wcscmp(path, L"This PC") == 0)
	{
		LoadToLV();
		return;
	}
	InitFolderCol(ListView);

	WCHAR buffer[1024];
	wcscpy_s(buffer, path);

	if (wcslen(path) == 3)
		wcscat_s(buffer, _T("*"));
	else
		wcscat_s(buffer, _T("\\*"));

	WIN32_FIND_DATA fd;
	HANDLE hFile;
	BOOL bFound = true;
	TCHAR * folderPath;
	int count = 0;

	hFile = FindFirstFileW(buffer, &fd);
	bFound = TRUE;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN) &&
			(wcscmp(fd.cFileName, L".") != 0) && (wcscmp(fd.cFileName, L"..") != 0))
		{
			folderPath = new WCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			wcscpy_s(folderPath, wcslen(path) + wcslen(fd.cFileName) + 2, path);

			if (wcslen(path) != 3)
				wcscat_s(folderPath, wcslen(path) + wcslen(fd.cFileName) + 2, L"\\");

			wcscat_s(folderPath, wcslen(path) + wcslen(fd.cFileName) + 2, fd.cFileName);
			LV_ITEM lv;
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = count;
			lv.iImage = 2;
			lv.iSubItem = 0;		
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)folderPath;
			ListView_InsertItem(ListView, &lv);

			ListView_SetItemText(ListView, count, 2, L"Folder");

			ListView_SetItemText(ListView, count, 3, GetDateModified(fd.ftLastWriteTime));
			count++;
		}
		bFound = FindNextFileW(hFile, &fd);
	}//while

	DWORD folderCount = count;

							  
	WCHAR * filePath;
	DWORD fileSizeCount = 0;
	DWORD fileCount = 0;

	hFile = FindFirstFileW(buffer, &fd);
	bFound = TRUE;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	while (bFound)
	{
		if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN))
		{
			filePath = new WCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			wcscpy_s(filePath, wcslen(path) + wcslen(fd.cFileName) + 2, path);

			if (wcslen(path) != 3)
				wcscat_s(filePath, wcslen(path) + wcslen(fd.cFileName) + 2, L"\\");

			wcscat_s(filePath, wcslen(path) + wcslen(fd.cFileName) + 2, fd.cFileName);
			LVITEM lv;
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = count;
			lv.iImage = 6;
			if (wcscmp(GetType(fd.cFileName), L"") == 0)
			lv.iImage = 7;
			lv.iSubItem = 0;	
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)filePath;
			ListView_InsertItem(ListView, &lv);

			ListView_SetItemText(ListView, count, 1, ChangeByte(fd.nFileSizeLow));


			ListView_SetItemText(ListView, count, 2, GetType(fd.cFileName));

			ListView_SetItemText(ListView, count, 3, GetDateModified(fd.ftLastWriteTime));

			count++;
			fileCount++;
		}
		bFound = FindNextFileW(hFile, &fd);
	}

	/*TVITEMEX tv;
	TCHAR *folder = new TCHAR[512];
	TCHAR *info = new TCHAR[256];

	tv.mask = TVIF_TEXT;
	tv.hItem = TreeView_GetNextItem(GetDlgItem(m_hParent, IDC_TREEVIEW), NULL, TVGN_CARET);
	tv.pszText = folder;
	tv.cchTextMax = 256;
	TreeView_GetItem(GetDlgItem(m_hParent, IDC_TREEVIEW), &tv);

	wsprintf(info, _T(" có %d thư mục con và %d tập tin. (Không tính ẩn và hệ thống)"), folderCount, fileCount);
	StrCat(folder, info);

	SendMessage(GetDlgItem(m_hParent, IDC_STATUSBAR), SB_SETTEXT, 0, (LPARAM)tv.pszText);
	SendMessage(GetDlgItem(m_hParent, IDC_STATUSBAR), SB_SETTEXT, 1, (LPARAM)CConverter::Convert(fileSizeCount));*/
}

LPWSTR GetType(LPWSTR name)
{
	int i;
	int len = wcslen(name);
	TCHAR* res = new TCHAR[50];
	for (i = len - 1; i >= 0 && name[i] != '.'; i--);
	if (i == -1)
		return L"FILE";
	wcscpy_s(res, 50, &name[i + 1]);
	_wcsupr_s(res, 50);
	return res;
}

LPWSTR GetDateModified(const FILETIME &ftLastWrite)
{
	SYSTEMTIME stUTC, stLocal;
	FileTimeToSystemTime(&ftLastWrite, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	TCHAR *buffer = new TCHAR[50];
	wsprintf(buffer, _T("%02d/%02d/%04d %02d:%02d %s"),
		stLocal.wDay, stLocal.wMonth, stLocal.wYear,
		(stLocal.wHour>12) ? (stLocal.wHour / 12) : (stLocal.wHour),
		stLocal.wMinute,
		(stLocal.wHour>12) ? (_T("PM")) : (_T("AM")));

	return buffer;
}