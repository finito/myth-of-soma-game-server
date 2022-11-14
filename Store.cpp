#include "stdafx.h"
#include "1p1emu.h"
#include "Store.h"
#include "Extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CStore::CStore()
{
}

CStore::~CStore()
{
	for (int i = 0; i < m_arItems.GetSize(); i++)
	{
		delete m_arItems[i];
	}
	m_arItems.RemoveAll();
}
