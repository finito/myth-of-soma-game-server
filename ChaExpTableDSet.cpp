#include "stdafx.h"
#include "1p1emu.h"
#include "ChaExpTableDSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CChaExpTableDSet, CRecordset)

CChaExpTableDSet::CChaExpTableDSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CChaExpTableDSet)
	m_sid;
	m_slevel;
	m_cha;
	m_rdcha;
	m_wdcha;

	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CChaExpTableDSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CChaExpTableDSet::GetDefaultSQL()
{
	return _T("[dbo].[d_tb_inc_cha]");
}

void CChaExpTableDSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CChaExpTableDSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sid]"), m_sid);
	RFX_Int(pFX, _T("[slevel]"), m_slevel);
	RFX_Int(pFX, _T("[cha]"), m_cha);
	RFX_Int(pFX, _T("[rdcha]"), m_rdcha);
	RFX_Int(pFX, _T("[wdcha]"), m_wdcha);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CChaExpTableSet diagnostics

#ifdef _DEBUG
void CChaExpTableDSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CChaExpTableDSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
