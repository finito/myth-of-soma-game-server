#include "stdafx.h"
#include "1p1emu.h"
#include "ConExpTableDSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CConExpTableDSet, CRecordset)

CConExpTableDSet::CConExpTableDSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CConExpTableDSet)
	m_sid;
	m_slevel;
	m_con;
	m_rdcon;
	m_wdcon;

	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CConExpTableDSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CConExpTableDSet::GetDefaultSQL()
{
	return _T("[dbo].[d_tb_inc_con]");
}

void CConExpTableDSet::DoFieldExchange(CFieldExchange* pFX)
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
void CConExpTableDSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CConExpTableDSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
