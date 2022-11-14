#include "stdafx.h"
#include "1p1emu.h"
#include "GuildMember.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

GuildMember::GuildMember()
{
	::ZeroMemory(m_strUserId, sizeof(m_strUserId));
	m_sGuildRank = 0;
	m_lUsed = 0;
}

GuildMember::~GuildMember()
{

}

GuildMember& GuildMember::operator=(GuildMember& other)
{
	m_sGuildRank = other.m_sGuildRank;
	m_lUsed = other.m_lUsed;
	strncpy(m_strUserId, other.m_strUserId, sizeof(m_strUserId));
	return *this;
}
