/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#include "AboutDlg.h"
#include "FontSelectionDlg.h"

#include "DialogFx.h"
#include "MainDialogFx.h"
#include "ButtonFx.h"
#include "StaticFx.h"
#include "EditFx.h"
#include "ComboBoxFx.h"
#include "ListCtrlFx.h"
#include "UtilityFx.h"
#include "OsInfoFx.h"
#include "SystemInfoFx.h"

class CCrystalMarkDlg : public CMainDialogFx
{
public:
	CCrystalMarkDlg(CWnd* pParent = NULL);
	~CCrystalMarkDlg();

	enum { IDD = IDD_MAIN };

#ifdef SUISHO_SHIZUKU_SUPPORT
	static const int SIZE_X = 1200;
	static const int SIZE_Y = 616;
	static const int SIZE_MIN_Y = 616;
	static const int SIZE_MAX_Y = 616;
	static const int OFFSET_X = 224;
#else
	static const int SIZE_X = 976;
	static const int SIZE_Y = 616;
	static const int SIZE_MIN_Y = 616;
	static const int SIZE_MAX_Y = 616;
	static const int OFFSET_X = 0;
#endif

	void ChangeLang(CString LangName);
	void UpdateDialogSize();
	void SetWindowTitle(CString message);
	void SaveText(CString fileName);
	void SetMeter(CStaticFx* control, int score);
	
	// Benchmark
	volatile CWinThread* m_WinThread;
	volatile BOOL m_BenchStatus;
	void UpdateThemeInfo();

	// CPU Information
	int m_Cores;
	int m_Threads;

	// System Information
	CString m_CpuInfo;
	CString m_GpuInfo;
	CString m_SystemInfo;
	CString m_OsInfo;
	CString m_ScreenInfo;
	CString m_MemoryInfo;

	// Score
	__int64 m_Score[5][5];

	// Benchmark Client Version
	CString m_CMRCPUVersion;
	CString m_DiskSpdVersion;
	CString m_CMRGDIVersion;
	CString m_CMROGLVersion;

	// HiyoGL
	__int64 m_Scene1Lines;
	__int64 m_Scene2Polygons;
	CString m_OpenGLVersion;
	CString m_OpenGLVendor;
	CString m_OpenGLRenderer;

	// Margin
	int m_MarginButtonTop;
	int m_MarginButtonLeft;
	int m_MarginButtonBottom;
	int m_MarginButtonRight;
	int m_MarginMeterTop;
	int m_MarginMeterLeft;
	int m_MarginMeterBottom;
	int m_MarginMeterRight;
	int m_MarginCommentTop;
	int m_MarginCommentLeft;
	int m_MarginCommentBottom;
	int m_MarginCommentRight;

	// CrystalDiskMark 8.0 compatible
	CString m_ValueTestSize;
	CString m_ValueTestDrive;
	CString m_TestDriveInfo;
	CString m_TestTargetPath;
	long m_TestDriveLetter;

	int m_IndexTestCount;

	int m_BenchType[2];
	int m_BenchSize[2];
	int m_BenchQueues[2];
	int m_BenchThreads[2];
	int m_IntervalTime;
	int m_MeasureTime;

	int m_TestData;
	int m_Profile;
	int m_Benchmark;
		
	// Message //
	CString m_MesDiskCapacityError;
	CString m_MesDiskWriteError;
	CString m_MesDiskReadError;
	CString m_MesStopBenchmark;
	CString m_MesDiskCreateFileError;
	CString m_MesDiskSpdNotFound;
	CString m_MesExeFileNotFound;
	CString m_MesExeFileModified;
	CString m_MesAttachScreenshotManually;

protected:
	void SetControlFont();
	BOOL CheckRadioZoomType(int id, int value);
	void CheckRadioZoomType();
	void EnableMenus();
	void DisableMenus();

	void UpdateScore();
	void ChangeControlStatus(BOOL status);
	void Stop();

	CString SD(__int64 score);
	CString VOICE(__int64 score);

	BOOL CheckThemeEdition(CString name);
	void DoDataExchange(CDataExchange* pDX);
	BOOL OnInitDialog();
	void SetClientSize(int sizeX, int sizeY, double zoomRatio);
	void OnOK();
	void OnCancel();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT OnQueryEndSession(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnUpdateScore(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExitBenchmark(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStartBenchmark(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSecretVoice(WPARAM wParam, LPARAM lParam);
	afx_msg void OnExit();
	afx_msg void OnCopy();
	afx_msg void OnSaveText();
	afx_msg void OnSaveImage();
	afx_msg void OnZoom50();
	afx_msg void OnZoom75();
	afx_msg void OnZoom100();
	afx_msg void OnZoom125();
	afx_msg void OnZoom150();
	afx_msg void OnZoom200();
	afx_msg void OnZoom250();
	afx_msg void OnZoom300();
	afx_msg void OnZoomAuto();
	afx_msg void OnAbout();
	afx_msg void OnFontSetting();
	afx_msg void OnHelp();
	afx_msg void OnCrystalDewWorld();
	afx_msg void OnSD();
//	afx_msg void OnPost();
	afx_msg void OnTweet();
	afx_msg void OnStart0();
	afx_msg void OnStart1();
	afx_msg void OnStart2();
	afx_msg void OnStart3();
	afx_msg void OnStart4();

	afx_msg void OnMainUIinEnglish();

#ifdef SUISHO_AOI_SUPPORT
	afx_msg void OnVoiceEnglish();
	afx_msg void OnVoiceJapanese();
#endif

#ifdef SUISHO_SHIZUKU_SUPPORT
	afx_msg void OnVoiceVolume000();
	afx_msg void OnVoiceVolume010();
	afx_msg void OnVoiceVolume020();
	afx_msg void OnVoiceVolume030();
	afx_msg void OnVoiceVolume040();
	afx_msg void OnVoiceVolume050();
	afx_msg void OnVoiceVolume060();
	afx_msg void OnVoiceVolume070();
	afx_msg void OnVoiceVolume080();
	afx_msg void OnVoiceVolume090();
	afx_msg void OnVoiceVolume100();
#endif

	HICON m_hIcon;
	HICON m_hIconMini;
	BOOL m_AdminMode;
	BOOL m_MainUIinEnglish;

	CStaticFx* m_CtrlScore[5][5];

	CAboutDlg* m_AboutDlg;

	CButtonFx m_CtrlStart0;
	CButtonFx m_CtrlStart1;
	CButtonFx m_CtrlStart2;
	CButtonFx m_CtrlStart3;
	CButtonFx m_CtrlStart4;

	CStaticFx m_CtrlScore0_0;
	CStaticFx m_CtrlScore1_0;
	CStaticFx m_CtrlScore2_0;
	CStaticFx m_CtrlScore3_0;
	CStaticFx m_CtrlScore4_0;

	CStaticFx m_CtrlScore1_1;
	CStaticFx m_CtrlScore1_2;
	CStaticFx m_CtrlScore1_3;
	CStaticFx m_CtrlScore1_4;

	CStaticFx m_CtrlScore2_1;
	CStaticFx m_CtrlScore2_2;
	CStaticFx m_CtrlScore2_3;
	CStaticFx m_CtrlScore2_4;

	CStaticFx m_CtrlScore3_1;
	CStaticFx m_CtrlScore3_2;
	CStaticFx m_CtrlScore3_3;
	CStaticFx m_CtrlScore3_4;

	CStaticFx m_CtrlScore4_1;
	CStaticFx m_CtrlScore4_2;
	CStaticFx m_CtrlScore4_3;
	CStaticFx m_CtrlScore4_4;

	CStaticFx m_LabelSystemInfo1;
	CStaticFx m_LabelSystemInfo2;
	CStaticFx m_LabelSystemInfo3;
	CStaticFx m_LabelSystemInfo4;
	CStaticFx m_LabelSystemInfo5;
	CStaticFx m_LabelSystemInfo6;

	CStaticFx m_CtrlSystemInfo1;
	CStaticFx m_CtrlSystemInfo2;
	CStaticFx m_CtrlSystemInfo3;
	CStaticFx m_CtrlSystemInfo4;
	CStaticFx m_CtrlSystemInfo5;
	CStaticFx m_CtrlSystemInfo6;

#if _MSC_VER == 1310
	CStaticFx m_CtrlCommentUpper;
#endif
	CEditFx m_CtrlComment;

	CButtonFx m_CtrlSns1;
/*	CButtonFx m_CtrlSns2;
	CButtonFx m_CtrlSns3;
	CButtonFx m_CtrlSns4;
	CButtonFx m_CtrlSns5;
	CButtonFx m_CtrlSns6;
	CButtonFx m_CtrlSns7;
	CButtonFx m_CtrlSns8;
*/
	CStaticFx m_CtrlCrystalMark;
	CButtonFx m_CtrlPost;
#ifdef SUISHO_SHIZUKU_SUPPORT
	CButtonFx m_CtrlSD;
#endif
	CButtonFx m_CtrlSettings;

//	CButtonFx m_CtrlAds1;
//	CButtonFx m_CtrlAds2;
//	CButtonFx m_CtrlAds3;
};
