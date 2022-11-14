// UserMisc.cpp: implementation of the misc of USER class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "1p1emu.h"
#include "Extern.h"
#include "USER.h"
#include "COM.h"
#include "CircularBuffer.h"

#include "Search.h"
#include "1p1emudlg.h"

#include "BufferEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CSearch *g_pUserList;
extern CMy1p1EmuDlg *g_pMainDlg;

void USER::NpcEvent(TCHAR *pBuf)
{
	if (m_bTrading) return;

	int index = 0;
	int nUid = GetInt(pBuf, index);

	if (nUid < NPC_BAND || nUid >= INVALID_BAND) return;
	
	CNpc* pNpc = GetNpc(nUid - NPC_BAND);
	if (!pNpc) return;

	if (pNpc->m_NpcState == NPC_LIVE || pNpc->m_NpcState == NPC_DEAD) return;

	if (pNpc->m_sEventNum <= 0) return;

	if (!pNpc->GetDistance(m_sX, m_sY, 6)) return;

	int event_zone_index = -1;
	for (int i = 0; i < g_Events.GetSize(); i++)
	{
		if (g_Events[i]->m_Zone == m_sZ)
		{				
			event_zone_index = i;
			break;
		}
	}

	if (event_zone_index == -1) return;
	EVENT* pEvent = g_Events[event_zone_index];

	if (pEvent == NULL) return;
	if (pNpc->m_sEventNum >= pEvent->m_arEvent.GetSize()) return;
	if (pEvent->m_arEvent[pNpc->m_sEventNum] == NULL) return;

	m_iEventNpcId = pNpc->m_sNid; 

	TRACE("NPC EVENT!  event num : %d\n", pNpc->m_sEventNum);
	if (!CheckEventLogic(pEvent->m_arEvent[pNpc->m_sEventNum])) return;

	for (int j = 0; j < pEvent->m_arEvent[pNpc->m_sEventNum]->m_arExec.GetSize(); j++)
	{
		if (RunNpcEvent(pNpc, pEvent->m_arEvent[pNpc->m_sEventNum]->m_arExec[j]) == NULL) break;
	}
}

bool USER::CheckEventLogic(EVENT_DATA *pEventData)
{
	if (pEventData == NULL) return false;
	bool bExact = true;

	for (int i = 0; i < pEventData->m_arLogicElse.GetSize(); i++)
	{
		bExact = false;

		LOGIC_ELSE* pLE = pEventData->m_arLogicElse[i];
		if (!pLE) return false;

		TRACE("Running Npc Event Logic Command: %s\n", EventLogicToString(pLE->m_LogicElse));

		switch (pLE->m_LogicElse)
		{
		case LOGIC_ABSENT_EVENT:
			if (!FindEvent(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_ABSENT_ITEM:
			if (!FindItem(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_ABSENT_MAGETYPE:
			if (!IsHaveMagicMageType(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_ABSENT_MAGIC:
			if (!IsHaveMagic(pLE->m_LogicElseInt[0], MAGIC_TYPE_MAGIC)) bExact = true;
			break;
		case LOGIC_ACCMAKE:
			if (CheckStatMinMax(m_iAccMakeExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_ARMORMAKE:
			if (CheckStatMinMax(m_iArmorMakeExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_AXE:
			if (CheckStatMinMax(m_iAxeExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_BMAGIC:
			if (CheckStatMinMax(m_iBMagicExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_BOW:
			if (CheckStatMinMax(m_iBowExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_CHECK_ALIVE_NPC:
			if (CheckAliveNpc(pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_CHECK_AUCTION_BADLIST:
			break;
		case LOGIC_CHECK_CLASS_TOWN:
			if (CheckClassTown()) bExact = true;
			break;
		case LOGIC_CHECK_CLASS_TOWN_WAR:
			if (CheckClassTownWar()) bExact = true;
			break;
		case LOGIC_CHECK_COUNTER:
			break;
		case LOGIC_CHECK_DEMON:
			if (m_sClass >= DEVIL && m_sClass < 30) bExact = true;
			break;
		case LOGIC_CHECK_EVENTCOUNT:
			break;
		case LOGIC_CHECK_FIRST_CLICK:
			if (CheckFirstClick()) bExact = true;
			break;
		case LOGIC_CHECK_GUILD_RANK:
			if (CheckGuildRank(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_CHECK_GUILD_TOWN:
			if (CheckGuildTown(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_CHECK_GUILD_TOWN_ALLY:
			if (CheckGuildTownAlly(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_CHECK_GUILD_TOWN_LEVEL:
			if (CheckGuildTownLevel(pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_CHECK_GUILD_TOWN_WAR:
			if (CheckGuildTownWar(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_CHECK_HUMAN:
			if (m_sClass >= 0 && m_sClass < DEVIL) bExact = true;
			break;
		case LOGIC_CHECK_ITEMSLOT:
			if (CheckItemSlot(pLE->m_LogicElseInt[0], pLE)) bExact = true;
			break;
		case LOGIC_CHECK_JOB:
			if (CheckMyJob(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_CHECK_MONSTER:
			bExact = false;
			break;
		case LOGIC_CHECK_PAYTYPE:
			break;
		case LOGIC_CHECK_PKMODE:
			break;
		case LOGIC_CHECK_POSITION:
			bExact = true;
			break;
		case LOGIC_CHECK_TIMER:
			break;
		case LOGIC_COOKING:
			if (CheckStatMinMax(m_iCookingExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_DMAGIC:
			if (CheckStatMinMax(m_iDMagicExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_EXIST_EVENT:
			if (FindEvent(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_EXIST_ITEM:
			if (FindItem(pLE->m_LogicElseInt[0]) >= pLE->m_LogicElseInt[1]) bExact = true;
			break;
		case LOGIC_EXIST_MAGETYPE:
			if (IsHaveMagicMageType(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_EXIST_MAGIC:
			if (IsHaveMagic(pLE->m_LogicElseInt[0], MAGIC_TYPE_MAGIC)) bExact = true;
			break;
		case LOGIC_GAME_TIME:
			if (CheckGameTime(pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1], pLE->m_LogicElseInt[2], pLE->m_LogicElseInt[3])) bExact = true;
			break;
		case LOGIC_GENDER:
			if (m_sGender == pLE->m_LogicElseInt[0]) bExact = true;
			break;
		case LOGIC_GOLD:
			if (CheckMoneyMinMax(pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_HOWMUCH_ITEM:
			if (CheckItemMinMax(pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1], pLE->m_LogicElseInt[2])) bExact = true;
			break;
		case LOGIC_INT:
			if (CheckStatMinMax(m_iINT/CLIENT_BASE_STATS, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_KNUCKLE:
			if (CheckStatMinMax(m_iKnuckleExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_LEVEL:
			if (CheckLevel(pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true; 
			break;
		case LOGIC_MAGETYPE:
			if (CheckMageType(pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_MORAL:
			if (CheckStatMinMax(m_iMoral/CLIENT_MORAL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_POTIONMAKE:
			if (CheckStatMinMax(m_iPotionMakeExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_RAND:
			if (CheckRandom(pLE->m_LogicElseInt[0])) bExact = true;
			break;
		case LOGIC_RANK:
			if (m_sRank == pLE->m_LogicElseInt[0]) bExact = true;
			break;
		case LOGIC_SPEAR:
			if (CheckStatMinMax(m_iSpearExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_STAFF:
			if (CheckStatMinMax(m_iStaffExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_STORAGE_TYPE:
			break;
		case LOGIC_SWORD:
			if (CheckStatMinMax(m_iSwordExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_WEAPONMAKE:
			if (CheckStatMinMax(m_iWeaponMakeExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		case LOGIC_WEIGHT:
			if (CheckItemWeight(pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1], pLE->m_LogicElseInt[2],
													pLE->m_LogicElseInt[3], pLE->m_LogicElseInt[4],
													pLE->m_LogicElseInt[5], pLE->m_LogicElseInt[6],
													pLE->m_LogicElseInt[7], pLE->m_LogicElseInt[8],
													pLE->m_LogicElseInt[9], pLE->m_LogicElseInt[10])) bExact = true;
			break;
		case LOGIC_WMAGIC:
			if (CheckStatMinMax(m_iWMagicExp/CLIENT_SKILL, pLE->m_LogicElseInt[0], pLE->m_LogicElseInt[1])) bExact = true;
			break;
		default:
			return false;
		}

		if (!pLE->m_bAnd)
		{
			if (bExact)
			{
				return true;
			}
		}
		else
		{
			if (!bExact)
			{
				return false;
			}
		}
	}

	return bExact;
}

bool USER::RunNpcEvent(CNpc *pNpc, EXEC *pExec)
{
	int nDist = 0;
	int result = 0;
	short sID = 0;

	if (pNpc)
	{
		if (pNpc->m_sCurZ != m_sZ) return true;
	}

	TRACE("Running Npc Event Exec Command: %s\n", EventExecToString(pExec->m_Exec));

	switch (pExec->m_Exec)
	{
	case EXEC_ADD_EVENT:
		AddMyEventNum(pExec->m_ExecInt[0]);
		break;
	case EXEC_ADD_SERVER_EVENT:
		break;
	case EXEC_AUCTION_BUY_ITEM:
		break;
	case EXEC_AUCTION_BUY_OPEN:
		break;
	case EXEC_AUCTION_DELETE_BADLIST:
		break;
	case EXEC_AUCTION_DRAW_NOBID:
		break;
	case EXEC_AUCTION_DRAW_NOBUY:
		break;
	case EXEC_AUCTION_GET_MONEY:
		break;
	case EXEC_AUCTION_SELL_OPEN:
		break;
	case EXEC_BBS_OPEN:
		BBSOpen(pExec->m_ExecInt[0]);
		break;
	case EXEC_CHANGE_CHA:
		ChangeCha(pExec->m_ExecInt[1]*CLIENT_EXT_STATS);
		break;
	case EXEC_CHANGE_DEX:
		ChangeDex(pExec->m_ExecInt[1]*CLIENT_EXT_STATS);
		break;
	case EXEC_CHANGE_HP:
		CheckMaxValue(m_sHP, pExec->m_ExecInt[1]);
		if (m_sHP < 0) m_sHP = 0;
		if (m_sHP > GetMaxHP()) m_sHP = GetMaxHP();
		SendHPMP();
		break;
	case EXEC_CHANGE_INT:
		ChangeInt(pExec->m_ExecInt[1]*CLIENT_BASE_STATS);
		break;
	case EXEC_CHANGE_MAGETYPE:
		ChangeMageType(m_sMageType = pExec->m_ExecInt[0]);
		break;
	case EXEC_CHANGE_MAX_HP:
		ChangeExtStat(m_sMaxHP, pExec->m_ExecInt[1]);
		break;
	case EXEC_CHANGE_MP:
		CheckMaxValue(m_sMP, pExec->m_ExecInt[1]);
		if (m_sMP < 0) m_sMP = 0;
		if (m_sMP > GetMaxMP()) m_sMP = GetMaxMP();
		SendHPMP();
		break;
	case EXEC_CHANGE_STORAGE_TYPE:
		break;
	case EXEC_CHANGE_STR:
		ChangeStr(pExec->m_ExecInt[1]*CLIENT_BASE_STATS);
		break;
	case EXEC_CHANGE_WIS:
		ChangeWis(pExec->m_ExecInt[1]*CLIENT_EXT_STATS);
		break;
	case EXEC_CLASSPOINT_STORE_OPEN:
		SendClassStoreOpen(pExec->m_ExecInt[0]);
		break;
	case EXEC_COUNTER_INIT:
		break;
	case EXEC_COUNTER_START:
		break;
	case EXEC_DEL_EVENT:
		DelMyEventNum(pExec->m_ExecInt[0]);
		break;
	case EXEC_DEL_SERVER_EVENT:
		break;
	case EXEC_GIVE_GOLD:
		GiveMoney(pExec->m_ExecInt[0]);
		break;
	case EXEC_GIVE_ITEM:
		GiveItem(pExec->m_ExecInt[0], pExec->m_ExecInt[1]);
		break;
	case EXEC_GIVE_MAGIC:
		GiveMagic(pExec->m_ExecInt[0], MAGIC_TYPE_MAGIC);
		break;
	case EXEC_GIVE_MAKE_SKILL:
		GiveMagic(pExec->m_ExecInt[0], MAGIC_TYPE_ABILITY);
		break;
	case EXEC_GIVE_SPECIAL_ATTACK:
		GiveMagic(pExec->m_ExecInt[0], MAGIC_TYPE_SPECIAL);
		break;
	case EXEC_GUILDTOWN_TAXRATE:
		break;
	case EXEC_GUILD_OPEN:
		GuildOpen();
		break;
	case EXEC_KILL_NPC:
		KillNpc();
		break;
	case EXEC_LINK:
		if (pExec->m_ExecInt[2] != m_sZ)
		{
			LinkToOtherZone(pExec->m_ExecInt[2], pExec->m_ExecInt[0], pExec->m_ExecInt[1]);
		}
		else
		{
			LinkToSameZone(pExec->m_ExecInt[0], pExec->m_ExecInt[1]);
		}
		break;
	case EXEC_LIVE_NPC:
		LiveNpc(pExec->m_ExecInt[0], pExec->m_ExecInt[1], pExec->m_ExecInt[2], pExec->m_ExecInt[3]);
		break;
	case EXEC_MAGIC:
		EventMagic(pExec->m_ExecInt[0]);
		break;
	case EXEC_MAKER_OPEN:
		SendMakerOpen(pExec->m_ExecInt);
		break;
	case EXEC_MESSAGE_NONE:
		SendMsgBox(1, pExec->m_ExecInt[0]);
		break;
	case EXEC_MESSAGE_OK:
		SendMsgBox(0, pExec->m_ExecInt[0]);
		break;
	case EXEC_OPEN_CHANGE_ITEM:
		break;
	case EXEC_OPEN_GUILDTOWN_STONE:
		OpenGuildTownStone(pExec->m_ExecInt[0]);
		break;
	case EXEC_OPEN_GUILDTOWN_STORE:
		break;
	case EXEC_OPEN_GUILDTOWN_STORE_REG:
		break;
	case EXEC_OPEN_HAIRSHOP:
		OpenHairShop(pExec->m_ExecInt);
		break;
	case EXEC_OPEN_SPECIAL_MAKESKILL:
		SendOpenSpecialMake(pExec->m_ExecInt[0]);
		break;
	case EXEC_OPEN_TRADER:
		OpenTrader(pExec->m_ExecInt);
		break;
	case EXEC_PLUS_MORAL:
		break;
	case EXEC_REPAIR_ITEM_OPEN:
		RepairItemOpenReq();
		break;
	case EXEC_RESET_EVENTCOUNT:
		break;
	case EXEC_RETURN:
		return false;
	case EXEC_ROB_GOLD:
		RobMoney(pExec->m_ExecInt[0]);
		break;
	case EXEC_ROB_ITEM:
		RobItem(pExec->m_ExecInt[0], pExec->m_ExecInt[1]);
		break;
	case EXEC_ROB_MAGIC:
		RobMagic(pExec->m_ExecInt[0], MAGIC_TYPE_MAGIC);
		break;
	case EXEC_RUN_EVENT:
		{
			EVENT* pEvent = GetEventInCurrentZone();	
			if (!pEvent) break;
			if (!pEvent->m_arEvent[pExec->m_ExecInt[0]]) break;
			if (!CheckEventLogic(pEvent->m_arEvent[pExec->m_ExecInt[0]])) break;
			for (int i = 0; i < pEvent->m_arEvent[pExec->m_ExecInt[0]]->m_arExec.GetSize(); i++)
			{
				if (!RunNpcEvent(pNpc, pEvent->m_arEvent[pExec->m_ExecInt[0]]->m_arExec[i]))
				{
					return FALSE;
				}
			}
		}
		break;
	case EXEC_SAY:
		SendNpcSay(pNpc, pExec->m_ExecInt[0]);
		break;
	case EXEC_SELECT_MSG:
		m_iTalkToNpc = pNpc->m_sNid;
		SendSelectMsg(pExec->m_ExecInt);
		break;
	case EXEC_SET_EVENTCOUNT:
		break;
	case EXEC_SHOW_MAGIC:
		ShowMagic(pExec->m_ExecInt[0]);
		break;
	case EXEC_SPECIAL_ATTACK:
		EventSAttack(pExec->m_ExecInt[0]);
		break;
	case EXEC_STORAGE_OPEN:
		StorageOpen();
		break;
	case EXEC_STORE_OPEN:
		SendStoreOpen(pExec->m_ExecInt[0], pExec->m_ExecInt[1], pExec->m_ExecInt[2]);
		break;
	case EXEC_TIMER_INIT:
		break;
	case EXEC_TIMER_START:
		break;
	case EXEC_WARP_GATE:
		m_bIsUsingWarpItem = true;
		m_nWarpZone = pExec->m_ExecInt[0];
		m_nWarpX = pExec->m_ExecInt[1];
		m_nWarpY = pExec->m_ExecInt[2];
		TownPortalReqSend();
		break;
	case EXEC_XMASPOINT_GUILD:
		break;
	case EXEC_XMASPOINT_USER:
		break;
	default:
		break;
	}

	return true;
}

void USER::SendSelectMsg(int* iSelMsg)
{
	CBufferEx TempBuf;
	int chatnum = -1;
	TempBuf.Add(PKT_EVENTSELBOX);
	chatnum = GetNpcChatIndex(iSelMsg[0]);
	if (chatnum == -1)
	{
		TempBuf.Add((short)0);
	}
	else
	{
		TempBuf.Add((short)g_arNpcChat[chatnum]->m_strTalk.GetLength());
		TempBuf.AddData((char*)(LPCTSTR)g_arNpcChat[chatnum]->m_strTalk, g_arNpcChat[chatnum]->m_strTalk.GetLength());
		TRACE("Adding String: %s\n",g_arNpcChat[chatnum]->m_strTalk);
	}
	for (int i = 0; i < 4; i++)
	{
		chatnum = GetNpcChatIndex(iSelMsg[i*2+1]);
		if ((iSelMsg[i*2+1]==-1)||(chatnum == -1))
		{
            TempBuf.Add((short)0);			
		}
		else
		{
			TempBuf.Add((short)g_arNpcChat[chatnum]->m_strTalk.GetLength());
			TempBuf.AddData((char*)(LPCTSTR)g_arNpcChat[chatnum]->m_strTalk, g_arNpcChat[chatnum]->m_strTalk.GetLength());
			TRACE("Adding String: %s\n",g_arNpcChat[chatnum]->m_strTalk);
		}
		m_iSelectMsgResult[i] = iSelMsg[i*2+2];
	}
	Send(TempBuf, TempBuf.GetLength());
}

int USER::GetNpcChatIndex(int iChatNum)
{
	int talknum = -1;
	int i;

	if (iChatNum < 0) return -1;

	for (i = 0; i < g_arNpcChat.GetSize(); i++)
	{
		if (g_arNpcChat[i]->m_sCid == iChatNum)
		{
			talknum = i;
			break;
		}
	}
	return talknum;
}

EVENT* USER::GetEventInCurrentZone()
{
	int i;//, event_index = -1;
	EVENT *pEvent;

	for (i = 0; i < g_Events.GetSize(); i++)
	{
		pEvent = g_Events[i];
		if (!pEvent)continue;
		if (pEvent->m_Zone == m_sZ)
		{
//			event_index = i;
//			break;
			return g_Events[i];
		}
	}

//	if (event_index == -1) return NULL;

//	return g_event[event_index];
	return NULL;
}

void USER::AddMyEventNum(int iEventNum)
{
	int i;
	int* senum;

	if (MAX_EVENT_NUM <= m_arEventNum.GetSize()) return;

	for (i = 0; i < m_arEventNum.GetSize(); i++)
	{
		if (!m_arEventNum[i]) continue;

		if (*(m_arEventNum[i]) == iEventNum)
		{
			return;
		}
	}

	senum = new int;

	*senum = iEventNum;

	m_arEventNum.Add(senum);
}

void USER::ChangeCha(int iAmount)
{
	if (iAmount == 0) return;
	int iCurChr = m_iCHA/CLIENT_EXT_STATS;
	m_iCHA += iAmount;
	int iNewChr = m_iCHA/CLIENT_EXT_STATS;
	int iDiff = abs(iNewChr - iCurChr);
	int iCur = iCurChr%10;  

	if (iDiff == 0) return; // Stat hasn't changed
	
	if (iDiff + iCur >= 10) // Yellow Stat Gain
	{
		if (iAmount > 0)
		{
			SendSpecialMsg(IDS_USER_CHA_INC, YELLOW_STAT, TO_ME); // Stat increase
		}
		else
		{
			SendSpecialMsg(IDS_USER_CHA_DEC, YELLOW_STAT, TO_ME); // Stat Decrease
		}
	}
	else // Blue Stat gain
	{
		if (iAmount > 0)
		{
			SendSpecialMsg(IDS_USER_CHA_INC, BLUE_STAT, TO_ME); // Stat increase
		}
		else
		{
			SendSpecialMsg(IDS_USER_CHA_DEC, BLUE_STAT, TO_ME); // Stat Decrease
		}
	}
	//GetRecoverySpeed(); // Recovery Speed could have changed as a result of stat gain..
	//SetUserToMagicUser(); // Update Magic Stats... as normal stats have changed
	SendCharData(INFO_BASICVALUE); // Send stat change to user..
}

void USER::ChangeDex(int iAmount)
{
	if (iAmount == 0) return;
	int iCurDex = m_iDEX/CLIENT_EXT_STATS;
	m_iDEX += iAmount;
	int iNewDex = m_iDEX/CLIENT_EXT_STATS;
	int iDiff = abs(iNewDex - iCurDex);
	int iCur = iCurDex%10; 

	if (iDiff == 0) return; // Stat hasn't changed
	
	if (iDiff + iCur >= 10) // Yellow Stat Gain
	{
		if (iAmount > 0)
		{
			SendSpecialMsg(IDS_USER_DEX_INC, YELLOW_STAT, TO_ME); // Stat increase
		}
		else
		{
			SendSpecialMsg(IDS_USER_DEX_DEC, YELLOW_STAT, TO_ME); // Stat Decrease
		}
	}
	else // Blue Stat gain
	{
		if (iAmount > 0)
		{
			SendSpecialMsg(IDS_USER_DEX_INC, BLUE_STAT, TO_ME); // Stat increase
		}
		else
		{
			SendSpecialMsg(IDS_USER_DEX_DEC, BLUE_STAT, TO_ME); // Stat Decrease
		}
	}
	//GetRecoverySpeed(); // Recovery Speed could have changed as a result of stat gain..
	//SetUserToMagicUser(); // Update Magic Stats... as normal stats have changed
	SendCharData(INFO_BASICVALUE); // Send stat change to user..
}

void USER::ChangeInt(int iAmount)
{
	if (iAmount == 0) return;
	int iCurInt = m_iINT/CLIENT_BASE_STATS;
	m_iINT += iAmount; 
	int iNewInt = m_iINT/CLIENT_BASE_STATS;
	int iDiff = abs(iNewInt - iCurInt);
	int iCur = iCurInt%10;

	if (iDiff == 0) return; // Stat hasn't changed
	
	if (iDiff + iCur >= 10) // Yellow Stat Gain
	{
		if (iAmount > 0)
		{
			SendSpecialMsg(IDS_USER_INT_INC, YELLOW_STAT, TO_ME); // Stat increase
		}
		else
		{
			SendSpecialMsg(IDS_USER_INT_DEC, YELLOW_STAT, TO_ME); // Stat Decrease
		}
	}
	else // Blue Stat gain
	{
		if (iAmount > 0)
		{
			SendSpecialMsg(IDS_USER_INT_INC, BLUE_STAT, TO_ME); // Stat increase
		}
		else
		{
			SendSpecialMsg(IDS_USER_INT_DEC, BLUE_STAT, TO_ME); // Stat Decrease
		}
	}
	//GetRecoverySpeed(); // Recovery Speed could have changed as a result of stat gain..
	//SetUserToMagicUser(); // Update Magic Stats... as normal stats have changed
	SendCharData(INFO_BASICVALUE); // Send stat change to user..
}

void USER::ChangeStr(int iAmount)
{
	if (iAmount == 0) return;
	int iCurStr = m_iSTR/CLIENT_BASE_STATS;
	m_iSTR += iAmount;
	int iNewStr = m_iSTR/CLIENT_BASE_STATS;
	int iDiff = abs(iNewStr - iCurStr);
	int iCur = iCurStr%10;

	if (iDiff == 0) return; // Stat hasn't changed
	
	if (iDiff + iCur >= 10) // Yellow Stat Gain
	{
		if (iAmount > 0)
		{
			SendSpecialMsg(IDS_USER_STR_INC, YELLOW_STAT, TO_ME); // Stat increase
		}
		else
		{
			SendSpecialMsg(IDS_USER_STR_DEC, YELLOW_STAT, TO_ME); // Stat Decrease
		}
	}
	else // Blue Stat gain
	{
		if (iAmount > 0)
		{
			SendSpecialMsg(IDS_USER_STR_INC, BLUE_STAT, TO_ME); // Stat increase
		}
		else
		{
			SendSpecialMsg(IDS_USER_STR_DEC, BLUE_STAT, TO_ME); // Stat Decrease
		}
	}

	SendCharData(INFO_BASICVALUE); // Send stat change to user..
}

void USER::ChangeWis(int iAmount)
{
	if (iAmount == 0) return;
	int iCurWis = m_iWIS/CLIENT_EXT_STATS;
	m_iWIS += iAmount;
	int iNewWis = m_iWIS/CLIENT_EXT_STATS;
	int iDiff = abs(iNewWis - iCurWis);
	int iCur = iCurWis%10; 

	if (iDiff == 0) return; // Stat hasn't changed
	
	if (iDiff + iCur >= 10) // Yellow Stat Gain
	{
		if (iAmount > 0)
		{
			SendSpecialMsg(IDS_USER_WIS_INC, YELLOW_STAT, TO_ME); // Stat increase
		}
		else
		{
			SendSpecialMsg(IDS_USER_WIS_DEC, YELLOW_STAT, TO_ME); // Stat Decrease
		}
	}
	else // Blue Stat gain
	{
		if (iAmount > 0)
		{
			SendSpecialMsg(IDS_USER_WIS_INC, BLUE_STAT, TO_ME); // Stat increase
		}
		else
		{
			SendSpecialMsg(IDS_USER_WIS_DEC, BLUE_STAT, TO_ME); // Stat Decrease
		}
	}

	SendCharData(INFO_BASICVALUE); // Send stat change to user..
}

void USER::SendClassStoreOpen(int sStore)
{
	if (m_bTrading) return;

	CBufferEx TempBuf, TempBuf2;
	int i = 0;
	short sNum = 0;
	int iSize = 0;

	TempBuf.Add(PKT_CLASSPOINTBUY);
	TempBuf.Add((BYTE)1); // Open store

	for (i = 0; i < g_arClassStoreTable.GetSize(); i++)
	{
		if (g_arClassStoreTable[i]->m_sType == sStore)
		{
			TCHAR pData[150];
            iSize = GetStoreItemData(g_arClassStoreTable[i]->m_sItemNum, g_arClassStoreTable[i]->m_sNum, pData, g_arClassStoreTable[i]->m_iPoint, g_arClassStoreTable[i]->m_strName);
			if (iSize > 0)
			{
				TempBuf2.AddData(pData, iSize);
			}
			else
			{
				TRACE("Class Store Error: %d\n", sStore);
				return;
			}
			sNum++;
		}
	}
	TempBuf.Add(sNum);
	TempBuf.AddData(TempBuf2, TempBuf2.GetLength());

	Send(TempBuf, TempBuf.GetLength());
}

void USER::DelMyEventNum(int iEventNum)
{
	int i;

	for (i = 0; i < m_arEventNum.GetSize(); i++)
	{
		if (!m_arEventNum[i])continue;

		if (*(m_arEventNum[i]) == iEventNum)
		{
			delete m_arEventNum[i];
			m_arEventNum[i] = NULL;
			m_arEventNum.RemoveAt(i, 1);
			DelMyEventNum(iEventNum);
			return;
		}
	}
}

void USER::GiveMoney(int money)
{
	if (money <= 0) return;

	DWORD dwBackup = m_dwBarr;
	DWORD dwGiveMoney = money;

	CheckMaxValue((DWORD &)m_dwBarr, dwGiveMoney);

	// TODO: Implement soma version of update function
	//if (!UpdateUserItemMoney())
	//{
	//	m_dwBarr = dwBackup;
	//	return;
	//}

	SendMoneyChanged();
}

void USER::GiveItem(int sNum, int iCount)
{
	for (int i = 0; i < iCount; i++)
	{
		ItemList item;
		item.InitFromItemTable(sNum);
		int iSlot = PushItemInventory(&item);
		if (iSlot == -1)
		{
			break;
		}
		SendGetItem(iSlot);
	}
	SendChangeWgt();

	//if (sNum <= 0) return false;
	//if (iCount <= 0 || iCount > MAX_ITEM_DURA) return false;
	//short sSlot = -1;
	//int iWeight = 0;
	//ItemList GiveItem;
	//if (!GiveItem.InitFromItemTable(sNum))
	//{
	//	return false;
	//}

	//if (GiveItem.bType > TYPE_ACC) // Non equipment
	//{
	//	iWeight = GiveItem.sWgt * iCount;

	//	sSlot = GetSameItem(GiveItem, INVENTORY_SLOT);
	//}
	//else // Equipment
	//{
	//	iWeight = GiveItem.sWgt;
	//}

	//if (sSlot != -1)	
	//{
	//	CheckMaxValue((short &)m_InvItem[sSlot].sUsage, (short)iCount);
	//	SendItemInfoChange(BASIC_INV, sSlot, INFO_DUR); // Just Update duration as item already exists..
	//}
	//else 
	//{ // Equipment or new item
	//	sSlot = GetEmptySlot(INVENTORY_SLOT);

	//	if (sSlot < 0)return false; // No free inventory slots

	//	m_InvItem[sSlot].Init();
	//	m_InvItem[sSlot] = GiveItem;
	//	SendGetItem(sSlot); // Send whole item data as new item...
	//}
	//
	//// No need to check weight as LOGIC EVT's Should do that...
	//// TODO : Is it bad idea to rely upon evt checking the weight when giving item.
	//m_sWgt += iWeight;

	//// GetRecoverySpeed(); // Weight could affect recovery speed

	//return true;
}

void USER::GiveMagic(short sMid, BYTE byMagicType)
{
	if (m_nHaveMagicNum >= MAX_MAGIC_NUM) return; // Already at max magic capacity..

	// Do not allow invalid magic number and magic type
	if (sMid <= 0) return;
	if (byMagicType == MAGIC_TYPE_MAGIC)
	{
		if (sMid > g_arMagicTable.GetSize()) return;
	}
	else if (byMagicType == MAGIC_TYPE_SPECIAL)
	{
		if (sMid > g_arSpecialAttackTable.GetSize()) return;
	}
	else if (byMagicType == MAGIC_TYPE_ABILITY)
	{
		if (sMid > g_arMakeSkillTable.GetSize()) return;
	}
	else
	{
		return;
	}

	// Check user doesn't already have the magic first
	bool bFound = false;
	for (int i = 0; i < MAX_MAGIC_NUM; i++)
	{
		if (m_UserMagic[i].sMid == sMid && m_UserMagic[i].byType == byMagicType)
		{
			bFound = true;	
			break;
		}
	}

	// Only add the magic if it isn't already there
	if (!bFound)
	{
		// Find an empty slot and put the magic in there
		for (int i = 0; i < MAX_MAGIC_NUM; i++)
		{
			if (m_UserMagic[i].sMid == -1)
			{
				m_UserMagic[i].sMid = sMid; 
				m_UserMagic[i].byType = byMagicType;
				break;
			}
		}
		m_nHaveMagicNum++;

		if (byMagicType == MAGIC_TYPE_MAGIC)
		{
			if (m_sMageType == 0)
			{
				m_sMageType = g_arMagicTable[sMid-1]->m_tClass * 100;
			}
			else if (g_arMagicTable[sMid-1]->m_tClass != MAGIC_CLASS_BLUE &&
				m_sMageType / 100 != g_arMagicTable[sMid-1]->m_tClass)
			{
				m_sMageType = g_arMagicTable[sMid-1]->m_tClass * 100;
				for (int i = 0; i < MAGIC_BELT_SIZE; i++)
				{
					if (m_UserMagicBelt[i].sMid >= 1 && 
						m_UserMagicBelt[i].sMid < 1000 && 
						m_UserMagicBelt[i].byType == MAGIC_TYPE_MAGIC)
					{
						if (g_arMagicTable[m_UserMagicBelt[i].sMid-1]->m_tClass != MAGIC_CLASS_BLUE &&
							m_sMageType / 100 != g_arMagicTable[m_UserMagicBelt[i].sMid-1]->m_tClass)
						{
							m_UserMagicBelt[i].sMid = -1;
						}
					}
				}

				switch (m_sMageType / 100)
				{
				case MAGIC_CLASS_WHITE:
					m_iDMagicExp = static_cast<int>(m_iDMagicExp * 0.8);
					break;
				case MAGIC_CLASS_BLACK:
					m_iWMagicExp = static_cast<int>(m_iWMagicExp * 0.8);
					break;
				}

				m_sMageType += g_pMainDlg->m_nHour;
			}
		}

		SendCharMagicData();
	}
}

void USER::SendMakerOpen(int* iMakers)
{
	int i;
	CBufferEx TempBuf;
	int iTotalMake = 0;
	TempBuf.Add(PKT_MAKEROPEN);
	for (i = 0; i < 6; i++)
	{
		if (iMakers[i] >= 0) iTotalMake++;
	}
	if (iTotalMake == 0) return;

	TempBuf.Add((short)iTotalMake);

	for (i = 0; i < 6; i++)
	{
		if (iMakers[i] >= 0)
		{
            TempBuf.Add((short)iMakers[i]);
		}
	}
	Send(TempBuf, TempBuf.GetLength());
}

void USER::SendMsgBox(int iType, int nChatNum)
{
	CBufferEx TempBuf;
	int chatnum = -1;
	if (iType == 0)
	{
		TempBuf.Add(PKT_EVENTOKBOX);
	}
	else if (iType == 1)
	{
		TempBuf.Add(PKT_EVENTNORMAL);
	}

	chatnum = GetNpcChatIndex(nChatNum);
	if (chatnum == -1) return;

	TempBuf.Add((short)g_arNpcChat[chatnum]->m_strTalk.GetLength());
	TempBuf.AddData((char*)(LPCTSTR)g_arNpcChat[chatnum]->m_strTalk, g_arNpcChat[chatnum]->m_strTalk.GetLength());
	Send(TempBuf, TempBuf.GetLength());
}

void USER::SendOpenSpecialMake(int iMake)
{
	if (iMake == -1) return;
	CBufferEx TempBuf;

	TempBuf.Add(PKT_SKILL_ABILITY);

	TempBuf.Add((BYTE)0); // Open
	TempBuf.Add((BYTE)iMake);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::RepairItemOpenReq()
{
	CBufferEx TempBuf;
	TempBuf.Add(PKT_REPAIR_OPEN);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::RobMoney(int money)
{
	if (money <= 0)return;

	DWORD dwBackup = m_dwBarr;
	DWORD dwRobMoney = money;
	DWORD dwTemp = m_dwBarr;

	if (m_dwBarr <= dwRobMoney)
	{
		m_dwBarr = 0;
	}
	else
	{
		m_dwBarr = dwTemp - dwRobMoney;
	}

	// TODO: Implement soma version of update function
	//if (!UpdateUserItemMoney())
	//{
	//	m_dwBarr = dwBackup;
	//	return;
	//}

	SendMoneyChanged();
}

//////////////////////////////////////////////////////////////////////
//
//	Searches through users inventory and robs num of item sNum
//
void USER::RobItem(int sNum, int num)
{
	// TODO : RobItem - Handling of multiple non stackable items etc...
	if (sNum <= 0) return;
	if (num <= 0) return; // Not robbing any items...
	int i;
	short sSlot = -1;
	int iWeight = 0;
	ItemList	TempItem;
	CItemTable* pItem = NULL;
	if (!g_mapItemTable.Lookup(sNum, pItem)) return; // Item doesn't exist

	// TODO : Add searching through users Belt too??

	for (i = EQUIP_ITEM_NUM; i < INV_ITEM_NUM; i++)
	{
		if (m_InvItem[i].sNum == sNum)
		{
			sSlot = i; break;
		}
	}

	if (sSlot == -1) return; // Item not in user inv
	if (m_InvItem[sSlot].sUsage < num) return; // User doesn't have enough of the required item to rob
	
	if (m_InvItem[sSlot].bType > TYPE_ACC) // Non equipment
	{
		// Calculate the weight of the items being taken away
		iWeight = num * m_InvItem[sSlot].sWgt;
		// Rob whatever items the user has...
		if ((m_InvItem[sSlot].sUsage - num) <= 0) // User will have non of the items left	
		{
			m_InvItem[sSlot].Init();
			SendDeleteItem(BASIC_INV, sSlot);
		}
		else
		{
			m_InvItem[sSlot].sUsage  -= num;
			SendItemInfoChange(BASIC_INV, sSlot, INFO_DUR);
		}
	}
	else // Equipment
	{
		iWeight = m_InvItem[sSlot].sWgt;
		m_InvItem[sSlot].Init(); // Just delete the equipment
		SendDeleteItem(BASIC_INV, sSlot);
	}
	
	m_sWgt -= iWeight;
	if (m_sWgt < 0) m_sWgt = 0;

	// GetRecoverySpeed(); // Users recovery speed might have changed due to weight loss 	
}

void USER::RobMagic(short sMid, BYTE byMagicType)
{
	// Validate the magic number that is going to be taken
	if (sMid <= 0) return;
	if (byMagicType == MAGIC_TYPE_MAGIC && sMid > g_arMagicTable.GetSize()) return;
	if (byMagicType == MAGIC_TYPE_SPECIAL && sMid > g_arSpecialAttackTable.GetSize()) return;
	if (byMagicType == MAGIC_TYPE_ABILITY && sMid > g_arMakeSkillTable.GetSize()) return;

	// Check there is magic to be taken
	if (m_nHaveMagicNum == 0) return; 

	// Find the magic and remove it
	for (int i = 0; i < MAX_MAGIC_NUM; i++)
	{
		if (m_UserMagic[i].sMid == sMid && m_UserMagic[i].byType == byMagicType) // Magic found
		{
			m_UserMagic[i].sMid = -1;
			m_UserMagic[i].byType = 1;
			m_nHaveMagicNum--;
			break;
		}
	}
}

void USER::SendNpcSay(CNpc *pNpc, int nChatNum)
{
	if (!pNpc) return;

	int chatnum = GetNpcChatIndex(nChatNum);
	if (chatnum == -1) return;

	int index = 0;
	SetByte(m_TempBuf, PKT_CHAT, index);
	SetByte(m_TempBuf, NORMAL_CHAT, index);
	SetInt(m_TempBuf, pNpc->m_sNid + NPC_BAND, index);
	SetShort(m_TempBuf, pNpc->m_sClass, index);
	SetByte(m_TempBuf, 0, index);
	SetVarString(m_TempBuf, (LPTSTR)(LPCTSTR)g_arNpcChat[chatnum]->m_strTalk, g_arNpcChat[chatnum]->m_strTalk.GetLength(), index);
	Send(m_TempBuf, index);
}

void USER::ShowMagic(short sPostMagic, short sPreMagic)
{
	CBufferEx TempBuf;
	TempBuf.Add(PKT_SHOW_MAGIC);
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add(sPostMagic);
	TempBuf.Add(sPreMagic);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::StorageOpen()
{
	if (m_bTrading) return;

	CBufferEx TempBuf;

	CByteArray arItemSlotList;
	for (int i = 0; i < STORAGE_ITEM_NUM; i++)
	{												// 현재 보관된 아이템만 보여주기위해 정렬한다. 
		if ((m_StorageItem[i].sUsage > 0)&&(m_StorageItem[i].sNum > 0))  // Change to sNum??
		{
			arItemSlotList.Add(i);
		}
	}

	TempBuf.Add(PKT_STORAGEOPEN);
	TempBuf.Add((DWORD)m_dwBank);
	TempBuf.Add((short)GetMaxStorageCount());
	TempBuf.Add((short)arItemSlotList.GetSize());

	for (int i = 0; i < arItemSlotList.GetSize(); i++)
	{
		TCHAR pData[150];
		short tempSlot = 0;
		tempSlot = arItemSlotList[i];
		TempBuf.Add(tempSlot);
		TempBuf.AddData(pData, GetSendItemData(m_StorageItem[tempSlot], pData, ITEM_TYPE_STORAGE));
	}
	Send(TempBuf, TempBuf.GetLength());
}

void USER::SendStoreOpen(int nStore, int nBuyRate, int nSellRate)
{
	if (m_bTrading) return;

	CStore* pStore = GetStore(nStore);
	if (pStore == NULL) return;

	if (nBuyRate < 0 || nBuyRate > STORE_BUY_RATE_CAP) nBuyRate = 100;
	if (nSellRate < 0 || nSellRate > STORE_SELL_RATE_CAP) nSellRate = 100;

	m_nStoreBuyRate = nBuyRate;
	m_nStoreSellRate = nSellRate;

	CBufferEx TempBuf;
	int i = 0;
	short sNum = 0;

	TempBuf.Add(PKT_SHOPOPEN);
	TempBuf.Add((short)nStore);
	TempBuf.Add((short)pStore->m_tStoreType);
	TempBuf.Add((short)nBuyRate);
	TempBuf.Add((short)nSellRate);
	TempBuf.Add((short)0); // Tax rate, for when a guild owns the guild town.
	TempBuf.Add((short)(pStore->m_arItems.GetSize()));
	int iSize;
	for (i = 0; i < pStore->m_arItems.GetSize(); i++)
	{
		TCHAR pData[150];
		sNum = (short)pStore->m_arItems[i]->sItemNo;
		iSize = GetStoreItemData(sNum, (short)pStore->m_arItems[i]->sQuantity, pData);
		if (iSize > 0)
		{
			TempBuf.AddData(pData, iSize);
		}
		else
		{
			TRACE("Shop Error: %d\n", nStore);
			return;
		}
	}
	Send(TempBuf, TempBuf.GetLength());
}

bool USER::FindEvent(int event_num)
{
	for (int i = 0; i < m_arEventNum.GetSize(); i++)
	{
		if (*(m_arEventNum[i]) == event_num)
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////
//	Finds and returns the items usercount in the users inventory
//
int USER::FindItem(int sNum)
{
	if (sNum <= 0) return 0;
	CItemTable* pItem = NULL;
	int i;
	short sSlot = -1;
	if (!g_mapItemTable.Lookup(sNum, pItem)) return 0; // Item doesn't exist

	for (i = EQUIP_ITEM_NUM; i < INV_ITEM_NUM; i++)
	{
		if (m_InvItem[i].sNum == sNum)
		{
			sSlot = i; break;
		}
	}

	if (sSlot == -1) return 0; // Item not in user inv

	if (m_InvItem[sSlot].sUsage <= 0) return 0;

	if (m_InvItem[sSlot].bType > TYPE_ACC) // Non equipment
	{
        return m_InvItem[sSlot].sUsage;
	}
	else
	{
		return 1;
	}
}

bool USER::CheckStatMinMax(int iStat, int min, int max)
{
	if (iStat >= min && iStat < max)
	{
		return true;
	}
	return false;
}

bool USER::CheckItemSlot(int say, LOGIC_ELSE* pLE)
{
	int sNum = 0;
	short sSlot = -1;
	ItemList TempItem;
	CItemTable* pItem = NULL;

	for (int i = 1; i < 10; i += 2)
	{
		sNum = pLE->m_LogicElseInt[i];

		if (!TempItem.InitFromItemTable(sNum))
		{
			continue;
		}

		sSlot = GetSameItem(TempItem, INVENTORY_SLOT);
		if (sSlot == -1)
		{
			sSlot = GetEmptySlot(INVENTORY_SLOT);
			if (sSlot == -1)
			{
				SendMsgBox(1, say);
				return false;
			}
		}
	}
	return true;	
}

bool USER::CheckMoneyMinMax(int min, int max)
{
	if (min < -1 || max < -1)return false;
	if (min == -1 && max == -1)return false;

	DWORD dwMin = 0;
	DWORD dwMax = 0;

	if (min == -1 && max > 0)
	{
		dwMax = max;

		if (m_dwBarr < dwMax)
		{
			return true;
		}
	}
	else if (min > 0 && max == -1)
	{
		dwMin = min;

		if (m_dwBarr >= dwMin)
		{
			return true;
		}
	}
	else if (min > 0 && max > 0)
	{
		dwMin = min;
		dwMax = max;

		if (m_dwBarr >= dwMin && m_dwBarr < dwMax)
		{
			return true;
		}
	}

	return false;
}

bool USER::CheckItemMinMax(int sNum, int min, int max)
{
	// Check if the count of 'sNum' item is within min and max.
	int iItemCount = 0;
	for (int i = EQUIP_ITEM_NUM; i < INV_ITEM_NUM; i++)
	{
		if (m_InvItem[i].sNum == sNum)
		{
			if (m_InvItem[i].bType > TYPE_ACC) // Non equipment
			{
				iItemCount += m_InvItem[i].sUsage;
			}
			else
			{
				iItemCount++;
			}
		}
	}
	if (iItemCount >= min && iItemCount < max) 
	{
		return true;
	}
	return false;
}

bool USER::CheckLevel(int min, int max)
{
	if (m_sLevel >= min && m_sLevel < max) return true;
	return false;
}

bool USER::CheckRandom(int rand)
{
	int rand_result = myrand(0, 100);
	if (rand_result <= rand) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////
//
//	Checks the weight of all the items to ensure the user can accomdate them.. and sends a message of "say" to the user
//  based on the Table NPCCHAT
//
bool USER::CheckItemWeight(int say, int iSid1, int iNum1, int iSid2, int iNum2, 
							int iSid3, int iNum3, int iSid4, int iNum4, int iSid5, int iNum5)
{
	int iWeight = 0;

	iWeight += GetItemWeight(iSid1, iNum1);
	iWeight += GetItemWeight(iSid2, iNum2);
	iWeight += GetItemWeight(iSid3, iNum3);
	iWeight += GetItemWeight(iSid4, iNum4);
	iWeight += GetItemWeight(iSid5, iNum5);

	if (GetMaxWgt() < m_sWgt + iWeight)
	{
		SendMsgBox(1, say);
		return false;
	}

	return true;
}

void USER::ChangeExtStat(int &iStat, int iAmount)
{
	if (iAmount == 0) return;
	CheckMaxValue(iStat, iAmount);	
	//GetRecoverySpeed(); // Recovery Speed could have changed as a result of stat gain..
	//SetUserToMagicUser(); // Update Magic Stats... as normal stats have changed
    SendCharData(INFO_EXTVALUE);
}

void USER::ChangeExtStat(short &sStat, int iAmount)
{
	if (iAmount == 0) return;
	CheckMaxValue(sStat, (short)iAmount);	
	//GetRecoverySpeed(); // Recovery Speed could have changed as a result of stat gain..
	//SetUserToMagicUser(); // Update Magic Stats... as normal stats have changed
    SendCharData(INFO_EXTVALUE);
}

bool USER::CheckGameTime(int iMonth, int iDay, int iHourStart, int iHourEnd)
{
	if (iHourStart == -1 || iHourEnd == -1) return false;
	if (iMonth != -1 && iMonth != g_pMainDlg->m_nMonth) return false;
	if (iDay != -1 && iDay != g_pMainDlg->m_nDay) return false;

	if (iHourStart < iHourEnd)
	{
		if (g_pMainDlg->m_nHour < iHourStart ||
			g_pMainDlg->m_nHour >= iHourEnd)
		{
			return false;
		}
		return true;
	}
	else if (iHourStart > iHourEnd)
	{
		if (g_pMainDlg->m_nHour > iHourStart ||
			g_pMainDlg->m_nHour <= iHourEnd)
		{
			return true;
		}
		return false;
	}
	else
	{
		if (g_pMainDlg->m_nHour != iHourStart)
		{
			return false;
		}
		return true;
	}
}

bool USER::CheckFirstClick()
{
	CNpc *pNpc = GetNpc(m_iEventNpcId);
	if (!pNpc) return false;

	if (InterlockedCompareExchangePointer((PVOID*)&pNpc->m_bFirstClick, (PVOID)true, (PVOID)false) == (PVOID)false)
	{
		return true;
	}

	return false;
}

bool USER::CheckClassTown()
{
	if (CheckDemon(m_sClass) == CheckDemon(g_iClassWar))
	{
		return true;
	}
	return false;
}

bool USER::CheckClassTownWar()
{
	return g_bClassWar;
}

void USER::OpenTrader(int* iTypes)
{
	int index = 0;
	short sItemCount = 0;
	for (int i = 0; i < g_arTraderExchangeTable.GetSize(); i++)
	{
		CTraderExchangeTable* pTraderExchange = g_arTraderExchangeTable[i];
		if (pTraderExchange)
		{
			for (int j = 0; j < 6; j++)
			{
				if (pTraderExchange->m_sType == iTypes[j])
				{
					int iSize = GetTraderItemData(pTraderExchange, (m_TempBuf + index));
					if (iSize <= 0)
					{
						TRACE("Trader Exchange Error: %d\n", pTraderExchange->m_sId);
						return;
					}
					index += iSize;
					++sItemCount;
				}			
			}
		}
	}

	CBufferEx TempBuf;
	TempBuf.Add(PKT_TRADEREXCHANGE);
	TempBuf.Add((BYTE)1);
	TempBuf.Add(sItemCount);
	TempBuf.AddData(m_TempBuf, index);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::GuildOpen()
{
	CBufferEx TempBuf;
	TempBuf.Add(PKT_GUILD_OPEN);

	// Guild Application for non guild members.
	if (m_sGroup == -1)
	{
		// Client needs to know what the level requirement and barr needed to create a guild.
		// I know this is stupid why its here but it is! 
		// (Maybe it can be altered later but its here to stay for now)
		TempBuf.Add((BYTE)0);
		TempBuf.Add((DWORD)GUILD_MAKE_BARR);
		if (!CheckDemon(m_sClass))
		{
			TempBuf.Add((short)GUILD_MAKE_LEVEL_H);
		}
		else
		{
			TempBuf.Add((short)GUILD_MAKE_LEVEL_D);	
		}

		// Has a guild application request been made?
		// Gets the name of the guild also if the
		// guild no longer is there the user is removed as requesting application.
		Guild* pGuild = GetGuild(m_sGroupReq);
		if (pGuild == NULL || pGuild->GetApplicationUser(m_strUserId) < 0)
		{
			TempBuf.Add((BYTE)0);	
			m_sGroupReq = -1;
		}
		else
		{
			TempBuf.AddString(pGuild->m_strGuildName);	
		}
		ReleaseGuild();
	}
	// Guild menus for guild members based on their ranking within the guild.
	else
	{
		BYTE byMenu = 1;
		Guild* pGuild = GetGuild(m_sGroup);
		int iUserIndex = pGuild->GetUser(m_strUserId);
		if (pGuild && iUserIndex >= 0)
		{
			if (pGuild->m_arMembers[iUserIndex].m_sGuildRank == Guild::GUILD_CHIEF ||
				(pGuild->m_arMembers[iUserIndex].m_sGuildRank >= Guild::GUILD_FIRST && 
				pGuild->m_arMembers[iUserIndex].m_sGuildRank < Guild::GUILD_SECOND))
			{
				byMenu = 2;
			}
		}
		TempBuf.Add(byMenu);

		ReleaseGuild();
	}

	Send(TempBuf, TempBuf.GetLength());
}

bool USER::CheckGuildRank(short sGuildRank)
{
	Guild* pGuild = GetGuild(m_sGroup);
	int iUserIndex = pGuild->GetUser(m_strUserId);
	if (pGuild == NULL || pGuild->GetUser(m_strUserId) < 0)
	{
		ReleaseGuild();
		return false;
	}

	short sUserGuildRank = pGuild->m_arMembers[iUserIndex].m_sGuildRank;
	ReleaseGuild();
	return sUserGuildRank == sGuildRank;
}

void USER::KillNpc()
{
	CNpc *pNpc = GetNpc(m_iEventNpcId);
	if (!pNpc) return;
	// TODO: KillNpc: Set who killed the npc to -1
	pNpc->SetDead(m_pCom, -1, true);
}

void USER::LiveNpc(short sMId, int iCount, short sX, short sY)
{
	int iLiveCount = 0;
	CPoint pt;
	if (sX != -1 && sY != -1)
	{
		pt = ConvertToServer(sX, sY);
		if (pt.x == -1 || pt.y == -1)
		{
			return;
		}
	}

	for (int i = 0; i < g_arNpcTypeNoLive.GetSize(); i++)
	{
		NPC_TYPE_NOLIVE *NoLive =  g_arNpcTypeNoLive[i];

		if (!NoLive) continue;
		if (NoLive->sMId != sMId) continue;

		CNpc *pNpc = GetNpc(NoLive->sNId);
		if (!pNpc) continue;
			
		if (pNpc->m_sZone != m_sZ) continue;
		if (pNpc->m_NpcState != NPC_LIVE && pNpc->m_NpcState != NPC_DEAD) continue;

		if (iCount == -1)
		{
			pNpc->m_Delay = 3000;
			pNpc->m_NpcState = NPC_LIVE;
		}
		else
		{
			if (sX != -1 && sY != -1)
			{
				pNpc->m_sRecallX = static_cast<short>(pt.x);
				pNpc->m_sRecallY = static_cast<short>(pt.y);
			}
			else
			{
				pNpc->m_sRecallX = m_sX;
				pNpc->m_sRecallY = m_sY;				
			}

			pNpc->m_Delay = 3000;
			pNpc->m_NpcState = NPC_RECALL;

			iLiveCount++;
			if (iLiveCount >= iCount)
			{
				break;
			}
		}
	}
}

bool USER::CheckAliveNpc(short sMId, int iCheck)
{
	bool bCheck = false;
	if (iCheck != 0) bCheck = true;

	bool bAlive = false;
	for (int i = 0; i < g_arNpcTypeNoLive.GetSize(); i++)
	{
		NPC_TYPE_NOLIVE *NoLive =  g_arNpcTypeNoLive[i];
		if (NoLive->sMId != sMId) continue;

		CNpc* pNpc = GetNpc(NoLive->sNId);
		if (!pNpc) continue;
		if (pNpc->m_NpcState == NPC_LIVE || pNpc->m_NpcState == NPC_DEAD) continue;

		bAlive = true;
		break;
	}

	for (int i = 0; i < g_arNpcTypeTime.GetSize(); i++)
	{
		NPC_TYPE_TIME* Time = g_arNpcTypeTime[i];
		if (!Time) continue;
		if (Time->sMId != sMId) continue;
		
		CNpc* pNpc = GetNpc(Time->sNId);
		if (!pNpc) continue;
		if (pNpc->m_NpcState == NPC_LIVE || pNpc->m_NpcState == NPC_DEAD) continue;

		bAlive = true;
		break;
	}

	return bAlive == bCheck ? true : false;
}

void USER::OpenHairShop(int* iPrices)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_HAIRSHOPOK, index);
	SetByte(m_TempBuf, 4, index);
	for (int i = 0; i < HAIR_SHOP_PRICE_COUNT; i++)
	{
		m_iHairShopPrices[i] = iPrices[i];
		SetInt(m_TempBuf, iPrices[i], index);
	}
	Send(m_TempBuf, index);
}

void USER:: ChangeMageType(int iMageType)
{
	if (iMageType == 0) return;
	if (m_sMageType / 100 == iMageType) return;
	m_sMageType = (iMageType * 100) + (m_sMageType % 100);
}

bool USER::CheckMageType(int iMageType, int iCheck)
{
	bool bCheck = false;
	if (iCheck != 0) bCheck = true;
	
	if (bCheck && (m_sMageType / 100) == iMageType)
	{
		return true;
	}
	else if (!bCheck && (m_sMageType / 100) != iMageType)
	{
		return true;
	}

	return false;
}

bool USER::CheckGuildTown(short sTownNum)
{
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_arGuildTownData[i] == NULL)
			continue;

		if (g_arGuildTownData[i]->sTownNum == sTownNum)
		{
			if (g_arGuildTownData[i]->sGuildNum != -1 && g_arGuildTownData[i]->sGuildNum == m_sGroup)
			{
				return true;
			}
		}
	}

	return false;
}

bool USER::CheckGuildTownAlly(short sTownNum)
{
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_arGuildTownData[i] == NULL)
			continue;

		if (g_arGuildTownData[i]->sTownNum == sTownNum && g_arGuildTownData[i]->sGuildNum != -1)
		{
			for (int j = 0; j < 3; j++)
			{
				if (g_arGuildTownData[i]->sAlly[j] != -1 && g_arGuildTownData[i]->sAlly[j] == m_sGroup)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool USER::CheckGuildTownWar(short sTownNum)
{
	if (sTownNum < 1 || sTownNum > g_arGuildTownData.GetSize())
		return false;

	return g_bGuildTownWar[sTownNum-1];
}

bool USER::CheckGuildTownLevel(short sTownNum, short sLevel)
{
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		GuildTownData* GuildTown = g_arGuildTownData[i];
		if (GuildTown == NULL)
			continue;

		if (GuildTown->sTownNum == sTownNum && GuildTown->sGuildNum != -1)
			return GuildTown->sLevel >= sLevel ? true : false;
	}

	return false;
}

void USER::OpenGuildTownStone(short sTownNum)
{
	if (m_sGroup == -1)
		return;

	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		return;
	}

	int iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		ReleaseGuild();
		return;
	}

	
	int iUserRank = pGuild->m_arMembers[iUserIndex].m_sGuildRank;
	ReleaseGuild();

	BYTE byMenu = GUILD_STONE_MENU_MEMBER;
	if (iUserRank == Guild::GUILD_CHIEF)
	{
		byMenu = GUILD_STONE_MENU_CHIEF;
	}
	else if (iUserRank >= Guild::GUILD_FIRST && iUserRank < Guild::GUILD_SECOND)
	{
		byMenu = GUILD_STONE_MENU_FIRST;
	}
	else if (iUserRank >= Guild::GUILD_SECOND && iUserRank <= Guild::GUILD_RANKS)
	{
		byMenu = GUILD_STONE_MENU_SECOND;
	}

	int index = 0;
	SetByte(m_TempBuf, PKT_GUILDTOWNSTONE_OPEN, index);
	SetShort(m_TempBuf, sTownNum, index);
	SetByte(m_TempBuf, byMenu, index);
	Send(m_TempBuf, index);
}
