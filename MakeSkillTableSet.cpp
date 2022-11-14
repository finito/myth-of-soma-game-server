#include "stdafx.h"
#include "1p1emu.h"
#include "MakeSkillTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMakeSkillTableSet, CRecordset)

CMakeSkillTableSet::CMakeSkillTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CMakeSkillTableSet)
	m_sId;
	m_sType;
	m_strName;
	m_sMinExp;
	m_strInform;
	m_strGetInfo;

	m_nFields = 6;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CMakeSkillTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CMakeSkillTableSet::GetDefaultSQL()
{
	return _T("[dbo].[Make_Skill]");
}

void CMakeSkillTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CMakeSkillTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[sId]"), m_sId);
	RFX_Int(pFX, _T("[sType]"), m_sType);
	RFX_Text(pFX, _T("[strName]"), m_strName);
	RFX_Int(pFX, _T("[sMinExp]"), m_sMinExp);
	RFX_Text(pFX, _T("[strInform]"), m_strInform);
	RFX_Text(pFX, _T("[strGetInfo]"), m_strGetInfo);

	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMakeSkillTableSet diagnostics

#ifdef _DEBUG
void CMakeSkillTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CMakeSkillTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
