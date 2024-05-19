/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "../stdafx.h"
#include "CrystalMark.h"
#include "CrystalMarkDlg.h"
#include "DiskBench.h"

#include <afxmt.h>
#include <winioctl.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#pragma warning(disable : 4996)

static CString TestFilePath;
static CString TestFileDir;
static CString DiskSpdExe;

static HANDLE hFile;
static int DiskTestCount;
static UINT64 DiskTestSize;
static int BenchType[2];
static int BenchSize[2];
static int BenchQueues[2];
static int BenchThreads[2];
// static int Affinity;
static int MixRatio;

static void ShowErrorMessage(CString message);
static void Interval(void* dlg);

static BOOL Init(void* dlg);
static void DiskSpd(void* dlg, DISK_SPD_CMD cmd);

static UINT Exit(void* dlg, BOOL forceExit);
static void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static volatile BOOL WaitFlag;

extern BOOL BenchmarkAll;

#define DISK_SPD_EXE_32           _T("Resource\\Benchmark\\DiskSpd\\DiskSpd32.exe")
#define DISK_SPD_EXE_64           _T("Resource\\Benchmark\\DiskSpd\\DiskSpd64.exe")
#define DISK_SPD_EXE_32_LEGACY    _T("Resource\\Benchmark\\DiskSpd\\DiskSpd32L.exe")
#define DISK_SPD_EXE_64_LEGACY    _T("Resource\\Benchmark\\DiskSpd\\DiskSpd64L.exe")
#define DISK_SPD_EXE_ARM32        _T("Resource\\Benchmark\\DiskSpd\\DiskSpdA32.exe")
#define DISK_SPD_EXE_ARM64        _T("Resource\\Benchmark\\DiskSpd\\DiskSpdA64.exe")

extern PROCESS_INFORMATION pi;

int ExecAndWait(TCHAR *pszCmd, BOOL bNoWindow, double *latency)
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

	CString name;
	name.Format(_T("CrystalDiskMark%08X"), GetCurrentProcessId());
	DWORD size = 8;

	HANDLE hSharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, size, name.GetString());
	if (hSharedMemory != NULL)
	{
		double* pMemory = (double*)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, NULL, NULL, size);
		if (pMemory != NULL)
		{
			bSuccess = CreateProcess(NULL, pszCmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			if (bSuccess != TRUE)
			{
				UnmapViewOfFile(pMemory);
				CloseHandle(hSharedMemory);
				return 0;
			}

			WaitForInputIdle(pi.hProcess, INFINITE);
			WaitForSingleObject(pi.hProcess, INFINITE);
			GetExitCodeProcess(pi.hProcess, &Code);

			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);

			pi.hProcess = NULL;

			*latency = (double)*pMemory * 1000; // milli sec to micro sec

			UnmapViewOfFile(pMemory);
			CloseHandle(hSharedMemory);
		}
	}

	return Code;
}

void ShowErrorMessage(CString message)
{
	DWORD lastErrorCode = GetLastError();
	CString errorMessage;
	LPVOID lpMessageBuffer;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastErrorCode, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMessageBuffer, 0, NULL);
	errorMessage.Format(_T("0x%08X:%s"), lastErrorCode, (LPTSTR) lpMessageBuffer);

	AfxMessageBox(message + _T("\r\n") + errorMessage);
	LocalFree( lpMessageBuffer );
}

void Interval(void* dlg)
{
	int intervalTime = ((CCrystalMarkDlg*) dlg)->m_IntervalTime;
	CString title;

	for (int i = 0; i < intervalTime; i++)
	{
		if (!((CCrystalMarkDlg*) dlg)->m_BenchStatus)
		{
			return;
		}
		title.Format(_T("Interval Time %d/%d sec"), i, intervalTime);
		::PostMessage(((CCrystalMarkDlg*) dlg)->GetSafeHwnd(), WM_UPDATE_MESSAGE, (WPARAM) &title, 0);
		Sleep(1000);
	}
}

UINT ExecDiskBenchAll(LPVOID dlg)
{
	int benchmark = ((CCrystalMarkDlg*)dlg)->m_Benchmark;

	if(Init(dlg))
	{
		if (benchmark & BENCHMARK_READ)
		{
			if (!((CCrystalMarkDlg*)dlg)->m_BenchStatus) { return Exit(dlg, TRUE); }
			DiskSpd(dlg, TEST_READ_0);
			if (!((CCrystalMarkDlg*)dlg)->m_BenchStatus) { return Exit(dlg, TRUE); }
			Interval(dlg);
			if (!((CCrystalMarkDlg*)dlg)->m_BenchStatus) { return Exit(dlg, TRUE); }
			DiskSpd(dlg, TEST_READ_1);
		}
		if ((benchmark & BENCHMARK_READ) && (benchmark & BENCHMARK_WRITE))
		{
			if (!((CCrystalMarkDlg*)dlg)->m_BenchStatus) { return Exit(dlg, TRUE); }
			Interval(dlg);
		}
		if (benchmark & BENCHMARK_WRITE)
		{
			if (!((CCrystalMarkDlg*)dlg)->m_BenchStatus) { return Exit(dlg, TRUE); }
			DiskSpd(dlg, TEST_WRITE_0);
			if (!((CCrystalMarkDlg*)dlg)->m_BenchStatus) { return Exit(dlg, TRUE); }
			Interval(dlg);
			if (!((CCrystalMarkDlg*)dlg)->m_BenchStatus) { return Exit(dlg, TRUE); }
			DiskSpd(dlg, TEST_WRITE_1); 
		}
	}
	Exit(dlg, FALSE);
	return 1;
}

BOOL Init(void* dlg)
{
	BOOL FlagArc;
	BOOL result;
	static CString cstr;
	TCHAR drive;

	ULARGE_INTEGER freeBytesAvailableToCaller;
	ULARGE_INTEGER totalNumberOfBytes;
	ULARGE_INTEGER totalNumberOfFreeBytes;

	// Init m_Ini
	TCHAR *ptrEnd;
	TCHAR temp[MAX_PATH];
	::GetModuleFileName(NULL, temp, MAX_PATH);
	if ((ptrEnd = _tcsrchr(temp, '\\')) != NULL)
	{
		*ptrEnd = '\0';
	}

	pi.hProcess = NULL;

	((CCrystalMarkDlg*)dlg)->m_DiskSpdVersion = _T("DiskSpd 2.0.21a");
#ifdef _M_ARM
	DiskSpdExe.Format(_T("%s\\%s"), temp, DISK_SPD_EXE_ARM32);
#elif _M_ARM64
	DiskSpdExe.Format(_T("%s\\%s"), temp, DISK_SPD_EXE_ARM64);
#elif _M_X64
	if(IsWin8orLater())
	{
		DiskSpdExe.Format(_T("%s\\%s"), temp, DISK_SPD_EXE_64);
	}
	else
	{
		DiskSpdExe.Format(_T("%s\\%s"), temp, DISK_SPD_EXE_64_LEGACY);
		((CCrystalMarkDlg*)dlg)->m_DiskSpdVersion = _T("DiskSpd 2.0.15a");
	}
#else
#if _MSC_VER > 1310
	if (IsWin8orLater())
	{
		DiskSpdExe.Format(_T("%s\\%s"), temp, DISK_SPD_EXE_32);
	}
	else
	{
		DiskSpdExe.Format(_T("%s\\%s"), temp, DISK_SPD_EXE_32_LEGACY);
		((CCrystalMarkDlg*)dlg)->m_DiskSpdVersion = _T("DiskSpd 2.0.15a");
	}
#else
	DiskSpdExe.Format(_T("%s\\%s"), temp, DISK_SPD_EXE_32_LEGACY);
	((CCrystalMarkDlg*)dlg)->m_DiskSpdVersion = _T("DiskSpd 2.0.15a");
#endif
#endif

	/*
	if (! CheckCodeSign(CERTNAME_DISKSPD, DiskSpdExe))
	{
		AfxMessageBox(((CCrystalMarkDlg*)dlg)->m_MesExeFileModified);
		Exit(dlg);
		return FALSE;
	}
	*/

	if (! IsFileExist(DiskSpdExe))
	{
		AfxMessageBox(((CCrystalMarkDlg*) dlg)->m_MesDiskSpdNotFound);
		return Exit(dlg, TRUE);
	}
	DiskTestCount = ((CCrystalMarkDlg*) dlg)->m_IndexTestCount;
	
	CString testSize = ((CCrystalMarkDlg*)dlg)->m_ValueTestSize;
	if (testSize.Find(_T("M")) == -1) // GiB
	{
		DiskTestSize = (UINT64)_tstoi(testSize) * 1024;
	}
	else // MiB
	{
		DiskTestSize = (UINT64)_tstoi(testSize);
	}

	for (int i = 0; i < 2; i++)
	{
		BenchType[i] = ((CCrystalMarkDlg*)dlg)->m_BenchType[i];
		BenchSize[i] = ((CCrystalMarkDlg*)dlg)->m_BenchSize[i];
		BenchQueues[i] = ((CCrystalMarkDlg*)dlg)->m_BenchQueues[i];
		BenchThreads[i] = ((CCrystalMarkDlg*)dlg)->m_BenchThreads[i];
	}

	CString RootPath;
	// if(((CCrystalMarkDlg*)dlg)->m_MaxIndexTestDrive != ((CCrystalMarkDlg*)dlg)->m_IndexTestDrive)
	{
		drive = ((CCrystalMarkDlg*)dlg)->m_ValueTestDrive.GetAt(0);
		cstr.Format(_T("%C:"), drive);
		GetDiskFreeSpaceEx(cstr, &freeBytesAvailableToCaller, &totalNumberOfBytes, &totalNumberOfFreeBytes);
		if (totalNumberOfBytes.QuadPart < ((ULONGLONG)8 * 1024 * 1024 * 1024)) // < 8 GB
		{
			((CCrystalMarkDlg*)dlg)->m_TestDriveInfo.Format(_T("%C: %.1f%% (%.1f/%.1f MiB)"), drive,
				(double)(totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart) / (double)totalNumberOfBytes.QuadPart * 100,
				(totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart) / 1024 / 1024.0,
				totalNumberOfBytes.QuadPart / 1024 / 1024.0);
		}
		else
		{
			((CCrystalMarkDlg*)dlg)->m_TestDriveInfo.Format(_T("%C: %.1f%% (%.1f/%.1f GiB)"), drive,
				(double)(totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart) / (double)totalNumberOfBytes.QuadPart * 100,
				(totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart) / 1024 / 1024 / 1024.0,
				totalNumberOfBytes.QuadPart / 1024 / 1024 / 1024.0);
		}
		RootPath.Format(_T("%c:\\"), drive);
	}
	/*
	else
	{
		RootPath = ((CCrystalMarkDlg*)dlg)->m_TestTargetPath;
		RootPath += _T("\\");
	}
	*/

	TestFileDir.Format(_T("%sCrystalDiskMark%08X"), RootPath.GetString(), timeGetTime());
	CreateDirectory(TestFileDir, NULL);
	TestFilePath.Format(_T("%s\\CrystalDiskMark%08X.tmp"), TestFileDir.GetString(), timeGetTime());

	DWORD FileSystemFlags;
	GetVolumeInformation(RootPath, NULL, NULL, NULL, NULL, &FileSystemFlags, NULL, NULL);
	if(FileSystemFlags & FS_VOL_IS_COMPRESSED)
	{
		FlagArc = TRUE;
	}
	else
	{
		FlagArc = FALSE;
	}

// Check Disk Capacity //
	OSVERSIONINFO osVersionInfo;
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVersionInfo);

	ULARGE_INTEGER FreeBytesAvailableToCaller, TotalNumberOfBytes, TotalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(RootPath, &FreeBytesAvailableToCaller, &TotalNumberOfBytes, &TotalNumberOfFreeBytes);
	if(DiskTestSize > TotalNumberOfFreeBytes.QuadPart / 1024 / 1024 )
	{
		AfxMessageBox(((CCrystalMarkDlg*)dlg)->m_MesDiskCapacityError);
		((CCrystalMarkDlg*)dlg)->m_BenchStatus = FALSE;
		return FALSE;
	}

	CString title;
	title.Format(_T("Preparing... Create Test File"));
	::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_UPDATE_MESSAGE, (WPARAM)& title, 0);

// Preapare Test File
	hFile = ::CreateFile(TestFilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING|FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(((CCrystalMarkDlg*)dlg)->m_MesDiskCreateFileError);
		((CCrystalMarkDlg*)dlg)->m_BenchStatus = FALSE;
		return FALSE;
	}

// Set End Of File to prevent fragmentation of test file
#if _MSC_VER > 1310
	LARGE_INTEGER nFileSize;
	nFileSize.QuadPart = 1024 * 1024 * DiskTestSize;

	LARGE_INTEGER nStart;
	nStart.QuadPart = 0;

	SetFilePointerEx(hFile, nFileSize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);
	SetFilePointerEx(hFile, nStart, NULL, FILE_BEGIN);
#else
	LONG nFileSize = 1024 * 1024 * (LONG)DiskTestSize;
	LONG nStart = 0;

	SetFilePointer(hFile, (LONG)nFileSize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);
	SetFilePointer(hFile, (LONG)nStart, NULL, FILE_BEGIN);
#endif

// COMPRESSION_FORMAT_NONE
	USHORT lpInBuffer = COMPRESSION_FORMAT_NONE;
	DWORD lpBytesReturned = 0;
	DeviceIoControl(hFile, FSCTL_SET_COMPRESSION, (LPVOID) &lpInBuffer,
				sizeof(USHORT), NULL, 0, (LPDWORD)&lpBytesReturned, NULL);

// Fill Test Data
	char* buf = NULL;
	int BufSize;
	int Loop;
	DWORD writesize;
	BufSize = 1024 * 1024;
	Loop = (int)DiskTestSize;

	buf = (char*) VirtualAlloc(NULL, BufSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (buf == NULL)
	{
		AfxMessageBox(_T("Failed VirtualAlloc()."));
		((CCrystalMarkDlg*) dlg)->m_BenchStatus = FALSE;
		return FALSE;
	}

	if (((CCrystalMarkDlg*) dlg)->m_TestData == TEST_DATA_ALL0X00)
	{
		for (int i = 0; i < BufSize; i++)
		{
			buf[i] = 0;
		}
	}
	else
	{
		// Compatible with DiskSpd
		for (int i = 0; i < BufSize; i++)
		{
			buf[i] = (char) (rand() % 256);
		}
	}

	for (int i = 0; i < Loop; i++)
	{
		if (((CCrystalMarkDlg*) dlg)->m_BenchStatus)
		{
			result = WriteFile(hFile, buf, BufSize, &writesize, NULL);
		}
		else
		{
			CloseHandle(hFile);
			VirtualFree(buf, 0, MEM_RELEASE);
			((CCrystalMarkDlg*) dlg)->m_BenchStatus = FALSE;
			return FALSE;
		}
	}
	VirtualFree(buf, 0, MEM_RELEASE);
	CloseHandle(hFile);

	return TRUE;
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if(idEvent == TIMER_ID)
	{
		WaitFlag = FALSE;
		KillTimer(hwnd, idEvent);
	}
}

UINT Exit(void* dlg, BOOL forceExit)
{
	DeleteFile(TestFilePath);
	RemoveDirectory(TestFileDir);

	static CString cstr;
	cstr = _T("");

	if(((CCrystalMarkDlg*)dlg)->m_TestData == TEST_DATA_ALL0X00)
	{
		cstr = ALL_0X00_0FILL;
	}

	::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_UPDATE_MESSAGE, NULL, (LPARAM)&cstr);

	if (! BenchmarkAll || forceExit)
	{
		::PostMessage(((CCrystalMarkDlg*)dlg)->GetSafeHwnd(), WM_EXIT_BENCHMARK, 0, 0);
		((CCrystalMarkDlg*)dlg)->m_BenchStatus = FALSE;
		((CCrystalMarkDlg*)dlg)->m_WinThread = NULL;
	}

	return 0;
}

void DiskSpd(void* dlg, DISK_SPD_CMD cmd)
{
	static CString cstr;
	int *maxScore = NULL;
	CString command;
	CString title;
	CString option;
	CString bufOption;

	int duration = 5;
	int index = 0;
	int j = 0;

	if (!((CCrystalMarkDlg*) dlg)->m_BenchStatus)
	{
		return;
	}

	if (((CCrystalMarkDlg*) dlg)->m_TestData == TEST_DATA_ALL0X00)
	{
		bufOption += _T(" -Z");
	}
	else
	{
		switch (cmd)
		{
		case TEST_WRITE_0:
		case TEST_WRITE_1:
			index = cmd - TEST_WRITE_0;
			cstr.Format(_T(" -Z%dK"), BenchSize[index]);
			bufOption += cstr;
			break;
		}
	}

	switch (cmd)
	{
	case TEST_READ_0:
	case TEST_READ_1:
		index = cmd - TEST_READ_0;
		if (BenchType[index])
		{
			title.Format(_T("Random Read"));
			option.Format(_T("-b%dK -o%d -t%d -W0 -S -w0 -r"), BenchSize[index], BenchQueues[index], BenchThreads[index]);
		}
		else
		{
			title.Format(_T("Sequential Read"));
			option.Format(_T("-b%dK -o%d -t%d -W0 -S -w0"), BenchSize[index], BenchQueues[index], BenchThreads[index]);
		}
		maxScore = (int*) &(((CCrystalMarkDlg*)dlg)->m_Score[2][index + 1]);
		break;
	case TEST_WRITE_0:
	case TEST_WRITE_1:
		index = cmd - TEST_WRITE_0;
		if (BenchType[index])
		{
			title.Format(_T("Random Write"));
			option.Format(_T("-b%dK -o%d -t%d -W0 -S -w100 -r"), BenchSize[index], BenchQueues[index], BenchThreads[index]);
		}
		else
		{
			title.Format(_T("Sequential Write"));
			option.Format(_T("-b%dK -o%d -t%d -W0 -S -w100"), BenchSize[index], BenchQueues[index], BenchThreads[index]);
		}
		option += bufOption;
		maxScore = (int*) &(((CCrystalMarkDlg*)dlg)->m_Score[2][index + 3]);
		break;
	}

	option += _T(" -ag");

	int score = 0;
	double latency = 0.0;

	if (maxScore == NULL)
	{
		return ;
	}
	*maxScore = 0;

	for (j = 0; j <= DiskTestCount; j++)
	{
		if (j == 0)
		{
			duration = 5;
			cstr.Format(_T("Preparing... %s"), title.GetString());
		}
		else
		{
			duration = ((CCrystalMarkDlg*)dlg)->m_MeasureTime;
			cstr.Format(_T("%s (%d/%d)"), title.GetString(), j, DiskTestCount);
		}
		::PostMessage(((CCrystalMarkDlg*) dlg)->GetSafeHwnd(), WM_UPDATE_MESSAGE, (WPARAM) &cstr, 0);
		

		command.Format(_T("\"%s\" %s -d%d -A%d -L \"%s\""), (LPTSTR)DiskSpdExe.GetString(), (LPTSTR)option.GetString(), duration, GetCurrentProcessId(), (LPTSTR)TestFilePath.GetString());

		score = ExecAndWait((TCHAR*) (command.GetString()), TRUE, &latency) / 1000;

		if (score == 0 && latency > 0) { score = 1; }

		if (j > 0 && score > *maxScore)
		{
			*maxScore = score;
			::PostMessage(((CCrystalMarkDlg*) dlg)->GetSafeHwnd(), WM_UPDATE_SCORE, 0, 0);
		}

		if (!((CCrystalMarkDlg*) dlg)->m_BenchStatus)
		{
			return;
		}
	}
	::PostMessage(((CCrystalMarkDlg*) dlg)->GetSafeHwnd(), WM_UPDATE_SCORE, 0, 0);
}
