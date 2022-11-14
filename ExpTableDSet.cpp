#include "stdafx.h"
#include "1p1emu.h"
#include "ExpTableDSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CExpTableDSet, CRecordset)

CExpTableDSet::CExpTableDSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CExpTableDSet)
	m_sid;
	m_slevel;
	m_maxexp;
	m_rdexp;
	m_wdexp;

	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CExpTableDSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CExpTableDSet::GetDefaultSQL()
{
	return _T("[dbo].[d_tb_exp]");
}

void CExpTableDSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CExpTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sid]"), m_sid);
	RFX_Int(pFX, _T("[slevel]"), m_slevel);
	RFX_Int(pFX, _T("[maxexp]"), m_maxexp);
	RFX_Int(pFX, _T("[rdexp]"), m_rdexp);
	RFX_Int(pFX, _T("[wdexp]"), m_wdexp);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CExpTableSet diagnostics

#ifdef _DEBUG
void CExpTableDSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CExpTableDSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
