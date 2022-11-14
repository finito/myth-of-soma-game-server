#include "stdafx.h"
#include "1p1emu.h"
#include "ClassStoreTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CClassStoreTableSet, CRecordset)

CClassStoreTableSet::CClassStoreTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CClassStoreTableSet)
	m_sId;
	m_sType;
	m_sItemNum;
	m_strName;
	m_sNum;
	m_iPoint;

	m_nFields = 6;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CClassStoreTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CClassStoreTableSet::GetDefaultSQL()
{
	return _T("[dbo].[tb_ClassPointStore]");
}

void CClassStoreTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CClassStoreTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);

	RFX_Text(pFX, _T("[sId]"), m_sId);
	RFX_Text(pFX, _T("[sType]"), m_sType);
	RFX_Text(pFX, _T("[sItemNum]"), m_sItemNum);
	RFX_Text(pFX, _T("[strName]"), m_strName);
	RFX_Text(pFX, _T("[sNum]"), m_sNum);
	RFX_Text(pFX, _T("[iPoint]"), m_iPoint);

	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CClassStoreTableSet diagnostics

#ifdef _DEBUG
void CClassStoreTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CClassStoreTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
