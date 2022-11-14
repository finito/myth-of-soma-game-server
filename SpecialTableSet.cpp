#include "stdafx.h"
#include "1p1emu.h"
#include "SpecialTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSpecialTableSet, CRecordset)

CSpecialTableSet::CSpecialTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CSpecialTableSet)
	m_sMid;
	m_tClass;
	m_strName;
	m_tType;
	m_tTarget;
	m_sHpdec;
	m_sMpdec;
	m_sStmdec;
	m_tEValue;
	m_sEDist;
	m_sDamage;
	m_iTime;
	m_iTerm;
	m_iDecTerm;
	m_sTHp;
	m_sTMp;
	m_sMinExp;
	m_strInform;
	m_strGetInfo;
	m_sRate;
	m_sPlusExpRate;
	m_sPlusType;

	m_nFields = 22;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CSpecialTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CSpecialTableSet::GetDefaultSQL()
{
	return _T("[dbo].[Special_Attack]");
}

void CSpecialTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CSpecialTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[sMid]"), m_sMid);
	RFX_Text(pFX, _T("[tClass]"), m_tClass);
	RFX_Text(pFX, _T("[strName]"), m_strName);
	RFX_Text(pFX, _T("[tType]"), m_tType);
	RFX_Text(pFX, _T("[tTarget]"), m_tTarget);
	RFX_Text(pFX, _T("[sHpdec]"), m_sHpdec);
	RFX_Text(pFX, _T("[sMpdec]"), m_sMpdec);
	RFX_Text(pFX, _T("[sStmdec]"), m_sStmdec);
	RFX_Text(pFX, _T("[tEValue]"), m_tEValue);
	RFX_Text(pFX, _T("[sEDist]"), m_sEDist);
	RFX_Text(pFX, _T("[sDamage]"), m_sDamage);
	RFX_Text(pFX, _T("[iTime]"), m_iTime);
	RFX_Text(pFX, _T("[iTerm]"), m_iTerm);
	RFX_Text(pFX, _T("[iDecTerm]"), m_iDecTerm);
	RFX_Text(pFX, _T("[sTHp]"), m_sTHp);
	RFX_Text(pFX, _T("[sTMp]"), m_sTMp);
	RFX_Text(pFX, _T("[sMinExp]"), m_sMinExp);
	RFX_Text(pFX, _T("[strInform]"), m_strInform);
	RFX_Text(pFX, _T("[strGetInfo]"), m_strGetInfo);
	RFX_Text(pFX, _T("[sRate]"), m_sRate);
	RFX_Text(pFX, _T("[sPlusExpRate]"), m_sPlusExpRate);
	RFX_Text(pFX, _T("[sPlusType]"), m_sPlusType);

	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSpecialTableSet diagnostics

#ifdef _DEBUG
void CSpecialTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CSpecialTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
