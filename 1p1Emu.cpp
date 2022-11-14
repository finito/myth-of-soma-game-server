#include "stdafx.h"
#include "1p1Emu.h"
#include "1p1EmuDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy1p1EmuApp

BEGIN_MESSAGE_MAP(CMy1p1EmuApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMy1p1EmuApp construction

CMy1p1EmuApp::CMy1p1EmuApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMy1p1EmuApp object

CMy1p1EmuApp theApp;


// CMy1p1EmuApp initialization

BOOL CMy1p1EmuApp::InitInstance()
{
	// Load Winsock 2.2
	if (!LoadWinsock())
	{
		return FALSE;
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CMy1p1EmuDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//	Load Winsock DLL ver 2.2
//
BOOL CMy1p1EmuApp::LoadWinsock()
{
	// request minimum Winsock 2.2
	WORD wVersionRequested = MAKEWORD(2, 2);

	WSADATA wsaData;
	int err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0)
	{
		CString strError;
		strError.Format(_T("WSAStartup Error, Error# = %d"), ::GetLastError());
		AfxMessageBox(strError);
		return FALSE;
	}

	if (LOBYTE(wsaData.wVersion)!= 2 || HIBYTE(wsaData.wVersion)!= 2)
	{
		CString strError;
		strError.Format(_T("Unsupported WinSock version %d.%d"),
						 LOBYTE(wsaData.wVersion),
						 HIBYTE(wsaData.wVersion));

		AfxMessageBox(strError);
		return FALSE;
	}

	return TRUE;
}
