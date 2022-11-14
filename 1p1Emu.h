#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#define AUTOMATA_THREAD	4

// CMy1p1EmuApp:
// See 1p1Emu.cpp for the implementation of this class
//

class CMy1p1EmuApp : public CWinApp
{
public:
	CMy1p1EmuApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	BOOL LoadWinsock();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMy1p1EmuApp theApp;
