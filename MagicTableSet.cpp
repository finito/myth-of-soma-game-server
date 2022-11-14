#include "stdafx.h"
#include "1p1emu.h"
#include "MagicTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMagicTableSet, CRecordset)

CMagicTableSet::CMagicTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CMagicTableSet)
	m_sMid;
	m_tClass;
	m_strName;
	m_tType01;
	m_tType02;
	m_sStartTime;
	m_tTarget;
	m_sMpdec;
	m_tEValue;
	m_sEDist;
	m_sRange;
	m_sDamage;
	m_iTime;
	m_iTerm;
	m_sMinLevel;
	m_sMinMxp;
	m_sMinInt;
	m_sMinMoral;
	m_sMaxMoral;
	m_strInform;
	m_strGetInfo;
	m_sRate;
	m_sPlusExpRate;
	m_sPlusType;

	m_nFields = 24;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CMagicTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CMagicTableSet::GetDefaultSQL()
{
	return _T("[dbo].[MAGIC]");
}

void CMagicTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[sMid]"), m_sMid);
	RFX_Text(pFX, _T("[tClass]"), m_tClass);
	RFX_Text(pFX, _T("[strName]"), m_strName);
	RFX_Text(pFX, _T("[tType01]"), m_tType01);
	RFX_Text(pFX, _T("[tType02]"), m_tType02);
	RFX_Text(pFX, _T("[sStartTime]"), m_sStartTime);
	RFX_Text(pFX, _T("[tTarget]"), m_tTarget);
	RFX_Text(pFX, _T("[sMpdec]"), m_sMpdec);
	RFX_Text(pFX, _T("[tEValue]"), m_tEValue);
	RFX_Text(pFX, _T("[sEDist]"), m_sEDist);
	RFX_Text(pFX, _T("[sRange]"), m_sRange);
	RFX_Text(pFX, _T("[sDamage]"), m_sDamage);
	RFX_Text(pFX, _T("[iTime]"), m_iTime);
	RFX_Text(pFX, _T("[iTerm]"), m_iTerm);
	RFX_Text(pFX, _T("[sMinLevel]"), m_sMinLevel);
	RFX_Text(pFX, _T("[sMinMxp]"), m_sMinMxp);
	RFX_Text(pFX, _T("[sMinInt]"), m_sMinInt);
	RFX_Text(pFX, _T("[sMinMoral]"), m_sMinMoral);
	RFX_Text(pFX, _T("[sMaxMoral]"), m_sMaxMoral);
	RFX_Text(pFX, _T("[strInform]"), m_strInform);
	RFX_Text(pFX, _T("[strGetInfo]"), m_strGetInfo);
	RFX_Text(pFX, _T("[sRate]"), m_sRate);
	RFX_Text(pFX, _T("[sPlusExpRate]"), m_sPlusExpRate);
	RFX_Text(pFX, _T("[sPlusType]"), m_sPlusType);
	//}}AFX_FIELD_MAP
}

#ifdef _DEBUG
void CMagicTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CMagicTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
