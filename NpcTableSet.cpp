#include "stdafx.h"
#include "NpcTableSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CNpcTableSet, CRecordset)

CNpcTableSet::CNpcTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CNpcTableSet)

	m_sSid = "0";
	m_sPid = "0";
	m_tType = "0";
	m_sTypeAI = "0";
	m_sMinDamage = "0";
	m_sClass = "0";
	m_strName = "";
	m_sBlood = "0";
	m_sLevel = "0";
	m_iMaxExp = "0";
	m_sStr = "0";
	m_sWStr = "0";
	m_sAStr = "0";
	m_sDex_at = "0";
	m_sDex_df = "0";
	m_sIntel = "0";
	m_sCharm = "0";
	m_sWis = "0";
	m_sCon = "0";
	m_sMaxHp = "0";
	m_sMaxMp = "0";
	m_iStatus = "0";
	m_iMoral = "0";
	m_at_type = "0";
	m_can_escape = "0";
	m_can_find_enemy = "0";
	m_can_find_our = "0";
	m_have_item_num = "0";
	m_haved_item = "-1";
	m_have_magic_num = "0";
	m_haved_magic = "-1";
	m_have_skill_num = "0";
	m_haved_skill = "-1";
	m_search_range = "0";
	m_movable_range = "0";
	m_move_speed = "0";
	m_standing_time = "0";
	m_regen_time = "0";
	m_bmagicexp = "0";
	m_wmagicexp = "0";
	m_dmagicexp = "0";
	m_sRangeRate = "0";
	m_sBackRate = "0";
	m_sHowTarget = "0";
	m_sMoneyRate = "0";
	m_sMagicNum01 = "-1";
	m_sMagicRate01 = "0";
	m_sMagicNum02 = "-1";
	m_sMagicRate02 = "0";
	m_sMagicNum03 = "-1";
	m_sMagicRate03 = "0";
	m_sDistance = "0";
	m_nFields = 52;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CNpcTableSet::GetDefaultConnect()
{
	return g_strDatabaseConnectionString;
}

CString CNpcTableSet::GetDefaultSQL()
{
	return _T("[dbo].[MONSTER]");
}

void CNpcTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CNpcTableSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[sSid]"), m_sSid);
	RFX_Text(pFX, _T("[sPid]"), m_sPid);
	RFX_Text(pFX, _T("[tType]"), m_tType);
	RFX_Text(pFX, _T("[sTypeAI]"), m_sTypeAI);
	RFX_Text(pFX, _T("[sMinDamage]"), m_sMinDamage);
	RFX_Text(pFX, _T("[sClass]"), m_sClass);
	RFX_Text(pFX, _T("[strName]"), m_strName);
	RFX_Text(pFX, _T("[sBlood]"), m_sBlood);
	RFX_Text(pFX, _T("[sLevel]"), m_sLevel);
	RFX_Text(pFX, _T("[iMaxExp]"), m_iMaxExp);
	RFX_Text(pFX, _T("[sStr]"), m_sStr);
	RFX_Text(pFX, _T("[sWStr]"), m_sWStr);
	RFX_Text(pFX, _T("[sAStr]"), m_sAStr);
	RFX_Text(pFX, _T("[sDex_at]"), m_sDex_at);
	RFX_Text(pFX, _T("[sDex_df]"), m_sDex_df);
	RFX_Text(pFX, _T("[sIntel]"), m_sIntel);
	RFX_Text(pFX, _T("[sCharm]"), m_sCharm);
	RFX_Text(pFX, _T("[sWis]"), m_sWis);
	RFX_Text(pFX, _T("[sCon]"), m_sCon);
	RFX_Text(pFX, _T("[sMaxHp]"), m_sMaxHp);
	RFX_Text(pFX, _T("[sMaxMp]"), m_sMaxMp);
	RFX_Text(pFX, _T("[iStatus]"), m_iStatus);
	RFX_Text(pFX, _T("[iMoral]"), m_iMoral);
	RFX_Text(pFX, _T("[at_type]"), m_at_type);
	RFX_Text(pFX, _T("[can_escape]"), m_can_escape);
	RFX_Text(pFX, _T("[can_find_enemy]"), m_can_find_enemy);
	RFX_Text(pFX, _T("[can_find_our]"), m_can_find_our);
	RFX_Text(pFX, _T("[have_item_num]"), m_have_item_num);
	RFX_Text(pFX, _T("[haved_item]"), m_haved_item);
	RFX_Text(pFX, _T("[have_magic_num]"), m_have_magic_num);
	RFX_Text(pFX, _T("[haved_magic]"), m_haved_magic);
	RFX_Text(pFX, _T("[have_skill_num]"), m_have_skill_num);
	RFX_Text(pFX, _T("[haved_skill]"), m_haved_skill);
	RFX_Text(pFX, _T("[search_range]"), m_search_range);
	RFX_Text(pFX, _T("[movable_range]"), m_movable_range);
	RFX_Text(pFX, _T("[move_speed]"), m_move_speed);
	RFX_Text(pFX, _T("[standing_time]"), m_standing_time);
	RFX_Text(pFX, _T("[regen_time]"), m_regen_time);
	RFX_Text(pFX, _T("[bmagicexp]"), m_bmagicexp);
	RFX_Text(pFX, _T("[wmagicexp]"), m_wmagicexp);
	RFX_Text(pFX, _T("[dmagicexp]"), m_dmagicexp);
	RFX_Text(pFX, _T("[sRangeRate]"), m_sRangeRate);
	RFX_Text(pFX, _T("[sBackRate]"), m_sBackRate);
	RFX_Text(pFX, _T("[sHowTarget]"), m_sHowTarget);
	RFX_Text(pFX, _T("[sMoneyRate]"), m_sMoneyRate);
	RFX_Text(pFX, _T("[sMagicNum01]"), m_sMagicNum01);
	RFX_Text(pFX, _T("[sMagicRate01]"), m_sMagicRate01);
	RFX_Text(pFX, _T("[sMagicNum02]"), m_sMagicNum02);
	RFX_Text(pFX, _T("[sMagicRate02]"), m_sMagicRate02);
	RFX_Text(pFX, _T("[sMagicNum03]"), m_sMagicNum03);
	RFX_Text(pFX, _T("[sMagicRate03]"), m_sMagicRate03);
	RFX_Text(pFX, _T("[sDistance]"), m_sDistance);

/*	RFX_Int(pFX, _T("[sSid]"), m_sSid);
	RFX_Int(pFX, _T("[sPid]"), m_sPid);
	RFX_Byte(pFX, _T("[tType]"), m_tType);
	RFX_Int(pFX, _T("[sTypeAI]"), m_sTypeAI);
	RFX_Int(pFX, _T("[sMinDamage]"), m_sMinDamage);
	RFX_Int(pFX, _T("[sClass]"), m_sClass);
	RFX_Text(pFX, _T("[strName]"), m_strName);
	RFX_Int(pFX, _T("[sBlood]"), m_sBlood);
	RFX_Int(pFX, _T("[sLevel]"), m_sLevel);
	RFX_Long(pFX, _T("[iMaxExp]"), m_iMaxExp);
	RFX_Int(pFX, _T("[sStr]"), m_sStr);
	RFX_Int(pFX, _T("[sWStr]"), m_sWStr);
	RFX_Int(pFX, _T("[sAStr]"), m_sAStr);
	RFX_Int(pFX, _T("[sDex_at]"), m_sDex_at);
	RFX_Int(pFX, _T("[sDex_df]"), m_sDex_df);
	RFX_Int(pFX, _T("[sIntel]"), m_sIntel);
	RFX_Int(pFX, _T("[sCharm]"), m_sCharm);
	RFX_Int(pFX, _T("[sWis]"), m_sWis);
	RFX_Int(pFX, _T("[sCon]"), m_sCon);
	RFX_Int(pFX, _T("[sMaxHp]"), m_sMaxHp);
	RFX_Int(pFX, _T("[sMaxMp]"), m_sMaxMp);
	RFX_Long(pFX, _T("[iStatus]"), m_iStatus);
	RFX_Long(pFX, _T("[iMoral]"), m_iMoral);
	RFX_Byte(pFX, _T("[at_type]"), m_at_type);
	RFX_Byte(pFX, _T("[can_escape]"), m_can_escape);
	RFX_Byte(pFX, _T("[can_find_enemy]"), m_can_find_enemy);
	RFX_Byte(pFX, _T("[can_find_our]"), m_can_find_our);
	RFX_Byte(pFX, _T("[have_item_num]"), m_have_item_num);
	RFX_Byte(pFX, _T("[haved_item]"), m_haved_item);
	RFX_Byte(pFX, _T("[have_magic_num]"), m_have_magic_num);
	RFX_Byte(pFX, _T("[haved_magic]"), m_haved_magic);
	RFX_Byte(pFX, _T("[have_skill_num]"), m_have_skill_num);
	RFX_Byte(pFX, _T("[haved_skill]"), m_haved_skill);
	RFX_Byte(pFX, _T("[search_range]"), m_search_range);
	RFX_Byte(pFX, _T("[movable_range]"), m_movable_range);
	RFX_Int(pFX, _T("[move_speed]"), m_move_speed);
	RFX_Int(pFX, _T("[standing_time]"), m_standing_time);
	RFX_Int(pFX, _T("[regen_time]"), m_regen_time);
	RFX_Int(pFX, _T("[bmagicexp]"), m_bmagicexp);
	RFX_Int(pFX, _T("[wmagicexp]"), m_wmagicexp);
	RFX_Int(pFX, _T("[dmagicexp]"), m_dmagicexp);
	RFX_Int(pFX, _T("[sRangeRate]"), m_sRangeRate);
	RFX_Int(pFX, _T("[sBackRate]"), m_sBackRate);
	RFX_Int(pFX, _T("[sHowTarget]"), m_sHowTarget);
	RFX_Int(pFX, _T("[sMoneyRate]"), m_sMoneyRate);
	RFX_Int(pFX, _T("[sMagicNum01]"), m_sMagicNum01);
	RFX_Int(pFX, _T("[sMagicRate01]"), m_sMagicRate01);
	RFX_Int(pFX, _T("[sMagicNum02]"), m_sMagicNum02);
	RFX_Int(pFX, _T("[sMagicRate02]"), m_sMagicRate02);
	RFX_Int(pFX, _T("[sMagicNum03]"), m_sMagicNum03);
	RFX_Int(pFX, _T("[sMagicRate03]"), m_sMagicRate03);
	RFX_Int(pFX, _T("[sDistance]"), m_sDistance);*/
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CNpcTableSet diagnostics

#ifdef _DEBUG
void CNpcTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CNpcTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
