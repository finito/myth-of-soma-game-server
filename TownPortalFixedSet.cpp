#include "stdafx.h"
#include "1p1emu.h"
#include "TownPortalFixedSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTownPortalFixed, CRecordset)

CTownPortalFixed::CTownPortalFixed(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_sid = 0;
	m_szone = 0;
	m_smovezone = 0;
	m_sx = 0;
	m_sy = 0;
	m_nFields = 5;
	m_nDefaultType = snapshot;
}

CString CTownPortalFixed::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CTownPortalFixed::GetDefaultSQL()
{
	return _T("[dbo].[tb_townportal_fixed]");
}

void CTownPortalFixed::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sid]"), m_sid);
	RFX_Int(pFX, _T("[szone]"), m_szone);
	RFX_Int(pFX, _T("[smovezone]"), m_smovezone);
	RFX_Int(pFX, _T("[sx]"), m_sx);
	RFX_Int(pFX, _T("[sy]"), m_sy);

}
/////////////////////////////////////////////////////////////////////////////
// CTownPortalFixed diagnostics

#ifdef _DEBUG
void CTownPortalFixed::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTownPortalFixed::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
