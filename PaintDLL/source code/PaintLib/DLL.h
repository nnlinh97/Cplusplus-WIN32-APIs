#pragma once

#ifdef PAINTLIBRARY_EXPORTS
#define PAINTLIBRARY_API __declspec(dllexport) 
#else
#define PAINTLIBRARY_API __declspec(dllimport) 
#endif

namespace PaintLibrary
{
	//-----------------------------------------------------------------------------
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
		PAINTLIBRARY_API virtual void Draw(HDC hdc) = 0;
		PAINTLIBRARY_API virtual SHAPE* Create() = 0;
		PAINTLIBRARY_API virtual void setData(int a, int b, int c, int d) = 0;
	};
	//-----------------------------------------------------------------------------

	struct Point
	{
		int x, y;
	};
	PAINTLIBRARY_API Point newPoint(int x, int y);
	//-----------------------------------------------------------------------------


	class LINE :public SHAPE {
	public:
		PAINTLIBRARY_API void Draw(HDC hdc);
		PAINTLIBRARY_API SHAPE* Create();
		PAINTLIBRARY_API void setData(int a, int b, int c, int d);
	};
	//-----------------------------------------------------------------------------

	class RECTANGLE :public SHAPE
	{
	public:
		PAINTLIBRARY_API void Draw(HDC hdc);
		PAINTLIBRARY_API SHAPE* Create();
		PAINTLIBRARY_API void setData(int left, int top, int right, int bottom);
	};
	//-----------------------------------------------------------------------------

	class ELLIPSE :public SHAPE
	{
	public:
		PAINTLIBRARY_API void Draw(HDC hdc);
		PAINTLIBRARY_API SHAPE* Create();
		PAINTLIBRARY_API void setData(int left, int top, int right, int bottom);
	};
	//-----------------------------------------------------------------------------

}