/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#define STRICT
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <mmsystem.h>
#include <winnls32.h>
#include <stdio.h>

#include "bench.h"
#include "resource.h"

extern DWORD SCREEN_WIDTH;
extern DWORD SCREEN_HEIGHT;
extern DWORD SCREEN_BPP;

extern HINSTANCE hInst;
extern HWND hWnd;
extern HDC hDC;
extern HDC hBmpDC;
extern HBITMAP hBMP;
extern HBITMAP hOldBMP;

TCHAR CrystalMark[] = L"CrystalMark";

#define XY int X = rand() % SCREEN_WIDTH;int Y = rand() % SCREEN_HEIGHT;
#define SS static int Size = 64;RECT rect;rect.left  = rand() % SCREEN_WIDTH;rect.right = rect.left + Size;rect.top   = rand() % SCREEN_HEIGHT;rect.bottom= rect.top + Size;

//-----------------------------------------------------------------------------
// Initialize
//-----------------------------------------------------------------------------
void Initialize()
{
	TCHAR str[] = L"CrystalMark Retro\nGDI Benchmark";
	static int C = 0x66; // color
	static BOOL flag = TRUE;
	RECT rect;
	rect.left  = 0;rect.right = SCREEN_WIDTH;
	rect.top   = 0;rect.bottom= SCREEN_HEIGHT;
	int width = 0;
	int height = 0;

	HFONT hFont = NULL;
	hFont = CreateFont(SCREEN_WIDTH / 12,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_PRECIS,
						CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Arial");
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	SetBkMode(hDC,TRANSPARENT);
	SetTextColor(hDC,RGB(C/2,C/2,C));

	DrawTextW(hDC, str, (int)wcslen(str), &rect, DT_VCENTER | DT_CENTER | DT_CALCRECT);

	width = rect.right;
	height = rect.bottom;

	rect.left = SCREEN_WIDTH / 2  - width / 2;
	rect.right = SCREEN_WIDTH / 2 + width / 2;

	rect.top = SCREEN_HEIGHT / 2 - height / 2;
	rect.bottom = SCREEN_HEIGHT / 2 + height / 2;

	DrawTextW(hDC, str, (int)wcslen(str), &rect, DT_VCENTER | DT_CENTER);

	SelectObject(hDC,hOldFont);
	DeleteObject(hFont);
	
	if(flag){
		C++;
		if(C == 0xCC){flag = FALSE;}
	}else{
		C--;
		if(C == 0x33){flag = TRUE;}
	}
	SetBkMode(hDC,OPAQUE);
}
//-----------------------------------------------------------------------------
// TextTest
//-----------------------------------------------------------------------------
void TextTest1()
{
//	MessageBox(NULL,"","",MB_OK);
	SetBkMode(hDC,TRANSPARENT);
	HFONT hFont = NULL;
	hFont = CreateFont(20,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_PRECIS,
								CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Arial");
	HFONT hOldFont = (HFONT)SelectObject(hDC,hFont);
	for(int i=0;i<100;i++){
		XY
		SetTextColor(hDC,RGB(rand()%256,rand()%256,rand()%256));
		TextOut(hDC,X,Y,CrystalMark, (int)wcslen(CrystalMark));
	}
	SelectObject(hDC,hOldFont);
	DeleteObject(hFont);
}
void TextTest2()
{
	HFONT hFont = NULL;
	hFont = CreateFont(20,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_PRECIS,
								CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Arial");
	HFONT hOldFont = (HFONT)SelectObject(hDC,hFont);
	for(int i=0;i<100;i++){
		XY
		SetTextColor(hDC,RGB(rand()%256,rand()%256,rand()%256));
		TextOut(hDC,X,Y,CrystalMark, (int)wcslen(CrystalMark));
	}
	SelectObject(hDC,hOldFont);
	DeleteObject(hFont);
}
void TextTest3()
{
	HFONT hFont = NULL;
	hFont = CreateFont(50,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_PRECIS,
								CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Arial");
	HFONT hOldFont = (HFONT)SelectObject(hDC,hFont);
	for(int i=0;i<100;i++){
		XY
		SetTextColor(hDC,RGB(rand()%256,rand()%256,rand()%256));
		TextOut(hDC,X,Y,CrystalMark, (int)wcslen(CrystalMark));
	}
	SelectObject(hDC,hOldFont);
	DeleteObject(hFont);
}
void TextTest4()
{
	SetBkMode(hDC,TRANSPARENT);
	HFONT hFont = NULL;
	hFont = CreateFont(50,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_PRECIS,
								CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Arial");
	HFONT hOldFont = (HFONT)SelectObject(hDC,hFont);
	for(int i=0;i<100;i++){
		int X = rand() % SCREEN_WIDTH;
		int Y = rand() % SCREEN_HEIGHT;
		int R = rand() % 128 + 128;
		int G = rand() % 128 + 128;
		int B = rand() % 128 + 128;
		for(int j=0;j<4;j++){
			SetTextColor(hDC,RGB(R-=8,G-=8,B-=8));
			TextOut(hDC,X++,Y++,CrystalMark, (int)wcslen(CrystalMark));
		}
	}
	SelectObject(hDC,hOldFont);
	DeleteObject(hFont);
}
//-----------------------------------------------------------------------------
// CircleTest
//-----------------------------------------------------------------------------
void CircleTest1()
{
	SS
	HPEN hPen = CreatePen(PS_SOLID,1,RGB(rand()%256,rand()%256,rand()%256));
	HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);
	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,hBrush);
	Ellipse(hDC,rect.left,rect.top,rect.right,rect.bottom);
	SelectObject(hDC,hOldBrush);
	DeleteObject(hBrush);
	SelectObject(hDC,hOldPen);
	DeleteObject(hPen);
}
void CircleTest2()
{
	SS
	HPEN hPen = CreatePen(PS_SOLID,1,RGB(0,0,0));
	HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);
	HBRUSH hBrush = CreateSolidBrush(RGB(rand()%256,rand()%256,rand()%256));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,hBrush);
	Ellipse(hDC,rect.left,rect.top,rect.right,rect.bottom);
	SelectObject(hDC,hOldBrush);
	DeleteObject(hBrush);
	SelectObject(hDC,hOldPen);
	DeleteObject(hPen);
}
void CircleTest3()
{
	SS
	HPEN hPen = CreatePen(PS_SOLID,1,RGB(rand()%256,rand()%256,rand()%256));
	HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);
	HBRUSH hBrush = CreateHatchBrush(HS_CROSS,RGB(rand()%256,rand()%256,rand()%256));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,hBrush);
	Ellipse(hDC,rect.left,rect.top,rect.right,rect.bottom);
	SelectObject(hDC,hOldPen);
	DeleteObject(hPen);
	SelectObject(hDC,hOldBrush);
	DeleteObject(hBrush);
}
void CircleTest4()
{
	SS
	Size = 128;
	int R = rand() % 128 + 128;
	int G = rand() % 128 + 128;
	int B = rand() % 128 + 128;
	for(int i=1;i<64;i++){
		HPEN hPen = CreatePen(PS_SOLID,2,RGB(R--,G--,B--));
		HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);
		rect.left++;rect.top++;rect.right--;rect.bottom--;
		Ellipse(hDC,rect.left,rect.top,rect.right,rect.bottom);
		SelectObject(hDC,hOldPen);
		DeleteObject(hPen);
	}
}
//-----------------------------------------------------------------------------
// SquareTest
//-----------------------------------------------------------------------------
void SquareTest1()
{
	SS
	HPEN hPen = CreatePen(PS_SOLID,1,RGB(rand()%256,rand()%256,rand()%256));
	HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);
	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,hBrush);
	Rectangle(hDC,rect.left,rect.top,rect.right,rect.bottom);
	SelectObject(hDC,hOldBrush);
	DeleteObject(hBrush);
	SelectObject(hDC,hOldPen);
	DeleteObject(hPen);
}
void SquareTest2()
{
	SS
	HPEN hPen = CreatePen(PS_SOLID,1,RGB(0,0,0));
	HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);
	HBRUSH hBrush = CreateSolidBrush(RGB(rand()%256,rand()%256,rand()%256));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,hBrush);
	Rectangle(hDC,rect.left,rect.top,rect.right,rect.bottom);
	SelectObject(hDC,hOldBrush);
	DeleteObject(hBrush);
	SelectObject(hDC,hOldPen);
	DeleteObject(hPen);
}
void SquareTest3()
{
	SS
	HPEN hPen = CreatePen(PS_SOLID,1,RGB(rand()%256,rand()%256,rand()%256));
	HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);
	HBRUSH hBrush = CreateHatchBrush(HS_CROSS,RGB(rand()%256,rand()%256,rand()%256));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC,hBrush);
	Rectangle(hDC,rect.left,rect.top,rect.right,rect.bottom);
	SelectObject(hDC,hOldPen);
	DeleteObject(hPen);
	SelectObject(hDC,hOldBrush);
	DeleteObject(hBrush);
}
void SquareTest4()
{
	SS
	Size = 128;
	int R = rand() % 128 + 128;
	int G = rand() % 128 + 128;
	int B = rand() % 128 + 128;
	for(int i=1;i<64;i++){
		HPEN hPen = CreatePen(PS_SOLID,2,RGB(R--,G--,B--));
		HPEN hOldPen = (HPEN)SelectObject(hDC,hPen);
		rect.left++;rect.top++;rect.right--;rect.bottom--;
		Rectangle(hDC,rect.left,rect.top,rect.right,rect.bottom);
		SelectObject(hDC,hOldPen);
		DeleteObject(hPen);
	}
}
//-----------------------------------------------------------------------------
// BitBlt
//-----------------------------------------------------------------------------
void BitBltTest1()
{
//	hDC = GetDC(hWnd);
	hBMP = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BITMAP1));
	hBmpDC = CreateCompatibleDC(hDC);
	hOldBMP = (HBITMAP)SelectObject(hBmpDC,hBMP);

	XY
	BitBlt(hDC,X,Y,128,128,hBmpDC,0,0,SRCCOPY);

	SelectObject(hBmpDC,hOldBMP);
	DeleteDC(hBmpDC);
	DeleteObject(hBMP);
//	ReleaseDC(hWnd,hDC);
}
void BitBltTest2()
{
//	hDC = GetDC(hWnd);
	hBMP = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BITMAP1));
	hBmpDC = CreateCompatibleDC(hDC);
	hOldBMP = (HBITMAP)SelectObject(hBmpDC,hBMP);

	XY
	StretchBlt(hDC,X,Y,256,256,hBmpDC,0,0,128,128,SRCCOPY);

	SelectObject(hBmpDC,hOldBMP);
	DeleteDC(hBmpDC);
	DeleteObject(hBMP);
//	ReleaseDC(hWnd,hDC);
}
void BitBltTest3()
{
//	hDC = GetDC(hWnd);
	hBMP = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BITMAP1));
	hBmpDC = CreateCompatibleDC(hDC);
	hOldBMP = (HBITMAP)SelectObject(hBmpDC,hBMP);

	XY
	StretchBlt(hDC,X,Y,64,64,hBmpDC,0,0,128,128,SRCCOPY);

	SelectObject(hBmpDC,hOldBMP);
	DeleteDC(hBmpDC);
	DeleteObject(hBMP);
//	ReleaseDC(hWnd,hDC);
}
void BitBltTest4()
{
//	hDC = GetDC(hWnd);
	hBMP = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BITMAP1));
	hBmpDC = CreateCompatibleDC(hDC);
	hOldBMP = (HBITMAP)SelectObject(hBmpDC,hBMP);

	XY
	BLENDFUNCTION alpha;
	alpha.BlendFlags = 0;
	alpha.BlendOp = AC_SRC_OVER;
	alpha.AlphaFormat = 0;
	alpha.SourceConstantAlpha = rand()%256;
	AlphaBlend(hDC,X,Y,128,128,hBmpDC,0,0,128,128,alpha);

	SelectObject(hBmpDC,hOldBMP);
	DeleteDC(hBmpDC);
	DeleteObject(hBMP);
//	ReleaseDC(hWnd,hDC);
}
