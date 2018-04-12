// midterm.cpp : Defines the entry point for the application.
//
#include <windows.h>
#include <Windows.h>
#include "stdafx.h"
#include "midterm.h"
#include <Commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>

#include <ObjIdl.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include <locale>
#include <codecvt>
//#include <commctrl.h>

using namespace std;
using namespace Gdiplus;
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
    LoadStringW(hInstance, IDC_MIDTERM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MIDTERM));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MIDTERM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MIDTERM);
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
      CW_USEDEFAULT, 0, 800, 557, nullptr, nullptr, hInstance, nullptr);

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
//-------------------------------------------------------------------------------
#define COLOR_AU  RGB(255,128,128)	//hong
#define COLOR_DC  RGB(255,255,0)	//vang
#define COLOR_DV  RGB(0,255,64)		//xanh luc
#define COLOR_NC  RGB(128,255,255)	//xanh topaz
#define COLOR_NYP RGB(0,0,255)		//xanh nuoc bien
#define COLOR_XC  RGB(64,0,64)		//tim
#define COLOR_DF  RGB(255,255,255)	//trang

COLORREF color[7] = {COLOR_AU, COLOR_DC, COLOR_DV, COLOR_NC, COLOR_NYP, COLOR_XC, COLOR_DF};

//------------------------------------------------------------------------------
struct Item
{
	TCHAR iType[20];
	long long imoney;
	wstring iDescrpt;
};

//------------------------------------------------------------------------------
vector<Item*> ListItem;
TCHAR TypeCBB[6][25] = 
{
	L"Ăn uống", L"Di chuyển", L"Dịch vụ", L"Nhà cửa", L"Nhu yếu phẩm", L"Xe cộ"
};
static HWND ListView;
static HWND TreeView, g_hWnd;
static HWND hClose;
static HWND AddBtn;
static HWND hComboBox;
static HWND hmoney, SumMoney, vnd;
static HWND hDescrptIp, GroupBoxLV, GroupBoxTV, GroupBoxTK, hMoney;
static HWND hLoai, hTien, hNoidung;
long long TtMoney = 0;
long long *TMoney;
bool isAdd = false;
int LstItmSz = 0;
static wchar_t istr[10];
int curColor = 0;
int *curX;
bool flag = false;
//-------------------------------------------------------------------------
void InsertColLV();
bool InsertItemToLV(int iItem);
LPCWSTR ToString(long long num);
void AddItem();
void LoadListItemFrmFile(wstring file);
void LoadAllItemToListView();
void WritelistItemToFile(wstring file);
void fillRect(HDC hdc, int x1, int y1, int x2, int y2, COLORREF clr);
void DrawChart(HDC hdc);
int getTypeItem(Item *item);
void DrawCircle(HDC hdc);

//-------------------------------------------------------------------------


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(lf.lfHeight, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);
    switch (message)
    {
	
	case WM_CREATE:
	{

		ListView = CreateWindow(WC_LISTVIEW, L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL | LVS_REPORT, 280 , 60, 450, 200, hWnd, (HMENU)IDM_LISTVIEW, hInst, NULL);

		 GroupBoxLV = CreateWindowEx(0, L"BUTTON", L"Toàn bộ danh sách các chi tiêu",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP, 260, 30, 490, 250, hWnd, (HMENU)IDC_GROUPBOX, hInst, NULL);
		 SendMessage(GroupBoxLV, WM_SETFONT, WPARAM(hFont), TRUE);

		 GroupBoxTV = CreateWindowEx(0, L"BUTTON", L"Thêm một loại chi tiêu",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP, 30, 30, 210, 250, hWnd, (HMENU)IDC_GROUPBOX, hInst, NULL);
		 SendMessage(GroupBoxTV, WM_SETFONT, WPARAM(hFont), TRUE);

		 GroupBoxTK = CreateWindowEx(0, L"BUTTON", L"Thông tin thống kê",
			WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP, 30, 300, 724, 150, hWnd, (HMENU)IDC_GROUPBOX, hInst, NULL);
		 SendMessage(GroupBoxTK, WM_SETFONT, WPARAM(hFont), TRUE);

		hClose = CreateWindowEx(0, L"BUTTON", L"Thoát",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 358, 460, 80, 28, hWnd, (HMENU)IDC_BUTTON_CLOSE, hInst, NULL);
		SendMessage(hClose, WM_SETFONT, WPARAM(hFont), TRUE);

		AddBtn = CreateWindowEx(0, L"BUTTON", L"Thêm",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 90, 235, 90, 28, hWnd, (HMENU)IDC_BUTTON_ADD, hInst, NULL);
		SendMessage(AddBtn, WM_SETFONT, WPARAM(hFont), TRUE);

		hComboBox = CreateWindowEx(0, WC_COMBOBOX, TEXT(""),
			CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE| ES_AUTOVSCROLL,
			 60, 70, 150, 120, hWnd, NULL, hInst, NULL);
		SendMessage(hComboBox, WM_SETFONT, WPARAM(hFont), TRUE);


		hmoney = CreateWindowEx(0, L"EDIT", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,  60, 110, 150, 20, hWnd, NULL, hInst, NULL);
		SendMessage(hmoney, WM_SETFONT, WPARAM(hFont), TRUE);


		hDescrptIp = CreateWindowEx(0, L"EDIT", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 60, 150, 150, 70, hWnd, NULL, hInst, NULL);
		SendMessage(hDescrptIp, WM_SETFONT, WPARAM(hFont), TRUE);


		hMoney = CreateWindowEx(0, L"EDIT", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 360, 330, 70, 20, hWnd, NULL, hInst, NULL);
		SendMessage(hMoney, WM_SETFONT, WPARAM(hFont), TRUE);


		SumMoney = CreateWindowEx(0, L"STATIC", L"Tổng tiền:",
			WS_CHILD | WS_VISIBLE, 310, 333, 48, 20, hWnd, NULL, hInst, NULL);
		SendMessage(SumMoney, WM_SETFONT, WPARAM(hFont), TRUE);

		vnd = CreateWindowEx(0, L"STATIC", L"VND",
			WS_CHILD | WS_VISIBLE, 432, 333, 45, 20, hWnd, NULL, hInst, NULL);
		SendMessage(vnd, WM_SETFONT, WPARAM(hFont), TRUE);

		hLoai = CreateWindowEx(0, L"STATIC", L"Loại chi tiêu:",
			WS_CHILD | WS_VISIBLE, 60, 55, 90, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hLoai, WM_SETFONT, WPARAM(hFont), TRUE);
		

		hTien = CreateWindowEx(0, L"STATIC", L"Số tiền:",
			WS_CHILD | WS_VISIBLE, 60, 95, 50, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTien, WM_SETFONT, WPARAM(hFont), TRUE);

		hNoidung = CreateWindowEx(0, L"STATIC", L"Nội dung:",
			WS_CHILD | WS_VISIBLE, 60, 135, 47, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hNoidung, WM_SETFONT, WPARAM(hFont), TRUE);

		TCHAR ch[25];
		memset(&ch, 0, sizeof(ch));
		for (int i = 0; i < 6; i++)
		{
			wcscpy_s(ch, sizeof(ch) / sizeof(TCHAR), (TCHAR*)TypeCBB[i]);
			SendMessage(hComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)ch);
		}
		SendMessage(hComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		InsertColLV();
		LstItmSz = ListItem.size();
		LoadListItemFrmFile(L"file.txt");
		LoadAllItemToListView();
		SetWindowText(hMoney, ToString(TtMoney));
	}
		break;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkColor(hdcStatic, RGB(255, 255, 255));
		return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
	}break;
	
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDC_BUTTON_ADD:
			{
				AddItem();
				flag = true;
				InvalidateRect(hWnd, NULL, TRUE);
			}
				break;
			case IDC_BUTTON_CLOSE:
			{
				WritelistItemToFile(L"file.txt");
				PostQuitMessage(0);
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
			DrawChart(hdc);
			//DrawCircle(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		WritelistItemToFile(L"file.txt");
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

void InsertColLV()
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;

	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 130;
	lvCol.pszText = _T("Loại chi tiêu");
	ListView_InsertColumn(ListView, 0, &lvCol);

	lvCol.fmt = LVCFMT_LEFT;
	lvCol.pszText = _T("Số tiền");
	lvCol.cx = 130;
	ListView_InsertColumn(ListView, 1, &lvCol);

	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 200;
	lvCol.pszText = _T("Nội dung");
	ListView_InsertColumn(ListView, 2, &lvCol);
}
bool InsertItemToLV(int iItemCount)
{
	Item *item = new Item;
	TCHAR *buf;
	buf = new TCHAR[20];

	GetWindowText(hComboBox, buf, 20);
	wcscpy_s(item->iType, buf);

	int len = GetWindowTextLength(hmoney);
	if (len > 0)
	{
		buf = new TCHAR[len + 1];
		GetWindowText(hmoney, buf, len + 1);
		item->imoney = _wtoi64(buf);
	}
	else
	{
		MessageBox(g_hWnd, L"Phải điền vào chớ bạn ê!!!", L"Thông báo", NULL);
		return false;
	}
	len = GetWindowTextLength(hDescrptIp);
	if (len > 0)
	{
		buf = new TCHAR[len + 1];
		GetWindowText(hDescrptIp, buf, len + 1);
		item->iDescrpt = wstring(buf);
	}
	else
	{
		MessageBox(g_hWnd, L"Phải điền chú thích chớ bạn ê!!!", L"Thông báo", NULL);
		return false;
	}

	ListItem.push_back(item);
	TtMoney += item->imoney;

	LV_ITEM  lv;
	lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lv.iItem = iItemCount;
	lv.iSubItem = 0;
	lv.pszText = item->iType;
	ListView_InsertItem(ListView, &lv);


	lv.mask = LVIF_TEXT;

	lv.iSubItem = 1;
	buf = new WCHAR[20];
	wsprintf(buf, L"%I64d", item->imoney);
	lv.pszText = buf;
	ListView_SetItem(ListView, &lv);
	lv.iSubItem = 2;
	lv.pszText = (WCHAR*)item->iDescrpt.c_str();
	ListView_SetItem(ListView, &lv);

	SetWindowText(hMoney, ToString(TtMoney));
	return true;
}
void AddItem()
{
	if (InsertItemToLV(LstItmSz) == true) {
		LstItmSz++;
	}
}

LPCWSTR ToString(long long num) {
	_itow_s(num, istr, 10);
	return istr;
}

void WritelistItemToFile(wstring file)
{
	const std::locale utf8_locale = locale(std::locale(), new codecvt_utf8<wchar_t>());
	wofstream f;
	f.open(file, ios::out);
	f.imbue(utf8_locale);
	if(TtMoney != 0)
		f << TtMoney << endl;
	int len = ListItem.size();
	for (int i = 0; i < len; i++)
	{
		if (ListItem[i]->imoney != 0 && ListItem[i]->iDescrpt != L"") {
			f << wstring(ListItem[i]->iType) << endl;
			f << ListItem[i]->imoney << endl;
			f << wstring(ListItem[i]->iDescrpt) << endl;
		}
	}
	f.close();
}
void LoadListItemFrmFile(wstring file)
{
	const std::locale utf8_locale = locale(std::locale(), new codecvt_utf8<wchar_t>());
	wfstream f;
	f.imbue(utf8_locale);
	f.open(file, ios::in);
	wstring buf;
	if (f.is_open() == true)
	{
		getline(f, buf);
		TtMoney = _wtoi64(buf.c_str());
		while (!f.eof())
		{
			Item * item = new Item;
			getline(f, buf);
			wcscpy_s(item->iType, buf.c_str());
			getline(f, buf);
			item->imoney = _wtoi64(buf.c_str());
			getline(f, buf);
			item->iDescrpt = buf;

			if (item->imoney != 0)
				ListItem.push_back(item);
		}
	}
	f.close();
}
void LoadAllItemToListView()
{
	LV_ITEM lv;
	WCHAR *buf = new WCHAR[20];
	int len = ListItem.size();
	for (int i = 0; i < len; i++)
	{
		LV_ITEM  lv;
		lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lv.iItem = i;
		lv.iSubItem = 0;
		lv.pszText = ListItem[i]->iType;
		ListView_InsertItem(ListView, &lv);


		lv.mask = LVIF_TEXT;

		lv.iSubItem = 1;
		wsprintf(buf, L"%I64d", ListItem[i]->imoney);
		lv.pszText = buf;
		ListView_SetItem(ListView, &lv);

		lv.iSubItem = 2;
		lv.pszText = (WCHAR*)ListItem[i]->iDescrpt.c_str();
		ListView_SetItem(ListView, &lv);
	}
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
int CenterCircleX = 120;
int CenterCircleY = 380;
int RCircle = 60;
float *percent;
float *X;
float sumPercent = 0;
float start, end;
float CV = 2*3.14*RCircle;
void DrawCircle(HDC hdc)
{
	if (TtMoney <= 0)
		return;
	percent = new float[6];
	X = new float[6];

	for (int i = 0; i < 6; i++)
		X[0] = 0;
	for (int i = 0; i < 6; i++)
		percent[i] = 0;
	TMoney = new long long[6];
	for (int i = 0; i < 6; i++)
		TMoney[i] = 0;

	int len = ListItem.size();
	for (int i = 0; i < len; i++)
		TMoney[getTypeItem(ListItem[i])] += ListItem[i]->imoney;

	for (int i = 0; i < 5; i++) {
		percent[i] = (1.0*TMoney[i] / 1.0*TtMoney) * 360; //cung tron
		sumPercent += percent[i];
	}
	percent[5] = 360 - sumPercent;

	X[0] = 0;
	for (int i = 1; i < 6; i++)
	{
		X[i] = percent[i - 1] / 360 * CV;
	}
	for (int i = 0; i < 6; i++)
	{
		BeginPath(hdc);
		SetDCPenColor(hdc, color[i]); // màu d?
		SetDCBrushColor(hdc, color[i]);
		SelectObject(hdc, GetStockObject(DC_BRUSH));
		SelectObject(hdc, GetStockObject(DC_PEN));
		MoveToEx(hdc, CenterCircleX, CenterCircleY, (LPPOINT)NULL);
		AngleArc(hdc, CenterCircleX, CenterCircleY, RCircle, X[i], percent[i]);
		LineTo(hdc, CenterCircleX, CenterCircleY);
		EndPath(hdc);
		StrokeAndFillPath(hdc);
	}
	delete[]X;
	delete[]TMoney;
	delete[]percent;
}
void DrawChart(HDC hdc)
{
	if (TtMoney <= 0) {
		fillRect(hdc, 120, 380, 640, 420, color[6]);
		curColor = 6;
		return;
	}
	curX = new int[7];
	curX[0] = 120;
	int len = ListItem.size();
	TMoney = new long long[6];
	for (int i = 0; i < 6; i++)
		TMoney[i] = 0;
	for (int i = 0; i < len; i++)
		TMoney[getTypeItem(ListItem[i])] += ListItem[i]->imoney;
	for (int i = 1; i < 7; i++)
		curX[i] = curX[i - 1] + ((TMoney[i-1] / (TtMoney*1.0))*520);
	for (int i = 0; i < 6; i++)
		fillRect(hdc, curX[i], 380, curX[i + 1], 420, color[i]);
	delete[] TMoney;
	delete[] curX;
}

int getTypeItem(Item *item)
{
	if (wcscmp((item->iType), TypeCBB[0]) == 0)
		return 0;
	else if (wcscmp((item->iType), TypeCBB[1]) == 0)
		return 1;
	else if (wcscmp((item->iType), TypeCBB[2]) == 0)
		return 2;
	else if (wcscmp((item->iType), TypeCBB[3]) == 0)
		return 3;
	else if (wcscmp((item->iType), TypeCBB[4]) == 0)
		return 4;
	else 
		return 5;
}
