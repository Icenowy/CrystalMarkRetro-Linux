/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT              
#define _WIN32_WINNT 0x0501
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#include <afxwin.h>				// MFC core and standard component
#include <afxext.h>				// Extended MFC
#include <afxdtctl.h>			// MFC IE4 Common Control support
#include <afxcmn.h>				// MFC Windows Common Control support

#if _MSC_VER > 1310
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#ifdef SUISHO_AOI_SUPPORT
#ifdef _M_ARM
#define PRODUCT_EDITION			_T("Aoi Edition ARM32")
#elif _M_ARM64
#define PRODUCT_EDITION			_T("Aoi Edition ARM64")
#elif _M_X64
#define PRODUCT_EDITION			_T("Aoi Edition x64")
#else
#define PRODUCT_EDITION			_T("Aoi Edition x86")
#endif

#elif SUISHO_SHIZUKU_SUPPORT
#ifdef _M_ARM
#define PRODUCT_EDITION			_T("Shizuku Edition ARM32")
#elif _M_ARM64
#define PRODUCT_EDITION			_T("Shizuku Edition ARM64")
#elif _M_X64
#define PRODUCT_EDITION			_T("Shizuku Edition x64")
#else
#define PRODUCT_EDITION			_T("Shizuku Edition x86")
#endif

#else
#ifdef _M_ARM
#define PRODUCT_EDITION			_T("ARM32")
#elif _M_ARM64
#define PRODUCT_EDITION			_T("ARM64")
#elif _M_X64
#define PRODUCT_EDITION			_T("x64")
#else
#define PRODUCT_EDITION			_T("x86")
#endif

#endif

//#define UWP

// Version Information
#ifdef UWP
#define PRODUCT_NAME				_T("CrystalMark Retro Pro")
#else
#define PRODUCT_NAME				_T("CrystalMark Retro")
#endif

#define PRODUCT_FILENAME			_T("CrystalMarkRetro")
#define PRODUCT_VERSION				_T("1.0.2")
#define PRODUCT_SHORT_NAME		    _T("CMR")

#define PRODUCT_RELEASE				_T("2024/05/01")
#define PRODUCT_COPY_YEAR			_T("2004-2024")
#define PRODUCT_COPY_AUTHOR			_T("hiyohiyo, koinec")
#define PRODUCT_LICENSE				_T("MIT License")

#ifdef UNICODE
#ifdef SUISHO_AOI_SUPPORT
#define PRODUCT_COPYRIGHT_1         _T("© 2004-2024 hiyohiyo, koinec")
#define PRODUCT_COPYRIGHT_2         _T("© 2023-2024 nijihashi sola")
#elif SUISHO_SHIZUKU_SUPPORT
#define PRODUCT_COPYRIGHT_1         _T("© 2004-2024 hiyohiyo, koinec")
#define PRODUCT_COPYRIGHT_2         _T("© 2012-2024 kirino kasumu")
#else
#define PRODUCT_COPYRIGHT_1			_T("© 2004-2024 hiyohiyo, koinec")
#define PRODUCT_COPYRIGHT_2			_T("")
#endif
#else
#ifdef SUISHO_AOI_SUPPORT
#define PRODUCT_COPYRIGHT_1         _T("Copyright 2004-2024 hiyohiyo, koinec")
#define PRODUCT_COPYRIGHT_2         _T("Copyright 2023-2024 nijihashi sola")
#elif SUISHO_SHIZUKU_SUPPORT
#define PRODUCT_COPYRIGHT_1         _T("Copyright 2004-2024 hiyohiyo, koinec")
#define PRODUCT_COPYRIGHT_2         _T("Copyright 2012-2024 kirino kasumu")
#else
#define PRODUCT_COPYRIGHT_1			_T("Copyright 2004-2024 hiyohiyo, koinec")
#define PRODUCT_COPYRIGHT_2			_T("")
#endif
#endif

#ifdef SUISHO_AOI_SUPPORT
#define URL_MAIN_JA					_T("https://crystalmark.info/ja/aoi")
#define URL_MAIN_EN					_T("https://crystalmark.info/en/aoi")
#else
#define URL_MAIN_JA					_T("https://crystalmark.info/ja/")
#define URL_MAIN_EN 				_T("https://crystalmark.info/en/")
#endif

#define	URL_VERSION_JA				_T("https://crystalmark.info/ja/software/crystalmarkretro/crystalmarkretro-history/")
#define	URL_VERSION_EN				_T("https://crystalmark.info/en/software/crystalmarkretro/crystalmarkretro-history/")
#define	URL_LICENSE_JA				_T("https://crystalmark.info/ja/software/crystalmarkretro/crystalmarkretro-license/")
#define	URL_LICENSE_EN				_T("https://crystalmark.info/en/software/crystalmarkretro/crystalmarkretro-license/")

#define URL_HELP_JA					_T("https://crystalmark.info/ja/software/crystalmarkretro/")
#define URL_HELP_EN 				_T("https://crystalmark.info/en/software/crystalmarkretro/")

#ifdef SUISHO_AOI_SUPPORT
#define	URL_PROJECT_SITE_1		    _T("https://twitter.com/sola_no_crayon")
#define URL_PROJECT_SITE_2		    _T("https://twitter.com/harakeiko0718")
#define URL_PROJECT_SITE_3	        _T("https://instagram.com/kotomi_wicke?igshid=OGQ5ZDc2ODk2ZA==")
#define URL_PROJECT_SITE_4			_T("https://twitter.com/bellche")
#define URL_PROJECT_SITE_5			_T("")

#elif SUISHO_SHIZUKU_SUPPORT
#define	URL_PROJECT_SITE_1		    _T("https://twitter.com/kirinokasumu")
#define URL_PROJECT_SITE_2		    _T("https://linux-ha.osdn.jp/wp/")
#define URL_PROJECT_SITE_3	        _T("https://ch.nicovideo.jp/oss")
#define URL_PROJECT_SITE_4			_T("https://twitter.com/bellche")
#define URL_PROJECT_SITE_5			_T("https://suishoshizuku.com/")
#endif

//------------------------------------------------
// Cert
//------------------------------------------------

#define CERTNAME					_T("Open Source Developer, Noriyuki Miyazaki")
#define CERTNAME_DISKSPD			_T("Noriyuki Miyazaki")

static const int RE_EXEC = 5963;

#pragma warning(disable : 4996)

//------------------------------------------------
// Option Flags
//------------------------------------------------

// For Task Tray Icon Feature
// #define OPTION_TASK_TRAY

//------------------------------------------------
// Global Sttings
//------------------------------------------------

#define DEFAULT_FONT_FACE_1			_T("Segoe UI")
#define DEFAULT_FONT_FACE_2			_T("Tahoma")

#define THEME_DIR					_T("Resource\\Theme\\")
#define LANGUAGE_DIR				_T("Resource\\Language\\")
#define VOICE_DIR					_T("Resource\\Voice\\")


#define MENU_THEME_INDEX			1
#define MENU_LANG_INDEX				3

#define DEFAULT_THEME				_T("Default")
#define DEFAULT_LANGUAGE			_T("English")

#define TIMER_UPDATE_DIALOG			500

#define WM_UPDATE_SCORE				(WM_APP+0x1001)
#define WM_UPDATE_MESSAGE			(WM_APP+0x1002)
#define WM_EXIT_BENCHMARK			(WM_APP+0x1003)
#define WM_START_BENCHMARK			(WM_APP+0x1004)
#define WM_SECRET_VOICE				(WM_APP+0x1005)

