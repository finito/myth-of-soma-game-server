#pragma once

class GuildMember
{
public:
	GuildMember();
	virtual ~GuildMember();
	GuildMember& operator=(GuildMember& other);

	char	m_strUserId[NAME_LENGTH+1];
	short	m_sGuildRank;// TODO: Change to just m_sRank ? the 'Guild' is obsolete
	long	m_lUsed;
};
