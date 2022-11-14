// UserChat.cpp: implementation of the chat handling part of USER class.
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
extern NpcThreadArray g_arNpcThread;

void USER::SendServerChatMessage(TCHAR* pMsg, BYTE towho)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_CHAT, index);
	SetByte(m_TempBuf, SYSTEM_CHAT, index);
	SetVarString(m_TempBuf, pMsg, _tcslen(pMsg), index);
	switch (towho)
	{
	case TO_ALL:
		SendAll(m_TempBuf, index);
		break;
	case TO_ME:
		Send(m_TempBuf, index);
		break;
	case TO_ZONE:
		SendZone(m_TempBuf, index);
		break;
	case TO_INSIGHT:
	default:
		SendInsight(m_TempBuf, index);
		break;
	}
}

void USER::SendServerChatMessage(UINT strID, BYTE towho)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_CHAT, index);
	SetByte(m_TempBuf, SYSTEM_CHAT, index);
	char pMsg[1280];	memset(pMsg, NULL, 1280);
	strcpy(pMsg, _ID(strID));
	SetVarString(m_TempBuf, pMsg, _tcslen(pMsg), index);
	switch (towho)
	{
	case TO_ALL:
		SendAll(m_TempBuf, index);
		break;
	case TO_ME:
		Send(m_TempBuf, index);
		break;
	case TO_ZONE:
		SendZone(m_TempBuf, index);
		break;
	case TO_INSIGHT:
	default:
		SendInsight(m_TempBuf, index);
		break;
	}
}

void USER::NormalChat(TCHAR *pBuf)
{
	CBufferEx TempBuf;
	TempBuf.Add(PKT_CHAT);
	TempBuf.Add(NORMAL_CHAT);
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add(m_sClass);
	TempBuf.AddString(m_strUserId);
	TempBuf.AddString(pBuf);
	SendInsight(TempBuf, TempBuf.GetLength());
}

void USER::ShoutChat(TCHAR *pBuf)
{
	// Can use shout chat befor level 20 if single craft skill is >= 40 or total craft skill is >= 70
	// CheckMakeSkill (70, 40) (TotalCraftSkill, SingleCraftSkill)

	if (m_sLevel < SHOUT_CHAT_LEVEL)
	{
		SendServerChatMessage(IDS_USER_TOO_LOW_LEVEL_FOR_SHOUT, TO_ME);
		return;
	}

	short sStm = GetStm();
	if (sStm < SHOUT_CHAT_STM)
	{
		SendServerChatMessage(IDS_USER_NOT_ENOUGH_STAMINA_FOR_SHOUT, TO_ME);
		return;
	}

	DecStm(SHOUT_CHAT_STM);

	SendCharData(INFO_EXTVALUE);

	CBufferEx TempBuf;
	TempBuf.Add(PKT_CHAT);
	TempBuf.Add(SHOUT_CHAT);
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add(m_sClass);
	TempBuf.AddString(m_strUserId);
	TempBuf.AddString(pBuf);

	int min_x = m_sX - (SIGHT_SIZE_X * 2);
	int max_x = m_sX + (SIGHT_SIZE_X * 2);
	int min_y = m_sY - (SIGHT_SIZE_Y * 2);
	int max_y = m_sY + (SIGHT_SIZE_Y * 2);

	Send(TempBuf, TempBuf.GetLength());
	//SendToRange(TempBuf, TempBuf.GetLength(), min_x, min_y, max_x, max_y);
}

void USER::ZoneChat(TCHAR *pBuf)
{
	// Cannot use this chat if refusing it
	if (!m_bAllChatRecv)
	{
		SendServerChatMessage(IDS_ZONESHOUT_REFUSE, TO_ME);
		return;
	}

	// Can use shout chat before level 30 if single craft skill is >= 60 or total craft skill is >= 120
	// CheckMakeSkill (120, 60) (TotalCraftSkill, SingleCraftSkill)
	// need to add a check for the shout scroll here...
	// check for single shout scroll and normal shout scroll
	// delete the single shout scroll send item change packet to client
	// also send change weight etc...

	// Search for a shout scroll item
	short sShoutScrollSlot = -1;
	for (int i = EQUIP_ITEM_NUM; i < INV_ITEM_NUM; i++)
	{
		if ((m_InvItem[i].sSpecial == SPECIAL_SHOUT ||
			m_InvItem[i].sSpecial  == SPECIAL_SINGLE_SHOUT) &&
			CheckDemon(m_sClass) == m_InvItem[i].IsDemonItem())
		{
			sShoutScrollSlot = i;
			break;
		}
	}

	// Did we find a scroll?
	if (sShoutScrollSlot == -1)
	{
		SendServerChatMessage(IDS_USER_NEED_SCROLL_FOR_SHOUT, TO_ME);
		return;
	}

	// Shout scroll chat color
	int r = m_InvItem[sShoutScrollSlot].sSpOpt[0];
	int g = m_InvItem[sShoutScrollSlot].sSpOpt[1];
	int b = m_InvItem[sShoutScrollSlot].sSpOpt[2];

	// Handle single shout scroll items
	// they can be used without level 30 requirement and stamina
	if (m_InvItem[sShoutScrollSlot].sSpecial == SPECIAL_SINGLE_SHOUT)
	{
		PlusItemDur(&m_InvItem[sShoutScrollSlot], 1, true);
		SendItemInfoChange(BASIC_INV, sShoutScrollSlot, INFO_DUR);
		if (m_InvItem[sShoutScrollSlot].sUsage <= 0)
		{
			m_InvItem[sShoutScrollSlot].Init();
			SendDeleteItem(BASIC_INV, sShoutScrollSlot);
			SendItemInfo(sShoutScrollSlot);
			SendChangeWgt();
		}
	}
	else
	{
		if (m_sLevel < ZONE_CHAT_LEVEL)
		{
			SendServerChatMessage(IDS_USER_TOO_LOW_LEVEL_FOR_SHOUT, TO_ME);
			return;
		}

		short sStm = GetStm();
		short sMaxStm = GetMaxStm();
		if (sStm != sMaxStm ||
			sStm < ZONE_CHAT_STM)
		{
			SendServerChatMessage(IDS_USER_NOT_ENOUGH_STAMINA_FOR_SHOUT, TO_ME);
			return;
		}

		DecStm(ZONE_CHAT_STM);

		SendCharData(INFO_EXTVALUE);
	}

	CBufferEx TempBuf;
	TempBuf.Add(PKT_CHAT);
	TempBuf.Add(ZONE_CHAT);
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add(m_sClass);
	TempBuf.Add((BYTE)r);
	TempBuf.Add((BYTE)g);
	TempBuf.Add((BYTE)b);
	TempBuf.AddString(m_strUserId);
	TempBuf.AddString(pBuf);

	if (m_sLevel < ALL_CHAT_LEVEL)
	{
		SendZone(TempBuf, TempBuf.GetLength());
	}
	else
	{
		SendAll(TempBuf, TempBuf.GetLength());
	}
}

void USER::WhisperChat(TCHAR *pBuf, bool bFirst /*= false*/)
{
	CBufferEx TempMyBuf;
	CBufferEx TempYouBuf;
	BYTE result = FAIL, error_code = 0;
	int index = 0;

	if (!m_bPrivMsgRecv)
	{
		error_code = ERR_1;
		goto result_send;
	}

	char strUserName[NAME_LENGTH+1] = {0};
	int nLength;
	if (bFirst)
	{
		nLength = ParseSpaceInUser(strUserName, pBuf, NAME_LENGTH);
	}
	else
	{
		nLength = GetVarString(sizeof(strUserName), strUserName, pBuf, index);
	}
	if (nLength <= 0 || nLength > NAME_LENGTH)
	{
		error_code = ERR_2;
		goto result_send;
	}

	USER* pUser = GetUserId(strUserName);
	if (!pUser)
	{
		error_code = ERR_2;
		goto result_send;
		return;
	}

	if (pUser->m_State != STATE_GAMESTARTED)
	{
		error_code = ERR_2;
		goto result_send;
	}

	if (!pUser->m_bPrivMsgRecv)
	{
		error_code = ERR_3;
		goto result_send;
	}

	if (m_strUserId == pUser->m_strUserId)
	{
		error_code = ERR_4;
		goto result_send;
	}

	for (int i = 0; i < 5; i++)
	{
		if (_stricmp(m_strDenyMsgUserId[i], pUser->m_strUserId) == 0)
		{
			error_code = ERR_1;
			goto result_send;
		}
	}

	for (int i = 0; i < 5; i++)
	{
		if (_stricmp(pUser->m_strDenyMsgUserId[i], m_strUserId) == 0)
		{
			error_code = ERR_3;
			goto result_send;
		}
	}

	result = SUCCESS;

result_send:
	if (result != SUCCESS)
	{
		switch (error_code)
		{
		case ERR_1:
			SendServerChatMessage(IDS_USER_REJECT_WHISPER, TO_ME);
			break;
		case ERR_2:
			SendServerChatMessage(IDS_PM_USER_NOT_CONNECTED_NOW, TO_ME);
			break;
		case ERR_3:
			SendServerChatMessage(IDS_OTHER_USER_REJECT_WHISPER, TO_ME);
			break;
		case ERR_4:
			SendServerChatMessage(IDS_CANNOT_PM_SELF, TO_ME);
			break;
		}
		return;
	}

	TempMyBuf.Add(PKT_CHAT);
	TempMyBuf.Add(WHISPER_ME_CHAT);
	TempMyBuf.Add(pUser->m_sClass);
	if (IsServerRank(SERVER_RANK_ALL) || pUser->IsServerRank(SERVER_RANK_ALL))
	{
		TempMyBuf.Add((BYTE)1); // 1 if user is GM (sRank = 1)
	}
	else
	{
		TempMyBuf.Add((BYTE)0); // 1 if user is GM (sRank = 1)
	}
	TempMyBuf.AddString(pUser->m_strUserId);
	TempMyBuf.AddString(pBuf + nLength);
	Send(TempMyBuf, TempMyBuf.GetLength());

	TempYouBuf.Add(PKT_CHAT);
	TempYouBuf.Add(WHISPER_CHAT);
	TempYouBuf.Add(m_sClass);
	if (IsServerRank(SERVER_RANK_ALL) || pUser->IsServerRank(SERVER_RANK_ALL))
	{
		TempYouBuf.Add((BYTE)1); // 1 if user is GM (sRank = 1)
	}
	else
	{
		TempYouBuf.Add((BYTE)0); // 1 if user is GM (sRank = 1)
	}
	TempYouBuf.AddString(m_strUserId);
	TempYouBuf.AddString(pBuf + nLength);
	pUser->Send(TempYouBuf, TempYouBuf.GetLength());
}

void USER::GuildChat(TCHAR *pBuf, short sGroup, bool bSystem)
{
	if (sGroup < 0) return;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_CHAT);
	TempBuf.Add(GUILD_CHAT);
	TempBuf.Add(m_Uid + USER_BAND);
	if (bSystem)
		TempBuf.Add((BYTE)0);
	else
		TempBuf.AddString(m_strUserId);
	TempBuf.AddString(pBuf);

	// FIXME This could be improved!!!!!
	for (int i = 0; i< MAX_USER; i++)
	{
		USER *pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) continue;
		if (sGroup == pUser->m_sGroup) pUser->Send(TempBuf, TempBuf.GetLength());
	}
}

void USER::PartyChat(TCHAR *pBuf)
{
	if (!m_bInParty)
	{
		//SendServerChatMessage(IDS_USER_MAKE_PARTY_FIRST, TO_ME);
		return;
	}

	CBufferEx TempBuf;
	TempBuf.Add(PKT_CHAT);
	TempBuf.Add(PARTY_CHAT);
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.AddString(m_strUserId);
	TempBuf.AddString(pBuf);

	// Get the leader of the party
	USER *pLeader = GetUser(m_PartyMembers[0].uid);
	if (!pLeader) return;

	for (int i = 0; i < MAX_PARTY_USER_NUM; i++)
	{
		if (pLeader->m_PartyMembers[i].uid != -1)
		{
			USER *pMember = g_pUserList->GetUserUid(pLeader->m_PartyMembers[i].uid);
			if (!pMember) continue;
			if (pMember->m_State != STATE_GAMESTARTED) continue;
			// Below check is done incase the uid has been reused for another user
			// and for some reason user is still in party list.
			if (strcmp(pMember->m_strUserId, pLeader->m_PartyMembers[i].m_strUserId) != 0) continue;
			pMember->Send(TempBuf, TempBuf.GetLength());
		}
	}
}

void USER::CommandChat(TCHAR *pBuf)
{
	char fn[128] = {};
	int index = 0;

	index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));
	fn[127] = '\0';

	CString fn_str;
	fn_str = fn;
	if (fn_str.CompareNoCase("move") == 0)
	{
		if (!IsServerRank(SERVER_RANK_MOVEMENT)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL2)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL3)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL4)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int x = atoi(fn);
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int y = atoi(fn);

		LinkToSameZone(x, y);
	}
	else if (fn_str.CompareNoCase("zmove") == 0)
	{
		if (!IsServerRank(SERVER_RANK_MOVEMENT)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL2)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL3)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL4)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int z = atoi(fn);
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int x = atoi(fn);
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int y = atoi(fn);
		LinkToOtherZone(z, x, y);
	}
	else if (fn_str.CompareNoCase("party")== 0)
	{
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));
		PartyCreate(fn);
	}
	else if (fn_str.CompareNoCase("add") == 0)
	{
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));
		PartyInvite(fn);
	}
	else if (fn_str.CompareNoCase("war") == 0) // Guild War
	{
		if (m_sGroup == -1) return;
		if (m_bGuildWar) return;
		if (m_bInGuildWar)
		{
			SendServerChatMessage(IDS_ALREADY_IN_GUILD_WAR, TO_ME);
			return;
		}

		for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
		{
			if (g_bGuildTownWar[i])
			{
				SendServerChatMessage(IDS_GUILD_WAR_GUILD_TOWN_WAR, TO_ME);
				return;
			}
		}

		Guild* pGuild = GetGuild(m_sGroup);
		if (pGuild == NULL)
		{
			ReleaseGuild();
			return;
		}

		// Must be guild leader to start a war with another guild
		int i = pGuild->GetUser(m_strUserId);
		if (i < 0)
		{
			ReleaseGuild();
			return;
		}

		if (pGuild->m_arMembers[i].m_sGuildRank != Guild::GUILD_CHIEF)
		{
			SendServerChatMessage(IDS_GUILD_WAR_ONLY_LEADER, TO_ME);
			ReleaseGuild();
			return;
		}

		// An amount of money is needed to be in the guild to start a war
		if (pGuild->m_dwBarr < GUILD_WAR_BARR)
		{
			SendServerChatMessage(IDS_GUILD_WAR_NOT_ENOUGH_BARR, TO_ME);
			ReleaseGuild();
			return;
		}

		ReleaseGuild();

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));
		Guild* pGuildTarget = GetGuildByName(fn);
		if (pGuildTarget == NULL)
		{
			CString str;
			str.Format(IDS_GUILD_WAR_CANNOT_FIND_GUILD, fn);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			ReleaseGuild();
			return;
		}

		// TODO: check blocking guild war request

		// TODO: Check guild going to war with has got 100k in guild stash

		i = pGuildTarget->GetUserByRank(Guild::GUILD_CHIEF);
		if (i < 0)
		{
			ReleaseGuild();
			return;
		}

		USER* pTarget = GetUserId(pGuildTarget->m_arMembers[i].m_strUserId);
		if (pTarget == NULL || pTarget->m_State != STATE_GAMESTARTED)
		{
			CString str;
			str.Format(IDS_GUILD_WAR_CANNOT_FIND_GUILD_LEADER, pGuildTarget->m_arMembers[i].m_strUserId);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			ReleaseGuild();
			return;
		}

		// Check not trying to war a guild in another race
		if (CheckDemon(m_sClass) != CheckDemon(pTarget->m_sClass))
		{
			SendServerChatMessage(IDS_GUILD_WAR_CANNOT_WAR_OTHER_RACE, TO_ME);
			ReleaseGuild();
			return;
		}

		// Check the leader of guild going to war isn't already waging another
		if (pTarget->m_bGuildWar)
		{
			SendServerChatMessage(IDS_GUILD_WAR_OTHER_GUILD_IN_WAGER, TO_ME);
			ReleaseGuild();
			return;
		}

		// Check for challenging of own guild
		if (m_sGroup == pTarget->m_sGroup)
		{
			SendServerChatMessage(IDS_GUILD_WAR_CANNOT_WAR_OWN_GUILD, TO_ME);
			ReleaseGuild();
			return;
		}

		m_bGuildWar = true;
		pTarget->m_bGuildWar = true;

		ReleaseGuild();

		int index = 0;
		SetByte(m_TempBuf, PKT_GUILDWAR_REQ, index);
		SetVarString(m_TempBuf, m_strGuildName, strlen(m_strGuildName), index);
		SetVarString(m_TempBuf, pTarget->m_strGuildName, strlen(pTarget->m_strGuildName), index);
		Send(m_TempBuf, index);
		pTarget->Send(m_TempBuf, index);
	}
	else if (fn_str.CompareNoCase("surrender") == 0) // Guild War
	{
		if (m_sGroup == -1) return;
		if (!m_bInGuildWar) return;

		Guild* pGuild = GetGuild(m_sGroup);
		if (pGuild == NULL)
		{
			ReleaseGuild();
			return;
		}

		// Must be guild leader to surrender a war with another guild
		int i = pGuild->GetUser(m_strUserId);
		if (i < 0)
		{
			ReleaseGuild();
			return;
		}

		if (pGuild->m_arMembers[i].m_sGuildRank != Guild::GUILD_CHIEF)
		{
			ReleaseGuild();
			return;
		}

		ReleaseGuild();
		GuildWarEnd();
	}
	else if (fn_str.CompareNoCase("nowar") == 0)
	{
		return;

		if (m_sGroup == -1) return;

		Guild* pGuild = GetGuild(m_sGroup);
		if (pGuild == NULL)
		{
			ReleaseGuild();
			return;
		}

		// Must be guild leader to block war requests with other guilds
		int i = pGuild->GetUser(m_strUserId);
		if (i < 0)
		{
			ReleaseGuild();
			return;
		}

		if (pGuild->m_arMembers[i].m_sGuildRank != Guild::GUILD_CHIEF)
		{
			SendServerChatMessage(IDS_GUILD_WAR_ONLY_LEADER, TO_ME);
			ReleaseGuild();
			return;
		}

		ReleaseGuild();
	}
	else if (fn_str.CompareNoCase("townstart") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index))); int iTown = atoi(fn);
		if (iTown >= 1 && iTown <= g_arGuildTownData.GetSize())
		{
			g_bGuildTownWar[iTown-1] = true;
			g_bGuildTownWarStart[iTown-1] = true;
		}
	}
	else if (fn_str.CompareNoCase("townend") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));	int iTown = atoi(fn);
		if (iTown >= 1 && iTown <= g_arGuildTownData.GetSize())
		{
			g_bGuildTownWar[iTown-1] = true;
			g_bGuildTownWarEnd[iTown-1] = true;
		}
	}
	else if (fn_str.CompareNoCase("classstart") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;
		g_bClassWar = true;
		g_bClassWarStart = true;
	}
	else if (fn_str.CompareNoCase("classend") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;
		g_bClassWar = true;
		g_bClassWarEnd = true;
	}
	else if (fn_str.CompareNoCase("notice") == 0)
	{
		if (!IsServerRank(SERVER_RANK_OPERATING)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL2)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL3)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL4)) return;

		CString strNotice = (pBuf+index);
		strNotice.Trim();

		int index = 0;
		SetByte(m_TempBuf, PKT_CHAT, index);
		SetByte(m_TempBuf, GM_NOTICE_CHAT, index);
		SetVarString(m_TempBuf, (LPTSTR)(LPCTSTR)strNotice, strNotice.GetLength(), index);
		SendAll(m_TempBuf, index);
	}
	else if (fn_str.CompareNoCase("notify") == 0)
	{
		//if (!IsServerRank(SERVER_RANK_OPERATING)) return;
		//else if (!IsServerRank(SERVER_RANK_LEVEL2)) return;

		//index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));
		//CString strNotice = "";
		//strNotice.Format("%s", fn);
		//int index = 0;
		//SetByte(m_TempBuf, PKT_CHAT, index);
		//SetByte(m_TempBuf, GM_SCROLL_NOTICE_CHAT, index);
		//SetVarString(m_TempBuf, (LPTSTR)(LPCTSTR)strNotice, strNotice.GetLength(), index);
		//Send(m_TempBuf, index);
	}
	else if (fn_str.CompareNoCase("hidden") == 0) // Hides the GM
	{
		if (!IsServerRank(SERVER_RANK_OPERATING)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL2)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL3)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL4)) return;

		SendMyInfo(TO_INSIGHT, INFO_DELETE);
		m_bHidden = true;
		SendServerChatMessage(IDS_USER_HIDE, TO_ME);
	}
	else if (fn_str.CompareNoCase("appear") == 0) // Reveals the GM
	{
		if (!IsServerRank(SERVER_RANK_OPERATING)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL2)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL3)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL4)) return;

		m_bHidden = false;
		SendMyInfo(TO_INSIGHT, INFO_MODIFY);
		SendServerChatMessage(IDS_USER_APPEAR, TO_ME);
	}
	else if (fn_str.CompareNoCase("capguild") == 0) // Sends all conversations for this guild to the GM
	{
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int iGuildNum = atoi(fn);
	}
	else if (fn_str.CompareNoCase("allnocapture") == 0) // Turns off all capturing
	{
	}
	else if (fn_str.CompareNoCase("nocapguild") == 0) // Turns off guild capturing for x guild
	{
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int iGuildNum = atoi(fn);
	}
	else if (fn_str.CompareNoCase("usercount") == 0)
	{
		ShowCurrentUser();
	}
	else if (fn_str.CompareNoCase("hp") == 0)
	{
		if (m_sRank == 0) return;
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));	int iHpPlus = atoi(fn);
		CheckMaxValue(m_sHP, iHpPlus);
		if (m_sHP <= 0) m_sHP = 1;
		if (m_sHP > GetMaxHP()) m_sHP = GetMaxHP();
		SendHPMP();
	}
	else if (fn_str.CompareNoCase("mp") == 0)
	{
		if (m_sRank == 0) return;
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));	int iMpPlus = atoi(fn);
		CheckMaxValue(m_sMP, iMpPlus);
		if (m_sMP <= 0) m_sMP = 1;
		if (m_sMP > GetMaxMP()) m_sMP = GetMaxMP();
		SendHPMP();
	}
	else if (fn_str.CompareNoCase("kick") == 0)
	{
		if (!IsServerRank(SERVER_RANK_OPERATING)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));

		int nLength = strlen(fn);
		if (nLength <= 0 || nLength > NAME_LENGTH) return;

		USER* pUser = GetUserId(fn);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) return;

		if (strcmp(pUser->m_strUserId, m_strUserId) == 0) return;	// cant kick yourself

		// pUser->LogOut(NULL);
		pUser->Close(); // Kick the user..

		CString strMessage;
		strMessage.Format(IDS_GM_KICK_USER, fn);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
	}
	else if (fn_str.CompareNoCase("recall") == 0)
	{
		if (!IsServerRank(SERVER_RANK_MOVEMENT)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL2)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL3)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));

		int nLength = strlen(fn);
		if (nLength <= 0 || nLength > NAME_LENGTH) return;

		USER* pUser = GetUserId(fn);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) return;

		if (strcmp(pUser->m_strUserId, m_strUserId) == 0) return;	// cant recall yourself

		CPoint pt = ConvertToClient(m_sX, m_sY);
		if (pt.x == -1 || pt.y == -1) return;

		if (pUser->m_sZ == m_sZ)
		{
			pUser->LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
		else
		{
			pUser->LinkToOtherZone(m_sZ, static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
	}
	else if (fn_str.CompareNoCase("trace") == 0)
	{
		if (!IsServerRank(SERVER_RANK_MOVEMENT)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL2)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL3)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL4)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));

		int nLength = strlen(fn);
		if (nLength <= 0 || nLength > NAME_LENGTH) return;

		USER* pUser = GetUserId(fn);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) return;
		if (strcmp(pUser->m_strUserId, m_strUserId) == 0) return;	// cant trace yourself

		CPoint pt = pUser->ConvertToClient(pUser->m_sX, pUser->m_sY);
		if (pt.x == -1 || pt.y == -1) return;

		if (pUser->m_sZ == m_sZ)
		{
			LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
		else
		{
			LinkToOtherZone(pUser->m_sZ, static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
	}
	else if (fn_str.CompareNoCase("revivemon") == 0) // Revive x amount of NPC's
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));	int iNumRevive = atoi(fn);
		CString strNpcName = (pBuf+index);
		strNpcName.Trim();

		int nLength = strNpcName.GetLength();
		if (nLength <= 0 || nLength > 50) return;

		CNpc* pNpc = NULL;
		bool bFinished = false;
		for (int i = 0; i < g_arNpcThread.GetSize(); i++)
		{
			if (bFinished) break;

			for (int j  = 0; j < NPC_NUM; j++)
			{
				pNpc = g_arNpcThread[i]->m_pNpc[j];
				if (!pNpc) continue;
				if (pNpc->m_sZone != m_sZ) continue;
				if (pNpc->m_NpcState != NPC_LIVE && pNpc->m_NpcState != NPC_DEAD) continue;

				if (strNpcName.CompareNoCase(pNpc->m_strName) == 0)
				{
					pNpc->m_Delay = 750;
					if (pNpc->m_NpcState == NPC_DEAD) pNpc->m_NpcState = NPC_LIVE;

					--iNumRevive;
					if (iNumRevive < 1)
					{
						bFinished = true;
						break;
					}
				}
			}
		}
	}
	else if (fn_str.CompareNoCase("recallmon") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));	int iNumRecall = atoi(fn);
		CString strNpcName = (pBuf+index);
		strNpcName.Trim();

		int nLength = strNpcName.GetLength();
		if (nLength <= 0 || nLength > 50) return;

		CNpc* pNpc = NULL;
		for (int i = m_iRecallNPCThreadOffset; i < g_arNpcThread.GetSize(); i++)
		{
			for (int j  = m_iRecallNPCThreadNPCOffset; j < NPC_NUM; j++)
			{
				pNpc = g_arNpcThread[i]->m_pNpc[j];
				if (!pNpc) continue;
				if (pNpc->m_sZone != m_sZ) continue;
				if (pNpc->m_NpcState == NPC_LIVE || pNpc->m_NpcState == NPC_DEAD) continue;

				if (strNpcName.CompareNoCase(pNpc->m_strName) == 0)
				{
					pNpc->m_Delay = 750;

					CPoint pt = pNpc->FindNearAvailablePoint_S(m_sX, m_sY, 5);
					if (pt.x == -1 || pt.y == -1) continue;

					pNpc->m_sMinX = pt.x - 10;
					pNpc->m_sMinY = pt.y - 10;
					pNpc->m_sMaxX = pt.x + 10;
					pNpc->m_sMaxY = pt.y + 10;
					pNpc->MoveNpc(static_cast<short>(pt.x), static_cast<short>(pt.y), m_pCom);

					--iNumRecall;
					if (iNumRecall < 1)
					{
						m_iRecallNPCThreadOffset = i;
						m_iRecallNPCThreadNPCOffset = j + 1;
						return;
					}
				}
			}
		}

		m_iRecallNPCThreadOffset = 0;
		m_iRecallNPCThreadNPCOffset = 0;
	}
	else if (fn_str.CompareNoCase("monster") == 0)
	{
		if (!IsServerRank(SERVER_RANK_MOVEMENT)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL2)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL3)) return;

		CString strNpcName = (pBuf+index);
		strNpcName.Trim();

		int nLength = strNpcName.GetLength();
		if (nLength <= 0 || nLength > 50) return;

		CNpc* pNpc = NULL;
		bool bFound = false;
		for (int i = 0; i < g_arNpcThread.GetSize(); i++)
		{
			if (bFound) break;

			for (int j  = 0; j < NPC_NUM; j++)
			{
				pNpc = g_arNpcThread[i]->m_pNpc[j];
				if (!pNpc) continue;
				if (pNpc->m_NpcState == NPC_LIVE || pNpc->m_NpcState == NPC_DEAD) continue;

				if (strNpcName.CompareNoCase(pNpc->m_strName) == 0)
				{
					bFound = true;
					break;
				}
			}
		}

		if (!bFound) return;

		CPoint pt = pNpc->ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);
		if (pt.x == -1 || pt.y == -1) return;

		if (pNpc->m_sCurZ == m_sZ)
		{
			LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
		else
		{
			LinkToOtherZone(pNpc->m_sCurZ, static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
	}
	else if (fn_str.CompareNoCase("zmonster") == 0)
	{
		if (!IsServerRank(SERVER_RANK_MOVEMENT)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL2)) return;
		else if (!IsServerRank(SERVER_RANK_LEVEL3)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));	short sZone = atoi(fn);
		CString strNpcName = (pBuf+index);
		strNpcName.Trim();

		int nLength = strNpcName.GetLength();
		if (nLength <= 0 || nLength > 50) return;

		CNpc* pNpc = NULL;
		bool bFound = false;
		for (int i = 0; i < g_arNpcThread.GetSize(); i++)
		{
			if (bFound) break;

			for (int j  = 0; j < NPC_NUM; j++)
			{
				pNpc = g_arNpcThread[i]->m_pNpc[j];
				if (!pNpc) continue;
				if (pNpc->m_NpcState == NPC_LIVE || pNpc->m_NpcState == NPC_DEAD) continue;
				if (pNpc->m_sCurZ != sZone) continue;

				if (strNpcName.CompareNoCase(pNpc->m_strName) == 0)
				{
					bFound = true;
					break;
				}
			}
		}

		if (!bFound) return;

		CPoint pt = pNpc->ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);
		if (pt.x == -1 || pt.y == -1) return;

		if (pNpc->m_sCurZ == m_sZ)
		{
			LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
		else
		{
			LinkToOtherZone(pNpc->m_sCurZ, static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
	}
	else if (fn_str.CompareNoCase("save") == 0)
	{
		// TODO: Are we having a /save command?
		// UpdateUserData();
	}
	else if (fn_str.CompareNoCase("bug") == 0)
	{
		BBSOpen(1);
	}
	else if (fn_str.CompareNoCase("pos") == 0)
	{
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));

		int nLength = strlen(fn);
		if (nLength <= 0 || nLength > NAME_LENGTH) return;

		USER* pUser = GetUserId(fn);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED)
		{
			CString strMessage = _T("");
			strMessage.Format(IDS_USER_NOT_CONNECTED_NOW, fn);
			SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
			return;
		}

		CString strMessage = _T("");
		strMessage.Format(IDS_USER_HAS_POS, pUser->m_strUserId, pUser->m_iClassPoint);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
	}
	else if (fn_str.CompareNoCase("shoutrefuse") == 0)
	{
		if (m_bAllChatRecv)
		{
			m_bAllChatRecv = false;
			SendServerChatMessage(IDS_ZONESHOUT_REFUSE, TO_ME);
		}
		else
		{
			m_bAllChatRecv = true;
			SendServerChatMessage(IDS_ZONESHOUT_ALLOW, TO_ME);
		}
	}
	else if (fn_str.CompareNoCase("refusenote") == 0)
	{
		if (m_bPrivMsgRecv)
		{
			m_bPrivMsgRecv = false;
			SendServerChatMessage(IDS_BLOCK_PM, TO_ME);
		}
		else
		{
			m_bPrivMsgRecv = true;
			SendServerChatMessage(IDS_ALLOW_PM, TO_ME);
		}
	}
	else if (fn_str.CompareNoCase("givemagic") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		short sMagicNo = atoi(fn);
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		BYTE byMagicType = atoi(fn);

		GiveMagic(sMagicNo, byMagicType);
	}
	else if (fn_str.CompareNoCase("giveitem") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int iNum = atoi(fn);
		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int iCount = atoi(fn);

		GiveItem(iNum, iCount);
	}
	else if (fn_str.CompareNoCase("dex") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= m_sStartDex && i < 0x7fffff)
		{
			m_iDEX = i * SERVER_EXT_STATS;
			SendCharData(INFO_BASICVALUE);
		}
	}
	else if (fn_str.CompareNoCase("int") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= m_sStartInt && i < 0x7fffff)
		{
			m_iINT = i * SERVER_BASE_STATS;
			SendCharData(INFO_BASICVALUE);
		}
	}
	else if (fn_str.CompareNoCase("str") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= m_sStartStr && i < 0x7fffff)
		{
			m_iSTR = i * SERVER_BASE_STATS;
			SendCharData(INFO_BASICVALUE);
		}
	}
	else if (fn_str.CompareNoCase("wis") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= m_sStartWis && i < 0x7fffff)
		{
			m_iWIS = i * SERVER_EXT_STATS;
			SendCharData(INFO_BASICVALUE);
		}
	}
	else if (fn_str.CompareNoCase("dmagic") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= 1 && i < 0x7fffff)
		{
			m_iDMagicExp = i * SERVER_SKILL;
			SendCharData(INFO_MAGICEXP);
		}
	}
	else if (fn_str.CompareNoCase("wmagic") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= 1 && i < 0x7fffff)
		{
			m_iWMagicExp = i * SERVER_SKILL;
			SendCharData(INFO_MAGICEXP);
		}
	}
	else if (fn_str.CompareNoCase("bmagic") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= 1 && i < 0x7fffff)
		{
			m_iBMagicExp = i * SERVER_SKILL;
			SendCharData(INFO_MAGICEXP);
		}
	}
	else if (fn_str.CompareNoCase("sword") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= 1 && i < 0x7fffff)
		{
			m_iSwordExp = i * SERVER_SKILL;
			SendCharData(INFO_WEAPONEXP);
		}
	}
	else if (fn_str.CompareNoCase("axe") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= 1 && i < 0x7fffff)
		{
			m_iAxeExp = i * SERVER_SKILL;
			SendCharData(INFO_WEAPONEXP);
		}
	}
	else if (fn_str.CompareNoCase("bow") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= 1 && i < 0x7fffff)
		{
			m_iBowExp = i * SERVER_SKILL;
			SendCharData(INFO_WEAPONEXP);
		}
	}
	else if (fn_str.CompareNoCase("spear") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= 1 && i < 0x7fffff)
		{
			m_iSpearExp = i * SERVER_SKILL;
			SendCharData(INFO_WEAPONEXP);
		}
	}
	else if (fn_str.CompareNoCase("knuckle") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= 1 && i < 0x7fffff)
		{
			m_iKnuckleExp = i * SERVER_SKILL;
			SendCharData(INFO_WEAPONEXP);
		}
	}
	else if (fn_str.CompareNoCase("staff") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));		int i = atoi(fn);

		if (i >= 1 && i < 0x7fffff)
		{
			m_iStaffExp = i * SERVER_SKILL;
			SendCharData(INFO_WEAPONEXP);
		}
	}
	else if (fn_str.CompareNoCase("status") == 0)
	{
		CString strStatus;
		GetStatus(strStatus);

		CString strMessage;
		if (strStatus.GetLength() > 0) strMessage.Format("Status: %s", strStatus);
		else strMessage = "Status is empty";

		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
	}
	else if (fn_str.CompareNoCase("setstatus") == 0)
	{
		CString strStatus = (pBuf+index);
		strStatus.Trim();

		int nLength = strStatus.GetLength();

		if (nLength < 0 || nLength > STATUS_LENGTH) return;

		SetStatus(strStatus);

		CString strMessage;
		if (strStatus.GetLength() > 0) strMessage.Format("Status changed: %s", strStatus);
		else strMessage = "Status removed";

		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
	}
	else if (fn_str.CompareNoCase("dismissed") == 0)
	{
		PartyDissolve(this);
	}
	else if (fn_str.CompareNoCase("delete") == 0)
	{
		if (!CheckPartyLeader()) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));
		int nLength = strlen(fn);
		if (nLength <= 0 || nLength > NAME_LENGTH) return;

		USER* pMember = GetUserId(fn);
		if (pMember == NULL || pMember->m_State != STATE_GAMESTARTED) return;
		if (pMember->m_Uid == m_Uid)
		{
			PartyDissolve(this);
		}
		else
		{
			PartyWithdraw(pMember->m_strUserId);
		}
	}
	else if (fn_str.CompareNoCase("inv") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		index += ParseSpaceInUser(fn, pBuf+index, min(sizeof(fn), strlen(pBuf+index)));

		int nLength = strlen(fn);
		if (nLength <= 0 || nLength > NAME_LENGTH) return;

		USER* pUser = GetUserId(fn);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) return;
		if (strcmp(pUser->m_strUserId, m_strUserId) == 0) return;

		strcpy(m_strInventoryOtherUserId, pUser->m_strUserId);
		m_bHasInventoryOther = true;
		for (int i = 0; i < INV_ITEM_NUM; i++)
		{
			m_OtherInvItem[i] = pUser->m_InvItem[i];
		}

		for (int i = 0; i < BELT_ITEM_NUM; i++)
		{
			m_OtherBeltItem[i] = pUser->m_BeltItem[i];
		}
		SendCharItemData();

		CString strMessage;
		strMessage.Format(IDS_GM_INV_CHANGED, pUser->m_strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
	}
	else if (fn_str.CompareNoCase("invrestore") == 0)
	{
		if (!IsServerRank(SERVER_RANK_ALL)) return;

		ZeroMemory(m_strInventoryOtherUserId, sizeof(m_strInventoryOtherUserId));
		m_bHasInventoryOther = false;
		SendCharItemData();
		SendServerChatMessage(IDS_GM_INV_RESTORED, TO_ME);
	}
	else if (fn_str.CompareNoCase("return") == 0)
	{
		if (!CheckDemon(m_sClass))
		{
			if (!CheckGuildTownWar(1) && m_sZ == 1 && CheckGuildTown(1))
			{
				LinkToSameZone(190, 836);
			}
		}
		else
		{
			if (!CheckGuildTownWar(2) && m_sZ == 11 && CheckGuildTown(2))
			{
				LinkToSameZone(294, 250);
			}
		}
	}
}

void USER::SendSpecialMsg(TCHAR *pMsg, BYTE type, int nWho)
{
	CBufferEx TempBuf;

	TempBuf.Add(PKT_CHAT);
	TempBuf.Add(type);
	TempBuf.Add(pMsg, _tcslen(pMsg));

	switch (nWho)
	{
	case TO_ALL:
		SendAll(TempBuf, TempBuf.GetLength());
		break;

	case TO_ME:
		Send(TempBuf, TempBuf.GetLength());
		break;

	case TO_ZONE:
		SendZone(TempBuf, TempBuf.GetLength());
		break;

	case TO_INSIGHT:
	default:
		SendInsight(TempBuf, TempBuf.GetLength());
		break;

	}
}

void USER::SendSpecialMsg(UINT strID, BYTE type, int nWho)
{
	SendSpecialMsg((LPTSTR)(LPCTSTR)_ID(strID), type, nWho);
}

void USER::SendServerMessageGameMaster(TCHAR* pMsg, BYTE towho)
{
	if (IsServerRank(SERVER_RANK_ALL))
	{
		SendServerChatMessage(pMsg, towho);
	}
}

void USER::SendServerMessageGameMaster(UINT strID, BYTE towho)
{
	if (IsServerRank(SERVER_RANK_ALL))
	{
		SendServerChatMessage(strID, towho);
	}
}

void USER::SendAllChatStatus()
{
	int index = 0;
	SetByte(m_TempBuf, PKT_ALLCHAT_STATUS, index);
	SetByte(m_TempBuf, m_bAllChatRecv, index);
	Send(m_TempBuf, index);
}
