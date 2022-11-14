#include "stdafx.h"
#include "1p1emu.h"
#include "ChangeOtherItemTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChaExpTableSet

IMPLEMENT_DYNAMIC(CChangeOtherItemTableSet, CRecordset)

CChangeOtherItemTableSet::CChangeOtherItemTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CChangeOtherItemTableSet)
	m_sId;
	m_sSpecialNum;
	m_sItemNum;
	m_strItemName;

	m_nFields = 4;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CChangeOtherItemTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CChangeOtherItemTableSet::GetDefaultSQL()
{
	return _T("[dbo].[tb_changeotheritem]");
}

void CChangeOtherItemTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CChangeOtherItemTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sId]"), m_sId);
	RFX_Int(pFX, _T("[sSpecialNum]"), m_sSpecialNum);
	RFX_Int(pFX, _T("[sItemNum]"), m_sItemNum);
	RFX_Text(pFX, _T("[strItemName]"), m_strItemName);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CChangeOtherItemTableSet diagnostics

#ifdef _DEBUG
void CChangeOtherItemTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CChangeOtherItemTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
