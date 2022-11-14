#pragma once

#include "GuildMember.h"

class Guild
{
public:
	Guild();
	virtual ~Guild();

	void Initialize();

	int GetUser(TCHAR* strUserId);
	int	GetApplicationUser(TCHAR* strUserId);
	int GetUserByRank(short sRank);

	bool AddMember(TCHAR *strUserId, short sRank);
	bool AddApplicant(TCHAR* strUserId);

	void RemoveMember(TCHAR* strUserId);
	void RemoveApplicant(TCHAR* strUserId);

	bool IsRankEmpty(short sRank);

	void SetCallName(short sRank, TCHAR* strCallName);

	int GetMemberCount();

	static const int GUILD_RANKS = 9;
	static const int GUILD_INFO_SIZE = 120;
	static const int GUILD_CHIEF = 1;
	static const int GUILD_FIRST = 2;
	static const int GUILD_SECOND = 5;
	static const int GUILD_MEMBER = 10;
	static const int GUILD_CALL_LENGTH = 12;

	enum
	{
		GUILD_INVALID_GUILD_NAME = 1,
		GUILD_INVALID_GUILD_INFO,
		GUILD_FULL,
		GUILD_SYSTEM_ERROR,
		GUILD_ALREADY_JOIN,
		GUILD_SMALL_LEVEL,
		GUILD_SAME_GUILD_NAME,
		GUILD_ABSENT_JOIN,
		GUILD_ABSENT_REQ_USER,
		GUILD_ABSENT_GUILD_NAME,
		GUILD_SMALL_RANK,
		GUILD_ABSENT_JOIN_REQ,
		GUILD_ALREADY_JOIN_REQ,
		GUILD_NEED_EMPTY_RANK,
		GUILD_NOT_GUILD_USER,
		GUILD_INVALID_RANK,
		GUILD_INVALID_GUILD_CALL_NAME,
		GUILD_SMALL_MONEY,
	};

	short		m_sNum;
	short		m_sClass;
	short		m_sState;
	short		m_sStorageOpenType;
	short		m_TaxRate;
	short		m_sSymbolVersion;

	TCHAR		m_strInfo[GUILD_INFO_SIZE+1];
	TCHAR		m_strSymbol[GUILD_SYMBOL_SIZE+1];
	TCHAR		m_strGuildName[GUILD_NAME_LENGTH+1];
	TCHAR		m_strCallName[GUILD_RANKS][GUILD_CALL_LENGTH+1];

	long		m_lUsed;
	int			m_iUsedUid;
	TCHAR		m_strUsedUser[NAME_LENGTH+1];

	DWORD		m_dwBarr;

	GuildMember	m_arMembers[MAX_GUILD_MEMBERS];
	CArray <CString, CString> m_arApplicants;

	// Guild storage locking variables
	// long		m_lUsed;
	// int			m_iUsedUser;				// User that is using the guild .. i.e. guild storage
	// TCHAR	m_strUsedUser[NAME_LENGTH];
};
