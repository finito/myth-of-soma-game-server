#include "stdafx.h"
#include "USER.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString USER::EventExecToString(BYTE byExec)
{
	CString strExec = "";
	switch (byExec)
	{
	case EXEC_ADD_EVENT:
		strExec = "EXEC_ADD_EVENT";
		break;
	case EXEC_ADD_SERVER_EVENT:
		strExec = "EXEC_ADD_SERVER_EVENT";
		break;
	case EXEC_AUCTION_BUY_ITEM:
		strExec = "EXEC_AUCTION_BUY_ITEM";
		break;
	case EXEC_AUCTION_BUY_OPEN:
		strExec = "EXEC_AUCTION_BUY_OPEN";
		break;
	case EXEC_AUCTION_DELETE_BADLIST:
		strExec = "EXEC_AUCTION_DELETE_BADLIST";
		break;
	case EXEC_AUCTION_DRAW_NOBID:
		strExec = "EXEC_AUCTION_DELETE_BADLIST";
		break;
	case EXEC_AUCTION_DRAW_NOBUY:
		strExec = "EXEC_AUCTION_DELETE_BADLIST";
		break;
	case EXEC_AUCTION_GET_MONEY:
		strExec = "EXEC_AUCTION_GET_MONEY";
		break;
	case EXEC_AUCTION_SELL_OPEN:
		strExec = "EXEC_AUCTION_SELL_OPEN";
		break;
	case EXEC_BBS_OPEN:
		strExec = "EXEC_BBS_OPEN";
		break;
	case EXEC_CHANGE_CHA:
		strExec = "EXEC_CHANGE_CHA";
		break;
	case EXEC_CHANGE_DEX:
		strExec = "EXEC_CHANGE_DEX";
		break;
	case EXEC_CHANGE_HP:
		strExec = "EXEC_CHANGE_HP";
		break;
	case EXEC_CHANGE_INT:
		strExec = "EXEC_CHANGE_INT";
		break;
	case EXEC_CHANGE_MAGETYPE:
		strExec = "EXEC_CHANGE_MAGETYPE";
		break;
	case EXEC_CHANGE_MAX_HP:
		strExec = "EXEC_CHANGE_MAX_HP";
		break;
	case EXEC_CHANGE_MP:
		strExec = "EXEC_CHANGE_MP";
		break;
	case EXEC_CHANGE_PKMODE:
		strExec = "EXEC_CHANGE_PKMODE";
		break;
	case EXEC_CHANGE_STORAGE_TYPE:
		strExec = "EXEC_CHANGE_STORAGE_TYPE";
		break;
	case EXEC_CHANGE_STR:
		strExec = "EXEC_CHANGE_STR";
		break;
	case EXEC_CHANGE_WIS:
		strExec = "EXEC_CHANGE_WIS";
		break;
	case EXEC_CLASSPOINT_STORE_OPEN:
		strExec = "EXEC_CLASSPOINT_STORE_OPEN";
		break;
	case EXEC_COUNTER_INIT:
		strExec = "EXEC_COUNTER_INIT";
		break;
	case EXEC_COUNTER_START:
		strExec = "EXEC_COUNTER_START";
		break;
	case EXEC_DEL_EVENT:
		strExec = "EXEC_DEL_EVENT";
		break;
	case EXEC_DEL_SERVER_EVENT:
		strExec = "EXEC_DEL_SERVER_EVENT";
		break;
	case EXEC_GIVE_GOLD:
		strExec = "EXEC_GIVE_GOLD";
		break;
	case EXEC_GIVE_ITEM:
		strExec = "EXEC_GIVE_ITEM";
		break;
	case EXEC_GIVE_MAGIC:
		strExec = "EXEC_GIVE_MAGIC";
		break;
	case EXEC_GIVE_MAKE_SKILL:
		strExec = "EXEC_GIVE_MAKE_SKILL";
		break;
	case EXEC_GIVE_SPECIAL_ATTACK:
		strExec = "EXEC_GIVE_SPECIAL_ATTACK";
		break;
	case EXEC_GUILDTOWN_TAXRATE:
		strExec = "EXEC_GUILDTOWN_TAXRATE";
		break;
	case EXEC_GUILD_OPEN:
		strExec = "EXEC_GUILD_OPEN";
		break;
	case EXEC_KILL_NPC:
		strExec = "EXEC_KILL_NPC";
		break;
	case EXEC_LINK:
		strExec = "EXEC_LINK";
		break;
	case EXEC_LIVE_NPC:
		strExec = "EXEC_LIVE_NPC";
		break;
	case EXEC_MAGIC:
		strExec = "EXEC_MAGIC";
		break;
	case EXEC_MAKER_OPEN:
		strExec = "EXEC_MAKER_OPEN";
		break;
	case EXEC_MESSAGE_NONE:
		strExec = "EXEC_MESSAGE_NONE";
		break;
	case EXEC_OPEN_CHANGE_ITEM:
		strExec = "EXEC_OPEN_CHANGE_ITEM";
		break;
	case EXEC_OPEN_GUILDTOWN_STONE:
		strExec = "EXEC_OPEN_GUILDTOWN_STONE";
		break;
	case EXEC_OPEN_GUILDTOWN_STORE:
		strExec = "EXEC_OPEN_GUILDTOWN_STORE";
		break;
	case EXEC_OPEN_GUILDTOWN_STORE_REG:
		strExec = "EXEC_OPEN_GUILDTOWN_STORE_REG";
		break;
	case EXEC_OPEN_HAIRSHOP:
		strExec = "EXEC_OPEN_HAIRSHOP";
		break;
	case EXEC_OPEN_SPECIAL_MAKESKILL:
		strExec = "EXEC_OPEN_SPECIAL_MAKESKILL";
		break;
	case EXEC_OPEN_TRADER:
		strExec = "EXEC_OPEN_TRADER";
		break;
	case EXEC_PLUS_MORAL:
		strExec = "EXEC_PLUS_MORAL";
		break;
	case EXEC_REPAIR_ITEM_OPEN:
		strExec = "EXEC_REPAIR_ITEM_OPEN";
		break;
	case EXEC_RESET_EVENTCOUNT:
		strExec = "EXEC_RESET_EVENTCOUNT";
		break;
	case EXEC_RETURN:
		strExec = "EXEC_RETURN";
		break;
	case EXEC_ROB_GOLD:
		strExec = "EXEC_ROB_GOLD";
		break;
	case EXEC_ROB_ITEM:
		strExec = "EXEC_ROB_ITEM";
		break;
	case EXEC_ROB_MAGIC:
		strExec = "EXEC_ROB_MAGIC";
		break;
	case EXEC_RUN_EVENT:
		strExec = "EXEC_RUN_EVENT";
		break;
	case EXEC_SAY:
		strExec = "EXEC_SAY";
		break;
	case EXEC_SELECT_MSG:
		strExec = "EXEC_SELECT_MSG";
		break;
	case EXEC_SET_EVENTCOUNT:
		strExec = "EXEC_SET_EVENTCOUNT";
		break;
	case EXEC_SHOW_MAGIC:
		strExec = "EXEC_SHOW_MAGIC";
		break;
	case EXEC_SPECIAL_ATTACK:
		strExec = "EXEC_SPECIAL_ATTACK";
		break;
	case EXEC_STORAGE_OPEN:
		strExec = "EXEC_STORAGE_OPEN";
		break;
	case EXEC_STORE_OPEN:
		strExec = "EXEC_STORE_OPEN";
		break;
	case EXEC_TIMER_INIT:
		strExec = "EXEC_TIMER_INIT";
		break;
	case EXEC_TIMER_START:
		strExec = "EXEC_TIMER_START";
		break;
	case EXEC_WARP_GATE:
		strExec = "EXEC_WARP_GATE";
		break;
	case EXEC_XMASPOINT_GUILD:
		strExec = "EXEC_XMASPOINT_GUILD";
		break;
	case EXEC_XMASPOINT_USER:
		strExec = "EXEC_XMASPOINT_USER";
		break;
	case EXEC_MESSAGE_OK:
		strExec = "EXEC_MESSAGE_OK";
		break;
	default:
		strExec = "Unknown";
		break;
	}
	return strExec;
}

CString USER::EventLogicToString(BYTE byLogic)
{
	CString strLogic = "";
	switch (byLogic)
	{
	case LOGIC_ABSENT_EVENT:
		strLogic = "LOGIC_ABSENT_EVENT";
		break;
	case LOGIC_ABSENT_ITEM:
		strLogic = "LOGIC_ABSENT_ITEM";
		break;
	case LOGIC_ABSENT_MAGETYPE:
		strLogic = "LOGIC_ABSENT_MAGETYPE";
		break;
	case LOGIC_ABSENT_MAGIC:
		strLogic = "LOGIC_ABSENT_MAGIC";
		break;
	case LOGIC_ACCMAKE:
		strLogic = "LOGIC_ACCMAKE";
		break;
	case LOGIC_ARMORMAKE:
		strLogic = "LOGIC_ARMORMAKE";
		break;
	case LOGIC_AXE:
		strLogic = "LOGIC_AXE";
		break;
	case LOGIC_BMAGIC:
		strLogic = "LOGIC_BMAGIC";
		break;
	case LOGIC_BOW:
		strLogic = "LOGIC_BOW";
		break;
	case LOGIC_CHECK_ALIVE_NPC:
		strLogic = "LOGIC_CHECK_ALIVE_NPC";
		break;
	case LOGIC_CHECK_AUCTION_BADLIST:
		strLogic = "LOGIC_CHECK_AUCTION_BADLIST";
		break;
	case LOGIC_CHECK_CLASS_TOWN:
		strLogic = "LOGIC_CHECK_CLASS_TOWN";
		break;
	case LOGIC_CHECK_CLASS_TOWN_WAR:
		strLogic = "LOGIC_CHECK_CLASS_TOWN_WAR";
		break;
	case LOGIC_CHECK_COUNTER:
		strLogic = "LOGIC_CHECK_COUNTER";
		break;
	case LOGIC_CHECK_DEMON:
		strLogic = "LOGIC_CHECK_DEMON";
		break;
	case LOGIC_CHECK_EVENTCOUNT:
		strLogic = "LOGIC_CHECK_EVENTCOUNT";
		break;
	case LOGIC_CHECK_FIRST_CLICK:
		strLogic = "LOGIC_CHECK_FIRST_CLICK";
		break;
	case LOGIC_CHECK_GUILD_RANK:
		strLogic = "LOGIC_CHECK_GUILD_RANK";
		break;
	case LOGIC_CHECK_GUILD_TOWN:
		strLogic = "LOGIC_CHECK_GUILD_TOWN";
		break;
	case LOGIC_CHECK_GUILD_TOWN_ALLY:
		strLogic = "LOGIC_CHECK_GUILD_TOWN_ALLY";
		break;
	case LOGIC_CHECK_GUILD_TOWN_LEVEL:
		strLogic = "LOGIC_CHECK_GUILD_TOWN_LEVEL";
		break;
	case LOGIC_CHECK_GUILD_TOWN_WAR:
		strLogic = "LOGIC_CHECK_GUILD_TOWN_WAR";
		break;
	case LOGIC_CHECK_HUMAN:
		strLogic = "LOGIC_CHECK_HUMAN";
		break;
	case LOGIC_CHECK_ITEMSLOT:
		strLogic = "LOGIC_CHECK_ITEMSLOT";
		break;
	case LOGIC_CHECK_JOB:
		strLogic = "LOGIC_CHECK_JOB";
		break;
	case LOGIC_CHECK_MONSTER:
		strLogic = "LOGIC_CHECK_MONSTER";
		break;
	case LOGIC_CHECK_PAYTYPE:
		strLogic = "LOGIC_CHECK_PAYTYPE";
		break;
	case LOGIC_CHECK_PKMODE:
		strLogic = "LOGIC_CHECK_PKMODE";
		break;
	case LOGIC_CHECK_POSITION:
		strLogic = "LOGIC_CHECK_POSITION";
		break;
	case LOGIC_CHECK_TIMER:
		strLogic = "LOGIC_CHECK_TIMER";
		break;
	case LOGIC_COOKING:
		strLogic = "LOGIC_COOKING";
		break;
	case LOGIC_DMAGIC:
		strLogic = "LOGIC_DMAGIC";
		break;
	case LOGIC_EXIST_EVENT:
		strLogic = "LOGIC_EXIST_EVENT";
		break;
	case LOGIC_EXIST_ITEM:
		strLogic = "LOGIC_EXIST_ITEM";
		break;
	case LOGIC_EXIST_MAGETYPE:
		strLogic = "LOGIC_EXIST_MAGETYPE";
		break;
	case LOGIC_EXIST_MAGIC:
		strLogic = "LOGIC_EXIST_MAGIC";
		break;
	case LOGIC_GAME_TIME:
		strLogic = "LOGIC_GAME_TIME";
		break;
	case LOGIC_GENDER:
		strLogic = "LOGIC_GENDER";
		break;
	case LOGIC_GOLD:
		strLogic = "LOGIC_GOLD";
		break;
	case LOGIC_HOWMUCH_ITEM:
		strLogic = "LOGIC_HOWMUCH_ITEM";
		break;
	case LOGIC_INT:
		strLogic = "LOGIC_INT";
		break;
	case LOGIC_KNUCKLE:
		strLogic = "LOGIC_KNUCKLE";
		break;
	case LOGIC_LEVEL:
		strLogic = "LOGIC_LEVEL";
		break;
	case LOGIC_MAGETYPE:
		strLogic = "LOGIC_MAGETYPE";
		break;
	case LOGIC_MORAL:
		strLogic = "LOGIC_MORAL";
		break;
	case LOGIC_POTIONMAKE:
		strLogic = "LOGIC_POTIONMAKE";
		break;
	case LOGIC_RAND:
		strLogic = "LOGIC_RAND";
		break;
	case LOGIC_RANK:
		strLogic = "LOGIC_RANK";
		break;
	case LOGIC_SPEAR:
		strLogic = "LOGIC_SPEAR";
		break;
	case LOGIC_STAFF:
		strLogic = "LOGIC_STAFF";
		break;
	case LOGIC_STORAGE_TYPE:
		strLogic = "LOGIC_STORAGE_TYPE";
		break;
	case LOGIC_SWORD:
		strLogic = "LOGIC_SWORD";
		break;
	case LOGIC_WEAPONMAKE:
		strLogic = "LOGIC_WEAPONMAKE";
		break;
	case LOGIC_WEIGHT:
		strLogic = "LOGIC_WEIGHT";
		break;
	case LOGIC_WMAGIC:
		strLogic = "LOGIC_WMAGIC";
		break;
	default:
		strLogic = "Unknown";
		break;
	}
	return strLogic;
}
