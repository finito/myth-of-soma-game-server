#include "stdafx.h"
#include "LOGIC_ELSE.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LOGIC_ELSE::LOGIC_ELSE()
{

}

LOGIC_ELSE::~LOGIC_ELSE()
{

}

void LOGIC_ELSE::Init()
{
	for (int i = 0; i < MAX_LOGIC_ELSE_INT; i++)
	{
		m_LogicElseInt[i] = -1;
	}

	m_bAnd = TRUE;
}

void LOGIC_ELSE::Parse(char *pBuf)
{
	int index = 0, i = 0;
	char temp[1024];

	index += ParseSpace(temp, pBuf+index);

	if (!strcmp(temp, "ABSENT_EVENT"))
	{
		m_LogicElse = LOGIC_ABSENT_EVENT;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "ABSENT_ITEM"))
	{
		m_LogicElse = LOGIC_ABSENT_ITEM;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "ABSENT_MAGETYPE"))
	{
		m_LogicElse = LOGIC_ABSENT_MAGETYPE;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "ABSENT_MAGIC"))
	{
		m_LogicElse = LOGIC_ABSENT_MAGIC;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "ACCMAKE"))
	{
		m_LogicElse = LOGIC_ACCMAKE;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "ARMORMAKE"))
	{
		m_LogicElse = LOGIC_ARMORMAKE;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "AXE"))
	{
		m_LogicElse = LOGIC_AXE;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "BMAGIC"))
	{
		m_LogicElse = LOGIC_BMAGIC;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "BOW"))
	{
		m_LogicElse = LOGIC_BOW;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_ALIVE_NPC"))
	{
		m_LogicElse = LOGIC_CHECK_ALIVE_NPC;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_AUCTION_BADLIST"))
	{
		m_LogicElse = LOGIC_CHECK_AUCTION_BADLIST;
	}
	else if (!strcmp(temp, "CHECK_CLASS_TOWN"))
	{
		m_LogicElse = LOGIC_CHECK_CLASS_TOWN;
	}
	else if (!strcmp(temp, "CHECK_CLASS_TOWN_WAR"))
	{
		m_LogicElse = LOGIC_CHECK_CLASS_TOWN_WAR;
	}
	else if (!strcmp(temp, "CHECK_COUNTER"))
	{
		m_LogicElse = LOGIC_CHECK_COUNTER;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_DEMON"))
	{
		m_LogicElse = LOGIC_CHECK_DEMON;
	}
	else if (!strcmp(temp, "CHECK_EVENTCOUNT")) // might be wrong
	{
		m_LogicElse = LOGIC_CHECK_EVENTCOUNT;
	}
	else if (!strcmp(temp, "CHECK_FIRST_CLICK"))
	{
		m_LogicElse = LOGIC_CHECK_FIRST_CLICK;
	}
	else if (!strcmp(temp, "CHECK_GUILD_RANK"))
	{
		m_LogicElse = LOGIC_CHECK_GUILD_RANK;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_GUILD_TOWN"))
	{
		m_LogicElse = LOGIC_CHECK_GUILD_TOWN;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_GUILD_TOWN_ALLY"))
	{
		m_LogicElse = LOGIC_CHECK_GUILD_TOWN_WAR;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_GUILD_TOWN_LEVEL"))
	{
		m_LogicElse = LOGIC_CHECK_GUILD_TOWN_LEVEL;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_GUILD_TOWN_WAR"))
	{
		m_LogicElse = LOGIC_CHECK_GUILD_TOWN_WAR;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_HUMAN"))
	{
		m_LogicElse = LOGIC_CHECK_HUMAN;
	}
	else if (!strcmp(temp, "CHECK_ITEMSLOT"))
	{
		m_LogicElse = LOGIC_CHECK_ITEMSLOT;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// SAY
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// Item uid
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// No
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// Item uid
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// No
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// Item uid
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_JOB"))
	{
		m_LogicElse = LOGIC_CHECK_JOB;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_MONSTER"))
	{
		m_LogicElse = LOGIC_CHECK_MONSTER;
	}
	else if (!strcmp(temp, "CHECK_PAYTYPE")) // might be wrong
	{
		m_LogicElse = LOGIC_CHECK_PAYTYPE;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_PKMODE"))
	{
		m_LogicElse = LOGIC_CHECK_PKMODE;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHECK_POSITION"))
	{
		m_LogicElse = LOGIC_CHECK_POSITION;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp); // zone

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp); // minx
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp); // miny

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp); // maxx
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp); // maxy
	}
	else if (!strcmp(temp, "CHECK_TIMER"))
	{
		m_LogicElse = LOGIC_CHECK_TIMER;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "COOKING"))
	{
		m_LogicElse = LOGIC_COOKING;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "DMAGIC"))
	{
		m_LogicElse = LOGIC_DMAGIC;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "EXIST_EVENT"))
	{
		m_LogicElse = LOGIC_EXIST_EVENT;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "EXIST_ITEM"))
	{
		m_LogicElse = LOGIC_EXIST_ITEM;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "EXIST_MAGETYPE"))
	{
		m_LogicElse = LOGIC_EXIST_MAGETYPE;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "EXIST_MAGIC"))
	{
		m_LogicElse = LOGIC_EXIST_MAGIC;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "GAME_TIME"))
	{
		m_LogicElse = LOGIC_GAME_TIME;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "GENDER"))
	{
		m_LogicElse = LOGIC_GENDER;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "GOLD"))
	{
		m_LogicElse = LOGIC_GOLD;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "HOWMUCH_ITEM"))
	{
		m_LogicElse = LOGIC_HOWMUCH_ITEM;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "INT"))
	{
		m_LogicElse = LOGIC_INT;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "KNUCKLE"))
	{
		m_LogicElse = LOGIC_KNUCKLE;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "LEVEL"))
	{
		m_LogicElse = LOGIC_LEVEL;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "MAGETYPE"))
	{
		m_LogicElse = LOGIC_MAGETYPE;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "MORAL"))
	{
		m_LogicElse = LOGIC_MORAL;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "POTIONMAKE"))
	{
		m_LogicElse = LOGIC_POTIONMAKE;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "RAND"))
	{
		m_LogicElse = LOGIC_RAND;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "RANK"))
	{
		m_LogicElse = LOGIC_RANK;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "SPEAR"))
	{
		m_LogicElse = LOGIC_SPEAR;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "STAFF"))
	{
		m_LogicElse = LOGIC_STAFF;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "STORAGE_TYPE"))
	{
		m_LogicElse = LOGIC_STORAGE_TYPE;
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "SWORD"))
	{
		m_LogicElse = LOGIC_SWORD;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "WEAPONMAKE"))
	{
		m_LogicElse = LOGIC_WEAPONMAKE;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "WEIGHT"))
	{
		m_LogicElse = LOGIC_WEIGHT;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// SAY
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// Item uid
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// No
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// Item uid
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// No
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);	// Item uid
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "WMAGIC"))
	{
		m_LogicElse = LOGIC_WMAGIC;

		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_LogicElseInt[i++] = atoi(temp);
	}
}

void LOGIC_ELSE::Parse_and(char *pBuf)
{
	Parse(pBuf);
	m_bAnd = TRUE;
}

void LOGIC_ELSE::Parse_or(char *pBuf)
{
	Parse(pBuf);
	m_bAnd = FALSE;
}
