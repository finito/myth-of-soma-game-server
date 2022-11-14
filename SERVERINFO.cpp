#include "stdafx.h"
#include "1p1Emu.h"
#include "SERVERINFO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] =__FILE__;
#define new DEBUG_NEW
#endif

SERVERINFO::SERVERINFO()
{
	m_szAddr = _T("");
	m_nPort = 0;
	m_strGTime = _T("");
}

SERVERINFO::~SERVERINFO()
{
	for (int i = 0; i < m_zone.GetSize(); i++)
	{
		if (m_zone[i]) delete m_zone[i];
	}

	m_zone.RemoveAll();
}
