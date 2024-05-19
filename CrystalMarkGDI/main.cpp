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


// typedef int (WINAPI *_AlphaBlend_) (HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);
// _AlphaBlend_ _AlphaBlend = NULL;

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"msimg32.lib") // for AlphaBlend

#define VERSION 100 // 1.0 = 100 1.5 = 150 //

DWORD Scene = 0;
DWORD Score[8] = {0,0,0,0,0,0,0,0};
double ops[17];			// Object Per Second
DWORD count = 0;
DWORD LastScene = 16; // AlphaBlend
BOOL IsActive = FALSE;

DWORD SCREEN_WIDTH;
DWORD SCREEN_HEIGHT;
DWORD SCREEN_BPP;

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
HRESULT WinInit( HINSTANCE hInst, int nCmdShow, HWND* phWnd );

HINSTANCE hInst = NULL;
HWND hWnd = NULL;
HDC hDC = NULL;
HDC hBmpDC = NULL;
HBITMAP hBMP = NULL;
HBITMAP hOldBMP = NULL;

//#define HIGH_PRIORITY

// #include "bench.cpp"

void BenchMarkResult();
void DrawStatus();

// From Share.cpp 
int FontSetting();

void (*DrawScene[])() = {	Initialize,
							TextTest1,TextTest2,TextTest3,TextTest4,
							SquareTest1,SquareTest2,SquareTest3,SquareTest4,
							CircleTest1,CircleTest2,CircleTest3,CircleTest4,
							BitBltTest1,BitBltTest2,BitBltTest3,BitBltTest4,
						};

#define WINDOW_NAME L"CrystalMarkGDI"

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything and calls
//       UpdateFrame() when idle from the message pump.
//-----------------------------------------------------------------------------
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow )
{
    MSG		 msg;
	hInst = hInstance;

	for(;;)
	switch(*pCmdLine)
	{
	case L'V':
	case L'v':
		return VERSION;
		break;

	case L'B':
	case L'b':
		goto EXEC;
		break;
	case L'\0':
//		MessageBox(NULL,"",WINDOW_NAME,MB_OK);
//		return FALSE;
		goto EXEC;
		break;
		
	case ' ':
	case '-':
	case '/':
		pCmdLine++;
		break;

	default:
		return 0;
	}

EXEC:

	HDC hDC = ::GetDC(NULL);
	SCREEN_WIDTH  = GetDeviceCaps(hDC,HORZRES);
	SCREEN_HEIGHT = GetDeviceCaps(hDC,VERTRES);
	SCREEN_BPP    = GetDeviceCaps(hDC,BITSPIXEL);
	::ReleaseDC(hWnd,hDC);

    HACCEL hAccel = LoadAccelerators( hInst, MAKEINTRESOURCE(IDR_MAIN_ACCEL) );

	WinInit( hInst, nCmdShow, &hWnd );
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);

#ifdef HIGH_PRIORITY
	SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
#endif

	BOOL LoopFlag = TRUE;
    while( LoopFlag )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
			if (msg.message == WM_QUIT)
			{
				LoopFlag = FALSE;
				return VERSION;
			}
			else if( 0 == TranslateAccelerator( hWnd, hAccel, &msg ) )
			{
                TranslateMessage( &msg ); 
                DispatchMessage( &msg );
            }
        }

        else
        {
			DrawScene[Scene]();
			DrawStatus();
        }
    }
#ifdef HIGH_PRIORITY
	SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
#endif

	return VERSION;
}

void DrawStatus()
{
	static DWORD last = timeGetTime();
	DWORD current = timeGetTime();
	count++;
	if( ( current - last > 4000 ) && Scene > 0 
	||	( current - last > 5000 ) && Scene == 0 ) {
		// 描画回数を保存 //
		ops[Scene] = (count / ((current - last) / 1000.0f));
		if(Scene == LastScene){
			// Score 計算
			double font=1.0;
			switch ( FontSetting() )
			{
				case 0:	font = 1.0;		break;	// None
				case 1:	font = 10.0;	break;	// Normal
				case 2: font = 50.0;	break;	// ClearType
				default:font = 1.0;		break;
			}
			double color = 1.0;
			switch ( SCREEN_BPP )
			{
			case 32:
			case 24:
				color = 1.0;
				break;
			case 16:
			case 15:
				color = 0.5;
				break;
			case 8:
			case 4:
			case 2:
			case 1:
				color = 0.25;
				break;
			default:
				color = 1.0;
				break;
			}
			
			Score[0] = (int)( ( ops[1] + ops[2] + ops[3] * 25 + ops[4] * 25        ) / 85.4732 * font * color );
			Score[1] = (int)( ( ops[5] + ops[6] * 2 + ops[7] * 2 + ops[8] * 100    ) / 25.3659 * color );
			Score[2] = (int)( ( ops[9] + ops[10] * 2 + ops[11] * 2 + ops[12] * 100 ) / 23.3887 * color );
			Score[3] = (int)( ( ops[13] + ops[14] + ops[15] + ops[16] ) / 3.9071 * color );

			wprintf(L"%d,%d,%d,%d\n", Score[0], Score[1], Score[2], Score[3]);
/*
			char str[1024];
			sprintf(str,
				"\
				Score = %d : ops = %f,%f,%f,%f\r\n\
				Score = %d : ops = %f,%f,%f,%f\r\n\
				Score = %d : ops = %f,%f,%f,%f\r\n\
				Score = %d : ops = %f,%f,%f,%f\r\n\
				",
				Score[0],ops[1],ops[2],ops[3],ops[4],
				Score[1],ops[5],ops[6],ops[7],ops[8],
				Score[2],ops[9],ops[10],ops[11],ops[12],
				Score[3],ops[13],ops[14],ops[15],ops[16]);
	
			MessageBox(hWnd,str,str,MB_OK);
*/
			BenchMarkResult(); // データの保存
			PostMessage( hWnd, WM_CLOSE, 0, 0 );// 終了 //
		}
		last = current;
		count = 0;
		Scene++;
		SetBkMode(hDC,OPAQUE);
	}
}

//-----------------------------------------------------------------------------
// Name: WinInit()
// Desc: Init the window
//-----------------------------------------------------------------------------
HRESULT WinInit( HINSTANCE hInst, int nCmdShow, HWND* phWnd )
{
    WNDCLASS wc;
    HWND     hWnd;

    wc.lpszClassName = WINDOW_NAME;
    wc.lpfnWndProc   = MainWndProc;
    wc.style         = CS_VREDRAW | CS_HREDRAW;
    wc.hInstance     = hInst;
    wc.hIcon         = NULL;//LoadIcon( hInst, MAKEINTRESOURCE(IDI_MAIN) );
    wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;

    if( RegisterClass( &wc ) == 0 )
        return E_FAIL;

    hWnd = CreateWindowEx( 0,WINDOW_NAME,WINDOW_NAME,
                           WS_POPUP|WS_EX_TOPMOST,
						   0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
						   NULL, NULL, hInst, NULL );
    if( hWnd == NULL )
    	return E_FAIL;

//	ShowWindow( hWnd, nCmdShow );

    *phWnd = hWnd;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MainWndProc()
// Desc: The main window procedure
//-----------------------------------------------------------------------------
LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static HINSTANCE hLib = NULL;
    switch (msg)
    {
        case WM_COMMAND:
            switch( LOWORD(wParam) )
            {
                case IDM_EXIT:
        			PostMessage( hWnd, WM_CLOSE, 0, 0 );
                    return 0L;
            }
            break;
		case WM_CREATE:
			hDC = GetDC(hWnd);
		//	hBMP = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BITMAP1));
		//	hBmpDC = CreateCompatibleDC(hDC);
		//	hOldBMP = (HBITMAP)SelectObject(hBmpDC,hBMP);
			while( ! ShowCursor(FALSE) );
			timeBeginPeriod(1);
			ShowWindow(hWnd,SW_SHOW);
			UpdateWindow(hWnd);
			SetForegroundWindow(hWnd);
			SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE | SWP_SHOWWINDOW);
			SetActiveWindow(hWnd);

			WINNLSEnableIME(hWnd,FALSE);

			// AlphaBlend Check
			// hLib = LoadLibrary("msimg32.dll");
			// _AlphaBlend = (_AlphaBlend_) GetProcAddress (hLib,"AlphaBlend");
			// if(_AlphaBlend == NULL){LastScene = 15;}

			return 0;
        case WM_DESTROY:
		//	if(hLib != NULL){CloseHandle(hLib);}
		//	SelectObject(hBmpDC,hOldBMP);
		//	DeleteDC(hBmpDC);
		//	DeleteObject(hBMP);
			ReleaseDC(hWnd,hDC);
			timeEndPeriod(1);
			WINNLSEnableIME(hWnd,TRUE);
			ShowCursor(TRUE);
			PostQuitMessage(0);
            return 0L;
			break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void BenchMarkResult()
{
	/*
	TCHAR ini[MAX_PATH];
	TCHAR str[256];
	TCHAR score[256];
	TCHAR* ptrEnd;

	::GetModuleFileName(NULL, ini, MAX_PATH);
	if ( (ptrEnd = wcsrchr(ini, L'\\')) != NULL ) {
		*ptrEnd = '\0';
		wcscat(ini, L"\\CMRGDI.ini");
	}

	::WritePrivateProfileString(L"Setting", L"Product", L"CMRGDI",ini);
	::wsprintf(str,L"%d",VERSION);
	::WritePrivateProfileString(L"Setting",L"Version",str,ini);
	::wsprintf(str,L"%dx%dx%d",SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP);
	::WritePrivateProfileString(L"Setting",L"DisplayMode",str,ini);
	// FPS //
	for(int i=0;i < 8;i++){
		::wsprintf(str,L"Scene%d",i+1);
		::wsprintf(score,L"%d", Score[i]);
		::WritePrivateProfileString(L"Result",str,score,ini);
	}

	TCHAR result[MAX_PATH];
	::GetModuleFileName(NULL, result, MAX_PATH);
	if ((ptrEnd = wcsrchr(result, L'\\')) != NULL) {
		*ptrEnd = '\0';
		wcscat(result, L"\\CMRGDI.csv");
	}

	::wsprintf(str, L"%d,%d,%d,%d\n", Score[0], Score[1], Score[2], Score[3]);
	FILE* fp{};
	_wfopen_s(&fp, result, L"ac");
	if (fp != NULL)
	{
		fwprintf(fp, L"%s", (LPCTSTR)str);
		fflush(fp);
		fclose(fp);
	}
	*/

	/// Memory Map for Benchmark result
	TCHAR name[32];
	wsprintfW(name, L"CrystalMarkRetro");
	TCHAR result[1024];

	wsprintfW(result,
		L"\
Version=%s\n\
Scene1=%d\n\
Scene2=%d\n\
Scene3=%d\n\
Scene4=%d\n\
",
L"1.0.0.2024",
Score[0],
Score[1],
Score[2],
Score[3]
);

	auto size = 2048;

	HANDLE hSharedMemory = CreateFileMappingW(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, name);
	if (hSharedMemory != NULL)
	{
		TCHAR* pMemory = (TCHAR*)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, size);
		if (pMemory != NULL)
		{
			RtlCopyMemory(pMemory, result, size);
			UnmapViewOfFile(pMemory);
			CloseHandle(hSharedMemory);
		}
	}
}

int FontSetting()
{
	unsigned long data;
	OSVERSIONINFO osVersionInfo;
	osVersionInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&osVersionInfo);

	SystemParametersInfo(SPI_GETFONTSMOOTHING,0,&data,SPIF_SENDCHANGE);
	if(data > 0){data = 1;}
	if(data != 0 && osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && osVersionInfo.dwBuildNumber >= 2600){
		HKEY hKey;
		unsigned long ulLength;
		unsigned long ulType=REG_DWORD;
		ulLength=sizeof(data);

		if( RegOpenKeyEx(HKEY_CURRENT_USER,L"Control Panel\\Desktop",0, KEY_READ, &hKey) == ERROR_SUCCESS ){
			RegQueryValueEx(hKey,L"FontSmoothingType",NULL,&ulType,(unsigned char *)&data,&ulLength);
			RegCloseKey(hKey);
		}
	}

	// Font Setting
	return data;
}