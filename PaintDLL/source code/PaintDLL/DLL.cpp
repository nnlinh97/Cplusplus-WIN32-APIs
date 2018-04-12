#include "stdafx.h"
#include "DLL.h"

namespace PaintLibrary
{
	//-----------------------------------------------------------------------------
	Point newPoint(int x, int y)
	{
		Point p;
		p.x = x;
		p.y = y;
		return p;
	}
	//-----------------------------------------------------------------------------


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
	//-----------------------------------------------------------------------------
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

}