#include "stdafx.h"
#include "1p1emu.h"
#include "NpcChatSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNpcChatSet, CRecordset)

CNpcChatSet::CNpcChatSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CNpcChatSet)
	m_sCid = _T("0");
	m_strTalk = _T("");
	m_nFields = 2;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CNpcChatSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CNpcChatSet::GetDefaultSQL()
{
	return _T("[dbo].[NPCCHAT]");
}

void CNpcChatSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CNpcChatSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[sCid]"), m_sCid);
	RFX_Text(pFX, _T("[strTalk]"), m_strTalk, 1024);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CNpcChatSet diagnostics

#ifdef _DEBUG
void CNpcChatSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CNpcChatSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
