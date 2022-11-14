#include "stdafx.h"
#include "NpcTable.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CNpcTable::CNpcTable()
{
	m_sSid = 0;
	m_sPid = 0;
	m_tType = 0;
	m_sTypeAI = 0;
	m_sMinDamage = 0;
	m_sClass = 0;
	::ZeroMemory(m_strName, sizeof(m_strName));		// MONSTER(NPC) Name
	m_sBlood = 0;
	m_sLevel = 0;
	m_iMaxExp = 0;
	m_sStr = 0;
	m_sWStr = 0;
	m_sAStr = 0;
	m_sDex_at = 0;
	m_sDex_df = 0;
	m_sIntel = 0;
	m_sCharm = 0;
	m_sWis = 0;
	m_sCon = 0;
	m_sMaxHp = 0;
	m_sMaxMp = 0;
	m_iStatus = 0;
	m_iMoral = 0;
	m_at_type = 0;
	m_can_escape = 0;
	m_can_find_enemy = 0;
	m_can_find_our = 0;
	m_have_item_num = 0;
	m_haved_item = -1;
	m_have_magic_num = 0;
	m_haved_magic = -1;
	m_have_skill_num = 0;
	m_haved_skill = -1;
	m_search_range = 0;
	m_movable_range = 0;
	m_move_speed = 0;
	m_standing_time = 0;
	m_regen_time = 0;
	m_bmagicexp = 0;
	m_wmagicexp = 0;
	m_dmagicexp = 0;
	m_sRangeRate = 0;
	m_sBackRate = 0;
	m_sHowTarget = 0;
	m_sMoneyRate = 0;
	m_sMagicNum01 = -1;
	m_sMagicRate01 = 0;
	m_sMagicNum02 = -1;
	m_sMagicRate02 = 0;
	m_sMagicNum03 = -1;
	m_sMagicRate03 = 0;
	m_sDistance = 0;
}

CNpcTable::~CNpcTable()
{

}
