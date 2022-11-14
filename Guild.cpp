#include "stdafx.h"
#include "1p1emu.h"
#include "Guild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

Guild::Guild()
{
	Initialize();
}

Guild::~Guild()
{
}

void Guild::Initialize()
{
	//m_lUsed = 0;
	m_sNum = -1;	
	m_dwBarr = 0;
	m_sState = -1;
	m_sStorageOpenType = 1;
	m_TaxRate = 0;
	m_sSymbolVersion = 0;
	m_iUsedUid = -1;
	m_lUsed = 0;

	::ZeroMemory(m_strGuildName, sizeof(m_strGuildName));
	::ZeroMemory(m_strInfo, sizeof(m_strInfo));
	::ZeroMemory(m_strSymbol, sizeof(m_strSymbol));
	::ZeroMemory(m_strUsedUser, sizeof(m_strUsedUser));

	for (int i = 0; i < Guild::GUILD_RANKS; i++)
	{
		::ZeroMemory(m_strCallName[i], sizeof(m_strCallName[i]));
	}

	for (int i = 0; i < MAX_GUILD_MEMBERS; i++)
	{
		m_arMembers[i].m_sGuildRank = 0;
		m_arMembers[i].m_lUsed = 0;
		::ZeroMemory(m_arMembers[i].m_strUserId, sizeof(m_arMembers[i].m_strUserId));
	}

	m_arApplicants.RemoveAll();
}

int Guild::GetUser(TCHAR *strUserId)
{
	int nLen = 0;
	nLen = strlen(strUserId);

	if (nLen <= 0 || nLen > NAME_LENGTH) return -1;

	for (int i = 0; i < MAX_GUILD_MEMBERS; i++)
	{
		if (m_arMembers[i].m_lUsed > 0)
		{
			if (_stricmp(strUserId, m_arMembers[i].m_strUserId) == 0)
			{
				return i;
			}
		}
	}

	return -1;	
}

int Guild::GetApplicationUser(TCHAR *strUserId)
{
	CString ID;
	ID.Format("%s", strUserId);

	for (int i = 0; i < m_arApplicants.GetSize(); i++)
	{
		if (!ID.CompareNoCase(m_arApplicants[i]))
		{
			return i;
		}
	}

	return -1;
}

void Guild::RemoveMember(TCHAR *strUserId)
{
	int nLen = 0;
	nLen = strlen(strUserId);

	if (nLen <= 0 || nLen > NAME_LENGTH) return;

	for (int i = 0; i < MAX_GUILD_MEMBERS; i++)
	{
		if (m_arMembers[i].m_lUsed > 0)
		{
			if (_stricmp(strUserId, m_arMembers[i].m_strUserId) == 0)
			{
				if (InterlockedCompareExchangePointer((PVOID*)&m_arMembers[i].m_lUsed, (PVOID)0, (PVOID)1) == (PVOID*)1)
				{
					::ZeroMemory(m_arMembers[i].m_strUserId, sizeof(m_arMembers[i].m_strUserId));
					InterlockedExchange((long*)&m_arMembers[i].m_sGuildRank, 0);
				}				
			}
		}
	}
}

bool Guild::AddMember(TCHAR *strUserId, short sRank)
{
	int nLen = strlen(strUserId);
	if (nLen <= 0 || nLen > NAME_LENGTH) 
		return false;

	for (int i = 0; i < MAX_GUILD_MEMBERS; i++)
	{
		if (InterlockedCompareExchangePointer((PVOID*)&m_arMembers[i].m_lUsed, (PVOID)1, (PVOID)0) == (PVOID*)0)
		{
			m_arMembers[i].m_sGuildRank = sRank;
			strncpy(m_arMembers[i].m_strUserId, strUserId, nLen);
			return true;
		}
	}

	return false;
}

bool Guild::AddApplicant(TCHAR *strUserId)
{
	int nLen = strlen(strUserId);
	if (nLen <= 0 || nLen > NAME_LENGTH) 
		return false;

	if (m_arApplicants.GetSize() >= MAX_GUILD_APPLICATION_USER)
		return false;

	m_arApplicants.Add(strUserId);	
	return true;
}

void Guild::RemoveApplicant(TCHAR *strUserId)
{
	int nLen = strlen(strUserId);
	if (nLen <= 0 || nLen > NAME_LENGTH) 
		return;

	for (int i = 0; i < m_arApplicants.GetSize(); i++)
	{
		if (m_arApplicants[i].CompareNoCase(strUserId) == 0)
		{
			m_arApplicants.RemoveAt(i);
			break;
		}
	}
}

void Guild::SetCallName(short sRank, TCHAR* strCallName)
{
	if (sRank < Guild::GUILD_CHIEF || sRank > Guild::GUILD_RANKS) 
		return;

	int nLen = strlen(strCallName);
	if (nLen < 0 || nLen > Guild::GUILD_CALL_LENGTH) 
		return;

	strcpy(m_strCallName[sRank-1], strCallName);
}

int Guild::GetMemberCount()
{
	int iCount = 0;
	for (int i = 0; i < MAX_GUILD_MEMBERS; i++)
	{
		if (m_arMembers[i].m_lUsed)
		{
			iCount++;
		}
	}
	return iCount;
}

int Guild::GetUserByRank(short sRank)
{
	if (sRank < Guild::GUILD_CHIEF || sRank > Guild::GUILD_RANKS) return -1;

	for (int i = 0; i < MAX_GUILD_MEMBERS; i++)
	{
		if (m_arMembers[i].m_lUsed > 0 &&
			m_arMembers[i].m_sGuildRank == sRank)
		{
			return i;
		}
	}

	return -1;
}

bool Guild::IsRankEmpty(short sRank)
{
	return GetUserByRank(sRank) != -1 ? false : true;
}
