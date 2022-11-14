#include "stdafx.h"
#include "NpcPosSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNpcPosSet, CRecordset)

CNpcPosSet::CNpcPosSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CNpcPosSet)
	
	m_sSid = _T("0");
	m_sZone = _T("0");
	m_sMinX = _T("0");
	m_sMinY = _T("0");
	m_sMaxX = _T("0");
	m_sMaxY = _T("0");
	m_sNum = _T("0");
	m_sTableNum = _T("0");
	m_sRegenType = _T("0");
	m_sRegenTime = _T("0");
	m_sRegenEvent = _T("-1");
	m_sGroup = _T("-1");
	m_sGuild = _T("-1");
	m_sGuildOpt = _T("-1");
	m_sChat01 = _T("-1");
	m_sChat02 = _T("-1");
	m_sChat03 = _T("-1");
	m_sMaxItemNum = _T("-1");
	m_sItem01 = _T("-1");
	m_sItem01Rand = _T("-1");
	m_sItem02 = _T("-1");
	m_sItem02Rand = _T("-1");
	m_sItem03 = _T("-1");
	m_sItem03Rand = _T("-1");
	m_sItem04 = _T("-1");
	m_sItem04Rand = _T("-1");
	m_sItem05 = _T("-1");
	m_sItem05Rand = _T("-1");
	m_sItem06 = _T("-1");
	m_sItem06Rand = _T("-1");
	m_sItem07 = _T("-1");
	m_sItem07Rand = _T("-1");
	m_sItem08 = _T("-1");
	m_sItem08Rand = _T("-1");
	m_sItem09 = _T("-1");
	m_sItem09Rand = _T("-1");
	m_sItem10 = _T("-1");
	m_sItem10Rand = _T("-1");
	m_sItem11 = _T("-1");
	m_sItem11Rand = _T("-1");
	m_sItem12 = _T("-1");
	m_sItem12Rand = _T("-1");
	m_sItem13 = _T("-1");
	m_sItem13Rand = _T("-1");
	m_sItem14 = _T("-1");
	m_sItem14Rand = _T("-1");
	m_sItem15 = _T("-1");
	m_sItem15Rand = _T("-1");
	m_sEventNum = _T("-1");
	m_sMoneyType = _T("-1");
	m_sMoneyMin = _T("-1");
	m_sMoneyMax = _T("-1");

	m_nFields = 52;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CNpcPosSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CNpcPosSet::GetDefaultSQL()
{
	return _T("[dbo].[MONSTERSET]");
}

void CNpcPosSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CNpcPosSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[sSid]"), m_sSid);
	RFX_Text(pFX, _T("[sZone]"), m_sZone);
	RFX_Text(pFX, _T("[sMinX]"), m_sMinX);
	RFX_Text(pFX, _T("[sMinY]"), m_sMinY);
	RFX_Text(pFX, _T("[sMaxX]"), m_sMaxX);
	RFX_Text(pFX, _T("[sMaxY]"), m_sMaxY);
	RFX_Text(pFX, _T("[sNum]"), m_sNum);
	RFX_Text(pFX, _T("[sTableNum]"), m_sTableNum);
	RFX_Text(pFX, _T("[sRegenType]"), m_sRegenType);
	RFX_Text(pFX, _T("[sRegenTime]"), m_sRegenTime);
	RFX_Text(pFX, _T("[sRegenEvent]"), m_sRegenEvent);
	RFX_Text(pFX, _T("[sGroup]"), m_sGroup);
	RFX_Text(pFX, _T("[sGuild]"), m_sGuild);
	RFX_Text(pFX, _T("[sGuildOpt]"), m_sGuildOpt);
	RFX_Text(pFX, _T("[sChat01]"), m_sChat01);
	RFX_Text(pFX, _T("[sChat02]"), m_sChat02);
	RFX_Text(pFX, _T("[sChat03]"), m_sChat03);
	RFX_Text(pFX, _T("[sMaxItemNum]"), m_sMaxItemNum);
	RFX_Text(pFX, _T("[sItem01]"), m_sItem01);
	RFX_Text(pFX, _T("[sItem01Rand]"), m_sItem01Rand);
	RFX_Text(pFX, _T("[sItem02]"), m_sItem02);
	RFX_Text(pFX, _T("[sItem02Rand]"), m_sItem02Rand);
	RFX_Text(pFX, _T("[sItem03]"), m_sItem03);
	RFX_Text(pFX, _T("[sItem03Rand]"), m_sItem03Rand);
	RFX_Text(pFX, _T("[sItem04]"), m_sItem04);
	RFX_Text(pFX, _T("[sItem04Rand]"), m_sItem04Rand);
	RFX_Text(pFX, _T("[sItem05]"), m_sItem05);
	RFX_Text(pFX, _T("[sItem05Rand]"), m_sItem05Rand);
	RFX_Text(pFX, _T("[sItem06]"), m_sItem06);
	RFX_Text(pFX, _T("[sItem06Rand]"), m_sItem06Rand);
	RFX_Text(pFX, _T("[sItem07]"), m_sItem07);
	RFX_Text(pFX, _T("[sItem07Rand]"), m_sItem07Rand);
	RFX_Text(pFX, _T("[sItem08]"), m_sItem08);
	RFX_Text(pFX, _T("[sItem08Rand]"), m_sItem08Rand);
	RFX_Text(pFX, _T("[sItem09]"), m_sItem09);
	RFX_Text(pFX, _T("[sItem09Rand]"), m_sItem09Rand);
	RFX_Text(pFX, _T("[sItem10]"), m_sItem10);
	RFX_Text(pFX, _T("[sItem10Rand]"), m_sItem10Rand);
	RFX_Text(pFX, _T("[sItem11]"), m_sItem11);
	RFX_Text(pFX, _T("[sItem11Rand]"), m_sItem11Rand);
	RFX_Text(pFX, _T("[sItem12]"), m_sItem12);
	RFX_Text(pFX, _T("[sItem12Rand]"), m_sItem12Rand);
	RFX_Text(pFX, _T("[sItem13]"), m_sItem13);
	RFX_Text(pFX, _T("[sItem13Rand]"), m_sItem13Rand);
	RFX_Text(pFX, _T("[sItem14]"), m_sItem14);
	RFX_Text(pFX, _T("[sItem14Rand]"), m_sItem14Rand);
	RFX_Text(pFX, _T("[sItem15]"), m_sItem15);
	RFX_Text(pFX, _T("[sItem15Rand]"), m_sItem15Rand);
	RFX_Text(pFX, _T("[sEventNum]"), m_sEventNum);
	RFX_Text(pFX, _T("[sMoneyType]"), m_sMoneyType);
	RFX_Text(pFX, _T("[sMoneyMin]"), m_sMoneyMin);
	RFX_Text(pFX, _T("[sMoneyMax]"), m_sMoneyMax);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CNpcPosSet diagnostics

#ifdef _DEBUG
void CNpcPosSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CNpcPosSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
