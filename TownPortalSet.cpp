#include "stdafx.h"
#include "TownPortalSet.h"
IMPLEMENT_DYNAMIC(CTownPortal, CRecordset)

CTownPortal::CTownPortal(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_sid = 0;
	m_sitemnum = 0;
	m_smovezone = 0;
	m_sx = 0;
	m_sy = 0;
	m_susezone1 = 0;
	m_susezone2 = 0;
	m_susezone3 = 0;
	m_susezone4 = 0;
	m_nFields = 9;
	m_nDefaultType = snapshot;
}

CString CTownPortal::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CTownPortal::GetDefaultSQL()
{
	return _T("[dbo].[tb_townportal]");
}

void CTownPortal::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sid]"), m_sid);
	RFX_Int(pFX, _T("[sitemnum]"), m_sitemnum);
	RFX_Int(pFX, _T("[smovezone]"), m_smovezone);
	RFX_Int(pFX, _T("[sx]"), m_sx);
	RFX_Int(pFX, _T("[sy]"), m_sy);
	RFX_Int(pFX, _T("[susezone1]"), m_susezone1);
	RFX_Int(pFX, _T("[susezone2]"), m_susezone2);
	RFX_Int(pFX, _T("[susezone3]"), m_susezone3);
	RFX_Int(pFX, _T("[susezone4]"), m_susezone4);

}
/////////////////////////////////////////////////////////////////////////////
// CTownPortal diagnostics

#ifdef _DEBUG
void CTownPortal::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTownPortal::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
