#include "stdafx.h"
#include "1p1emu.h"
#include "TraderExchangeTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTraderExchangeTableSet, CRecordset)

CTraderExchangeTableSet::CTraderExchangeTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CTraderExchangeTableSet)
	m_sId;
	m_sType;
	m_sItem;
	m_sMoney;
	m_sNeedItem[5];
	m_sNeedItemNum[5];

	m_nFields = 14;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CTraderExchangeTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CTraderExchangeTableSet::GetDefaultSQL()
{
	return _T("[dbo].[trader_exchange]");
}

void CTraderExchangeTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CTraderExchangeTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sId]"), m_sId);
	RFX_Int(pFX, _T("[stype]"), m_sType);
	RFX_Int(pFX, _T("[sItem]"), m_sItem);
	RFX_Int(pFX, _T("[sMoney]"), m_sMoney);
	RFX_Int(pFX, _T("[sNeedItem01]"), m_sNeedItem[0]);
	RFX_Int(pFX, _T("[sNeedItemNum01]"), m_sNeedItemNum[0]);
	RFX_Int(pFX, _T("[sNeedItem02]"), m_sNeedItem[1]);
	RFX_Int(pFX, _T("[sNeedItemNum02]"), m_sNeedItemNum[1]);
	RFX_Int(pFX, _T("[sNeedItem03]"), m_sNeedItem[2]);
	RFX_Int(pFX, _T("[sNeedItemNum03]"), m_sNeedItemNum[2]);
	RFX_Int(pFX, _T("[sNeedItem04]"), m_sNeedItem[3]);
	RFX_Int(pFX, _T("[sNeedItemNum04]"), m_sNeedItemNum[3]);
	RFX_Int(pFX, _T("[sNeedItem05]"), m_sNeedItem[4]);
	RFX_Int(pFX, _T("[sNeedItemNum05]"), m_sNeedItemNum[4]);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTraderExchangeTableSet diagnostics

#ifdef _DEBUG
void CTraderExchangeTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTraderExchangeTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
