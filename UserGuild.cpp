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

Guild* USER::GetGuild(short sGuildNum)
{
	EnterCriticalSection(&(m_pCom->m_critGuild));

	if (sGuildNum < 0)return NULL;
	if (sGuildNum >= g_arGuild.GetSize()) return NULL;

	if (g_arGuild[sGuildNum]->m_sNum == sGuildNum)
	{
		return g_arGuild[sGuildNum];
	}

	return NULL;
}

Guild* USER::GetGuildByName(TCHAR* strGuildName)
{
	EnterCriticalSection(&(m_pCom->m_critGuild));

	CString tempName;
	tempName.Format("%s", strGuildName);

	for (int i = 0; i < g_arGuild.GetSize(); i++)
	{
		if (!tempName.CompareNoCase(g_arGuild[i]->m_strGuildName))
		{
			return g_arGuild[i];
		}
	}

	return NULL;
}

int USER::GetEmptyGuildNum()
{
	EnterCriticalSection(&(m_pCom->m_critGuild));

	for (int i = 0; i < g_arGuild.GetSize(); i++)
	{
		if (g_arGuild[i]->m_sNum < 0)
		{
			return i;
		}
	}

	return -1;
}

void USER::CheckGuildBossDead()
{
	if (!m_bInGuildWar)
		return;

	if (m_sGuildRank != Guild::GUILD_CHIEF)
		return;

	Guild* pGuild = GetGuildByName(m_strGuildName);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		return;
	}

	int i = pGuild->GetUser(m_strUserId);
	if (i < 0 || pGuild->m_arMembers[i].m_sGuildRank != Guild::GUILD_CHIEF)
	{
		ReleaseGuild();
		return;
	}

	ReleaseGuild();
	GuildWarEnd();
}

void USER::GuildWarEnd()
{
	if (m_sGroup == -1)
		return;

	if (!m_bInGuildWar)
		return;

	if (m_sGuildRank != Guild::GUILD_CHIEF)
		return;

	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		return;
	}

	// Must be guild leader of the guild to make the war end
	int iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		ReleaseGuild();
		return;
	}
	if (pGuild->m_arMembers[iUserIndex].m_sGuildRank != Guild::GUILD_CHIEF)
	{
		ReleaseGuild();
		return;
	}

	// TODO: UpdateGuildState

	pGuild->m_sState = -1;
	ReleaseGuild();

	pGuild = GetGuild(m_sGuildWar);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		return;
	}
	pGuild->m_sState = -1;

	TCHAR strOtherGuildName[GUILD_NAME_LENGTH+1] = {0};
	strcpy(strOtherGuildName, pGuild->m_strGuildName);

	ReleaseGuild();

	// FIXME This could be improved!!!!!
	for (int i = 0; i< MAX_USER; i++)
	{
		USER *pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) continue;
		if (pUser->m_sGroup == m_sGroup || pUser->m_sGroup == m_sGuildWar)
		{
			pUser->m_bInGuildWar = false;
			pUser->m_bGuildWarDead = false;
			pUser->m_sGuildWar = -1;
			pUser->SendMyInfo(TO_INSIGHT, INFO_MODIFY);
		}

		CString str;
		str.Format(IDS_GUILDWAR_END, m_strGuildName, strOtherGuildName, strOtherGuildName);
		pUser->SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}
}

short USER::GetGuildNumByMember(TCHAR* strUserId)
{
	EnterCriticalSection(&(m_pCom->m_critGuild));

	CString tempName;
	tempName.Format("%s", strUserId);

	for (int i = 0; i < g_arGuild.GetSize(); i++)
	{
		if (g_arGuild[i]->GetUser(strUserId) >= 0)
		{
			LeaveCriticalSection(&(m_pCom->m_critGuild));
			return g_arGuild[i]->m_sNum;
		}
	}

	LeaveCriticalSection(&(m_pCom->m_critGuild));
	return -1;
}

short USER::GetGuildReqNumByMember(TCHAR* strUserId)
{
	EnterCriticalSection(&(m_pCom->m_critGuild));

	CString tempName;
	tempName.Format("%s", strUserId);

	for (int i = 0; i < g_arGuild.GetSize(); i++)
	{
		if (g_arGuild[i]->GetApplicationUser(strUserId) >= 0)
		{
			LeaveCriticalSection(&(m_pCom->m_critGuild));
			return g_arGuild[i]->m_sNum;
		}
	}

	LeaveCriticalSection(&(m_pCom->m_critGuild));
	return -1;
}

void USER::ReleaseGuild()
{
	LeaveCriticalSection(&(m_pCom->m_critGuild));
}

bool USER::CheckGuildWar()
{
	if (m_sGroup == -1) return false;
	Guild* pGuild = GetGuildByName(m_strGuildName);
	if (!pGuild || pGuild->m_sState == -1)
	{
		ReleaseGuild();
		return false;
	}

	ReleaseGuild();
	return true;
}
