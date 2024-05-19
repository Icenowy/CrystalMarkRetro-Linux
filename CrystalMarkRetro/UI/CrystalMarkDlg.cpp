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

#include "AboutDlg.h"

#include <math.h>
#include <afxinet.h>
#pragma comment(lib, "wininet.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern PROCESS_INFORMATION pi;

CCrystalMarkDlg::CCrystalMarkDlg(CWnd* pParent /*=NULL*/)
	: CMainDialogFx(CCrystalMarkDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIconMini = AfxGetApp()->LoadIcon(IDI_TRAY_ICON);
	m_SizeX = SIZE_X;
	m_SizeY = SIZE_Y;
	m_AdminMode = IsUserAnAdmin();
	m_MainUIinEnglish = GetPrivateProfileInt(_T("Setting"), _T("MainUIinEnglish"), 0, m_Ini);

	m_WinThread = NULL;
	m_BenchStatus = FALSE;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			m_Score[i][j] = 0;
		}
	}

	m_CtrlScore[0][0] = &m_CtrlScore0_0;

	m_CtrlScore[1][0] = &m_CtrlScore1_0;
	m_CtrlScore[2][0] = &m_CtrlScore2_0;
	m_CtrlScore[3][0] = &m_CtrlScore3_0;
	m_CtrlScore[4][0] = &m_CtrlScore4_0;

	m_CtrlScore[1][1] = &m_CtrlScore1_1;
	m_CtrlScore[1][2] = &m_CtrlScore1_2;
	m_CtrlScore[1][3] = &m_CtrlScore1_3;
	m_CtrlScore[1][4] = &m_CtrlScore1_4;

	m_CtrlScore[2][1] = &m_CtrlScore2_1;
	m_CtrlScore[2][2] = &m_CtrlScore2_2;
	m_CtrlScore[2][3] = &m_CtrlScore2_3;
	m_CtrlScore[2][4] = &m_CtrlScore2_4;

	m_CtrlScore[3][1] = &m_CtrlScore3_1;
	m_CtrlScore[3][2] = &m_CtrlScore3_2;
	m_CtrlScore[3][3] = &m_CtrlScore3_3;
	m_CtrlScore[3][4] = &m_CtrlScore3_4;

	m_CtrlScore[4][1] = &m_CtrlScore4_1;
	m_CtrlScore[4][2] = &m_CtrlScore4_2;
	m_CtrlScore[4][3] = &m_CtrlScore4_3;
	m_CtrlScore[4][4] = &m_CtrlScore4_4;

	m_AboutDlg = NULL;

#ifdef SUISHO_AOI_SUPPORT
	m_DefaultTheme = _T("Aoi");
	m_RecommendTheme = _T("Aoi");
	m_ThemeKeyName = _T("ThemeAoi");

	m_MarginButtonTop = 16;
	m_MarginButtonLeft = 0;
	m_MarginButtonBottom = 16;
	m_MarginButtonRight = 0;
	m_MarginMeterTop = 4;
	m_MarginMeterLeft = 8;
	m_MarginMeterBottom = 4;
	m_MarginMeterRight = 8;
	m_MarginCommentTop = 12;
	m_MarginCommentLeft = 16;
	m_MarginCommentBottom = 0;
	m_MarginCommentRight = 64;

#elif SUISHO_SHIZUKU_SUPPORT
	m_DefaultTheme = _T("Shizuku");
	m_RecommendTheme = _T("Shizuku");
	m_ThemeKeyName = _T("ThemeShizuku");

	m_MarginButtonTop = 16;
	m_MarginButtonLeft = 0;
	m_MarginButtonBottom = 16;
	m_MarginButtonRight = 0;
	m_MarginButtonRight = 0;
	m_MarginMeterTop = 4;
	m_MarginMeterLeft = 8;
	m_MarginMeterBottom = 4;
	m_MarginMeterRight = 8;
	m_MarginCommentTop = 0;
	m_MarginCommentLeft = 16;
	m_MarginCommentBottom = 0;
	m_MarginCommentRight = 16;
#else
	m_DefaultTheme = _T("Default");
	m_RecommendTheme = _T("Default");
	m_ThemeKeyName = _T("Theme");

	m_MarginButtonTop = 16;
	m_MarginButtonLeft = 0;
	m_MarginButtonBottom = 16;
	m_MarginButtonRight = 0;
	m_MarginMeterTop = 4;
	m_MarginMeterLeft = 8;
	m_MarginMeterBottom = 4;
	m_MarginMeterRight = 8;
	m_MarginCommentTop = 0;
	m_MarginCommentLeft = 16;
	m_MarginCommentBottom = 0;
	m_MarginCommentRight = 16;
#endif

	m_BackgroundName = _T("Background");
	m_RandomThemeLabel = _T("Random");
	m_RandomThemeName = _T("");

	m_Cores = 1;
	m_Threads = 1;

	// CrystalDiskMark
	TCHAR str[MAX_PATH];
	UINT result = GetWindowsDirectory(str, MAX_PATH);
	if (result > 0)
	{
		m_ValueTestDrive = str;
	}

	m_ValueTestSize = _T("1GiB");
	m_TestDriveInfo = _T("");
	m_TestTargetPath = _T("C:\\");
	m_TestDriveLetter = 2;

	m_IndexTestCount = 5; // default loop => 5

	m_BenchType[0] = 0;
	m_BenchType[1] = 1;
	m_BenchSize[0] = 1024;
	m_BenchSize[1] = 4;
	m_BenchQueues[0] = 1;
	m_BenchQueues[1] = 1;
	m_BenchThreads[0] = 1;
	m_BenchThreads[1] = 1;

	m_IntervalTime = 5;
	m_MeasureTime = 5;

	m_TestData = 0;
	m_Profile = 0;
	m_Benchmark = 3; // BENCHMARK_READ_WRITE

	m_Scene1Lines = 0;
	m_Scene2Polygons = 0;
}

CCrystalMarkDlg::~CCrystalMarkDlg()
{
#ifdef SUISHO_SHIZUKU_SUPPORT
	AlertSound(_T(""), 0);
#endif
}

void CCrystalMarkDlg::DoDataExchange(CDataExchange* pDX)
{
	CMainDialogFx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_START_0, m_CtrlStart0);
	DDX_Control(pDX, IDC_START_1, m_CtrlStart1);
	DDX_Control(pDX, IDC_START_2, m_CtrlStart2);
	DDX_Control(pDX, IDC_START_3, m_CtrlStart3);
	DDX_Control(pDX, IDC_START_4, m_CtrlStart4);

	DDX_Control(pDX, IDC_SCORE_0_0, m_CtrlScore0_0);

	DDX_Control(pDX, IDC_SCORE_1_0, m_CtrlScore1_0);
	DDX_Control(pDX, IDC_SCORE_2_0, m_CtrlScore2_0);
	DDX_Control(pDX, IDC_SCORE_3_0, m_CtrlScore3_0);
	DDX_Control(pDX, IDC_SCORE_4_0, m_CtrlScore4_0);

	DDX_Control(pDX, IDC_SCORE_1_1, m_CtrlScore1_1);
	DDX_Control(pDX, IDC_SCORE_1_2, m_CtrlScore1_2);
	DDX_Control(pDX, IDC_SCORE_1_3, m_CtrlScore1_3);
	DDX_Control(pDX, IDC_SCORE_1_4, m_CtrlScore1_4);

	DDX_Control(pDX, IDC_SCORE_2_1, m_CtrlScore2_1);
	DDX_Control(pDX, IDC_SCORE_2_2, m_CtrlScore2_2);
	DDX_Control(pDX, IDC_SCORE_2_3, m_CtrlScore2_3);
	DDX_Control(pDX, IDC_SCORE_2_4, m_CtrlScore2_4);

	DDX_Control(pDX, IDC_SCORE_3_1, m_CtrlScore3_1);
	DDX_Control(pDX, IDC_SCORE_3_2, m_CtrlScore3_2);
	DDX_Control(pDX, IDC_SCORE_3_3, m_CtrlScore3_3);
	DDX_Control(pDX, IDC_SCORE_3_4, m_CtrlScore3_4);

	DDX_Control(pDX, IDC_SCORE_4_1, m_CtrlScore4_1);
	DDX_Control(pDX, IDC_SCORE_4_2, m_CtrlScore4_2);
	DDX_Control(pDX, IDC_SCORE_4_3, m_CtrlScore4_3);
	DDX_Control(pDX, IDC_SCORE_4_4, m_CtrlScore4_4);

	DDX_Control(pDX, IDC_LABEL_SYSTEM_INFO_1, m_LabelSystemInfo1);
	DDX_Control(pDX, IDC_LABEL_SYSTEM_INFO_2, m_LabelSystemInfo2);
	DDX_Control(pDX, IDC_LABEL_SYSTEM_INFO_3, m_LabelSystemInfo3);
	DDX_Control(pDX, IDC_LABEL_SYSTEM_INFO_4, m_LabelSystemInfo4);
	DDX_Control(pDX, IDC_LABEL_SYSTEM_INFO_5, m_LabelSystemInfo5);
	DDX_Control(pDX, IDC_LABEL_SYSTEM_INFO_6, m_LabelSystemInfo6);

	DDX_Control(pDX, IDC_SYSTEM_INFO_1, m_CtrlSystemInfo1);
	DDX_Control(pDX, IDC_SYSTEM_INFO_2, m_CtrlSystemInfo2);
	DDX_Control(pDX, IDC_SYSTEM_INFO_3, m_CtrlSystemInfo3);
	DDX_Control(pDX, IDC_SYSTEM_INFO_4, m_CtrlSystemInfo4);
	DDX_Control(pDX, IDC_SYSTEM_INFO_5, m_CtrlSystemInfo5);
	DDX_Control(pDX, IDC_SYSTEM_INFO_6, m_CtrlSystemInfo6);

#if _MSC_VER == 1310
	DDX_Control(pDX, IDC_COMMENT_UPPER, m_CtrlCommentUpper);

#endif
	DDX_Control(pDX, IDC_COMMENT, m_CtrlComment);

	DDX_Control(pDX, IDC_SNS_1, m_CtrlSns1);
/*	DDX_Control(pDX, IDC_SNS_2, m_CtrlSns2);
	DDX_Control(pDX, IDC_SNS_3, m_CtrlSns3);
	DDX_Control(pDX, IDC_SNS_4, m_CtrlSns4);
	DDX_Control(pDX, IDC_SNS_5, m_CtrlSns5);
	DDX_Control(pDX, IDC_SNS_6, m_CtrlSns6);
	DDX_Control(pDX, IDC_SNS_7, m_CtrlSns7);
	DDX_Control(pDX, IDC_SNS_8, m_CtrlSns8);
*/
	DDX_Control(pDX, IDC_CRYSTALMARK, m_CtrlCrystalMark);
//	DDX_Control(pDX, IDC_POST, m_CtrlPost);
#ifdef SUISHO_SHIZUKU_SUPPORT
	DDX_Control(pDX, IDC_SD, m_CtrlSD);
#endif
//	DDX_Control(pDX, IDC_SETTINGS, m_CtrlSettings);
//	DDX_Control(pDX, IDC_ADS_1, m_CtrlAds1);
//	DDX_Control(pDX, IDC_ADS_2, m_CtrlAds2);
//	DDX_Control(pDX, IDC_ADS_3, m_CtrlAds3);
}

BEGIN_MESSAGE_MAP(CCrystalMarkDlg, CMainDialogFx)
	ON_WM_SIZE()

	ON_COMMAND(ID_COPY, &CCrystalMarkDlg::OnCopy)
	ON_COMMAND(ID_SAVE_TEXT, &CCrystalMarkDlg::OnSaveText)
	ON_COMMAND(ID_SAVE_IMAGE, &CCrystalMarkDlg::OnSaveImage)
	ON_COMMAND(ID_EXIT, &CCrystalMarkDlg::OnExit)
	ON_COMMAND(ID_ABOUT, &CCrystalMarkDlg::OnAbout)

	ON_MESSAGE(WM_UPDATE_SCORE, OnUpdateScore)
	ON_MESSAGE(WM_UPDATE_MESSAGE, OnUpdateMessage)
	ON_MESSAGE(WM_EXIT_BENCHMARK, OnExitBenchmark)
	ON_MESSAGE(WM_START_BENCHMARK, OnStartBenchmark)
	ON_MESSAGE(WM_SECRET_VOICE, OnSecretVoice)

	ON_COMMAND(ID_ZOOM_50, &CCrystalMarkDlg::OnZoom50)
	ON_COMMAND(ID_ZOOM_75, &CCrystalMarkDlg::OnZoom75)
	ON_COMMAND(ID_ZOOM_100, &CCrystalMarkDlg::OnZoom100)
	ON_COMMAND(ID_ZOOM_125, &CCrystalMarkDlg::OnZoom125)
	ON_COMMAND(ID_ZOOM_150, &CCrystalMarkDlg::OnZoom150)
	ON_COMMAND(ID_ZOOM_200, &CCrystalMarkDlg::OnZoom200)
	ON_COMMAND(ID_ZOOM_250, &CCrystalMarkDlg::OnZoom250)
	ON_COMMAND(ID_ZOOM_300, &CCrystalMarkDlg::OnZoom300)
	ON_COMMAND(ID_ZOOM_AUTO, &CCrystalMarkDlg::OnZoomAuto)
	ON_COMMAND(ID_HELP, &CCrystalMarkDlg::OnHelp)
	ON_COMMAND(ID_CRYSTALDEWWORLD, &CCrystalMarkDlg::OnCrystalDewWorld)
	ON_COMMAND(ID_FONT_SETTING, &CCrystalMarkDlg::OnFontSetting)

	ON_BN_CLICKED(IDC_SD, &CCrystalMarkDlg::OnSD)
//	ON_BN_CLICKED(IDC_POST, &CCrystalMarkDlg::OnPost)
	ON_BN_CLICKED(IDC_SNS_1, &CCrystalMarkDlg::OnTweet)
	ON_BN_CLICKED(IDC_START_0, &CCrystalMarkDlg::OnStart0)
	ON_BN_CLICKED(IDC_START_1, &CCrystalMarkDlg::OnStart1)
	ON_BN_CLICKED(IDC_START_2, &CCrystalMarkDlg::OnStart2)
	ON_BN_CLICKED(IDC_START_3, &CCrystalMarkDlg::OnStart3)
	ON_BN_CLICKED(IDC_START_4, &CCrystalMarkDlg::OnStart4)


	ON_COMMAND(ID_MAIN_UI_IN_ENGLISH, &CCrystalMarkDlg::OnMainUIinEnglish)

#ifdef SUISHO_AOI_SUPPORT
	ON_COMMAND(ID_VOICE_ENGLISH, &CCrystalMarkDlg::OnVoiceEnglish)
	ON_COMMAND(ID_VOICE_JAPANESE, &CCrystalMarkDlg::OnVoiceJapanese)
#endif

#ifdef SUISHO_SHIZUKU_SUPPORT
	ON_COMMAND(ID_VOICE_VOLUME_000, &CCrystalMarkDlg::OnVoiceVolume000)
	ON_COMMAND(ID_VOICE_VOLUME_010, &CCrystalMarkDlg::OnVoiceVolume010)
	ON_COMMAND(ID_VOICE_VOLUME_020, &CCrystalMarkDlg::OnVoiceVolume020)
	ON_COMMAND(ID_VOICE_VOLUME_030, &CCrystalMarkDlg::OnVoiceVolume030)
	ON_COMMAND(ID_VOICE_VOLUME_040, &CCrystalMarkDlg::OnVoiceVolume040)
	ON_COMMAND(ID_VOICE_VOLUME_050, &CCrystalMarkDlg::OnVoiceVolume050)
	ON_COMMAND(ID_VOICE_VOLUME_060, &CCrystalMarkDlg::OnVoiceVolume060)
	ON_COMMAND(ID_VOICE_VOLUME_070, &CCrystalMarkDlg::OnVoiceVolume070)
	ON_COMMAND(ID_VOICE_VOLUME_080, &CCrystalMarkDlg::OnVoiceVolume080)
	ON_COMMAND(ID_VOICE_VOLUME_090, &CCrystalMarkDlg::OnVoiceVolume090)
	ON_COMMAND(ID_VOICE_VOLUME_100, &CCrystalMarkDlg::OnVoiceVolume100)
#endif

END_MESSAGE_MAP()

LRESULT CCrystalMarkDlg::OnQueryEndSession(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CCrystalMarkDlg::CheckThemeEdition(CString name)
{
#ifdef SUISHO_AOI_SUPPORT
	if(name.Find(_T("Aoi")) == 0) { return TRUE; }
#elif SUISHO_SHIZUKU_SUPPORT
	if(name.Find(_T("Shizuku")) == 0) { return TRUE; }
#else
	if(name.Find(_T("Shizuku")) != 0 && name.Find(_T("Aoi")) != 0 && name.Find(_T(".")) != 0) { return TRUE; }
#endif

	return FALSE;
}

CString CCrystalMarkDlg::VOICE(__int64 score)
{
	CString voiceName;
	if (score >= 100000)
	{
		voiceName = _T("Mark01");
	}
	else if (score >= 50000)
	{
		voiceName = _T("Mark02");
	}
	else if (score >= 10000)
	{
		voiceName = _T("Mark03");
	}
	else if (score >= 5000)
	{
		voiceName = _T("Mark04");
	}
	else if (score >= 1000)
	{
		voiceName = _T("Mark05");
	}
	else if (score > 0)
	{
		voiceName = _T("Mark06");
	}
	else if (score == 0)
	{
		switch (rand() % 5)
		{
		case 0: voiceName = _T("Mark07"); break;
		case 1: voiceName = _T("Mark08"); break;
		case 2: voiceName = _T("Mark09"); break;
		case 3: voiceName = _T("Mark10"); break;
		default:voiceName = _T("Mark11"); break;
		}
	}
	else if (score == -1) // Secret Voice
	{
		voiceName = _T("Mark12");
	}

	CString voicePath;
	voicePath.Format(_T("%s%s\\%s.wav"), m_VoiceDir.GetString(), m_CurrentVoice.GetString(), voiceName.GetString());
	if (IsFileExist(voicePath))
	{
		return voicePath;
	}

	voicePath.Format(_T("%s%s\\%s.mp3"), m_VoiceDir.GetString(), m_CurrentVoice.GetString(), voiceName.GetString());
	if (IsFileExist(voicePath))
	{
		return voicePath;
	}

	return _T("");
}


CString CCrystalMarkDlg::SD(__int64 score)
{
#ifdef SUISHO_SHIZUKU_SUPPORT
	CString imageName;

#ifdef SUISHO_AOI_SUPPORT
	#define SD_CHARACTER _T("SDAoi")
#elif SUISHO_SHIZUKU_SUPPORT
	#define SD_CHARACTER _T("SDShizuku")
#endif

	if (score >= 50000)
	{
		imageName.Format(_T("%s%s"), SD_CHARACTER, _T("S"));
	}
	else if (score >= 10000)
	{
		imageName.Format(_T("%s%s"), SD_CHARACTER, _T("A"));
	}
	else if (score >= 5000)
	{
		imageName.Format(_T("%s%s"), SD_CHARACTER, _T("B"));
	}
	else if (score >= 1000)
	{
		imageName.Format(_T("%s%s"), SD_CHARACTER, _T("C"));
	}
	else if (score > 0)
	{
		imageName.Format(_T("%s%s"), SD_CHARACTER, _T("D"));
	}
	else if (score == 0)
	{
		imageName.Format(_T("%s%s"), SD_CHARACTER, _T("E"));
	}

	CString imagePath;
	imagePath.Format(_T("%s%s\\%s-%03d.png"), m_ThemeDir.GetString(), m_CurrentTheme.GetString(), imageName.GetString(), (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}
	imagePath.Format(_T("%s%s\\%s-%03d.png"), m_ThemeDir.GetString(), m_ParentTheme1.GetString(), imageName.GetString(), (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}
	imagePath.Format(_T("%s%s\\%s-%03d.png"), m_ThemeDir.GetString(), m_ParentTheme2.GetString(), imageName.GetString(), (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}
	imagePath.Format(_T("%s%s\\%s-%03d.png"), m_ThemeDir.GetString(), m_DefaultTheme.GetString(), imageName.GetString(), (DWORD)(m_ZoomRatio * 100));
	if (IsFileExist(imagePath))
	{
		return imagePath;
	}
#endif
	return _T("");
}


void CCrystalMarkDlg::UpdateThemeInfo()
{
	CMainDialogFx::UpdateThemeInfo();

	CString theme = m_ThemeDir + m_CurrentTheme + _T("\\theme.ini");

#ifdef SUISHO_AOI_SUPPORT
	m_MarginButtonTop = GetPrivateProfileInt(_T("Margin"), _T("ButtonTop"), 16, theme);
	m_MarginButtonLeft = GetPrivateProfileInt(_T("Margin"), _T("ButtonLeft"), 0, theme);
	m_MarginButtonBottom = GetPrivateProfileInt(_T("Margin"), _T("ButtonBottom"), 16, theme);
	m_MarginButtonRight = GetPrivateProfileInt(_T("Margin"), _T("ButtonRight"), 0, theme);
	m_MarginMeterTop = GetPrivateProfileInt(_T("Margin"), _T("MeterTop"), 0, theme);
	m_MarginMeterLeft = GetPrivateProfileInt(_T("Margin"), _T("MeterLeft"), 0, theme);
	m_MarginMeterBottom = GetPrivateProfileInt(_T("Margin"), _T("MeterBottom"), 0, theme);
	m_MarginMeterRight = GetPrivateProfileInt(_T("Margin"), _T("MeterRight"), 16, theme);
	m_MarginCommentTop = GetPrivateProfileInt(_T("Margin"), _T("CommentTop"), 12, theme);
	m_MarginCommentLeft = GetPrivateProfileInt(_T("Margin"), _T("CommentLeft"), 16, theme);
	m_MarginCommentBottom = GetPrivateProfileInt(_T("Margin"), _T("CommentBottom"), 0, theme);
	m_MarginCommentRight = GetPrivateProfileInt(_T("Margin"), _T("CommentRight"), 64, theme);

#elif SUISHO_SHIZUKU_SUPPORT
	m_MarginButtonTop = GetPrivateProfileInt(_T("Margin"), _T("ButtonTop"), 8, theme);
	m_MarginButtonLeft = GetPrivateProfileInt(_T("Margin"), _T("ButtonLeft"), 0, theme);
	m_MarginButtonBottom = GetPrivateProfileInt(_T("Margin"), _T("ButtonBottom"), 8, theme);
	m_MarginButtonRight = GetPrivateProfileInt(_T("Margin"), _T("ButtonRight"), 0, theme);
	m_MarginMeterTop = GetPrivateProfileInt(_T("Margin"), _T("MeterTop"), 0, theme);
	m_MarginMeterLeft = GetPrivateProfileInt(_T("Margin"), _T("MeterLeft"), 0, theme);
	m_MarginMeterBottom = GetPrivateProfileInt(_T("Margin"), _T("MeterBottom"), 0, theme);
	m_MarginMeterRight = GetPrivateProfileInt(_T("Margin"), _T("MeterRight"), 16, theme);
	m_MarginCommentTop = GetPrivateProfileInt(_T("Margin"), _T("CommentTop"), 0, theme);
	m_MarginCommentLeft = GetPrivateProfileInt(_T("Margin"), _T("CommentLeft"), 16, theme);
	m_MarginCommentBottom = GetPrivateProfileInt(_T("Margin"), _T("CommentBottom"), 0, theme);
	m_MarginCommentRight = GetPrivateProfileInt(_T("Margin"), _T("CommentRight"), 16, theme);

#else
	m_MarginButtonTop = GetPrivateProfileInt(_T("Margin"), _T("ButtonTop"), 4, theme);
	m_MarginButtonLeft = GetPrivateProfileInt(_T("Margin"), _T("ButtonLeft"), 0, theme);
	m_MarginButtonBottom = GetPrivateProfileInt(_T("Margin"), _T("ButtonBottom"), 4, theme);
	m_MarginButtonRight = GetPrivateProfileInt(_T("Margin"), _T("ButtonRight"), 0, theme);
	m_MarginMeterTop = GetPrivateProfileInt(_T("Margin"), _T("MeterTop"), 0, theme);
	m_MarginMeterLeft = GetPrivateProfileInt(_T("Margin"), _T("MeterLeft"), 0, theme);
	m_MarginMeterBottom = GetPrivateProfileInt(_T("Margin"), _T("MeterBottom"), 0, theme);
	m_MarginMeterRight = GetPrivateProfileInt(_T("Margin"), _T("MeterRight"), 4, theme);
	m_MarginCommentTop = GetPrivateProfileInt(_T("Margin"), _T("CommentTop"), 0, theme);
	m_MarginCommentLeft = GetPrivateProfileInt(_T("Margin"), _T("CommentLeft"), 8, theme);
	m_MarginCommentBottom = GetPrivateProfileInt(_T("Margin"), _T("CommentBottom"), 0, theme);
	m_MarginCommentRight = GetPrivateProfileInt(_T("Margin"), _T("CommentRight"), 8, theme);

#endif
}

BOOL CCrystalMarkDlg::OnInitDialog()
{
	CMainDialogFx::OnInitDialog();

	m_hAccelerator = ::LoadAccelerators(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDR_ACCELERATOR));

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIconMini, FALSE);

	TCHAR str[256];
	GetPrivateProfileString(_T("Setting"), _T("FontFace"), GetDefaultFont(), str, 256, m_Ini);
	m_FontFace = str;

	m_FontScale = GetPrivateProfileInt(_T("Setting"), _T("FontScale"), 100, m_Ini);
	if (m_FontScale > 150 || m_FontScale < 50)
	{
		m_FontScale = 100;
		m_FontRatio = 1.0;
	}
	else
	{
		m_FontRatio = m_FontScale / 100.0;
	}

	InitThemeLang();
	InitMenu();
	ChangeTheme(m_CurrentTheme);
	ChangeLang(m_CurrentLang);
	UpdateThemeInfo();

	UpdateData(FALSE);

	ChangeZoomType(m_ZoomType);
	switch (GetPrivateProfileInt(_T("Setting"), _T("ZoomType"), 0, m_Ini))
	{
	case  50:  CheckRadioZoomType(ID_ZOOM_50,   50); break;
	case  75:  CheckRadioZoomType(ID_ZOOM_75,   75); break;
	case 100:  CheckRadioZoomType(ID_ZOOM_100, 100); break;
	case 125:  CheckRadioZoomType(ID_ZOOM_125, 125); break;
	case 150:  CheckRadioZoomType(ID_ZOOM_150, 150); break;
	case 200:  CheckRadioZoomType(ID_ZOOM_200, 200); break;
	case 250:  CheckRadioZoomType(ID_ZOOM_250, 250); break;
	case 300:  CheckRadioZoomType(ID_ZOOM_300, 300); break;
	default:   CheckRadioZoomType(ID_ZOOM_AUTO,  0); break;
	}

	// System Information
	CString cpuInfo = _T("");
	CString gpuInfo = _T("");
	CString gpuInfoTooltip = _T("");
	CString baseBoardInfo = _T("");
	CString computerSystemInfo = _T("");
	CString systemInfoTooltip = _T("");
	CString osInfo = _T("");
	CString screenInfo = _T("");
	CString memoryInfo = _T("");

	// CPU
	GetCpuInfo(cpuInfo, &m_Cores, &m_Threads);
	m_CtrlSystemInfo1.SetWindowText(cpuInfo);
	m_CtrlSystemInfo1.SetToolTipText(cpuInfo);
	m_CpuInfo = cpuInfo;

	// GPU
	GetGpuInfo(gpuInfo);
	m_CtrlSystemInfo2.SetWindowText(gpuInfo);
	gpuInfoTooltip = gpuInfo;
	gpuInfoTooltip.Replace(_T(" | "), _T("\n"));
	m_CtrlSystemInfo2.SetToolTipText(gpuInfoTooltip);

	m_GpuInfo = gpuInfo;

	// Mother Board / Product Name
	GetComputerSystemInfo(computerSystemInfo);
	GetBaseBoardInfo(baseBoardInfo);

	if (!computerSystemInfo.IsEmpty() && !baseBoardInfo.IsEmpty() && computerSystemInfo.Compare(baseBoardInfo.GetString()) != 0)
	{
		m_SystemInfo = computerSystemInfo + _T(" | ") + baseBoardInfo;
		systemInfoTooltip = computerSystemInfo + _T("\n") + baseBoardInfo;
		
		// for ASUS
		if (computerSystemInfo.Find(_T("ASUS")) == 0 && computerSystemInfo.GetLength() == 4)
		{
			computerSystemInfo = _T("");
		}
		// Trim //
		computerSystemInfo.Replace(_T("Micro-Star International Co., Ltd."), _T("MSI"));
		computerSystemInfo.Replace(_T("ASUSTeK Computer Inc."), _T("ASUS"));
		computerSystemInfo.Replace(_T("ASUSTeK COMPUTER INC."), _T("ASUS"));
		computerSystemInfo.Replace(_T("Gigabyte Technology Co., Ltd"), _T("Gigabyte"));
		computerSystemInfo.Replace(_T("HP HP"), _T("HP"));

		baseBoardInfo.Replace(_T("Micro-Star International Co., Ltd."), _T("MSI"));
		baseBoardInfo.Replace(_T("ASUSTeK Computer Inc."), _T("ASUS"));
		baseBoardInfo.Replace(_T("ASUSTeK COMPUTER INC."), _T("ASUS"));
		baseBoardInfo.Replace(_T("Gigabyte Technology Co., Ltd"), _T("Gigabyte"));
		baseBoardInfo.Replace(_T("HP HP"), _T("HP"));

		if (computerSystemInfo.IsEmpty())
		{
			m_CtrlSystemInfo3.SetWindowText(baseBoardInfo);
		}
		else
		{
			m_CtrlSystemInfo3.SetWindowText(computerSystemInfo + _T(" | ") + baseBoardInfo);
		}
	}
	else if (! baseBoardInfo.IsEmpty())
	{
		m_CtrlSystemInfo3.SetWindowText(baseBoardInfo);
		m_SystemInfo = baseBoardInfo;
		systemInfoTooltip = baseBoardInfo;
	}
	else
	{
		m_CtrlSystemInfo3.SetWindowText(computerSystemInfo);
		m_SystemInfo = computerSystemInfo;
		systemInfoTooltip = computerSystemInfo;
	}
	m_CtrlSystemInfo3.SetToolTipText(systemInfoTooltip);

	GetOsName(osInfo);
	m_CtrlSystemInfo4.SetWindowText(osInfo);
	m_OsInfo = osInfo;

	GetScreenInfo(screenInfo);
	m_CtrlSystemInfo5.SetWindowText(screenInfo);
	m_ScreenInfo = screenInfo;

	GetMemoryInfo(memoryInfo);
	m_CtrlSystemInfo6.SetWindowText(memoryInfo);
	m_MemoryInfo = memoryInfo;	
	
	SetWindowTitle(_T(""));

	m_bShowWindow = TRUE;
	RestoreWindowPosition();
	m_Dpi = GetDpi();
	ChangeZoomType(m_ZoomType);
	UpdateDialogSize();

	m_bInitializing = FALSE;

	SetForegroundWindow();

	/*
	// Error Check
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL, path, MAX_PATH);
	if (!CheckCodeSign(CERTNAME, path))
	{
		AfxMessageBox(m_MesExeFileModified);
		OnExit();
		return FALSE;
	}
	*/

	return TRUE;
}

typedef int(WINAPI* FuncGetSystemMetricsForDpi) (int nIndex, UINT dpi);
typedef UINT(WINAPI* FuncGetDpiForWindow) (HWND hWnd);

void CCrystalMarkDlg::UpdateDialogSize()
{
	CDialogFx::UpdateDialogSize();

	int offsetX = 0;
#ifdef SUISHO_SHIZUKU_SUPPORT
	if (m_CharacterPosition == 0)
	{
		offsetX = OFFSET_X;
	}
#endif

	ShowWindow(SW_HIDE);

	m_SizeX = SIZE_X;
	int y = GetPrivateProfileInt(_T("Setting"), _T("Height"), INT_MIN, m_Ini);
	if (y > 0)
	{
		m_SizeY = y;
	}

	if (m_SizeY < SIZE_MIN_Y)
	{
		m_SizeY = SIZE_MIN_Y;
	}
	else if (m_SizeY > SIZE_MAX_Y)
	{
		m_SizeY = SIZE_MAX_Y;
	}

	SetClientSize(m_SizeX, m_SizeY, m_ZoomRatio);
	UpdateBackground(TRUE, FALSE);
	SetControlFont();

	////
	//// InitControl
	////
	m_CtrlStart0.InitControl(8 + offsetX, 208, 128, 64, m_ZoomRatio, &m_BkDC, IP(_T("Button")), 3, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlStart1.InitControl(8 + offsetX, 280, 128, 64, m_ZoomRatio, &m_BkDC, IP(_T("Button")), 3, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlStart2.InitControl(8 + offsetX, 352, 128, 64, m_ZoomRatio, &m_BkDC, IP(_T("Button")), 3, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlStart3.InitControl(8 + offsetX, 424, 128, 64, m_ZoomRatio, &m_BkDC, IP(_T("Button")), 3, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlStart4.InitControl(8 + offsetX, 496, 128, 64, m_ZoomRatio, &m_BkDC, IP(_T("Button")), 3, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);

	m_CtrlScore0_0.InitControl(144 + offsetX, 208, 408, 64, m_ZoomRatio, &m_BkDC, IP(_T("MeterTotal")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);

	m_CtrlScore1_0.InitControl(144 + offsetX, 208, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore2_0.InitControl(352 + offsetX, 208, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore3_0.InitControl(560 + offsetX, 208, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore4_0.InitControl(768 + offsetX, 208, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);

	m_CtrlScore1_1.InitControl(144 + offsetX, 280, 408, 64, m_ZoomRatio, &m_BkDC, IP(_T("MeterTotal")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore1_2.InitControl(560 + offsetX, 280, 408, 64, m_ZoomRatio, &m_BkDC, IP(_T("MeterTotal")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore1_3.InitControl(560 + offsetX, 280, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore1_4.InitControl(768 + offsetX, 280, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, SS_CENTER, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);

	m_CtrlScore2_1.InitControl(144 + offsetX, 352, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore2_2.InitControl(352 + offsetX, 352, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore2_3.InitControl(560 + offsetX, 352, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore2_4.InitControl(768 + offsetX, 352, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);

	m_CtrlScore3_1.InitControl(144 + offsetX, 424, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore3_2.InitControl(352 + offsetX, 424, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore3_3.InitControl(560 + offsetX, 424, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore3_4.InitControl(768 + offsetX, 424, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);

	m_CtrlScore4_1.InitControl(144 + offsetX, 496, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore4_2.InitControl(352 + offsetX, 496, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore4_3.InitControl(560 + offsetX, 496, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	m_CtrlScore4_4.InitControl(768 + offsetX, 496, 200, 64, m_ZoomRatio, &m_BkDC, IP(_T("Meter")), 2, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);

	m_CtrlComment.SetGlassColor(m_Glass, m_GlassAlpha);

#if _MSC_VER == 1310
	#ifdef SUISHO_AOI_SUPPORT
		m_CtrlCommentUpper.InitControl(8 + offsetX, 560, 912, 16 + m_SizeY - SIZE_MIN_Y, m_ZoomRatio, &m_BkDC, IP(_T("CommentU")), 1, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
		m_CtrlComment.InitControl(8 + offsetX, 576, 912, 40 + m_SizeY - SIZE_MIN_Y, m_ZoomRatio, &m_BkDC, IP(_T("CommentL")), 1, ES_LEFT, OwnerDrawImage, m_bHighContrast, FALSE, FALSE, FALSE);
	#else
		m_CtrlComment.InitControl(8 + offsetX, 568, 912, 40 + m_SizeY - SIZE_MIN_Y, m_ZoomRatio, &m_BkDC, IP(_T("Comment")), 1, ES_LEFT, OwnerDrawImage, m_bHighContrast, FALSE, FALSE, FALSE);
	#endif
#else
	#ifdef SUISHO_AOI_SUPPORT
		m_CtrlComment.InitControl(8 + offsetX, 560, 912, 56 + m_SizeY - SIZE_MIN_Y, m_ZoomRatio, &m_BkDC, IP(_T("Comment")), 1, ES_LEFT, OwnerDrawImage, m_bHighContrast, FALSE, FALSE, FALSE);
	#else
		m_CtrlComment.InitControl(8 + offsetX, 568, 912, 40 + m_SizeY - SIZE_MIN_Y, m_ZoomRatio, &m_BkDC, IP(_T("Comment")), 1, ES_LEFT, OwnerDrawImage, m_bHighContrast, FALSE, FALSE, FALSE);
	#endif
#endif
//	m_CtrlComment.SetDrawFrame(TRUE);
	m_CtrlComment.SetMargin(m_MarginCommentTop, m_MarginCommentLeft, m_MarginCommentBottom, m_MarginCommentRight, m_ZoomRatio);


	m_CtrlSns1.InitControl(928 + offsetX, 568, 40, 40, m_ZoomRatio, &m_BkDC, IP(_T("X")), 3, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
	/*
	m_CtrlSns2.InitControl(864 + offsetX, 168, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, TRUE);
	m_CtrlSns3.InitControl(900 + offsetX, 168, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, TRUE);
	m_CtrlSns4.InitControl(936 + offsetX, 168, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, TRUE);
	m_CtrlSns5.InitControl(0 + offsetX, 0, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, TRUE);
	m_CtrlSns6.InitControl(0 + offsetX, 0, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, TRUE);
	m_CtrlSns7.InitControl(0 + offsetX, 0, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, TRUE);
	m_CtrlSns8.InitControl(0 + offsetX, 0, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, TRUE);
	*/

//	m_CtrlPost.InitControl(768 + offsetX, 568, 152, 40, m_ZoomRatio, &m_BkDC, IP(_T("ButtonMini"), 5, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
//	m_CtrlSettings.InitControl(928 + offsetX, 568, 40, 40, m_ZoomRatio, &m_BkDC, IP(_T("ButtonMicro"), 5, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);

#ifdef SUISHO_SHIZUKU_SUPPORT
	m_CtrlSD.InitControl(8 + offsetX, 8, 128, 192, m_ZoomRatio, &m_BkDC, SD(m_Score[0][0]), 1, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
#endif

#ifdef UWP
//	m_CtrlAds1.InitControl(560 + offsetX, 136, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
//	m_CtrlAds2.InitControl(768 + offsetX, 280, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, SystemDraw, m_bHighContrast, FALSE, TRUE);
//	m_CtrlAds3.InitControl(836 + offsetX, 664, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, SystemDraw, m_bHighContrast, FALSE, TRUE);
#else
//	m_CtrlAds1.InitControl(560 + offsetX, 112, 408, 160, m_ZoomRatio, &m_BkDC, IP(_T("Ads"), 1, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
//	m_CtrlAds2.InitControl(768 + offsetX, 280, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, SystemDraw, m_bHighContrast, FALSE, TRUE);
//	m_CtrlAds3.InitControl(836 + offsetX, 664, 0, 0, m_ZoomRatio, &m_BkDC, NULL, 0, BS_CENTER, SystemDraw, m_bHighContrast, FALSE, TRUE);
#endif

#ifdef SUISHO_SHIZUKU_SUPPORT
	m_CtrlCrystalMark.InitControl(144 + offsetX, 152, 408, 48, m_ZoomRatio, &m_BkDC, IP(_T("CrystalMark")), 1, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);

	m_LabelSystemInfo1.InitControl(144 + offsetX,  8, 100, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_LabelSystemInfo2.InitControl(144 + offsetX, 32, 100, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_LabelSystemInfo3.InitControl(144 + offsetX, 56, 100, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_LabelSystemInfo4.InitControl(144 + offsetX, 80, 100, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_LabelSystemInfo5.InitControl(144 + offsetX, 104, 100, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_LabelSystemInfo6.InitControl(144 + offsetX, 128, 100, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);

	m_CtrlSystemInfo1.InitControl(252 + offsetX,  8, 700, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlSystemInfo2.InitControl(252 + offsetX, 32, 700, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlSystemInfo3.InitControl(252 + offsetX, 56, 700, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlSystemInfo4.InitControl(252 + offsetX, 80, 700, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlSystemInfo5.InitControl(252 + offsetX, 104, 300, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlSystemInfo6.InitControl(252 + offsetX, 128, 300, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);

#else
	m_CtrlCrystalMark.InitControl(144 + offsetX, 152, 408, 48, m_ZoomRatio, &m_BkDC, IP(_T("CrystalMark")), 1, BS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);

	m_LabelSystemInfo1.InitControl(8 + offsetX, 8, 128, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_LabelSystemInfo2.InitControl(8 + offsetX, 32, 128, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_LabelSystemInfo3.InitControl(8 + offsetX, 56, 128, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_LabelSystemInfo4.InitControl(8 + offsetX, 80, 128, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_LabelSystemInfo5.InitControl(8 + offsetX, 104, 128, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_LabelSystemInfo6.InitControl(8 + offsetX, 128, 128, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_RIGHT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);

	m_CtrlSystemInfo1.InitControl(144 + offsetX, 8, 824, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlSystemInfo2.InitControl(144 + offsetX, 32, 824, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlSystemInfo3.InitControl(144 + offsetX, 56, 824, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlSystemInfo4.InitControl(144 + offsetX, 80, 824, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlSystemInfo5.InitControl(144 + offsetX, 104, 408, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);
	m_CtrlSystemInfo6.InitControl(144 + offsetX, 128, 408, 24, m_ZoomRatio, &m_BkDC, NULL, 0, SS_LEFT, OwnerDrawTransparent, m_bHighContrast, FALSE, FALSE);

#endif
	m_CtrlScore0_0.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore1_0.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore2_0.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore3_0.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore4_0.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);

	m_CtrlScore1_1.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore1_2.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore1_3.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore1_4.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore2_1.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore2_2.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore2_3.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore2_4.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore3_1.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore3_2.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore3_3.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore3_4.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore4_1.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore4_2.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore4_3.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);
	m_CtrlScore4_4.SetMargin(m_MarginMeterTop, m_MarginMeterLeft, m_MarginMeterBottom, m_MarginMeterRight, m_ZoomRatio);

	m_CtrlScore0_0.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore0_0.SetLabelUnit(_T(""), _T(""));

	m_CtrlScore1_1.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore1_2.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore1_3.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore1_4.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

	m_CtrlScore2_1.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore2_2.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore2_3.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore2_4.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

	m_CtrlScore3_1.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore3_2.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore3_3.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore3_4.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

	m_CtrlScore4_1.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore4_2.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore4_3.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore4_4.SetLabelUnitFormat(DT_LEFT | DT_TOP | DT_SINGLELINE, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

	m_CtrlScore0_0.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore1_0.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore2_0.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore3_0.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore4_0.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

	m_CtrlScore1_1.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore1_2.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore1_3.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore1_4.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

	m_CtrlScore2_1.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore2_2.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore2_3.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore2_4.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

	m_CtrlScore3_1.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore3_2.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore3_3.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore3_4.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

	m_CtrlScore4_1.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore4_2.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore4_3.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	m_CtrlScore4_4.SetTextFormat(DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

	/*
	m_CtrlScore0_0.SetWindowTextW(_T("0");

	m_CtrlScore1_0.SetWindowTextW(_T("0");
	m_CtrlScore2_0.SetWindowTextW(_T("0");
	m_CtrlScore3_0.SetWindowTextW(_T("0");
	m_CtrlScore4_0.SetWindowTextW(_T("0");

	m_CtrlScore1_1.SetWindowTextW(_T("0");
	m_CtrlScore1_2.SetWindowTextW(_T("0");
	m_CtrlScore1_3.SetWindowTextW(_T("");
	m_CtrlScore1_4.SetWindowTextW(_T("");

	m_CtrlScore2_1.SetWindowTextW(_T("0");
	m_CtrlScore2_2.SetWindowTextW(_T("0");
	m_CtrlScore2_3.SetWindowTextW(_T("0");
	m_CtrlScore2_4.SetWindowTextW(_T("0");

	m_CtrlScore3_1.SetWindowTextW(_T("0");
	m_CtrlScore3_2.SetWindowTextW(_T("0");
	m_CtrlScore3_3.SetWindowTextW(_T("0");
	m_CtrlScore3_4.SetWindowTextW(_T("0");

	m_CtrlScore4_1.SetWindowTextW(_T("0");
	m_CtrlScore4_2.SetWindowTextW(_T("0");
	m_CtrlScore4_3.SetWindowTextW(_T("0");
	m_CtrlScore4_4.SetWindowTextW(_T("0");

	m_CtrlScore0_0.SetMeter(TRUE, 0.0);

	m_CtrlScore1_0.SetMeter(TRUE, 0.0);
	m_CtrlScore2_0.SetMeter(TRUE, 0.0);
	m_CtrlScore3_0.SetMeter(TRUE, 0.0);
	m_CtrlScore4_0.SetMeter(TRUE, 0.0);

	m_CtrlScore1_1.SetMeter(TRUE, 0.0);
	m_CtrlScore1_2.SetMeter(TRUE, 0.0);
	m_CtrlScore1_3.SetMeter(TRUE, 0.0);
	m_CtrlScore1_4.SetMeter(TRUE, 0.0);

	m_CtrlScore2_1.SetMeter(TRUE, 0.0);
	m_CtrlScore2_2.SetMeter(TRUE, 0.0);
	m_CtrlScore2_3.SetMeter(TRUE, 0.0);
	m_CtrlScore2_4.SetMeter(TRUE, 0.0);

	m_CtrlScore3_1.SetMeter(TRUE, 0.0);
	m_CtrlScore3_2.SetMeter(TRUE, 0.0);
	m_CtrlScore3_3.SetMeter(TRUE, 0.0);
	m_CtrlScore3_4.SetMeter(TRUE, 0.0);

	m_CtrlScore4_1.SetMeter(TRUE, 0.0);
	m_CtrlScore4_2.SetMeter(TRUE, 0.0);
//	m_CtrlScore4_3.SetMeter(TRUE, 0.0);
//	m_CtrlScore4_4.SetMeter(TRUE, 0.0);
*/
	m_CtrlStart0.SetHandCursor();
	m_CtrlStart1.SetHandCursor();
	m_CtrlStart2.SetHandCursor();
	m_CtrlStart3.SetHandCursor();
	m_CtrlStart4.SetHandCursor();


	m_CtrlSns1.SetHandCursor();
	/*
	m_CtrlSns2.SetHandCursor();
	m_CtrlSns3.SetHandCursor();
	m_CtrlSns4.SetHandCursor();
	m_CtrlSns5.SetHandCursor();
	m_CtrlSns6.SetHandCursor();
	m_CtrlSns7.SetHandCursor();
	m_CtrlSns8.SetHandCursor();
	*/

//	m_CtrlCrystalMark.SetHandCursor();
//	m_CtrlPost.SetHandCursor();
//	m_CtrlSettings.SetHandCursor();
#ifdef SUISHO_SHIZUKU_SUPPORT
	m_CtrlSD.SetHandCursor();
#endif
//	m_CtrlAds1.SetHandCursor();
//	m_CtrlAds2.SetHandCursor();
//	m_CtrlAds3.SetHandCursor();


	m_CtrlSns1.SetToolTipText(_T("X(Twitter)"));

	/*
	m_CtrlSns1.SetWindowTextW(_T("");
	m_CtrlSns2.SetWindowTextW(_T("");
	m_CtrlSns3.SetWindowTextW(_T("");
	m_CtrlSns4.SetWindowTextW(_T("");
	m_CtrlSns5.SetWindowTextW(_T("");
	m_CtrlSns6.SetWindowTextW(_T("");
	m_CtrlSns7.SetWindowTextW(_T("");
	m_CtrlSns8.SetWindowTextW(_T("");
	*/

	m_CtrlCrystalMark.SetWindowText(_T(""));
//	m_CtrlPost.SetWindowTexW(_T("Post"));
//	m_CtrlSettings.SetWindowTexW(_T("⚙"));
#ifdef SUISHO_SHIZUKU_SUPPORT
	m_CtrlSD.SetWindowText(_T(""));
#endif
//	m_CtrlAds1.SetWindowText(_T(""));
//	m_CtrlAds2.SetWindowText(_T(""));
//	m_CtrlAds3.SetWindowText(_T(""));

//	m_CtrlPost.EnableWindow(FALSE);
//	m_CtrlSettings.EnableWindow(FALSE);

	Invalidate();

	ShowWindow(SW_SHOW);
}

void CCrystalMarkDlg::SetControlFont()
{
	m_CtrlStart0.SetFontEx(m_FontFace, 32, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlStart1.SetFontEx(m_FontFace, 32, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlStart2.SetFontEx(m_FontFace, 32, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlStart3.SetFontEx(m_FontFace, 32, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlStart4.SetFontEx(m_FontFace, 32, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	m_CtrlScore0_0.SetFontEx(m_FontFace, 52, 12, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	m_CtrlScore1_0.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore2_0.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore3_0.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore4_0.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	m_CtrlScore1_1.SetFontEx(m_FontFace, 52, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore1_2.SetFontEx(m_FontFace, 52, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore1_3.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore1_4.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	m_CtrlScore2_1.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore2_2.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore2_3.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore2_4.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	m_CtrlScore3_1.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore3_2.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore3_3.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore3_4.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	m_CtrlScore4_1.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore4_2.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore4_3.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlScore4_4.SetFontEx(m_FontFace, 32, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	m_CtrlComment.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio, m_EditText, FW_BOLD);

	m_CtrlSns1.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	/*	m_CtrlSns2.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSns3.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSns4.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSns5.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSns6.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSns7.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSns8.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	*/

//	m_CtrlPost.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
//	m_CtrlSettings.SetFontEx(m_FontFace, 20, 20, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
#ifdef SUISHO_SHIZUKU_SUPPORT
	m_CtrlSD.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
#endif
//	m_CtrlAds1.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
//	m_CtrlAds2.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
//	m_CtrlAds3.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	m_LabelSystemInfo1.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_LabelSystemInfo2.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_LabelSystemInfo3.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_LabelSystemInfo4.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_LabelSystemInfo5.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_LabelSystemInfo6.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	m_CtrlSystemInfo1.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSystemInfo2.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSystemInfo3.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSystemInfo4.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSystemInfo5.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSystemInfo6.SetFontEx(m_FontFace, 16, 16, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	/*	m_CtrlCpuName.SetFontEx(m_FontFace, 12, 24, m_ZoomRatio, m_FontRatio, m_LabelText, FW_BOLD);

//	m_CtrlScore.SetFontEx(m_FontFace, 32, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlSingle.SetFontEx(m_FontFace, 20, 24, m_ZoomRatio, m_FontRatio, m_LabelText, FW_BOLD);
	m_CtrlMulti.SetFontEx(m_FontFace, 20, 24, m_ZoomRatio, m_FontRatio, m_LabelText, FW_BOLD);
	m_CtrlSingleMeter.SetFontEx(m_FontFace, 64, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);
	m_CtrlMultiMeter.SetFontEx(m_FontFace, 64, 24, m_ZoomRatio, m_FontRatio, m_ButtonText, FW_BOLD);

	m_CtrlArchitecture.SetFontEx(m_FontFace, 24, 24, m_ZoomRatio, m_FontRatio, m_ComboText, m_ComboTextSelected, FW_NORMAL);

	m_CtrlArchitecture.SetFontHeight(24, m_ZoomRatio, m_FontRatio);
	m_CtrlArchitecture.SetItemHeightEx(-1, 48, m_ZoomRatio, m_FontRatio);
	for (int i = 0; i < m_CtrlArchitecture.GetCount(); i++)
	{
		m_CtrlArchitecture.SetItemHeightEx(i, 32, m_ZoomRatio, m_FontRatio);
	}
	*/
}

void CCrystalMarkDlg::OnSize(UINT nType, int cx, int cy)
{
	CMainDialogFx::OnSize(nType, cx, cy);

	static BOOL flag = FALSE;

	if (flag)
	{
		int positionX = 0;
		if (m_CharacterPosition == 0)
		{
			positionX = 0;
		}
		else
		{
			positionX = SIZE_X - OFFSET_X;
		}
	}
	flag = TRUE;

	if (m_bInitializing == FALSE && m_bDpiChanging == FALSE && cy > 0)
	{
		CString cstr;
		m_SizeY = (int)(cy / m_ZoomRatio);
		cstr.Format(_T("%d"), m_SizeY);
		WritePrivateProfileStringFx(_T("Setting"), _T("Height"), cstr, m_Ini);
	}

	if (m_bInitializing == FALSE)
	{
		int offsetX = 0;
#ifdef SUISHO_SHIZUKU_SUPPORT
		if (m_CharacterPosition == 0)
		{
			offsetX = OFFSET_X;
		}
#endif

#if _MSC_VER == 1310
	#ifdef SUISHO_AOI_SUPPORT
		m_CtrlCommentUpper.InitControl(8 + offsetX, 560, 912, 16 + m_SizeY - SIZE_MIN_Y, m_ZoomRatio, &m_BkDC, IP(_T("CommentU")), 1, SS_CENTER, OwnerDrawImage, m_bHighContrast, FALSE, FALSE);
		m_CtrlComment.InitControl(8 + offsetX, 576, 912, 40 + m_SizeY - SIZE_MIN_Y, m_ZoomRatio, &m_BkDC, IP(_T("CommentL")), 1, ES_LEFT, OwnerDrawImage, m_bHighContrast, FALSE, FALSE, FALSE);
	#else
		m_CtrlComment.InitControl(8 + offsetX, 568, 912, 40 + m_SizeY - SIZE_MIN_Y, m_ZoomRatio, &m_BkDC, IP(_T("Comment")), 1, ES_LEFT, OwnerDrawImage, m_bHighContrast, FALSE, FALSE, FALSE);
	#endif
#else
	#ifdef SUISHO_AOI_SUPPORT
		m_CtrlComment.InitControl(8 + offsetX, 560, 912, 56 + m_SizeY - SIZE_MIN_Y, m_ZoomRatio, &m_BkDC, IP(_T("Comment")), 1, ES_LEFT, OwnerDrawImage, m_bHighContrast, FALSE, FALSE, FALSE);
	#else
		m_CtrlComment.InitControl(8 + offsetX, 568, 912, 40 + m_SizeY - SIZE_MIN_Y, m_ZoomRatio, &m_BkDC, IP(_T("Comment")), 1, ES_LEFT, OwnerDrawImage, m_bHighContrast, FALSE, FALSE, FALSE);
	#endif
#endif
	//	m_CtrlComment.SetDrawFrame(TRUE);
		m_CtrlComment.SetMargin(m_MarginCommentTop, m_MarginCommentLeft, m_MarginCommentBottom, m_MarginCommentRight, m_ZoomRatio);
	}
}

void CCrystalMarkDlg::SetClientSize(int sizeX, int sizeY, double zoomRatio)
{
	RECT rw, rc;
	GetWindowRect(&rw);
	GetClientRect(&rc);

	if (rc.right != 0)
	{
		int ncaWidth = (rw.right - rw.left) - (rc.right - rc.left);
		int ncaHeight = (rw.bottom - rw.top) - (rc.bottom - rc.top);

		m_MinSizeX = (int)(sizeX * zoomRatio) + ncaWidth;
		m_MaxSizeX = m_MinSizeX;
		m_MinSizeY = (int)(SIZE_MIN_Y * m_ZoomRatio + ncaHeight);
		m_MaxSizeY = (int)(SIZE_MAX_Y * m_ZoomRatio + ncaHeight);

		SetWindowPos(NULL, 0, 0, (int)(sizeX * zoomRatio) + ncaWidth, (int)(sizeY * zoomRatio) + ncaHeight, SWP_NOMOVE | SWP_NOZORDER);
	}
}

void CCrystalMarkDlg::SaveText(CString fileName)
{
	CString cstr, clip;

	UpdateData(TRUE);

	clip = _T("\
------------------------------------------------------------------------------\r\n\
%PRODUCT% %VERSION%%EDITION% (C) %COPY_YEAR% %COPY_AUTHOR%\r\n\
                                  Crystal Dew World: https://crystalmark.info/\r\n\
------------------------------------------------------------------------------\r\n\
\
%SCORE%\
\r\n\
%SYSTEM%\
\r\n\
%OPENGL%\
\r\n\
%BENCHMARK%\
");

	clip.Replace(_T("%PRODUCT%"), PRODUCT_NAME);
	clip.Replace(_T("%VERSION%"), PRODUCT_VERSION);

	cstr = PRODUCT_EDITION;
	if (!cstr.IsEmpty())
	{
		clip.Replace(_T("%EDITION%"), _T(" ") PRODUCT_EDITION);
	}
	else
	{
		clip.Replace(_T("%EDITION%"), PRODUCT_EDITION);
	}
	clip.Replace(_T("%COPY_YEAR%"), PRODUCT_COPY_YEAR);
	clip.Replace(_T("%COPY_AUTHOR%"), PRODUCT_COPY_AUTHOR);

	CString mode = _T("");
	if (m_AdminMode) { mode += _T(" [Admin]"); }

	CString date = _T("");
	SYSTEMTIME st;
	GetLocalTime(&st);
	date.Format(_T("%04d/%02d/%02d %d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	CString comment = _T("");
	m_CtrlComment.GetWindowText(comment);

	// Score
	CString score;

	score.Format(_T("\
-- Score ---------------------------------------------------------------------\r\n\
       Total: %lld\r\n\
[CPU]\r\n\
      Single: %lld\r\n\
       Multi: %lld\r\n\
[Disk]\r\n\
   Seq. Read: %lld\r\n\
 Random Read: %lld\r\n\
  Seq. Write: %lld\r\n\
Random Write: %lld\r\n\
[2D(GDI)]\r\n\
        Text: %lld\r\n\
      Square: %lld\r\n\
      Circle: %lld\r\n\
       Image: %lld\r\n\
[3D(OpenGL)]\r\n\
      Scene1: %lld (CPU: %lld, %lld Lines)\r\n\
      Scene2: %lld (CPU: %lld, %lld Polygons)\r\n\
\r\n\
-- Settings ------------------------------------------------------------------\r\n\
    Date: %s\r\n\
    Mode:%s\r\n\
 Comment: %s\r\n\
"),
m_Score[0][0],
m_Score[1][1],
m_Score[1][2],
m_Score[2][1],
m_Score[2][2],
m_Score[2][3],
m_Score[2][4],
m_Score[3][1],
m_Score[3][2],
m_Score[3][3],
m_Score[3][4],
m_Score[4][1],
m_Score[4][3],
m_Scene1Lines,
m_Score[4][2],
m_Score[4][4],
m_Scene2Polygons,
date.GetString(),
mode.GetString(),
comment.GetString()
);

	clip.Replace(_T("%SCORE%"), score);

	// System
	CString system;

	system.Format(_T("\
-- System --------------------------------------------------------------------\r\n\
     CPU: %s\r\n\
     GPU: %s\r\n\
  System: %s\r\n\
      OS: %s\r\n\
  Screen: %s\r\n\
  Memory: %s\r\n\
"),
m_CpuInfo.GetString(),
m_GpuInfo.GetString(),
m_SystemInfo.GetString(),
m_OsInfo.GetString(),
m_ScreenInfo.GetString(),
m_MemoryInfo.GetString()
);
	clip.Replace(_T("%SYSTEM%"), system);

	// OpenGL
	CString openGL;

	openGL.Format(_T("\
-- OpenGL --------------------------------------------------------------------\r\n\
 Version: %s\r\n\
  Vendor: %s\r\n\
Renderer: %s\r\n\
"),
m_OpenGLVersion.GetString(),
m_OpenGLVendor.GetString(),
m_OpenGLRenderer.GetString()
);
	clip.Replace(_T("%OPENGL%"), openGL);

	// Benchmark version
	CString benchmark;

	benchmark.Format(_T("\
-- Benchmark Version ---------------------------------------------------------\r\n\
     CPU: %s\r\n\
    Disk: %s\r\n\
      2D: %s\r\n\
      3D: %s\r\n\
"),
m_CMRCPUVersion.GetString(),
m_DiskSpdVersion.GetString(),
m_CMRGDIVersion.GetString(),
m_CMROGLVersion.GetString()
);
	clip.Replace(_T("%BENCHMARK%"), benchmark);

	if (fileName.IsEmpty())
	{
		if (OpenClipboard())
		{
			HGLOBAL clipbuffer;
			TCHAR* buffer = NULL;
			EmptyClipboard();
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, sizeof(TCHAR) * (clip.GetLength() + 1));
			if (clipbuffer != NULL)
			{
				buffer = (TCHAR*)GlobalLock(clipbuffer);
				if (buffer != NULL)
				{
#if _MSC_VER == 1310
					_tcscpy(buffer, LPCTSTR(clip));
#else
					_tcscpy_s(buffer, clip.GetLength() + 1, LPCTSTR(clip));
#endif
				}
				GlobalUnlock(clipbuffer);
#ifdef UNICODE
				SetClipboardData(CF_UNICODETEXT, clipbuffer);
#else
				SetClipboardData(CF_TEXT, clipbuffer);
#endif
			}
			CloseClipboard();
		}
	}
	else
	{
		CT2A utf8(clip, CP_UTF8);

		CFile file;
		if (file.Open(fileName, CFile::modeCreate | CFile::modeWrite))
		{
			file.Write((char*)utf8, (UINT)strlen(utf8));
			file.Close();
		}
	}
}

BOOL CCrystalMarkDlg::PreTranslateMessage(MSG* pMsg) 
{
	if( 0 != ::TranslateAccelerator(m_hWnd, m_hAccelerator, pMsg) )
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CCrystalMarkDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CMainDialogFx::OnPaint();
	}
}

HCURSOR CCrystalMarkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCrystalMarkDlg::OnOK()
{
}

void CCrystalMarkDlg::OnCancel()
{
	if (m_BenchStatus)
	{
		Stop();
		// AfxMessageBox(m_MesStopBenchmark);
		return;
	}

	/*
	if (m_BenchStatus)
	{
		m_BenchStatus = FALSE;

		if (pi.hProcess != NULL)
		{
			TerminateProcess(pi.hProcess, 0);
		}
	}
	*/

	SaveWindowPosition();
	CMainDialogFx::OnCancel();
}

void CCrystalMarkDlg::OnExit()
{
	OnCancel();
}

void CCrystalMarkDlg::OnCopy()
{
	SaveText(_T(""));
}

void CCrystalMarkDlg::OnSaveText()
{
	CString path;
	SYSTEMTIME st;
	GetLocalTime(&st);
	path.Format(_T("%s_%04d%02d%02d%0d%02d%02d"), PRODUCT_FILENAME, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	CString filter = _T("TEXT (*.txt)|*.txt||");
	CFileDialog save(FALSE, _T("txt"), path, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, filter);

	if (save.DoModal() == IDOK)
	{
		SaveText(save.GetPathName());
	}
}

void CCrystalMarkDlg::OnSaveImage()
{
	SaveImage();
}

void CCrystalMarkDlg::OnAbout()
{
	m_AboutDlg = new CAboutDlg(this);
	m_AboutDlg->Create(CAboutDlg::IDD, m_AboutDlg, ID_ABOUT, this);
}

void CCrystalMarkDlg::EnableMenus()
{
	CMenu *menu = GetMenu();
	for (int i = 0; i < (int)menu->GetMenuItemCount(); i++)
	{
		menu->EnableMenuItem(i, MF_BYPOSITION | MF_ENABLED);
	}
	SetMenu(menu);
}

void CCrystalMarkDlg::DisableMenus()
{
	CMenu* menu = GetMenu();
	for (int i = 0; i < (int)menu->GetMenuItemCount(); i++)
	{
		menu->EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED);
	}
	SetMenu(menu);
}

void CCrystalMarkDlg::ChangeLang(CString LangName)
{
#ifdef UNICODE
	m_CurrentLangPath.Format(_T("%s\\%s.lang"), (LPTSTR)m_LangDir.GetString(), (LPTSTR)LangName.GetString());
#else
	m_CurrentLangPath.Format(_T("%s\\%s9x.lang"), (LPTSTR)m_LangDir.GetString(), (LPTSTR)LangName.GetString());
#endif

	CString cstr;
	CMenu *menu = GetMenu();
	CMenu subMenu;

	cstr = i18n(_T("Menu"), _T("FILE"));
	menu->ModifyMenu(0, MF_BYPOSITION | MF_STRING, 0, cstr);
	cstr = i18n(_T("Menu"), _T("THEME"));
	menu->ModifyMenu(1, MF_BYPOSITION | MF_STRING, 1, cstr);
	cstr = i18n(_T("Menu"), _T("HELP"));
	menu->ModifyMenu(2, MF_BYPOSITION | MF_STRING, 2, cstr);
	cstr = i18n(_T("Menu"), _T("LANGUAGE"));
	if(cstr.Find(_T("Language")) >= 0)
	{
		cstr = _T("&Language");
		menu->ModifyMenu(3, MF_BYPOSITION | MF_STRING, 3, cstr);
	}
	else
	{
		menu->ModifyMenu(3, MF_BYPOSITION | MF_STRING, 3, cstr + _T(" (&Language)"));
	}

	// File
	cstr = i18n(_T("Menu"), _T("COPY"));
	cstr += _T("\tCtrl + Shift + C");
	menu->ModifyMenu(ID_COPY, MF_STRING, ID_COPY, cstr);
	cstr = i18n(_T("Menu"), _T("SAVE_TEXT"));
	cstr += _T("\tCtrl + T");
	menu->ModifyMenu(ID_SAVE_TEXT, MF_STRING, ID_SAVE_TEXT, cstr);
	cstr = i18n(_T("Menu"), _T("SAVE_IMAGE"));
	cstr += _T("\tCtrl + S");
	menu->ModifyMenu(ID_SAVE_IMAGE, MF_STRING, ID_SAVE_IMAGE, cstr);
	cstr = i18n(_T("Menu"), _T("EXIT"));
	cstr += _T("\tAlt + F4");
	menu->ModifyMenu(ID_EXIT, MF_STRING, ID_EXIT, cstr);


	cstr = i18n(_T("Menu"), _T("HELP")) + _T("\tF1");
	menu->ModifyMenu(ID_HELP, MF_STRING, ID_HELP, cstr);
	cstr = i18n(_T("Menu"), _T("ABOUT"));
	menu->ModifyMenu(ID_ABOUT, MF_STRING, ID_ABOUT, cstr);

	// Theme
	subMenu.Attach(menu->GetSubMenu(MENU_THEME_INDEX)->GetSafeHmenu());
	cstr = i18n(_T("Menu"), _T("ZOOM"));
	subMenu.ModifyMenu(0, MF_BYPOSITION, 0, cstr);
	subMenu.Detach();

	cstr = i18n(_T("Menu"), _T("AUTO"));
	menu->ModifyMenu(ID_ZOOM_AUTO, MF_STRING, ID_ZOOM_AUTO, cstr);

	cstr = i18n(_T("Menu"), _T("FONT_SETTING")) + _T("\tCtrl + F");
	menu->ModifyMenu(ID_FONT_SETTING, MF_STRING, ID_FONT_SETTING, cstr);

	CheckRadioZoomType();

	cstr = i18n(_T("Menu"), _T("MAIN_UI_IN_ENGLISH"));
	menu->ModifyMenu(ID_MAIN_UI_IN_ENGLISH, MF_STRING, ID_MAIN_UI_IN_ENGLISH, cstr);


	if (m_MainUIinEnglish)
	{
		menu->CheckMenuItem(ID_MAIN_UI_IN_ENGLISH, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_MAIN_UI_IN_ENGLISH, MF_UNCHECKED);
	}

	SetMenu(menu);

	// Message //
	m_MesStopBenchmark = i18n(_T("Message"), _T("STOP_BENCHMARK"));
	m_MesDiskCapacityError = i18n(_T("Message"), _T("DISK_CAPACITY_ERROR"));
	m_MesDiskCreateFileError = i18n(_T("Message"), _T("DISK_CREATE_FILE_ERROR"));
	m_MesDiskWriteError = i18n(_T("Message"), _T("DISK_WRITE_ERROR"));
	m_MesDiskReadError = i18n(_T("Message"), _T("DISK_READ_ERROR"));
	m_MesDiskSpdNotFound = i18n(_T("Message"), _T("DISK_SPD_NOT_FOUND"));
	m_MesExeFileNotFound = i18n(_T("Message"), _T("EXE_FILE_NOT_FOUND"));
	m_MesExeFileModified = i18n(_T("Message"), _T("EXE_FILE_MODIFIED"));
	m_MesAttachScreenshotManually = i18n(_T("Message"), _T("ATTACH_SCREENSHOT_MANUALLY"));

	// MainWindow

	m_LabelSystemInfo1.SetWindowText(i18n(_T("MainWindow"), _T("SYSTEM_INFO_1"), m_MainUIinEnglish));
	m_LabelSystemInfo2.SetWindowText(i18n(_T("MainWindow"), _T("SYSTEM_INFO_2"), m_MainUIinEnglish));
	m_LabelSystemInfo3.SetWindowText(i18n(_T("MainWindow"), _T("SYSTEM_INFO_3"), m_MainUIinEnglish));
	m_LabelSystemInfo4.SetWindowText(i18n(_T("MainWindow"), _T("SYSTEM_INFO_4"), m_MainUIinEnglish));
	m_LabelSystemInfo5.SetWindowText(i18n(_T("MainWindow"), _T("SYSTEM_INFO_5"), m_MainUIinEnglish));
	m_LabelSystemInfo6.SetWindowText(i18n(_T("MainWindow"), _T("SYSTEM_INFO_6"), m_MainUIinEnglish));

	m_CtrlStart0.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_0"), m_MainUIinEnglish));
	m_CtrlStart1.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_1"), m_MainUIinEnglish));
	m_CtrlStart2.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_2"), m_MainUIinEnglish));
	m_CtrlStart3.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_3"), m_MainUIinEnglish));
	m_CtrlStart4.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_4"), m_MainUIinEnglish));

	m_CtrlScore1_1.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_1_1"), m_MainUIinEnglish), _T(""));
	m_CtrlScore1_2.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_1_2"), m_MainUIinEnglish), _T(""));
	m_CtrlScore1_3.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_1_3"), m_MainUIinEnglish), _T(""));
	m_CtrlScore1_4.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_1_4"), m_MainUIinEnglish), _T(""));

	m_CtrlScore2_1.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_2_1"), m_MainUIinEnglish), _T(""));
	m_CtrlScore2_2.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_2_2"), m_MainUIinEnglish), _T(""));
	m_CtrlScore2_3.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_2_3"), m_MainUIinEnglish), _T(""));
	m_CtrlScore2_4.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_2_4"), m_MainUIinEnglish), _T(""));

	m_CtrlScore3_1.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_3_1"), m_MainUIinEnglish), _T(""));
	m_CtrlScore3_2.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_3_2"), m_MainUIinEnglish), _T(""));
	m_CtrlScore3_3.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_3_3"), m_MainUIinEnglish), _T(""));
	m_CtrlScore3_4.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_3_4"), m_MainUIinEnglish), _T(""));

	m_CtrlScore4_1.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_4_1"), m_MainUIinEnglish), _T(""));
	m_CtrlScore4_2.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_4_2"), m_MainUIinEnglish), _T(""));
	m_CtrlScore4_3.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_4_3"), m_MainUIinEnglish), _T(""));
	m_CtrlScore4_4.SetLabelUnit(i18n(_T("MainWindow"), _T("LABEL_4_4"), m_MainUIinEnglish), _T(""));

	Invalidate();

	WritePrivateProfileString(_T("Setting"), _T("Language"), LangName, m_Ini);

#ifdef SUISHO_SHIZUKU_SUPPORT
	m_VoiceVolume = GetPrivateProfileIntFx(_T("Setting"), _T("VoiceVolume"), 80, m_Ini);

	int id = ID_VOICE_VOLUME_080;

	switch (m_VoiceVolume)
	{
	case  0: id = ID_VOICE_VOLUME_000;	break;
	case 10: id = ID_VOICE_VOLUME_010;	break;
	case 20: id = ID_VOICE_VOLUME_020;	break;
	case 30: id = ID_VOICE_VOLUME_030;	break;
	case 40: id = ID_VOICE_VOLUME_040;	break;
	case 50: id = ID_VOICE_VOLUME_050;	break;
	case 60: id = ID_VOICE_VOLUME_060;	break;
	case 70: id = ID_VOICE_VOLUME_070;	break;
	case 80: id = ID_VOICE_VOLUME_080;	break;
	case 90: id = ID_VOICE_VOLUME_090;	break;
	case 100:id = ID_VOICE_VOLUME_100;	break;
	default: id = ID_VOICE_VOLUME_080;	break;
	}

	subMenu.Attach(menu->GetSubMenu(3)->GetSafeHmenu());
	cstr = i18n(_T("Menu"), _T("VOICE_VOLUME"));
	subMenu.ModifyMenu(3, MF_BYPOSITION, 3, cstr);
	subMenu.Detach();

	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
#endif

#ifdef SUISHO_AOI_SUPPORT
	TCHAR str[256];
	GetPrivateProfileStringFx(_T("Setting"), _T("VoiceLanguage"), _T(""), str, 256, m_Ini);
	CString voiceLanguage = str;

	if (cstr.IsEmpty()) // First Time
	{
		if (GetUserDefaultLCID() == 0x0411)// Japanese
		{
			GetPrivateProfileStringFx(_T("Setting"), _T("VoiceLanguage"), _T("Japanese"), str, 256, m_Ini);
			WritePrivateProfileStringFx(_T("Setting"), _T("VoiceLanguage"), _T("Japanese"), m_Ini);
		}
		else
		{
			GetPrivateProfileStringFx(_T("Setting"), _T("VoiceLanguage"), _T("English"), str, 256, m_Ini);
			WritePrivateProfileStringFx(_T("Setting"), _T("VoiceLanguage"), _T("English"), m_Ini);
		}
	}
	else
	{
		GetPrivateProfileStringFx(_T("Setting"), _T("VoiceLanguage"), _T("Japanese"), str, 256, m_Ini);
	}

	voiceLanguage = str;
	if (voiceLanguage.Find(_T("Japanese") == 0))
	{
		m_CurrentVoice = _T("Aoi-ja");
	}
	else
	{
		m_CurrentVoice = _T("Aoi-en");
	}

	subMenu.Attach(menu->GetSubMenu(3)->GetSafeHmenu());
	cstr = i18n(_T("Menu"), _T("VOICE_LANGUAGE"));
	subMenu.ModifyMenu(4, MF_BYPOSITION, 4, cstr);
	subMenu.Detach();

	if (voiceLanguage.Find(_T("Japanese") == 0))
	{
		menu->CheckMenuRadioItem(ID_VOICE_ENGLISH, ID_VOICE_JAPANESE, ID_VOICE_JAPANESE, MF_BYCOMMAND);
	}
	else
	{
		menu->CheckMenuRadioItem(ID_VOICE_ENGLISH, ID_VOICE_JAPANESE, ID_VOICE_ENGLISH, MF_BYCOMMAND);
	}

#elif SUISHO_SHIZUKU_SUPPORT
	m_CurrentVoice = _T("Shizuku-ja");
#endif

}

BOOL CCrystalMarkDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// Select Theme
	if (WM_THEME_ID <= wParam && wParam < WM_THEME_ID + (UINT)m_MenuArrayTheme.GetSize())
	{
		CMenu menu;
		CMenu subMenu;
		menu.Attach(GetMenu()->GetSafeHmenu());
		subMenu.Attach(menu.GetSubMenu(MENU_THEME_INDEX)->GetSafeHmenu());

		m_CurrentTheme = m_MenuArrayTheme.GetAt(wParam - WM_THEME_ID);
		if (m_CurrentTheme.Compare(m_RandomThemeLabel) == 0)
		{
			m_CurrentTheme = GetRandomTheme();
			m_RandomThemeLabel = _T("Random");
			m_RandomThemeName = _T(" (") + m_CurrentTheme + _T(")");

			// ChangeTheme save the theme configuration to profile; so if we are on
			// Random, then save Random to profile.
			ChangeTheme(m_RandomThemeLabel);
		}
		else
		{
			ChangeTheme(m_MenuArrayTheme.GetAt(wParam - WM_THEME_ID));
			m_RandomThemeName = _T("");
		}

		subMenu.ModifyMenu(WM_THEME_ID, MF_STRING, WM_THEME_ID, m_RandomThemeLabel + m_RandomThemeName);
		subMenu.CheckMenuRadioItem(WM_THEME_ID, WM_THEME_ID + (UINT)m_MenuArrayTheme.GetSize(),
			(UINT)wParam, MF_BYCOMMAND);
		subMenu.Detach();
		menu.Detach();

		UpdateThemeInfo();
		UpdateDialogSize();

		return TRUE;
	}

	// Select Language
	if(WM_LANGUAGE_ID <= wParam && wParam < WM_LANGUAGE_ID + (UINT)m_MenuArrayLang.GetSize())
	{
		CMenu menu;
		CMenu subMenu;
		CMenu subMenuAN;
		CMenu subMenuOZ;
		menu.Attach(GetMenu()->GetSafeHmenu());
		subMenu.Attach(menu.GetSubMenu(MENU_LANG_INDEX)->GetSafeHmenu());
		subMenuAN.Attach(subMenu.GetSubMenu(0)->GetSafeHmenu());
		subMenuOZ.Attach(subMenu.GetSubMenu(1)->GetSafeHmenu());

		m_CurrentLang = m_MenuArrayLang.GetAt(wParam - WM_LANGUAGE_ID);
		ChangeLang(m_MenuArrayLang.GetAt(wParam - WM_LANGUAGE_ID));
		subMenuAN.CheckMenuRadioItem(WM_LANGUAGE_ID, WM_LANGUAGE_ID + (UINT)m_MenuArrayLang.GetSize(),
									(UINT)wParam, MF_BYCOMMAND);
		subMenuOZ.CheckMenuRadioItem(WM_LANGUAGE_ID, WM_LANGUAGE_ID + (UINT)m_MenuArrayLang.GetSize(),
									(UINT)wParam, MF_BYCOMMAND);

		subMenuOZ.Detach();
		subMenuAN.Detach();
		subMenu.Detach();
		menu.Detach();
	}

	return CMainDialogFx::OnCommand(wParam, lParam);
}

void CCrystalMarkDlg::OnZoom50()
{
	if (CheckRadioZoomType(ID_ZOOM_50, 50))
	{
		UpdateDialogSize();
	}
}

void CCrystalMarkDlg::OnZoom75()
{
	if (CheckRadioZoomType(ID_ZOOM_75, 75))
	{
		UpdateDialogSize();
	}
}

void CCrystalMarkDlg::OnZoom100()
{
	if (CheckRadioZoomType(ID_ZOOM_100, 100))
	{
		UpdateDialogSize();
	}
}

void CCrystalMarkDlg::OnZoom125()
{
	if (CheckRadioZoomType(ID_ZOOM_125, 125))
	{
		UpdateDialogSize();
	}
}

void CCrystalMarkDlg::OnZoom150()
{
	if (CheckRadioZoomType(ID_ZOOM_150, 150))
	{
		UpdateDialogSize();
	}
}

void CCrystalMarkDlg::OnZoom200()
{
	if (CheckRadioZoomType(ID_ZOOM_200, 200))
	{
		UpdateDialogSize();
	}
}

void CCrystalMarkDlg::OnZoom250()
{
	if (CheckRadioZoomType(ID_ZOOM_250, 250))
	{
		UpdateDialogSize();
	}
}

void CCrystalMarkDlg::OnZoom300()
{
	if (CheckRadioZoomType(ID_ZOOM_300, 300))
	{
		UpdateDialogSize();
	}
}

void CCrystalMarkDlg::OnZoomAuto()
{
	if (CheckRadioZoomType(ID_ZOOM_AUTO, 0))
	{
		UpdateDialogSize();
	}
}

BOOL CCrystalMarkDlg::CheckRadioZoomType(int id, int value)
{
	if(m_ZoomType == value)
	{
		return FALSE;
	}

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_ZOOM_50, ID_ZOOM_AUTO, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();

	m_ZoomType = value;

	CString cstr;
	cstr.Format(_T("%d"), value);
	WritePrivateProfileString(_T("Setting"), _T("ZoomType"), cstr, m_Ini);

	ChangeZoomType(m_ZoomType);

	return TRUE;
}

void CCrystalMarkDlg::CheckRadioZoomType()
{
	int id = ID_ZOOM_AUTO;

	switch(m_ZoomType)
	{
	case  50: id = ID_ZOOM_50;	break;
	case  75: id = ID_ZOOM_75;	break;
	case 100: id = ID_ZOOM_100;	break;
	case 125: id = ID_ZOOM_125;	break;
	case 150: id = ID_ZOOM_150;	break;
	case 200: id = ID_ZOOM_200;	break;
	case 250: id = ID_ZOOM_250;	break;
	case 300: id = ID_ZOOM_300;	break;
	default:  id = ID_ZOOM_AUTO;break;
	}

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(ID_ZOOM_50, ID_ZOOM_AUTO, id, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
}

void CCrystalMarkDlg::OnHelp()
{
	if (GetUserDefaultLCID() == 0x0411) // Japanese
	{
		OpenUrl(URL_HELP_JA);
	}
	else // Other Language
	{
		OpenUrl(URL_HELP_EN);
	}
}

void CCrystalMarkDlg::OnCrystalDewWorld()
{
	if (GetUserDefaultLCID() == 0x0411)// Japanese
	{
		OpenUrl(URL_MAIN_JA);
	}
	else // Other Language
	{
		OpenUrl(URL_MAIN_EN);
	}
}

void CCrystalMarkDlg::OnFontSetting()
{
	CFontSelectionDlg fontSelection(this);
	if (fontSelection.DoModal() == IDOK)
	{
		m_FontFace = fontSelection.GetFontFace();
		m_FontScale = fontSelection.GetFontScale();
		m_FontRatio = m_FontScale / 100.0;

		CString cstr;
		WritePrivateProfileString(_T("Setting"), _T("FontFace"), _T("\"") + m_FontFace + _T("\""), m_Ini);
		cstr.Format(_T("%d"), m_FontScale);
		WritePrivateProfileString(_T("Setting"), _T("FontScale"), cstr, m_Ini);

		UpdateDialogSize();
	}
}

void CCrystalMarkDlg::SetWindowTitle(CString message)
{
	CString title;

	if (!message.IsEmpty())
	{
		title.Format(_T("%s - %s"), PRODUCT_SHORT_NAME, message.GetString());
	}
	else
	{
		title.Format(_T("%s %s %s"), PRODUCT_NAME, PRODUCT_VERSION, PRODUCT_EDITION);
	}

	if (m_AdminMode)
	{
		title += _T(" [Admin]");
	}

	SetWindowText(title);
}

void CCrystalMarkDlg::OnSD()
{
	AlertSound(VOICE(m_Score[0][0]), m_VoiceVolume);
}

#ifdef POST_SUPPORT
// UTF-16文字列をUTF-8文字列に変換する関数
CStringA ConvertUTF16ToUTF8(const CStringW& utf16str)
{
	// CW2A マクロを使用して変換
	CStringA utf8str(CW2A(utf16str, CP_UTF8));
	return utf8str;
}

// UTF-16文字列をUTF-8文字列に変換する関数
CStringW ConvertUTF8ToUTF16(const CStringA& utf8str)
{
	// CA2W マクロを使用して変換
	CStringW utf16str;
	utf16str = CA2W(utf8str);
	return utf16str;
}

// JSONデータをPOSTする関数
BOOL PostJSONData(const CString& strServer, const CString& strObject, const CStringA& strData)
{
	CInternetSession session;
	CHttpConnection* pConnection = nullptr;
	CHttpFile* pFile = nullptr;
	BOOL bResult = FALSE;

	try
	{
		// サーバーへのHTTPS接続を開始
		pConnection = session.GetHttpConnection(strServer, INTERNET_FLAG_SECURE, INTERNET_DEFAULT_HTTPS_PORT);
		pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

		// ヘッダーを設定
		CString strHeaders = _T("Content-Type: application/json; charset=\"utf-8\"");
		// strHeaders.AppendFormat(_T("Content-Length: %d"), strData.GetLength());
		// strHeaders.AppendFormat(_T("Accept: */*"));
		pFile->AddRequestHeaders(strHeaders);

		// POSTリクエストを送信
		bResult = pFile->SendRequest(strHeaders, (LPVOID)(LPCSTR)strData.GetString(), strData.GetLength());

		// 応答を受け取る
		if (bResult)
		{
			DWORD dwStatusCode = 0;
			pFile->QueryInfoStatusCode(dwStatusCode);

			// ステータスコードが200 OKの場合、応答を読み取る
			if (dwStatusCode == HTTP_STATUS_OK || TRUE)
			{
				CString strResponse;
				TCHAR szBuffer[1024];
				while (pFile->ReadString(szBuffer, 1024))
				{
					strResponse += szBuffer;
				}
				// 応答内容を処理する（例：ログに記録する）
				// ...

				// ConvertUTF8ToUTF16(strResponse);
				AfxMessageBox(strResponse);
			}
		}
	}
	catch (CInternetException* e)
	{
		e->Delete();
		bResult = FALSE;
	}

	// リソースを解放
	if (pFile) pFile->Close();
	delete pFile;
	if (pConnection) pConnection->Close();
	delete pConnection;
	session.Close();

	return bResult;
}

// HTTP POSTリクエストを送信する関数
void SendHttpPostRequest(CString body)
{
	// インターネットセッションを開始
	CInternetSession session;
	CString strHeaders = _T("Content-Type: application/json; charset=\"utf-8\"");
	CStringA strFormData = ConvertUTF16ToUTF8(body); // 送信するデータ

	// HTTP接続を開始
	CHttpConnection* pConnection = session.GetHttpConnection(_T("gsgw.crystalmark.info"), INTERNET_FLAG_SECURE, INTERNET_DEFAULT_HTTPS_PORT);
	CHttpFile* pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/api/store"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

	// ヘッダーとフォームデータを送信
	BOOL bRequestSent = pFile->SendRequest(strHeaders, (LPVOID)(LPCSTR)strFormData, strFormData.GetLength());

	// 応答を受け取る
	if (bRequestSent)
	{
		CString strResponseText;
		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);

		// HTTPステータスコードが200 OKの場合、応答を読み取る
		if (dwRet == HTTP_STATUS_OK)
		{
			TCHAR szBuffer[1024];
			while (pFile->ReadString(szBuffer, 1024))
			{
				strResponseText += szBuffer;
			}
		}
	}

	// リソースを解放
	pFile->Close();
	delete pFile;
	pConnection->Close();
	delete pConnection;
	session.Close();
}

// HTTP GETリクエストを送信する関数
void SendHttpGetRequest()
{
	// インターネットセッションを開始
	CInternetSession session;
	CString strHeaders = _T("");
	//CStringA strFormData = ConvertUTF16ToUTF8(body); // 送信するデータ

	// HTTP接続を開始
	CHttpConnection* pConnection = session.GetHttpConnection(_T("google.com"), INTERNET_FLAG_SECURE, INTERNET_DEFAULT_HTTPS_PORT);
	CHttpFile* pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, _T("/"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

	// ヘッダーとフォームデータを送信
	BOOL bRequestSent = pFile->SendRequest(strHeaders, 0, 0);

	// 応答を受け取る
	if (bRequestSent)
	{
		CString strResponseText;
		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);

		// HTTPステータスコードが200 OKの場合、応答を読み取る
		if (dwRet == HTTP_STATUS_OK)
		{
			TCHAR szBuffer[1024];
			while (pFile->ReadString(szBuffer, 1024))
			{
				strResponseText += szBuffer;
			}
		}
	}

	// リソースを解放
	pFile->Close();
	delete pFile;
	pConnection->Close();
	delete pConnection;
	session.Close();
}

void CCrystalMarkDlg::OnPost()
{
	CString body;
	CStringA bodyA;

	body.Format(_T("{\n\
\"name\" : \"%s\"), \n\
\"comment\" : \"%s\",\n\
\"single\" : %lld,\n\
\"multi\" : %lld,\n\
\"platform\" : \"%s\",\n\
\"cpu\" : \"%s\",\n\
\"logicalProcessors\" : %d,\n\
\"system\" : \"%s\",\n\
\"model\" : \"%s\",\n\
\"model_id\" : \"%s\",\n\
\"os\" : \"%s\"\n\
}",
		_T("hiyohiyo",
		_T("TEST",
		(__int64)5959,
		(__int64)9999,
		_T("Windows",
		_T("AMD TR 5995 WX",
		128,
		_T("ASRock",
		_T("ASRock WRX80 Creator",
		_T("No ID",
		_T("Windows 11 Pro");

	bodyA = ConvertUTF16ToUTF8(body);

	CString clip;
	clip = body;
	if (OpenClipboard())
	{
		HGLOBAL clipbuffer;
		TCHAR* buffer = NULL;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, sizeof(TCHAR) * (clip.GetLength() + 1));
		if (clipbuffer != NULL)
		{
			buffer = (TCHAR*)GlobalLock(clipbuffer);
			if (buffer != NULL)
			{
#if _MSC_VER == 1310
				_tcscpy(buffer, LPCTSTR(clip));
#else
				_tcscpy_s(buffer, clip.GetLength() + 1, LPCTSTR(clip));
#endif
			}
			GlobalUnlock(clipbuffer);
#ifdef UNICODE
			SetClipboardData(CF_UNICODETEXT, clipbuffer);
#else
			SetClipboardData(CF_TEXT, clipbuffer);
#endif
		}
		CloseClipboard();
	}


	// SendHttpPostRequest(body);
	// AfxMessageBox(body);

	// SendHttpGetRequest();

	PostJSONData(_T(""), _T(""), bodyA);
}
#endif

void CCrystalMarkDlg::OnTweet()
{
	CString url;
	CString total;
	CString single;
	CString multi;
	CString comment;

	m_CtrlScore0_0.GetWindowText(total);
	m_CtrlScore1_1.GetWindowText(single);
	m_CtrlScore1_2.GetWindowText(multi);
	m_CtrlComment.GetWindowText(comment);

	AfxMessageBox(m_MesAttachScreenshotManually);

	url.Format(_T("https://twitter.com/intent/tweet?text=%s%%20%s%%0aTotal:%%20%d%%0aSingle:%%20%d%%0aMulti:%%20%d%%0a%s%%0a&hashtags=CrystalMarkRetro&url=crystalmark.info"), PRODUCT_NAME, PRODUCT_VERSION, _tstoi(total), _tstoi(single), _tstoi(multi), comment.GetString());
	OpenUrl(url);
}

void CCrystalMarkDlg::UpdateScore()
{
	CString cstr;

	/*
	m_Score[1][1] = 10000;
	m_Score[1][2] = 10000;
	m_Score[2][1] = 10000;
	m_Score[2][2] = 10000;
	m_Score[2][3] = 10000;
	m_Score[2][4] = 10000;
	m_Score[3][1] = 10000;
	m_Score[3][2] = 10000;
	m_Score[3][3] = 10000;
	m_Score[3][4] = 10000;
	m_Score[4][1] = 10000;
	m_Score[4][2] = 10000;
	*/
	/*	/*
	m_Score[0][0] = (int)(
		pow(
		  sqrt((double)m_Score[1][1] * (double)m_Score[1][2])
		* sqrt((double)m_Score[2][1] * (double)m_Score[2][2])
		* sqrt((double)m_Score[2][3] * (double)m_Score[2][4])
		* sqrt((double)m_Score[3][1] * (double)m_Score[3][2])
		* sqrt((double)m_Score[3][3] * (double)m_Score[3][4])
		* sqrt((double)m_Score[4][1] * (double)m_Score[4][2]),
		0.16666666));
	*/

	m_Score[0][0] = (int)
		pow(  (double)m_Score[1][1]
			* (double)m_Score[1][2]
			* pow((double)m_Score[2][1] * (double)m_Score[2][2] * (double)m_Score[2][3] * (double)m_Score[2][4], 0.25)
			* pow((double)m_Score[3][1] * (double)m_Score[3][2] * (double)m_Score[3][3] * (double)m_Score[3][4], 0.25)
			* pow((double)m_Score[4][1] * (double)m_Score[4][2] * (double)m_Score[4][2] * (double)m_Score[4][2], 0.25)
			, 0.2);
		

	cstr.Format(_T("%lld"), m_Score[0][0]);
	m_CtrlScore[0][0]->SetWindowText(cstr);
	SetMeter(m_CtrlScore[0][0], (int)m_Score[0][0]);
	for (int i = 1; i <= 4; i++){
		for (int j = 0; j <= 4; j++) {
			cstr.Format(_T("%lld"), m_Score[i][j]);
			m_CtrlScore[i][j]->SetWindowText(cstr);
			if (! ((i == 4 && j == 3) || (i == 4 && j == 4)) )
			{
				SetMeter(m_CtrlScore[i][j], (int)m_Score[i][j]);
			}
		}
	}
}

void CCrystalMarkDlg::ChangeControlStatus(BOOL status)
{
	if (status)
	{
		m_CtrlStart0.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_0"), m_MainUIinEnglish));
		m_CtrlStart1.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_1"), m_MainUIinEnglish));
		m_CtrlStart2.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_2"), m_MainUIinEnglish));
		m_CtrlStart3.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_3"), m_MainUIinEnglish));
		m_CtrlStart4.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_4"), m_MainUIinEnglish));

		m_CtrlSns1.EnableWindow(TRUE);
		m_CtrlSns1.SetHandCursor(TRUE);
	}
	else
	{
		m_CtrlStart0.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_STOP"), m_MainUIinEnglish));
		m_CtrlStart1.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_STOP"), m_MainUIinEnglish));
		m_CtrlStart2.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_STOP"), m_MainUIinEnglish));
		m_CtrlStart3.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_STOP"), m_MainUIinEnglish));
		m_CtrlStart4.SetWindowText(i18n(_T("MainWindow"), _T("BUTTON_STOP"), m_MainUIinEnglish));

		m_CtrlSns1.EnableWindow(FALSE);
		m_CtrlSns1.SetHandCursor(FALSE);
	}
}

void CCrystalMarkDlg::Stop()
{
	if (m_BenchStatus)
	{
		m_BenchStatus = FALSE;

		if (pi.hProcess != NULL)
		{
			TerminateProcess(pi.hProcess, 0);
		}
	}
	ChangeControlStatus(TRUE);
	EnableMenus();
}

void CCrystalMarkDlg::OnStart0()
{
	if (m_BenchStatus == FALSE)
	{
		UpdateData(TRUE);

		for (int i = 0; i <= 4; i++) {
			for (int j = 0; j <= 4; j++) {
				m_Score[i][j] = 0;				
			}
		}

		UpdateScore();
		m_BenchStatus = TRUE;
		m_WinThread = AfxBeginThread(ExecBenchmarkAll, (void*)this);
		if (m_WinThread == NULL)
		{
			m_BenchStatus = FALSE;
		}
		else
		{
			ChangeControlStatus(FALSE);
		}
		DisableMenus();
	}
	else
	{
		Stop();
	}
}

void CCrystalMarkDlg::OnStart1()
{
	if (m_BenchStatus == FALSE)
	{
		UpdateData(TRUE);

		for (int j = 0; j <= 4; j++) {
			m_Score[1][j] = 0;
		}

		UpdateScore();
		m_BenchStatus = TRUE;
		m_WinThread = AfxBeginThread(ExecCPUBench, (void*)this);
		if (m_WinThread == NULL)
		{
			m_BenchStatus = FALSE;
		}
		else
		{
			ChangeControlStatus(FALSE);
		}
		DisableMenus();
	}
	else
	{
		Stop();
	}
}

void CCrystalMarkDlg::OnStart2()
{
	if (m_BenchStatus == FALSE)
	{
		UpdateData(TRUE);

		for (int j = 0; j <= 4; j++) {
			m_Score[2][j] = 0;
		}

		UpdateScore();
		m_BenchStatus = TRUE;
		m_WinThread = AfxBeginThread(ExecDiskBenchAll, (void*)this);
		if (m_WinThread == NULL)
		{
			m_BenchStatus = FALSE;
		}
		else
		{
			ChangeControlStatus(FALSE);
		}
		DisableMenus();
	}
	else
	{
		Stop();
	}
}

void CCrystalMarkDlg::OnStart3()
{
	if (m_BenchStatus == FALSE)
	{
		UpdateData(TRUE);

		for (int j = 0; j <= 4; j++) {
			m_Score[3][j] = 0;
		}

		UpdateScore();
		m_BenchStatus = TRUE;
		m_WinThread = AfxBeginThread(ExecGDIBench, (void*)this);
		if (m_WinThread == NULL)
		{
			m_BenchStatus = FALSE;
		}
		else
		{
			ChangeControlStatus(FALSE);
		}
		DisableMenus();
	}
	else
	{
		Stop();
	}
}

void CCrystalMarkDlg::OnStart4()
{
	if (m_BenchStatus == FALSE)
	{
		UpdateData(TRUE);

		for (int j = 0; j <= 4; j++) {
			m_Score[4][j] = 0;
		}

		UpdateScore();
		m_BenchStatus = TRUE;
		m_WinThread = AfxBeginThread(ExecHiyoGL, (void*)this);
		if (m_WinThread == NULL)
		{
			m_BenchStatus = FALSE;
		}
		else
		{
			ChangeControlStatus(FALSE);
		}
		DisableMenus();
	}
	else
	{
		Stop();
	}
}

LRESULT CCrystalMarkDlg::OnUpdateScore(WPARAM wParam, LPARAM lParam)
{
	UpdateScore();
	return 0;
}

LRESULT CCrystalMarkDlg::OnExitBenchmark(WPARAM wParam, LPARAM lParam)
{
	CString screenInfo = _T("");
	GetScreenInfo(screenInfo);
	m_CtrlSystemInfo5.SetWindowText(screenInfo);
	m_ScreenInfo = screenInfo;

	ChangeControlStatus(TRUE);
	EnableMenus();

#ifdef SUISHO_SHIZUKU_SUPPORT
	if (m_Score[0][0] > 0)
	{
		m_CtrlSD.ReloadImage(SD(m_Score[0][0]), 1);
		AlertSound(VOICE(m_Score[0][0]), m_VoiceVolume);
	}
#endif

	return 0;
}

LRESULT CCrystalMarkDlg::OnStartBenchmark(WPARAM wParam, LPARAM lParam)
{
	ChangeControlStatus(FALSE);
	DisableMenus();

	return 0;
}

LRESULT CCrystalMarkDlg::OnSecretVoice(WPARAM wParam, LPARAM lParam)
{
	AlertSound(VOICE(-1), m_VoiceVolume);

	return 0;
}

LRESULT CCrystalMarkDlg::OnUpdateMessage(WPARAM wParam, LPARAM lParam)
{
	CString wstr = _T("");
	CString lstr = _T("");

	if (wParam != NULL)
	{
		wstr = *((CString*)wParam);
	}

	if (lParam != NULL)
	{
		lstr = *((CString*)lParam);
	}

	SetWindowTitle(wstr);
	return 0;
}

void CCrystalMarkDlg::SetMeter(CStaticFx* control, int score)
{
	double meterRatio = 0.0;

	if (score > 1)
	{
		meterRatio = 0.1 * log10((double)score);
	}
	else
	{
		meterRatio = 0.0;
	}

	control->SetMeter(TRUE, meterRatio);
}

void CCrystalMarkDlg::OnMainUIinEnglish()
{
	CMenu* menu = GetMenu();
	if (m_MainUIinEnglish)
	{
		m_MainUIinEnglish = FALSE;
		menu->CheckMenuItem(ID_MAIN_UI_IN_ENGLISH, MF_UNCHECKED);
		WritePrivateProfileStringFx(_T("Setting"), _T("MainUIinEnglish"), _T("0"), m_Ini);
	}
	else
	{
		m_MainUIinEnglish = TRUE;
		menu->CheckMenuItem(ID_MAIN_UI_IN_ENGLISH, MF_CHECKED);
		WritePrivateProfileStringFx(_T("Setting"), _T("MainUIinEnglish"), _T("1"), m_Ini);
	}
	SetMenu(menu);
	DrawMenuBar();

	ChangeLang(m_CurrentLang);
}

#ifdef SUISHO_AOI_SUPPORT
void CCrystalMarkDlg::OnVoiceEnglish()
{
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_ENGLISH, ID_VOICE_JAPANESE, ID_VOICE_ENGLISH, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();

	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceLanguage"), _T("English"), m_Ini);

	m_CurrentVoice = _T("Aoi-en");
}

void CCrystalMarkDlg::OnVoiceJapanese()
{
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_ENGLISH, ID_VOICE_JAPANESE, ID_VOICE_JAPANESE, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();

	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceLanguage"), _T("Japanese"), m_Ini);

	m_CurrentVoice = _T("Aoi-ja");
}
#endif

#ifdef SUISHO_SHIZUKU_SUPPORT
void CCrystalMarkDlg::OnVoiceVolume000()
{
	m_VoiceVolume = 0;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_000, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("0"), m_Ini);
}

void CCrystalMarkDlg::OnVoiceVolume010()
{
	m_VoiceVolume = 10;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_010, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("10"), m_Ini);
}

void CCrystalMarkDlg::OnVoiceVolume020()
{
	m_VoiceVolume = 20;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_020, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("20"), m_Ini);
}

void CCrystalMarkDlg::OnVoiceVolume030()
{
	m_VoiceVolume = 30;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_030, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("30"), m_Ini);
}

void CCrystalMarkDlg::OnVoiceVolume040()
{
	m_VoiceVolume = 40;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_040, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("40"), m_Ini);
}

void CCrystalMarkDlg::OnVoiceVolume050()
{
	m_VoiceVolume = 50;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_050, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("50"), m_Ini);
}

void CCrystalMarkDlg::OnVoiceVolume060()
{
	m_VoiceVolume = 60;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_060, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("60"), m_Ini);
}

void CCrystalMarkDlg::OnVoiceVolume070()
{
	m_VoiceVolume = 70;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_070, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("70"), m_Ini);
}

void CCrystalMarkDlg::OnVoiceVolume080()
{
	m_VoiceVolume = 80;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_080, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("80"), m_Ini);
}

void CCrystalMarkDlg::OnVoiceVolume090()
{
	m_VoiceVolume = 90;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_090, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("90"), m_Ini);
}

void CCrystalMarkDlg::OnVoiceVolume100()
{
	m_VoiceVolume = 100;
	CMenu* menu = GetMenu();
	menu->CheckMenuRadioItem(ID_VOICE_VOLUME_000, ID_VOICE_VOLUME_100, ID_VOICE_VOLUME_100, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	WritePrivateProfileStringFx(_T("Setting"), _T("VoiceVolume"), _T("100"), m_Ini);
}
#endif