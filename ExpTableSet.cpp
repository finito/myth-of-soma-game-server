#include "stdafx.h"
#include "1p1emu.h"
#include "ExpTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CExpTableSet, CRecordset)

CExpTableSet::CExpTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CExpTableSet)
	m_sid;
	m_slevel;
	m_maxexp;
	m_rdexp;
	m_wdexp;

	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CExpTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CExpTableSet::GetDefaultSQL()
{
	return _T("[dbo].[tb_exp]");
}

void CExpTableSet::DoFieldExchange(CFieldExchange* pFX)
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
void CExpTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CExpTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
