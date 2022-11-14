#include "stdafx.h"
#include "EXEC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

EXEC::EXEC()
{

}

EXEC::~EXEC()
{

}

void EXEC::Parse(char *pBuf)
{
	int index = 0, i = 0, j = 0;
	char temp[1024];

	index += ParseSpace(temp, pBuf+index);

	if (!strcmp(temp, "ADD_EVENT"))
	{
		m_Exec = EXEC_ADD_EVENT;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp); // Event No
	}
	else if (!strcmp(temp, "ADD_SERVER_EVENT"))
	{
		m_Exec = EXEC_ADD_SERVER_EVENT;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp); // Server Event No
	}
	else if (!strcmp(temp, "AUCTION_BUY_ITEM"))
	{
		m_Exec = EXEC_AUCTION_BUY_ITEM;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "AUCTION_BUY_OPEN"))
	{
		m_Exec = EXEC_AUCTION_BUY_OPEN;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "AUCTION_DELETE_BADLIST"))
	{
		m_Exec = EXEC_AUCTION_DELETE_BADLIST;
	}
	else if (!strcmp(temp, "AUCTION_DRAW_NOBID"))
	{
		m_Exec = EXEC_AUCTION_DRAW_NOBID;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "AUCTION_DRAW_NOBUY"))
	{
		m_Exec = EXEC_AUCTION_DRAW_NOBUY;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "AUCTION_GET_MONEY"))
	{
		m_Exec = EXEC_AUCTION_GET_MONEY;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "AUCTION_SELL_OPEN"))
	{
		m_Exec = EXEC_AUCTION_SELL_OPEN;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "BBS_OPEN"))
	{
		m_Exec = EXEC_BBS_OPEN;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_CHA"))
	{
		m_Exec = EXEC_CHANGE_CHA;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_DEX"))
	{
		m_Exec = EXEC_CHANGE_DEX;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_HP"))
	{
		m_Exec = EXEC_CHANGE_HP;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_INT"))
	{
		m_Exec = EXEC_CHANGE_INT;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_MAGETYPE"))
	{
		m_Exec = EXEC_CHANGE_MAGETYPE;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_MAX_HP"))
	{
		m_Exec = EXEC_CHANGE_MAX_HP;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_MP"))
	{
		m_Exec = EXEC_CHANGE_MP;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_PKMODE"))
	{
		m_Exec = EXEC_CHANGE_PKMODE;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_STORAGE_TYPE"))
	{
		m_Exec = EXEC_CHANGE_STORAGE_TYPE;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_STR"))
	{
		m_Exec = EXEC_CHANGE_STR;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CHANGE_WIS"))
	{
		m_Exec = EXEC_CHANGE_WIS;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "CLASSPOINT_STORE_OPEN"))
	{
		m_Exec = EXEC_CLASSPOINT_STORE_OPEN;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "COUNTER_INIT"))
	{
		m_Exec = EXEC_COUNTER_INIT;
	}
	else if (!strcmp(temp, "COUNTER_START"))
	{
		m_Exec = EXEC_COUNTER_START;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "DEL_EVENT"))
	{
		m_Exec = EXEC_DEL_EVENT;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "DEL_SERVER_EVENT"))
	{
		m_Exec = EXEC_DEL_SERVER_EVENT;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "GIVE_GOLD"))
	{
		m_Exec = EXEC_GIVE_GOLD;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = (int)_atoi64(temp);
	}
	else if (!strcmp(temp, "GIVE_ITEM"))
	{
		m_Exec = EXEC_GIVE_ITEM;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "GIVE_MAGIC"))
	{
		m_Exec = EXEC_GIVE_MAGIC;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "GIVE_MAKE_SKILL"))
	{
		m_Exec = EXEC_GIVE_MAKE_SKILL;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "GIVE_SPECIAL_ATTACK"))
	{
		m_Exec = EXEC_GIVE_SPECIAL_ATTACK;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "GUILDTOWN_TAXRATE"))
	{
		m_Exec = EXEC_GUILDTOWN_TAXRATE;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "GUILD_OPEN"))
	{
		m_Exec = EXEC_GUILD_OPEN;
	}
	else if (!strcmp(temp, "KILL_NPC"))
	{
		m_Exec = EXEC_KILL_NPC;
	}
	else if (!strcmp(temp, "LINK"))
	{
		m_Exec = EXEC_LINK;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "LIVE_NPC"))
	{
		m_Exec = EXEC_LIVE_NPC;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "MAGIC"))
	{
		m_Exec = EXEC_MAGIC;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "MAKER_OPEN"))
	{
		m_Exec = EXEC_MAKER_OPEN;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "MESSAGE_NONE"))
	{
		m_Exec = EXEC_MESSAGE_NONE;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "MESSAGE_OK"))
	{
		m_Exec = EXEC_MESSAGE_OK;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "OPEN_CHANGE_ITEM"))
	{
		m_Exec = EXEC_OPEN_CHANGE_ITEM;
	}
	else if (!strcmp(temp, "OPEN_GUILDTOWN_STONE"))
	{
		m_Exec = EXEC_OPEN_GUILDTOWN_STONE;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "OPEN_GUILDTOWN_STORE"))
	{
		m_Exec = EXEC_OPEN_GUILDTOWN_STORE;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "OPEN_GUILDTOWN_STORE_REG"))
	{
		m_Exec = EXEC_OPEN_GUILDTOWN_STORE_REG;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "OPEN_HAIRSHOP"))
	{
		m_Exec = EXEC_OPEN_HAIRSHOP;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "OPEN_SPECIAL_MAKESKILL"))
	{
		m_Exec = EXEC_OPEN_SPECIAL_MAKESKILL;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "OPEN_TRADER"))
	{
		m_Exec = EXEC_OPEN_TRADER;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "PLUS_MORAL"))
	{
		m_Exec = EXEC_PLUS_MORAL;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "REPAIR_ITEM_OPEN"))
	{
		m_Exec = EXEC_REPAIR_ITEM_OPEN;
//		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "RESET_EVENTCOUNT"))
	{
		m_Exec = EXEC_RESET_EVENTCOUNT;
	}
	else if (!strcmp(temp, "RETURN"))
	{
		m_Exec = EXEC_RETURN;
	}
	else if (!strcmp(temp, "ROB_GOLD"))
	{
		m_Exec = EXEC_ROB_GOLD;		//^^

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "ROB_ITEM"))
	{
		m_Exec = EXEC_ROB_ITEM;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "ROB_MAGIC"))
	{
		m_Exec = EXEC_ROB_MAGIC;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "RUN_EVENT"))
	{
		m_Exec = EXEC_RUN_EVENT;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "SAY"))
	{
		m_Exec = EXEC_SAY;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "SELECT_MSG"))
	{
		m_Exec = EXEC_SELECT_MSG;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);		// Main Msg

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);		// Option 1
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);		// Result 1

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);		// Option 2
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);		// Result 2

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);		// Option 3
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);		// Result 3

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);		// Option 4
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);		// Result 4
	}
	else if (!strcmp(temp, "SET_EVENTCOUNT"))
	{
		m_Exec = EXEC_SET_EVENTCOUNT;
	}
	else if (!strcmp(temp, "SHOW_MAGIC"))
	{
		m_Exec = EXEC_SHOW_MAGIC;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "SPECIAL_ATTACK"))
	{
		m_Exec = EXEC_SPECIAL_ATTACK;
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "STORAGE_OPEN"))
	{
		m_Exec = EXEC_STORAGE_OPEN;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "STORE_OPEN"))
	{
		m_Exec = EXEC_STORE_OPEN;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp); // store id
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp); // Buy %
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp); // Sell %
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp); // unknown
	}
	else if (!strcmp(temp, "TIMER_INIT"))
	{
		m_Exec = EXEC_TIMER_INIT;
	}
	else if (!strcmp(temp, "TIMER_START"))
	{
		m_Exec = EXEC_TIMER_START;
	}
	else if (!strcmp(temp, "WARP_GATE"))
	{
		m_Exec = EXEC_WARP_GATE;

		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
		index += ParseSpace(temp, pBuf+index);	m_ExecInt[i++] = atoi(temp);
	}
	else if (!strcmp(temp, "XMASPOINT_GUILD")) // no idea might be wrong
	{
		m_Exec = EXEC_XMASPOINT_GUILD;
	}
	else if (!strcmp(temp, "XMASPOINT_USER")) // no idea might be wrong
	{
		m_Exec = EXEC_XMASPOINT_USER;
	}
}

void EXEC::Init()
{
	for (int i = 0; i < MAX_EXEC_INT; i++)
	{
		m_ExecInt[i] = -1;
	}
}
