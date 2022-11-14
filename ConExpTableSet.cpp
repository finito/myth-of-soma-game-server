#include "stdafx.h"
#include "1p1emu.h"
#include "ConExpTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CConExpTableSet, CRecordset)

CConExpTableSet::CConExpTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CConExpTableSet)
	m_sid;
	m_slevel;
	m_con;
	m_rdcon;
	m_wdcon;

	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CConExpTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CConExpTableSet::GetDefaultSQL()
{
	return _T("[dbo].[tb_inc_con]");
}

void CConExpTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CConExpTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sid]"), m_sid);
	RFX_Int(pFX, _T("[slevel]"), m_slevel);
	RFX_Int(pFX, _T("[con]"), m_con);
	RFX_Int(pFX, _T("[rdcon]"), m_rdcon);
	RFX_Int(pFX, _T("[wdcon]"), m_wdcon);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CConExpTableSet diagnostics

#ifdef _DEBUG
void CConExpTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CConExpTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
