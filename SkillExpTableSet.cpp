#include "stdafx.h"
#include "1p1emu.h"
#include "SkillExpTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSkillExpTableSet, CRecordset)

CSkillExpTableSet::CSkillExpTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CSkillExpTableSet)
	m_sid;
	m_slevel;
	m_axe;
	m_bow;
	m_knuckle;
	m_spear;
	m_staff;
	m_sword;
	m_darkmg;
	m_whitemg;
	m_bluemg;
	m_wpmake;
	m_ammake;
	m_acmake;
	m_pomake;
	m_ckmake;
	m_rdaxe;
	m_wdaxe;
	m_rdbow;
	m_wdbow;
	m_rdknuckle;
	m_wdknuckle;
	m_rdspear;
	m_wdspear;
	m_rdstaff;
	m_wdstaff;
	m_rdsword;
	m_wdsword;
	m_rddarkmg;
	m_wddarkmg;
	m_rdwhitemg;
	m_wdwhitemg;
	m_rdbluemg;
	m_wdbluemg;

	m_nFields = 34;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CSkillExpTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CSkillExpTableSet::GetDefaultSQL()
{
	return _T("[dbo].[tb_inc_exp]");
}

void CSkillExpTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CSkillExpTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);

	RFX_Int(pFX, _T("[sid]"), m_sid);
	RFX_Int(pFX, _T("[slevel]"), m_slevel);
	RFX_Int(pFX, _T("[axe]"), m_axe);
	RFX_Int(pFX, _T("[bow]"), m_bow);
	RFX_Int(pFX, _T("[knuckle]"), m_knuckle);
	RFX_Int(pFX, _T("[spear]"), m_spear);
	RFX_Int(pFX, _T("[staff]"), m_staff);
	RFX_Int(pFX, _T("[sword]"), m_sword);
	RFX_Int(pFX, _T("[darkmg]"), m_darkmg);
	RFX_Int(pFX, _T("[whitemg]"), m_whitemg);
	RFX_Int(pFX, _T("[bluemg]"), m_bluemg);
	RFX_Int(pFX, _T("[wpmake]"), m_wpmake);
	RFX_Int(pFX, _T("[ammake]"), m_ammake);
	RFX_Int(pFX, _T("[acmake]"), m_acmake);
	RFX_Int(pFX, _T("[pomake]"), m_pomake);
	RFX_Int(pFX, _T("[ckmake]"), m_ckmake);
	RFX_Int(pFX, _T("[rdaxe]"), m_rdaxe);
	RFX_Int(pFX, _T("[wdaxe]"), m_wdaxe);
	RFX_Int(pFX, _T("[rdbow]"), m_rdbow);
	RFX_Int(pFX, _T("[wdbow]"), m_wdbow);
	RFX_Int(pFX, _T("[rdknuckle]"), m_rdknuckle);
	RFX_Int(pFX, _T("[wdknuckle]"), m_wdknuckle);
	RFX_Int(pFX, _T("[rdspear]"), m_rdspear);
	RFX_Int(pFX, _T("[wdspear]"), m_wdspear);
	RFX_Int(pFX, _T("[rdstaff]"), m_rdstaff);
	RFX_Int(pFX, _T("[wdstaff]"), m_wdstaff);
	RFX_Int(pFX, _T("[rdsword]"), m_rdsword);
	RFX_Int(pFX, _T("[wdsword]"), m_wdsword);
	RFX_Int(pFX, _T("[rddarkmg]"), m_rddarkmg);
	RFX_Int(pFX, _T("[wddarkmg]"), m_wddarkmg);
	RFX_Int(pFX, _T("[rdwhitemg]"), m_rdwhitemg);
	RFX_Int(pFX, _T("[wdwhitemg]"), m_wdwhitemg);
	RFX_Int(pFX, _T("[rdbluemg]"), m_rdbluemg);
	RFX_Int(pFX, _T("[wdbluemg]"), m_wdbluemg);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSkillExpTableSet diagnostics

#ifdef _DEBUG
void CSkillExpTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CSkillExpTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
