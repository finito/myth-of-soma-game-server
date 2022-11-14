#include "stdafx.h"
#include "1p1emu.h"
#include "PlusSpecialTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CPlusSpecialTableSet, CRecordset)

CPlusSpecialTableSet::CPlusSpecialTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CPlusSpecialTableSet)
	m_sId;
	m_sSpecial;
	m_sSpOpt1;
	m_sSpOpt2;
	m_sSpOpt3;

	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CPlusSpecialTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CPlusSpecialTableSet::GetDefaultSQL()
{
	return _T("[dbo].[tb_plus_special]");
}

void CPlusSpecialTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CPlusSpecialTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[sId]"), m_sId);
	RFX_Text(pFX, _T("[sSpecial]"), m_sSpecial);
	RFX_Text(pFX, _T("[sSpOpt1]"), m_sSpOpt1);
	RFX_Text(pFX, _T("[sSpOpt2]"), m_sSpOpt2);
	RFX_Text(pFX, _T("[sSpOpt3]"), m_sSpOpt3);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPlusSpecialTableSet diagnostics

#ifdef _DEBUG
void CPlusSpecialTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CPlusSpecialTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
