#include "stdafx.h"
#include "1p1Emu.h"
#include "ItemTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CItemTableSet, CRecordset)

CItemTableSet::CItemTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CItemTableSet)
	m_sNum = 0;
	m_sPicNum1 = 0;
	m_sPicNum2 = 0;
	m_sPicNum3 = 0;
	m_sPicNum4 = 0;
	m_strName = "";
	m_bType = 0;
	m_bArm = 0;
	m_bGender = 0;
	m_sAb1 = 0;
	m_sAb2 = 0;
	m_sWgt = 0;
	m_iCost = 0;
	m_sDur = 0;
	m_sNeedStr = 0;
	m_sNeedInt = 0;
	m_sMinExp = 0;
	m_sStr = 0;
	m_sDex = 0;
	m_sInt = 0;
	m_sAt = 0;
	m_sDf = 0;
	m_sHP = 0;
	m_sMP = 0;
	m_sMagicNo = 0;
	m_sMagicOpt = 0;
	m_sTime = 0;
	m_sSpecial = 0;
	m_sSpOpt1 = 0;
	m_sSpOpt2 = 0;
	m_sSpOpt3 = 0;
	m_nFields = 31;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CItemTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CItemTableSet::GetDefaultSQL()
{
	return _T("[dbo].[BASICITEM]");
}

void CItemTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CItemTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sNum]"), m_sNum);
	RFX_Int(pFX, _T("[sPicNum1]"), m_sPicNum1);
	RFX_Int(pFX, _T("[sPicNum2]"), m_sPicNum2);
	RFX_Int(pFX, _T("[sPicNum3]"), m_sPicNum3);
	RFX_Int(pFX, _T("[sPicNum4]"), m_sPicNum4);
	RFX_Text(pFX, _T("[strName]"), m_strName);
	RFX_Byte(pFX, _T("[bType]"), m_bType);
	RFX_Byte(pFX, _T("[bArm]"), m_bArm);
	RFX_Byte(pFX, _T("[Gender]"), m_bGender);
	RFX_Int(pFX, _T("[sAb1]"), m_sAb1);
	RFX_Int(pFX, _T("[sAb2]"), m_sAb2);
	RFX_Int(pFX, _T("[sWg]"), m_sWgt);
	RFX_Long(pFX, _T("[iCost]"), m_iCost);
	RFX_Int(pFX, _T("[sDur]"), m_sDur);
	RFX_Int(pFX, _T("[sNeedStr]"), m_sNeedStr);
	RFX_Int(pFX, _T("[sNeedInt]"), m_sNeedInt);
	RFX_Int(pFX, _T("[sMinExp]"), m_sMinExp);
	RFX_Int(pFX, _T("[sStr]"), m_sStr);
	RFX_Int(pFX, _T("[sDex]"), m_sDex);
	RFX_Int(pFX, _T("[sInt]"), m_sInt);
	RFX_Int(pFX, _T("[sAt]"), m_sAt);
	RFX_Int(pFX, _T("[sDf]"), m_sDf);
	RFX_Int(pFX, _T("[sHP]"), m_sHP);
	RFX_Int(pFX, _T("[sMP]"), m_sMP);
	RFX_Int(pFX, _T("[sMagicNo]"), m_sMagicNo);
	RFX_Int(pFX, _T("[sMagicOpt]"), m_sMagicOpt);
	RFX_Int(pFX, _T("[sTime]"), m_sTime);
	RFX_Int(pFX, _T("[sSpecial]"), m_sSpecial);
	RFX_Int(pFX, _T("[sSpOpt1]"), m_sSpOpt1);
	RFX_Int(pFX, _T("[sSpOpt2]"), m_sSpOpt2);
	RFX_Int(pFX, _T("[sSpOpt3]"), m_sSpOpt3);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CItemTableSet diagnostics

#ifdef _DEBUG
void CItemTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CItemTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
