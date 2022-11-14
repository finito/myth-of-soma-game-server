#include "stdafx.h"
#include "1p1emu.h"
#include "ChaExpTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CChaExpTableSet, CRecordset)

CChaExpTableSet::CChaExpTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CChaExpTableSet)
	m_sid;
	m_slevel;
	m_cha;
	m_rdcha;
	m_wdcha;

	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CChaExpTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CChaExpTableSet::GetDefaultSQL()
{
	return _T("[dbo].[tb_inc_cha]");
}

void CChaExpTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CChaExpTableSet)
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
void CChaExpTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CChaExpTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
