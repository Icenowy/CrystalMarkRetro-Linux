/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "CrystalMark.h"
#include "CrystalMarkDlg.h"

#include <afxole.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCrystalMarkApp, CWinApp)
END_MESSAGE_MAP()

CCrystalMarkApp theApp;

CCrystalMarkApp::CCrystalMarkApp()
{
}

BOOL CCrystalMarkApp::InitInstance()
{
	BOOL flagAfxOleInit = FALSE;
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (AfxOleInit())
	{
		flagAfxOleInit = TRUE;
		AfxOleGetMessageFilter()->SetMessagePendingDelay(60 * 1000);
		AfxOleGetMessageFilter()->EnableNotRespondingDialog(FALSE);
		AfxOleGetMessageFilter()->EnableBusyDialog(FALSE);
	}
	else
	{
		(void)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	}
	(void)CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

	CCrystalMarkDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	if (!flagAfxOleInit)
	{
		CoUninitialize();
	}

	return FALSE;
}