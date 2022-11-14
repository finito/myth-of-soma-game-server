#pragma once

//
//	MAX VALUE DEFINE
//
#define _MAX_DWORD				0xFFFFFFFF
#define _MAX_INT				0x7FFFFFFF
#define _MAX_SHORT				0x7FFF

//
//	Defines About Communication
//
#define OVL_RECEIVE				0X01
#define OVL_SEND				0X02
#define OVL_CLOSE				0X03

#define PACKET_START1			0XAA
#define PACKET_START2			0X55
#define PACKET_END1				0X55
#define PACKET_END2				0XAA

#define SOCKET_BUFF_SIZE	(1024*16)
#define MAX_PACKET_SIZE		(1024*8)

// Socket type
#define TYPE_ACCEPT				0x01
#define TYPE_CONNECT			0x02

//
//
//	About USER
//
#define USER_DEAD				0X00
#define USER_LIVE				0X01

//
//	About GUILD
//
#define MAX_GUILD				1000
#define MAX_GUILD_MEMBERS		60
#define MAX_GUILD_APPLICATION_USER MAX_GUILD_MEMBERS-1 // Take away 1 because the guild master does not count
#define GUILD_WAR_PREPARE		0
#define GUILD_WAR_DECISION		1
#define GUILD_MAX_TAX			200
#define GUILD_MAX_DONATION		1000000000

#define GUILD_NAME_LENGTH	12

#define MAX_GUILD_TOWN		2

// Guild Stone
#define GUILD_STONE_MENU_MEMBER 1
#define GUILD_STONE_MENU_CHIEF 2
#define GUILD_STONE_MENU_FIRST 3
#define GUILD_STONE_MENU_SECOND 4

//
//	About NPC
//
#define NPC_NUM					50
#define NPC_LIVE				0X00
#define NPC_DEAD				0X01
#define NPC_ATTACKING			0X02
#define NPC_ATTACKED			0X03
#define NPC_ESCAPE				0X04
#define NPC_STANDING			0X05
#define NPC_MOVING				0X06
#define NPC_TRACING				0X07
#define NPC_FIGHTING			0X08
#define NPC_STRATEGY			0x09
#define NPC_BACK				0x0A
#define NPC_RECALL				0x0B

//
//	About Map Object
//
#define USER_BAND				1
#define NPC_BAND				10000
#define INVALID_BAND			50000

//
//	Defines About Max Value
//
#define MAX_EVENT				3000

//
//	To Who ???
//
#define TO_ALL					0X01
#define TO_ZONE					0X02
#define TO_ME					0X03
#define TO_INSIGHT				0X04

#define SEND_USER				0
#define SEND_INSIGHT			1
#define SEND_ZONE				2
#define SEND_ALL				3
#define SEND_RANGE				4
#define SEND_SCREEN				5

//
//	시야처리
//
#define	SCREEN_X				15
#define	SCREEN_Y				19

#define SIGHT_SIZE_X			10
#define SIGHT_SIZE_Y			10

#define DIR_H					1
#define DIR_L					-1
#define DIR_OUTSIDE				3

//
//	State Value
//
#define STATE_ACCEPTED			0X01
#define STATE_CONNECTED			0X02
#define STATE_DISCONNECTED		0X03
#define STATE_GAMESTARTED		0X04
#define STATE_INITED			0X05
#define STATE_LOGOUT			0X06
#define STATE_GAMERESTART		0X07

//
//	Event
//
#define MAX_LOGIC_INT			5
#define MAX_EXEC_INT			10
#define MAX_LOGIC_ELSE_INT		20

//
//  Item
//
#define ITEM_NAME_LENGTH		20
#define MAX_THROW_ITEM			30000

#define ITEM_DB_SIZE			100

//
//	Attack
//
#define	DEFAULT_ATTACK_DELAY 900

const BYTE	ATTACK_SUCCESS			=	1;
const BYTE	ATTACK_FAIL				=	2;
const BYTE	ATTACK_MISS				=	3;

//	Execution Define...
#define EXEC_ADD_EVENT					0x01
#define EXEC_ADD_SERVER_EVENT			0x02
#define EXEC_AUCTION_BUY_ITEM			0x03
#define EXEC_AUCTION_BUY_OPEN			0x04
#define EXEC_AUCTION_DELETE_BADLIST		0x05
#define EXEC_AUCTION_DRAW_NOBID			0x06
#define EXEC_AUCTION_DRAW_NOBUY			0x07
#define EXEC_AUCTION_GET_MONEY			0x08
#define EXEC_AUCTION_SELL_OPEN			0x09
#define EXEC_BBS_OPEN					0x0a
#define EXEC_CHANGE_CHA					0x0b
#define EXEC_CHANGE_DEX					0x0c
#define EXEC_CHANGE_HP					0x0d
#define EXEC_CHANGE_INT					0x0e
#define EXEC_CHANGE_MAGETYPE			0x0f
#define EXEC_CHANGE_MAX_HP				0x10
#define EXEC_CHANGE_MP					0x11
#define EXEC_CHANGE_PKMODE				0x12
#define EXEC_CHANGE_STORAGE_TYPE		0x13
#define EXEC_CHANGE_STR					0x14
#define EXEC_CHANGE_WIS					0x15
#define EXEC_CLASSPOINT_STORE_OPEN		0x16
#define EXEC_COUNTER_INIT				0x17
#define EXEC_COUNTER_START				0x18
#define EXEC_DEL_EVENT					0x19
#define EXEC_DEL_SERVER_EVENT			0x1a
#define EXEC_GIVE_GOLD					0x1b
#define EXEC_GIVE_ITEM					0x1c
#define EXEC_GIVE_MAGIC					0x1d
#define EXEC_GIVE_MAKE_SKILL			0x1e
#define EXEC_GIVE_SPECIAL_ATTACK		0x1f
#define EXEC_GUILDTOWN_TAXRATE			0x20
#define EXEC_GUILD_OPEN					0x21
#define EXEC_KILL_NPC					0x22
#define EXEC_LINK						0x23
#define EXEC_LIVE_NPC					0x24
#define EXEC_MAGIC						0x25
#define EXEC_MAKER_OPEN					0x26
#define EXEC_MESSAGE_NONE				0x27
#define EXEC_OPEN_CHANGE_ITEM			0x28
#define EXEC_OPEN_GUILDTOWN_STONE		0x29
#define EXEC_OPEN_GUILDTOWN_STORE		0x2a
#define EXEC_OPEN_GUILDTOWN_STORE_REG	0x2b
#define EXEC_OPEN_HAIRSHOP				0x2c
#define EXEC_OPEN_SPECIAL_MAKESKILL		0x2d
#define EXEC_OPEN_TRADER				0x2e
#define EXEC_PLUS_MORAL					0x2f
#define EXEC_REPAIR_ITEM_OPEN			0x30
#define EXEC_RESET_EVENTCOUNT			0x31
#define EXEC_RETURN						0x32
#define EXEC_ROB_GOLD					0x33
#define EXEC_ROB_ITEM					0x34
#define EXEC_ROB_MAGIC					0x35
#define EXEC_RUN_EVENT					0x36
#define EXEC_SAY						0x37
#define EXEC_SELECT_MSG					0x38
#define EXEC_SET_EVENTCOUNT				0x39
#define EXEC_SHOW_MAGIC					0x3a
#define EXEC_SPECIAL_ATTACK				0x3b
#define EXEC_STORAGE_OPEN				0x3c
#define EXEC_STORE_OPEN					0x3d
#define EXEC_TIMER_INIT					0x3e
#define EXEC_TIMER_START				0x3f
#define EXEC_WARP_GATE					0x40
#define EXEC_XMASPOINT_GUILD			0x41
#define EXEC_XMASPOINT_USER				0x42
#define	EXEC_MESSAGE_OK					0x43

//	Logic Else Define...
#define LOGIC_ABSENT_EVENT				0x01
#define LOGIC_ABSENT_ITEM				0x02
#define LOGIC_ABSENT_MAGETYPE			0x03
#define LOGIC_ABSENT_MAGIC				0x04
#define LOGIC_ACCMAKE					0x05
#define LOGIC_ARMORMAKE					0x06
#define LOGIC_AXE						0x07
#define LOGIC_BMAGIC					0x08
#define LOGIC_BOW						0x09
#define LOGIC_CHECK_ALIVE_NPC			0x0a
#define LOGIC_CHECK_AUCTION_BADLIST		0x0b
#define LOGIC_CHECK_CLASS_TOWN			0x0c
#define LOGIC_CHECK_CLASS_TOWN_WAR		0x0d
#define LOGIC_CHECK_COUNTER				0x0e
#define LOGIC_CHECK_DEMON				0x0f
#define LOGIC_CHECK_EVENTCOUNT			0x10
#define LOGIC_CHECK_FIRST_CLICK			0x11
#define LOGIC_CHECK_GUILD_RANK			0x12
#define LOGIC_CHECK_GUILD_TOWN			0x13
#define LOGIC_CHECK_GUILD_TOWN_ALLY		0x14
#define LOGIC_CHECK_GUILD_TOWN_LEVEL	0x15
#define LOGIC_CHECK_GUILD_TOWN_WAR		0x16
#define LOGIC_CHECK_HUMAN				0x17
#define LOGIC_CHECK_ITEMSLOT			0x18
#define LOGIC_CHECK_JOB					0x19
#define LOGIC_CHECK_MONSTER				0x1a
#define LOGIC_CHECK_PAYTYPE				0x1b
#define LOGIC_CHECK_PKMODE				0x1c
#define LOGIC_CHECK_POSITION			0x1d
#define LOGIC_CHECK_TIMER				0x1e
#define LOGIC_COOKING					0x1f
#define LOGIC_DMAGIC					0x20
#define LOGIC_EXIST_EVENT				0x21
#define LOGIC_EXIST_ITEM				0x22
#define LOGIC_EXIST_MAGETYPE			0x23
#define LOGIC_EXIST_MAGIC				0x24
#define LOGIC_GAME_TIME					0x25
#define LOGIC_GENDER					0x26
#define LOGIC_GOLD						0x27
#define LOGIC_HOWMUCH_ITEM				0x28
#define LOGIC_INT						0x29
#define LOGIC_KNUCKLE					0x2a
#define LOGIC_LEVEL						0x2b
#define LOGIC_MAGETYPE					0x2c
#define LOGIC_MORAL						0x2d
#define LOGIC_POTIONMAKE				0x2e
#define LOGIC_RAND						0x2f
#define LOGIC_RANK						0x30
#define LOGIC_SPEAR						0x31
#define LOGIC_STAFF						0x32
#define LOGIC_STORAGE_TYPE				0x33
#define LOGIC_SWORD						0x34
#define LOGIC_WEAPONMAKE				0x35
#define LOGIC_WEIGHT					0x36
#define LOGIC_WMAGIC					0x37

//
//	User Define Struct
//
typedef union{
	short int	i;
	BYTE		b[2];
} MYSHORT;

typedef union{
	__int64		i;
	BYTE		b[8];
} MYINT64;

//
//	User Guild Require
//
#define GUILD_MAKE_LEVEL_H		25
#define GUILD_MAKE_LEVEL_D		30
#define GUILD_MAKE_BARR			100000
#define GUILD_MAKE_CHARISMA		35
#define GUILD_SYMBOL_BARR		30000
#define GUILD_SYMBOL_SIZE		524
#define GUILD_WAR_BARR			100000
#define GUILD_TOWN_WAR_BARR		300000
#define GUILD_TOWN_WAR_OWNER_BARR 100000
#define GUILD_TOWN_WAR_ALLY_BARR 50000

//
// User Event Max Num
//
#define MAX_EVENT_NUM		2000

// STATS
#define MIN_START_STAT			10
#define MAX_START_STAT			13
#define MAX_TOTAL_START_STAT	63

// Chat levels and Stamina requirements
#define SHOUT_CHAT_LEVEL		20
#define SHOUT_CHAT_STM			30
#define ZONE_CHAT_LEVEL			30
#define ZONE_CHAT_STM			50

// Party
#define MAX_PARTY_USER_NUM		5

// bArm values
#define ARM_HEAD				0
#define ARM_EAR					1
#define ARM_NECK				2
#define ARM_CHEST				3
#define ARM_LEG					4
#define ARM_FOOT				5
#define ARM_RHAND				6	// Weapon
#define ARM_LHAND				7	// Shield
#define ARM_HANDS				8
#define ARM_WRIST				9
#define ARM_USE					10
#define ARM_CRAFT				11

// bType values
#define TYPE_SWORD			0
#define TYPE_AXE			1
#define TYPE_BOW			2
#define TYPE_SPEAR			3
#define TYPE_KNUCKLE		4
#define	TYPE_STAFF			5
#define TYPE_ARMOR			7
#define	TYPE_ACC			8
#define TYPE_POTION			9
#define	TYPE_FOOD			10
#define TYPE_MTR			11			// Crafting material
#define TYPE_TOKEN			12 // Tradable Items (normal items, unequipable, some are for quests, stackable)
#define TYPE_MONEY			13
#define TYPE_UNSTACKABLE	14
#define TYPE_UNTRADABLE		15 // Untradable Items (mostly quest items)

// Craft material types (if bType 11 then bArm is this)
#define MTR_NUM				12 // 12 as 0 isn't a mat

#define MTR_METAL			1
#define MTR_WOOD			2
#define MTR_LEATHER			3
#define MTR_BONE			4
#define MTR_CLOTH			5
#define MTR_PLANTS			6
#define MTR_MEAT			7
#define MTR_POWDER			8
#define MTR_SPICE			9
#define MTR_GEMS			10
#define MTR_ETC				11

// SPECIAL ITEM
#define MAX_SPECIAL_OPT		3

#define SPECIAL_NONE		0
#define SPECIAL_RUN_EVENT	2
#define SPECIAL_LIGHT		3
#define SPECIAL_ANTIDOTE	4
#define SPECIAL_PORTAL		5
#define SPECIAL_TOWN_PORTAL	6
#define SPECIAL_MORAL_ADD	7
#define SPECIAL_MAG_ATT		8
#define SPECIAL_MAG_DEF		9
#define SPECIAL_STR			11
#define SPECIAL_INT			12
#define SPECIAL_DEX			13
#define SPECIAL_WIS			14
#define SPECIAL_CON			15
#define SPECIAL_MAX_HP		17
#define SPECIAL_MAX_MP		18
#define SPECIAL_MAX_STM		19
#define SPECIAL_MAX_WGT		20
#define SPECIAL_PHY_ATT		21
#define SPECIAL_PHY_DEF		22
#define SPECIAL_MAG_ATTK2	23
#define SPECIAL_MAG_DEF2	24
#define SPECIAL_BLUE_MAG_ATT	25
#define SPECIAL_WHITE_MAG_ATT	26
#define SPECIAL_BLACK_MAG_ATT	27
#define SPECIAL_BLUE_MAG_DEF	28
#define SPECIAL_WHITE_MAG_DEF	29
#define SPECIAL_BLACK_MAG_DEF	30
#define SPECIAL_COMPASS		31
#define SPECIAL_POS_GEM		32
#define SPECIAL_GBIRD_FEATHER	33
#define SPECIAL_STAT_MOD_HP	34
#define SPECIAL_STAT_MOD_MP	35
#define SPECIAL_STAT_MOD_STM	36
#define SPECIAL_STAT_MOD_WGT	37
#define SPECIAL_LOTTERY		38
#define SPECIAL_RANGE_CHANGE	39
#define SPECIAL_STM_RECOVER	40
#define SPECIAL_DETOX_BLUE	41
#define SPECIAL_DETOX_WHITE	42
#define SPECIAL_DETOX_BLACK	43
#define SPECIAL_REVIVE		44
#define SPECIAL_RESIST_POISON	45
#define SPECIAL_MAGIC_AVAIL	46
#define SPECIAL_EVASION		47
#define SPECIAL_REPAIR		48
#define SPECIAL_SMELTING	50
#define SPECIAL_COOKING		52
#define SPECIAL_UPGRADE		53
#define SPECIAL_SPEC_REPAIR	54
#define SPECIAL_ACCURACY	55
#define SPECIAL_FORTUNE_POUCH	56
#define SPECIAL_PLUS		57 // Looks up table plus special..
#define SPECIAL_GENDER_CHANGE	58
#define SPECIAL_SKIN_CHANGE	59
#define SPECIAL_SHOUT		60
#define SPECIAL_SPEC_GEM	61
#define SPECIAL_SINGLE_SHOUT	62
#define SPECIAL_HAIR_DYE	63

// Inventory type defines
#define BELT_INV				0
#define BASIC_INV				1

//
//	Item Change Info Types
//
#define INFO_TYPE				0x01
#define INFO_ARM				0x02
#define INFO_PICNUM				0x04
#define INFO_DUR				0x08
#define INFO_NAME				0x10
#define INFO_DAMAGE				0x20
#define INFO_LIMIT				0x40
#define INFO_SPECIAL			0x80

// Battle Modes
#define BATTLEMODE_NORMAL	0
#define BATTLEMODE_ATTACK	1
#define BATTLEMODE_PK		2

// Attack
#define MIN_HIT_RATE 20
#define MAX_HIT_RATE 90

// Money Item
#define TYPE_MONEY_NUM	133

// Magic
#define MAX_MAGIC_NUM		87
#define MAGIC_BELT_SIZE		12

// Divide the db stored values by these values to get the client values
// CHAR STAT VALUES
#define CLIENT_MORAL		100000
#define CLIENT_BASE_STATS	1000000 // STR, INT, CON
#define CLIENT_EXT_STATS	10000000 // DEX, WIS, CHA
#define CLIENT_SKILL		100000 // All skills, Weapon, Craft, Magic

#define SERVER_BASE_STATS	100000 // STR, INT, CON
#define SERVER_EXT_STATS	1000000 // DEX, WIS, CHA
#define SERVER_SKILL	10000 // This divides 10* less than the client skill so that server can do skill gains

// Item slot types
#define EQUIP_SLOT			0
#define INVENTORY_SLOT		1
#define TRADE_SLOT			2
#define STORAGE_SLOT		3
#define GUILD_SLOT			4

// Item dura
#define MAX_ITEM_DURA		29999

// Attack
#define NO_DAMAGE_TIME		5000

// Storage
#define MAX_STORAGE_MONEY   1000000000

// Item gender
#define ITEM_GENDER_ALL 0
#define ITEM_GENDER_FEMALE 1
#define ITEM_GENDER_MALE 2

// Item Demon
#define DEVIL_ITEM_BAND 2000

// Get Inc Types
#define INC_STR 1				// 01h
#define INC_DEX 2				// 02h
#define INC_INT 3				// 03h
#define INC_WIS 4				// 04h
#define INC_CON 5				// 05h
#define	INC_CHA 6				// 06h
#define INC_LEVEL_MAX_EXP 7		// 07h
#define INC_LEVEL_RDEXP	8		// 08h
#define INC_LEVEL_WDEXP	9		// 09h
#define INC_RDSTR 10			// 0Ah
#define INC_WDSTR 11			// 0Bh
#define INC_RDDEX 12			// 0Ch
#define INC_WDDEX 13			// 0Dh
#define INC_RDINT 14			// 0Eh
#define INC_WDINT 15			// 0Fh
#define INC_RDWIS 16			// 10h
#define INC_WDWIS 17			// 11h
#define INC_RDCON 18			// 12h
#define INC_WDCON 19			// 13h
#define	INC_RDCHA 20			// 14h
#define	INC_WDCHA 21			// 15h
#define INC_AXE 22				// 16h
#define INC_BOW 23				// 17h
#define INC_KNUCKLE 24			// 18h
#define INC_SPEAR 25			// 19h
#define INC_STAFF 26			// 1Ah
#define INC_SWORD 27			// 1Bh
#define INC_BLACK_MAGIC 28		// 1Ch
#define INC_WHITE_MAGIC 29		// 1Dh
#define INC_BLUE_MAGIC 30		// 1Eh
#define INC_WEAPON_MAKE 31		// 1Fh
#define INC_ARMOR_MAKE 32		// 20h
#define INC_ACCESSORY_MAKE 33	// 21h
#define INC_POTION_MAKE 34		// 22h
#define INC_COOKING_MAKE 35		// 23h
#define INC_RDAXE 36			// 24h
#define INC_RDBOW 37			// 25h
#define INC_RDKNUCKLE 38		// 26h
#define INC_RDSPEAR 39			// 27h
#define INC_RDSTAFF 40			// 28h
#define INC_RDSWORD 41			// 29h
#define INC_RDBLACK_MAGIC 42	// 2Ah
#define INC_RDWHITE_MAGIC 43	// 2Bh
#define INC_RDBLUE_MAGIC 44		// 2Ch
#define INC_WDAXE 45			// 2Dh
#define INC_WDBOW 46			// 2Eh
#define INC_WDKNUCKLE 47		// 2Fh
#define INC_WDSPEAR 48			// 30h
#define INC_WDSTAFF 49			// 31h
#define INC_WDSWORD 50			// 32h
#define INC_WDBLACK_MAGIC 51	// 33h
#define INC_WDWHITE_MAGIC 52	// 34h
#define INC_WDBLUE_MAGIC 53		// 35h

// Caps
#define WEAPON_SKILL_CAP 370
#define MAGIC_SKILL_CAP 200	// Basicly only allows two of the three magic types to have max skill
							// currently the max skill is 100.

// Weapon STR and DEX gain percentages
// hsoma
#define H_SWORD_PERCENT_STR 90
#define H_SWORD_PERCENT_DEX 90
#define H_AXE_PERCENT_STR 170
#define H_AXE_PERCENT_DEX 30
#define H_BOW_PERCENT_STR 30
#define H_BOW_PERCENT_DEX 170
#define H_SPEAR_PERCENT_STR 130
#define H_SPEAR_PERCENT_DEX 70
#define H_KNUCKLE_PERCENT_STR 50
#define H_KNUCKLE_PERCENT_DEX 150
#define H_STAFF_PERCENT_STR 30
#define H_STAFF_PERCENT_DEX 30
// dsoma
#define D_SWORD_PERCENT_STR 150
#define D_SWORD_PERCENT_DEX 100
#define D_SWORD_PERCENT_INT 3
#define D_SWORD_PERCENT_WIS 3
#define D_SWORD_PERCENT_CON 80
#define D_AXE_PERCENT_STR 150
#define D_AXE_PERCENT_DEX 50
#define D_AXE_PERCENT_INT 3
#define D_AXE_PERCENT_WIS 3
#define D_AXE_PERCENT_CON 80
#define D_BOW_PERCENT_STR 65
#define D_BOW_PERCENT_DEX 150
#define D_BOW_PERCENT_INT 3
#define D_BOW_PERCENT_WIS 3
#define D_BOW_PERCENT_CON 75
#define D_SPEAR_PERCENT_STR 125
#define D_SPEAR_PERCENT_DEX 65
#define D_SPEAR_PERCENT_INT 3
#define D_SPEAR_PERCENT_WIS 3
#define D_SPEAR_PERCENT_CON 100
#define D_KNUCKLE_PERCENT_STR 90
#define D_KNUCKLE_PERCENT_DEX 130
#define D_KNUCKLE_PERCENT_INT 3
#define D_KNUCKLE_PERCENT_WIS 3
#define D_KNUCKLE_PERCENT_CON 70
#define D_STAFF_PERCENT_STR 10
#define D_STAFF_PERCENT_DEX 5
#define D_STAFF_PERCENT_INT 25
#define D_STAFF_PERCENT_WIS 25
#define D_STAFF_PERCENT_CON 5

// Magic tEValue types
#define MAGIC_EFFECT_NORMAL 1
#define MAGIC_EFFECT_DEFENSE 3
#define MAGIC_EFFECT_POISON 8
#define MAGIC_EFFECT_CURE 13
#define MAGIC_EFFECT_GIGGLE 14
#define MAGIC_EFFECT_WEAKEN 15
#define MAGIC_EFFECT_REFLECT 16
#define MAGIC_EFFECT_SLOW 17
#define MAGIC_EFFECT_CONFUSION 18
#define MAGIC_EFFECT_BLIND 19

#define MAX_MAGIC_EFFECT 20

// Magic tType01 types
#define MAGIC_TYPE_ARROW 1
#define MAGIC_TYPE_RAIL 2
#define MAGIC_TYPE_CIRCLE 3

// Magic Class Types
#define MAGIC_CLASS_BLUE 1
#define MAGIC_CLASS_WHITE 2
#define MAGIC_CLASS_BLACK 3
#define MAGIC_CLASS_ALL 4

// Special Attack tEValue types
#define SPECIAL_EFFECT_HPREGEN 0
#define SPECIAL_EFFECT_DOUBLEATTACK 1
#define SPECIAL_EFFECT_INCREASEATTACK 2
#define SPECIAL_EFFECT_DEFENSE 3
#define SPECIAL_EFFECT_SPEED 4
#define SPECIAL_EFFECT_PIERCE 5
#define SPECIAL_EFFECT_DODGE 6
#define SPECIAL_EFFECT_MPREGEN 7
#define SPECIAL_EFFECT_RAPIDCAST 8
#define SPECIAL_EFFECT_MPSAVE 9
#define SPECIAL_EFFECT_HPDRAIN 10
#define SPECIAL_EFFECT_PARALYZE 12
#define SPECIAL_EFFECT_CONCUSSION 13
#define SPECIAL_EFFECT_TRACE 14
#define SPECIAL_EFFECT_MASTERTRACE 15
#define SPECIAL_EFFECT_AREAMULTIPLEATTACK 16
#define SPECIAL_EFFECT_MULTIPLEATTACK 18
#define SPECIAL_EFFECT_MASTERMULTIPLEATTACK 19

#define MAX_SPECIAL_EFFECT 20

// Special tType values
#define SPECIAL_TYPE_ARROW 1
#define SPECIAL_TYPE_RAIL 2
#define SPECIAL_TYPE_CIRCLE 3

// Live Types
#define LIVE_TYPE_REVIVE 1
#define LIVE_TYPE_WARP 2

// Death Types
#define DEATH_TYPE_GUARD 0x08
#define DEATH_TYPE_DUEL 0x10
#define DEATH_TYPE_GUILDWAR 0x20
#define DEATH_TYPE_HSOMA_GV 0x40
#define DEATH_TYPE_DSOMA_GV 0x80
#define DEATH_TYPE_WOTW	0x100	// Death during class war (WotW)
#define DEATH_TYPE_CLASS 0x200	// e.g. Human kill Devil.

// PK
#define LEVEL_CAN_PK 16 // TODO: Put this in a configuration file or database!

// Map Area Number (Also known as Map Range)
#define MAP_AREA_NO_STAMINA		101 // No Stamina Recover (<<< The desert sand is slowing down your feet. >>>)
#define MAP_AREA_HP_DECREASE	102 // HP Decrease (<<<The high temperature of the desert is burdening your movement >>>)
#define MAP_AREA_MP_CHANGE		103 // MP (<< RANGE_CHANGE_MP >>>)This aint used!
#define MAP_AREA_SPEED_CHANGE	104 // <<< RANGE_CHANGE_SPEED >>> This is aint used!
#define MAP_AREA_IN_HOUSE		105 // Inside a building such as auction house in tyt
#define MAP_AREA_HSOMA_GV		106 // HSOMA GVW Inside the hall!
#define MAP_AREA_DUEL			107	// Arena (Duel)
#define MAP_AREA_DSOMA_GV		108 // DSOMA GVW Inside the hall!
#define MAP_AREA_ON_WOTW_MAP	109 // Returns TRUE if user is on a WotW map and WotW has started. Returns FALSE if user isn't on a WotW map or WotW has not started.(HARD CODED CHECK IN 1P1)
#define MAP_AREA_WOTW_MAP		110 // Returns TRUE if user is on a WotW map. Returns FALSE if user is not on a Wotw map.
#define MAP_AREA_WOTW_SAFE		111 // WotW Safe Zone (only shows and activated when wotw is started)

// Gray Mode
#define GRAY_MODE_NONE			0
#define GRAY_MODE_NORMAL		1
#define GRAY_MODE_FLASH			2

// Cap
#define LEVEL_CAP				130

// Direction
#define DIR_DOWN				0
#define	DIR_DOWN_LEFT			1
#define DIR_LEFT				2
#define DIR_UP_LEFT				3
#define DIR_UP					4
#define DIR_UP_RIGHT			5
#define DIR_RIGHT				6
#define DIR_DOWN_RIGHT			7

// Magic Target
#define MAGIC_TARGET_SELF		1	// Can only target yourself
#define MAGIC_TARGET_ALL		2	// Can target all
#define MAGIC_TARGET_OTHER		3	// Can target all except yourself
#define MAGIC_TARGET_USER		4	// Can only target users (NOT NPC)

// Ranks of User
#define SERVER_RANK_NORMAL		0
#define	SERVER_RANK_ALL			1
#define SERVER_RANK_OPERATING	2
#define SERVER_RANK_MOVEMENT	3
#define SERVER_RANK_EVENT		4
#define SERVER_RANK_BROADCAST	5
#define SERVER_RANK_COUNT		6
#define SERVER_RANK_BBS			7
#define SERVER_RANK_LEVEL2		12
#define SERVER_RANK_LEVEL3		13
#define SERVER_RANK_LEVEL4		14
#define SERVER_RANK_SPUSER		100

// Moral Bands
//#define MORAL_BAND_DEVIL		-50
//#define MORAL_BAND_WICKED		-31
//#define MORAL_BAND_EVIL			-11
//#define MORAL_BAND_BAD			-4
//#define MORAL_BAND_NEUTRAL		3
//#define MORAL_BAND_GOOD			10
//#define MORAL_BAND_MORAL		30
//#define MORAL_BAND_VIRTUOUS		49
//#define MORAL_BAND_ANGEL		74

// Store Rate Cap
#define STORE_BUY_RATE_CAP		500
#define STORE_SELL_RATE_CAP		500

// Class (RvR)
#define CLASS_POINT_LOWEST		-2000

// Trace Types (Special Attack)
#define TRACE_HUMAN 0
#define TRACE_MONSTER 1
#define TRACE_NONE 2

// Item Repair
#define REPAIR_FAIL_REPAIR		0
#define REPAIR_FAIL_NONEED		1
#define REPAIR_FAIL_NOREPAIR	2
#define REPAIR_FAIL_NOMONEY		3
#define REPAIR_FAIL_DESTROY		4

// Hair Shop
#define STYLE_MAX 2
#define COLOR_MAX 2
#define HAIR_SHOP_PRICE_COUNT STYLE_MAX + COLOR_MAX

// Status
#define STATUS_LENGTH 100

// All chat
#define ALL_CHAT_LEVEL 100

// Devil Soma Classes
#define DEVIL_CLASS_SWORDSMAN 11
#define DEVIL_CLASS_KNIGHT 12
#define DEVIL_CLASS_ARCHER	13
#define DEVIL_CLASS_WARRIOR 14
#define DEVIL_CLASS_FIGHTER 15
#define DEVIL_CLASS_WIZARD 16
