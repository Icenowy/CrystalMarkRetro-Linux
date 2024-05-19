/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "CrystalMark.h"
#include "CrystalMarkDlg.h"
#include "ExecBench.h"
#include "DiskBench.h"

#include <afxmt.h>
#include <winioctl.h>
#include <mmsystem.h>
#include <math.h>
#pragma comment(lib,"winmm.lib")

#pragma warning(disable : 4996)

static CString BenchCoreExe;

static HANDLE hFile;

static UINT Exit(LPVOID dlg, BOOL forceExit);

#define BENCHMARK_EXE_32			_T("Resource\\Benchmark\\CMRCPU32.exe")
#define BENCHMARK_EXE_64			_T("Resource\\Benchmark\\CMRCPU64.exe")
#define BENCHMARK_EXE_32_LEGACY		_T("Resource\\Benchmark\\CMRCPU32L.exe")
#define BENCHMARK_EXE_64_LEGACY		_T("Resource\\Benchmark\\CMRCPU64L.exe")
#define BENCHMARK_EXE_ARM32			_T("Resource\\Benchmark\\CMRCPUA32.exe")
#define BENCHMARK_EXE_ARM64			_T("Resource\\Benchmark\\CMRCPUA64.exe")

#define HIYOGL_PATH					_T("Resource\\Benchmark\\HiyoGL\\")
#define HIYOGL_EXE_32				_T("Resource\\Benchmark\\HiyoGL\\CMROGL32.exe")	
#define HIYOGL_EXE_64				_T("Resource\\Benchmark\\HiyoGL\\CMROGL64.exe")
#define HIYOGL_EXE_ARM64			_T("Resource\\Benchmark\\HiyoGL\\CMROGLA64.exe")

#define HIYGDI_PATH					_T("Resource\\Benchmark\\")
#define HIYGDI_EXE_32				_T("Resource\\Benchmark\\CMRGDI32.exe")		
#define HIYGDI_EXE_64				_T("Resource\\Benchmark\\CMRGDI64.exe")		
#define HIYGDI_EXE_ARM64			_T("Resource\\Benchmark\\CMRGDIA64.exe")

PROCESS_INFORMATION pi = { 0 };
BOOL BenchmarkAll = FALSE;

int ExecAndWait(TCHAR *pszCmd, BOOL bNoWindow)
{
	DWORD Code = 0;
	BOOL bSuccess;
	STARTUPINFO si;

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (bNoWindow) {
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}

	bSuccess = CreateProcess(NULL, pszCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (bSuccess != TRUE)
	{
		return 0;
	}

	WaitForInputIdle(pi.hProcess, INFINITE);
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &Code);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return Code;
}

int ExecsAndWait(CString exePath, BOOL bNoWindow, int testNo, int maxThreads)
{
	DWORD exitCode = 0;
	STARTUPINFO si;
	int score = 0;

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (bNoWindow) {
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}

	CString command;

	int maxProcesses = (maxThreads - 1) / 64 + 1;
	HANDLE* hProcess = new HANDLE[maxProcesses];
	PROCESS_INFORMATION* processInfo = new PROCESS_INFORMATION[maxProcesses];

	for (int i = 0; i < maxProcesses; i++)
	{
		command.Format(_T("\"%s\" %d %d"), (LPTSTR)exePath.GetString(), testNo, maxThreads / maxProcesses);

		if (CreateProcess(NULL, (LPTSTR)command.GetString(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &processInfo[i]))
		{

		}
		hProcess[i] = processInfo[i].hProcess;
	}

	for (int i = 0; i < maxProcesses; i++)
	{
		WaitForInputIdle(hProcess[i], INFINITE);
	}

	WaitForMultipleObjects(maxProcesses, hProcess, TRUE, INFINITE);

	for (int i = 0; i < maxProcesses; i++)
	{
		if (hProcess[i] != NULL)
		{
			GetExitCodeProcess(hProcess[i], &exitCode);
			CloseHandle(hProcess[i]);

			score += exitCode;
		}
	}

	delete[] hProcess;
	delete[] processInfo;

	if (score == 0) { score = 1; }

	return score;
}

double geometricMean(double data[], int count)
{
	if (count <= 1) { return -1.0; }

	double value = data[0];
	for (int i = 1; i < count; i++)
	{
		value *= data[i];
	}

	value = pow(value, 1.0 / count);

	return value;
}

UINT ExecBenchmarkAll(LPVOID dlg)
{
	BenchmarkAll = TRUE;

	if (! ExecCPUBench(dlg)) { BenchmarkAll = FALSE; return FALSE; }
	// ((CCrystalMarkDlg*)dlg)->m_BenchStatus = TRUE;
	// ::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_START_BENCHMARK, 0, 0);
	
	if(! ExecDiskBenchAll(dlg)) { BenchmarkAll = FALSE; return FALSE; }
	// ((CCrystalMarkDlg*)dlg)->m_BenchStatus = TRUE;
	// ::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_START_BENCHMARK, 0, 0);

	if (! ExecGDIBench(dlg)) { BenchmarkAll = FALSE; return FALSE; }
	// ((CCrystalMarkDlg*)dlg)->m_BenchStatus = TRUE;
	// ::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_START_BENCHMARK, 0, 0);

	if (! ExecHiyoGL(dlg) ) { BenchmarkAll = FALSE; return FALSE; }

	BenchmarkAll = FALSE;
	Exit(dlg, FALSE);
	return TRUE;
}

UINT ExecGDIBench(LPVOID dlg)
{
	CString command;
	CString option;
	CString exePath;

	TCHAR* ptrEnd;
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL, path, MAX_PATH);
	if ((ptrEnd = _tcsrchr(path, '\\')) != NULL)
	{
		*ptrEnd = '\0';
	}
#ifdef _M_ARM64
	exePath.Format(_T("%s\\%s"), path, HIYGDI_EXE_ARM64);
#elif _M_X64
	exePath.Format(_T("%s\\%s"), path, HIYGDI_EXE_64);
#else
	exePath.Format(_T("%s\\%s"), path, HIYGDI_EXE_32);
#endif

	option.Format(_T(""));
	command.Format(_T("\"%s\" %s"), exePath.GetString(), option.GetString());
	// ExecAndWait((TCHAR*)command.GetString(), FALSE);

	/*
	if (! CheckCodeSign(CERTNAME, exePath))
	{
		AfxMessageBox(((CCrystalMarkDlg*)dlg)->m_MesExeFileModified);
		Exit(dlg);
		return FALSE;
	}
	*/

	if (! IsFileExist(exePath.GetString()))
	{
		AfxMessageBox(((CCrystalMarkDlg*)dlg)->m_MesExeFileNotFound);
		return Exit(dlg, TRUE);
	}

	DWORD Code = 0;
	BOOL bSuccess = FALSE;
	STARTUPINFO si = { 0 };

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	CString name;
	name.Format(_T("CrystalMarkRetro")); // , GetCurrentProcessId());
	DWORD size = 1024 * sizeof(TCHAR);

	TCHAR result[1024] = { 0 };

	HANDLE hSharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, size, name.GetString());
	if (hSharedMemory != NULL)
	{
		TCHAR* pMemory = (TCHAR*)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, NULL, NULL, size);
		if (pMemory != NULL)
		{
			bSuccess = CreateProcess(NULL, (TCHAR*)command.GetString(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			if (bSuccess != TRUE)
			{
				UnmapViewOfFile(pMemory);
				CloseHandle(hSharedMemory);
				return Exit(dlg, TRUE);
			}

			WaitForInputIdle(pi.hProcess, INFINITE);
			WaitForSingleObject(pi.hProcess, INFINITE);
			GetExitCodeProcess(pi.hProcess, &Code);

			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);

			pi.hProcess = NULL;

			RtlCopyMemory(result, pMemory, size);

			UnmapViewOfFile(pMemory);
			CloseHandle(hSharedMemory);
		}
	}

	// Read Score
	__int64 score[4] = { 0 };
	// score[0] = GetPrivateProfileIntW(_T("Result"), _T("Scene1"), 0, iniPath);
	// score[1] = GetPrivateProfileIntW(_T("Result"), _T("Scene2"), 0, iniPath);
	// score[2] = GetPrivateProfileIntW(_T("Result"), _T("Scene3"), 0, iniPath);
	// score[3] = GetPrivateProfileIntW(_T("Result"), _T("Scene4"), 0, iniPath);

	CString version;
	CString cstr;
	CString token;
	int curPos = 0;
	cstr = result;

	token = cstr.Tokenize(_T("\n"), curPos);
	while (token != _T(""))
	{
		CString leftPart = token.SpanExcluding(_T("="));
		CString rightPart = token.Mid(leftPart.GetLength() + 1);

		if (leftPart.Find(_T("Version")) == 0){ version = rightPart; }
		if (leftPart.Find(_T("Scene1")) == 0) { score[0] = _ttoi64(rightPart); }
		if (leftPart.Find(_T("Scene2")) == 0) { score[1] = _ttoi64(rightPart); }
		if (leftPart.Find(_T("Scene3")) == 0) { score[2] = _ttoi64(rightPart); }
		if (leftPart.Find(_T("Scene4")) == 0) { score[3] = _ttoi64(rightPart); }

		token = cstr.Tokenize(_T("\n"), curPos);
	}

	if (! version.IsEmpty())
	{
		((CCrystalMarkDlg*)dlg)->m_CMRGDIVersion = version;
		if (score[0] == 0) { score[0] = 1; }
		if (score[1] == 0) { score[1] = 1; }
		if (score[2] == 0) { score[2] = 1; }
		if (score[3] == 0) { score[3] = 1; }
	}
	else
	{
		Exit(dlg, TRUE);
		return FALSE;
	}

	((CCrystalMarkDlg*)dlg)->m_Score[3][0] = (int)pow((double)((double)score[0] * score[1] * score[2] * score[3]), 0.25);
	((CCrystalMarkDlg*)dlg)->m_Score[3][1] = score[0];
	((CCrystalMarkDlg*)dlg)->m_Score[3][2] = score[1];
	((CCrystalMarkDlg*)dlg)->m_Score[3][3] = score[2];
	((CCrystalMarkDlg*)dlg)->m_Score[3][4] = score[3];

	::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_UPDATE_SCORE, 0, 0);
	Exit(dlg, FALSE);
	return TRUE;
}

UINT ExecHiyoGL(LPVOID dlg)
{
	CString command;
	CString option;
	CString exePath;

	TCHAR* ptrEnd;
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL, path, MAX_PATH);
	if ((ptrEnd = _tcsrchr(path, '\\')) != NULL)
	{
		*ptrEnd = '\0';
	}

#ifdef _M_ARM64
	exePath.Format(_T("%s\\%s"), path, HIYOGL_EXE_ARM64);
#elif _M_X64
	exePath.Format(_T("%s\\%s"), path, HIYOGL_EXE_64);
#else
	exePath.Format(_T("%s\\%s"), path, HIYOGL_EXE_32);
#endif

	option.Format(_T(""));
	command.Format(_T("\"%s\" %s"), exePath.GetString(), option.GetString());
	// ExecAndWait((TCHAR*)command.GetString(), FALSE);

	/*
	if (! CheckCodeSign(CERTNAME, exePath))
	{
		AfxMessageBox(((CCrystalMarkDlg*)dlg)->m_MesExeFileModified);
		return Exit(dlg);
	}
	*/

	if (!IsFileExist(exePath.GetString()))
	{
		AfxMessageBox(((CCrystalMarkDlg*)dlg)->m_MesExeFileNotFound);
		return Exit(dlg, TRUE);
	}

	DWORD Code = 0;
	BOOL bSuccess = FALSE;
	STARTUPINFO si = { 0 };

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	CString name;
	name.Format(_T("CrystalMarkRetro")); // , GetCurrentProcessId());
	DWORD size = 1024 * sizeof(TCHAR);
	TCHAR result[1024] = {0};

	HANDLE hSharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, size, name.GetString());
	if (hSharedMemory != NULL)
	{
		TCHAR* pMemory = (TCHAR*)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, NULL, NULL, size);
		if (pMemory != NULL)
		{
			bSuccess = CreateProcess(NULL, (TCHAR*)command.GetString(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			if (bSuccess != TRUE)
			{
				UnmapViewOfFile(pMemory);
				CloseHandle(hSharedMemory);		
				return Exit(dlg, TRUE);
			}

			WaitForInputIdle(pi.hProcess, INFINITE);
			WaitForSingleObject(pi.hProcess, INFINITE);
			GetExitCodeProcess(pi.hProcess, &Code);

			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);

			pi.hProcess = NULL;

			RtlCopyMemory(result, pMemory, size);

			UnmapViewOfFile(pMemory);
			CloseHandle(hSharedMemory);
		}
	}

	// Read Score
	__int64 score1 = 0, score2 = 0;
	__int64 cpu1 = 0, cpu2 = 0;
	/*
	score1 = GetPrivateProfileIntW(_T("hiyoGL_Scene1"), _T("Scene1_Score"), 0, iniPath);
	score2 = GetPrivateProfileIntW(_T("hiyoGL_Scene2"), _T("Scene2_Score"), 0, iniPath);
	cpu1 = GetPrivateProfileIntW(_T("hiyoGL_Scene1"), _T("Scene1_CPU"), 0, iniPath);
	cpu2 = GetPrivateProfileIntW(_T("hiyoGL_Scene2"), _T("Scene2_CPU"), 0, iniPath);
	*/

	// AfxMessageBox(result);

	CString cstr;
	CString token;
	int curPos = 0;
	cstr = result;

	token = cstr.Tokenize(_T("\n"), curPos);
	while (token != _T(""))
	{
		CString leftPart = token.SpanExcluding(_T("="));
		CString rightPart = token.Mid(leftPart.GetLength() + 1);

		if (leftPart.Find(_T("Scene1_Score")) == 0){ score1 = _ttoi64(rightPart); }
		if (leftPart.Find(_T("Scene2_Score")) == 0){ score2 = _ttoi64(rightPart); }
		if (leftPart.Find(_T("Scene1_CPU")) == 0)	{ cpu1 = _ttoi64(rightPart); }
		if (leftPart.Find(_T("Scene2_CPU")) == 0)	{ cpu2 = _ttoi64(rightPart); }
		if (leftPart.Find(_T("Scene1_Lines")) == 0) { ((CCrystalMarkDlg*)dlg)->m_Scene1Lines = _ttoi64(rightPart); }
		if (leftPart.Find(_T("Scene2_Polygon")) == 0) { ((CCrystalMarkDlg*)dlg)->m_Scene2Polygons = _ttoi64(rightPart); }
		if (leftPart.Find(_T("HiyoGL_Version")) == 0)	{ ((CCrystalMarkDlg*)dlg)->m_CMROGLVersion = rightPart; }
		if (leftPart.Find(_T("OpenGL_Version")) == 0)	{ ((CCrystalMarkDlg*)dlg)->m_OpenGLVersion = rightPart; }
		if (leftPart.Find(_T("OpenGL_Vendor")) == 0)	{ ((CCrystalMarkDlg*)dlg)->m_OpenGLVendor = rightPart; }
		if (leftPart.Find(_T("OpenGL_Renderer")) == 0) { ((CCrystalMarkDlg*)dlg)->m_OpenGLRenderer = rightPart; }

		token = cstr.Tokenize(_T("\n"), curPos);
	}

	if(((CCrystalMarkDlg*)dlg)->m_CMROGLVersion.IsEmpty() )
	{
		Exit(dlg, TRUE);
		return FALSE;
	}

	if (score1 == 0 && cpu1 > 0) { score1 = 1; }
	if (score2 == 0 && cpu2 > 0) { score2 = 1; }

	((CCrystalMarkDlg*)dlg)->m_Score[4][0] = (int)pow((double)((double)score1 * score2 * score2 * score2), 0.25);// (int)sqrt((double)(score1 * score2));
	((CCrystalMarkDlg*)dlg)->m_Score[4][1] = (int)score1;
	((CCrystalMarkDlg*)dlg)->m_Score[4][2] = (int)score2;
	((CCrystalMarkDlg*)dlg)->m_Score[4][3] = (int)cpu1;
	((CCrystalMarkDlg*)dlg)->m_Score[4][4] = (int)cpu2;

	::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_UPDATE_SCORE, 0, 0);
	Exit(dlg, FALSE);
	return TRUE;
}

UINT ExecCPUBench(LPVOID dlg)
{
	CString command;
	CString option;
	CString exePath;

	TCHAR* ptrEnd;
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL, path, MAX_PATH);
	if ((ptrEnd = _tcsrchr(path, '\\')) != NULL)
	{
		*ptrEnd = '\0';
	}

#ifdef _M_ARM
	exePath.Format(_T("%s\\%s"), path, BENCHMARK_EXE_ARM32);
#elif _M_ARM64
	exePath.Format(_T("%s\\%s"), path, BENCHMARK_EXE_ARM64);
#elif _M_X64
	exePath.Format(_T("%s\\%s"), path, BENCHMARK_EXE_64);
#else
#if _MSC_VER > 1310
	if (IsWinXpOrLater())
	{
		exePath.Format(_T("%s\\%s"), path, BENCHMARK_EXE_32);
	}
	else
	{
		exePath.Format(_T("%s\\%s"), path, BENCHMARK_EXE_32_LEGACY);
	}
#else
	exePath.Format(_T("%s\\%s"), path, BENCHMARK_EXE_32);
#endif
#endif

	/*
	if (! CheckCodeSign(CERTNAME, exePath))
	{
		AfxMessageBox(((CCrystalMarkDlg*)dlg)->m_MesExeFileModified);
		return Exit(dlg);
	}
	*/

	if (!IsFileExist(exePath.GetString()))
	{
		AfxMessageBox(((CCrystalMarkDlg*)dlg)->m_MesExeFileNotFound);
		return Exit(dlg, TRUE);
	}

	// GetVersion

	GetFileVersionEx(exePath, ((CCrystalMarkDlg*)dlg)->m_CMRCPUVersion);

	const int testCount = 6;
	int maxTestThreads = ((CCrystalMarkDlg*)dlg)->m_Threads;
	CString cstr, scoreText;
	double score[testCount] = {};

	for (int i = 0; i < testCount; i++)
	{
		if (!((CCrystalMarkDlg*)dlg)->m_BenchStatus)
		{
			return Exit(dlg, TRUE);
		}

		cstr.Format(_T("%s (%d/%d)"), _T("CPU Single"), i + 1, testCount);
		::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_UPDATE_MESSAGE, (WPARAM)&cstr, 0);

		option.Format(_T(" %d %d"), i + 1, 1);
		command.Format(_T("\"%s\" %s"), exePath.GetString(), option.GetString());
		score[i] = (double)ExecAndWait((TCHAR*)command.GetString(), TRUE);

		cstr.Format(_T("%d %f\n"), i, score[i]);
		scoreText += cstr;
	}
	
	((CCrystalMarkDlg*)dlg)->m_Score[1][1] = (int)geometricMean(score, testCount);
	// AfxMessageBox(scoreText);
	::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_UPDATE_SCORE, 0, 0);

	for (int i = 0; i < testCount; i++)
	{
		if (!((CCrystalMarkDlg*)dlg)->m_BenchStatus)
		{
			return Exit(dlg, TRUE);
		}

		cstr.Format(_T("%s (%d/%d)"), _T("CPU Multi"), i + 1, testCount);
		::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_UPDATE_MESSAGE, (WPARAM)&cstr, 0);

		/* 
		option.Format(_T(" %d %d"), i + 1, maxTestThreads);
		command.Format(_T("\"%s\" %s"), exePath.GetString(), option.GetString());
		score[i] = ExecAndWait((TCHAR*)command.GetString(), TRUE);
		*/
		score[i] = ExecsAndWait(exePath, TRUE, i + 1, maxTestThreads);

		cstr.Format(_T("%d %f\n"), i, score[i]);
		scoreText += cstr;
	}

	((CCrystalMarkDlg*)dlg)->m_Score[1][2] = (int)geometricMean(score, testCount);
	// AfxMessageBox(scoreText);
	::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_UPDATE_SCORE, 0, 0);
	Exit(dlg, FALSE);

	return TRUE;
}

UINT Exit(LPVOID dlg, BOOL forceExit)
{
	static CString cstr;
	cstr = _T("");

	::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_UPDATE_MESSAGE, NULL, (LPARAM)&cstr);

	if (! BenchmarkAll || forceExit)
	{
		::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_EXIT_BENCHMARK, 0, 0);
		((CCrystalMarkDlg*)dlg)->m_BenchStatus = FALSE;
		((CCrystalMarkDlg*)dlg)->m_WinThread = NULL;
	}

	return 0;
}