#include "stdafx.h"
#include "1p1emu.h"
#include "StoreSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CStoreSet, CRecordset)

CStoreSet::CStoreSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CStoreSet)
	m_sStoreID;
	m_sType;
	m_sItemNum;
//	m_strItems;
//	m_strSellType;

	m_sStoreID = 0;
	m_sType = 0;
	m_sItemNum = 0;
//	m_strItems = "";
//	m_strSellType = "";
	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CStoreSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CStoreSet::GetDefaultSQL()
{
	return _T("[dbo].[STORE]");
}

void CStoreSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CStoreSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sId]"), m_sStoreID);
	RFX_Int(pFX, _T("[sType]"), m_sType);
	RFX_Int(pFX, _T("[sItemNum]"), m_sItemNum);
	RFX_Binary(pFX, _T("[Items]"), m_Items, 8000);
	RFX_Binary(pFX, _T("[SellType]"), m_SellType, 50);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CStoreSet diagnostics

#ifdef _DEBUG
void CStoreSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CStoreSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
