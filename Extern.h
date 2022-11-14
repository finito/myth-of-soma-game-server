#pragma once

#include "MyDatabase.h"
#include "SERVERINFO.h"
#include "ItemTable.h"
#include "MAP.h"
#include "EVENT.h"
#include "PlusSpecialTable.h"
#include "MagicTable.h"
#include "SpecialTable.h"
#include "MakeSkillTable.h"
#include "ClassStoreTable.h"
#include "Store.h"
#include "ChangeOtherItemTable.h"
#include "TraderExchangeTable.h"

// MONSTER
#include "NpcTable.h"
#include "Npc.h"
#include "NpcThread.h"
#include "NpcChat.h"

// Level up (EXP) tables HSOMA
#include "ExpTable.h"
#include "SkillExpTable.h"
#include "StatExpTable.h"
#include "ConExpTable.h"
#include "ChaExpTable.h"

// Level up (EXP) tables DSOMA
#include "ExpTableD.h"
#include "SkillExpTableD.h"
#include "StatExpTableD.h"
#include "ConExpTableD.h"
#include "ChaExpTableD.h"

// Guild
#include "Guild.h"

extern volatile long g_bShutDown;

// Database
extern CMyDB g_DB[];

// Server
typedef CTypedPtrArray <CPtrArray, SERVERINFO*>	ServerArray;
extern ServerArray g_ServerInfo;

// Monster
typedef CTypedPtrArray <CPtrArray, CNpc*> NpcArray;
typedef CTypedPtrArray <CPtrArray, NPC_TYPE_NOLIVE*>NpcTypeNoLiveArray;
typedef CTypedPtrArray <CPtrArray, NPC_TYPE_TIME*> NpcTypeTimeArray;
typedef CTypedPtrArray <CPtrArray, NPC_TYPE_CLASS*>	NpcTypeClassArray;
typedef CTypedPtrArray <CPtrArray, NPC_TYPE_GUILD*>	NpcTypeGuildArray;
typedef CTypedPtrArray <CPtrArray, CNpcTable*> NpcTableArray;
typedef CTypedPtrArray <CPtrArray, CNpcThread*> NpcThreadArray;
typedef CArray <int, int> NpcDetecterArray;

extern long	g_TotalNPC;	
extern long	g_CurrentNPCError;
extern long	g_CurrentNPC;	

extern bool	g_bNpcExit;
extern NpcArray	g_arNpc;
extern NpcTypeNoLiveArray g_arNpcTypeNoLive;
extern NpcTypeTimeArray	g_arNpcTypeTime;
extern NpcTypeClassArray g_arNpcTypeClass;
extern NpcTypeGuildArray g_arNpcTypeGuild;
extern NpcDetecterArray	g_arNpcTypeDetecter;

// Item
typedef CMap<int, int, CItemTable*, CItemTable*> ItemTableMap;
extern ItemTableMap	g_mapItemTable;

typedef CTypedPtrArray <CPtrArray, CPlusSpecialTable*> PlusSpecialTableArray;
extern PlusSpecialTableArray g_arPlusSpecialTable;

// Zone
typedef CTypedPtrArray <CPtrArray, MAP*> ZoneArray;
extern ZoneArray g_Zones;

// Event
typedef CTypedPtrArray <CPtrArray, EVENT*> EventArray;
extern EventArray g_Events;

// Magic
typedef CTypedPtrArray <CPtrArray, CMagicTable*> MagicTableArray;
extern MagicTableArray g_arMagicTable;

// Special Attack (Aura)
typedef CTypedPtrArray <CPtrArray, CSpecialTable*> SpecialAttackTableArray;
extern SpecialAttackTableArray g_arSpecialAttackTable;

// MakeSkill (Ability - Crafting)
typedef CTypedPtrArray <CPtrArray, CMakeSkillTable*> MakeSkillTableArray;
extern MakeSkillTableArray g_arMakeSkillTable;

// Npc Chat
typedef CTypedPtrArray <CPtrArray, CNpcChat*> NpcChatArray;
extern NpcChatArray	g_arNpcChat;

// Class Store (POS)
typedef CTypedPtrArray <CPtrArray, CClassStoreTable*> ClassStoreTableArray;
extern ClassStoreTableArray	g_arClassStoreTable;

// Store
typedef CTypedPtrArray <CPtrArray, CStore*>	StoreArray;
extern StoreArray g_arStore;

// Town portal
typedef CTypedPtrArray <CPtrArray, TOWNPORTALFIXED*> TownPortalFixedArray;
extern TownPortalFixedArray g_arTownPortalFixed;

typedef CTypedPtrArray <CPtrArray, TOWNPORTAL*>	TownPortalArray;
extern TownPortalArray g_arTownPortal;

// Attack success tables
extern int g_DexHitRateLow[51];
extern int g_DexHitRateHigh[51];

// Level up (EXP) tables HSOMA
typedef CTypedPtrArray <CPtrArray, CExpTable*> ExpTableArray;
extern ExpTableArray g_arExpTable;

typedef CTypedPtrArray <CPtrArray, CSkillExpTable*>	SkillExpTableArray;
extern SkillExpTableArray	g_arSkillExpTable; 

typedef CTypedPtrArray <CPtrArray, CStatExpTable*> StatExpTableArray;
extern StatExpTableArray g_arStatExpTable;

typedef CTypedPtrArray <CPtrArray, CConExpTable*> ConExpTableArray;
extern ConExpTableArray	g_arConExpTable;

typedef CTypedPtrArray <CPtrArray, CChaExpTable*> ChaExpTableArray;
extern ChaExpTableArray	g_arChaExpTable;

// Level up (EXP) tables DSOMA
typedef CTypedPtrArray <CPtrArray, CExpTableD*>	ExpTableDArray;
extern ExpTableDArray g_arExpTableD;

typedef CTypedPtrArray <CPtrArray, CSkillExpTableD*> SkillExpTableDArray;
extern SkillExpTableDArray g_arSkillExpTableD; 

typedef CTypedPtrArray <CPtrArray, CStatExpTableD*>	StatExpTableDArray;
extern StatExpTableDArray g_arStatExpTableD;

typedef CTypedPtrArray <CPtrArray, CConExpTableD*> ConExpTableDArray;
extern ConExpTableDArray g_arConExpTableD;

typedef CTypedPtrArray <CPtrArray, CChaExpTableD*> ChaExpTableDArray;
extern ChaExpTableDArray g_arChaExpTableD;

// Change Other Item
typedef CTypedPtrArray <CPtrArray, CChangeOtherItemTable*> ChangeOtherItemTableArray;
extern ChangeOtherItemTableArray g_arChangeOtherItemTable;

// Trader Exchange
typedef CTypedPtrArray <CPtrArray, CTraderExchangeTable*> TraderExchangeTableArray;
extern TraderExchangeTableArray	g_arTraderExchangeTable;

// Class War
extern bool g_bClassWarEnd;
extern bool g_bClassWarStart;
extern bool g_bClassWar;
extern int g_iClassWarTime;
extern int g_iClassWarCount;
extern int g_iClassWarLimit;
extern int g_iClassWarMinus;
extern int g_iClassWarPlusAttack;
extern int g_iClassWarPlusDefense;
extern int g_iClassWar;

// Zone (Human / Devil)
typedef CMap<int, int, int, int> ZoneClassMap;
extern ZoneClassMap	g_mapZoneClass;

// Zone (Extra details for esoma client)
struct ZoneDetails
{
	BYTE unknown;
	BYTE nightType;	// the type of night effect for map?
	BYTE hasMusic;	// specify if map has no music sound?
};
typedef CMap<int, int, ZoneDetails, ZoneDetails> ZoneDetailsMap;
extern ZoneDetailsMap g_mapZoneDetails;

// Guild
typedef CTypedPtrArray <CPtrArray, Guild*> GuildArray;
extern GuildArray g_arGuild;

// Event View Data
struct EventViewData
{
	short sId;
	short sStartEvent;
	short sEndEvent;
	short sClass;
	short sLevel;
	short sCheckEvent;
	CString strEventName;
	CString strEventInfo;
	CString strEventAll;
};
typedef CTypedPtrArray <CPtrArray, EventViewData*> EventViewDataArray;
extern EventViewDataArray g_arEventViewData;

// Special Item Data
struct SpecialItemData
{
	short sId;
	short sItemNum;
	short sMinAb1;
	short sMaxAb1;
	short sMinAb2;
	short sMaxAb2;
	short sMinDur;
	short sMaxDur;
	short sMinTime;
	short sMaxTime;
	short sMinWg;
	short sMaxWg;
};
typedef CTypedPtrArray <CPtrArray, SpecialItemData*> SpecialItemDataArray;
extern SpecialItemDataArray g_arSpecialItemData;

// Rand Item Data (Fortune Pouch, Abyss wish, etc)
struct ChangeRandItemData
{
	short sId;
	short sGroup;
	short sItemNum;
};
typedef CTypedPtrArray <CPtrArray, ChangeRandItemData*>	ChangeRandItemDataArray;
extern ChangeRandItemDataArray g_arChangeRandItemData[20];

// Guild Town War
struct GuildTownData
{
	short sType;
	short sTownNum;
	short sGuildNum;
	TCHAR strGuildName[GUILD_NAME_LENGTH];
	short sLevel;
	short sTaxRate;
	short sAlly[3];
};
typedef CTypedPtrArray <CPtrArray, GuildTownData*> GuildTownDataArray;
extern GuildTownDataArray g_arGuildTownData;

extern bool g_bGuildTownWarEnd[MAX_GUILD_TOWN];
extern bool g_bGuildTownWarStart[MAX_GUILD_TOWN];
extern bool g_bGuildTownWar[MAX_GUILD_TOWN];
extern int g_iGuildTownRange[MAX_GUILD_TOWN];

extern int g_iGuildTownWarStartTime;
extern int g_iGuildTownWarEndTime;
