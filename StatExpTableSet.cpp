#include "stdafx.h"
#include "1p1emu.h"
#include "StatExpTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CStatExpTableSet, CRecordset)

CStatExpTableSet::CStatExpTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CStatExpTableSet)
	m_sid;
	m_slevel;
	m_str;
	m_dex;
	m_int;
	m_wis;
	m_rdstr;
	m_wdstr;
	m_rddex;
	m_wddex;
	m_rdint;
	m_wdint;
	m_rdwis;
	m_wdwis;

	m_nFields = 14;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CStatExpTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CStatExpTableSet::GetDefaultSQL()
{
	return _T("[dbo].[tb_inc]");
}

void CStatExpTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CStatExpTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sid]"), m_sid);
	RFX_Int(pFX, _T("[slevel]"), m_slevel);
	RFX_Int(pFX, _T("[str]"), m_str);
	RFX_Int(pFX, _T("[dex]"), m_dex);
	RFX_Int(pFX, _T("[int]"), m_int);
	RFX_Int(pFX, _T("[wis]"), m_wis);
	RFX_Int(pFX, _T("[rdstr]"), m_rdstr);
	RFX_Int(pFX, _T("[wdstr]"), m_wdstr);
	RFX_Int(pFX, _T("[rddex]"), m_rddex);
	RFX_Int(pFX, _T("[wddex]"), m_wddex);
	RFX_Int(pFX, _T("[rdint]"), m_rdint);
	RFX_Int(pFX, _T("[wdint]"), m_wdint);
	RFX_Int(pFX, _T("[rdwis]"), m_rdwis);
	RFX_Int(pFX, _T("[wdwis]"), m_wdwis);

	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CStatExpTableSet diagnostics

#ifdef _DEBUG
void CStatExpTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CStatExpTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
