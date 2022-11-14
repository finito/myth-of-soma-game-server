#ifndef PACKET_H
#define PACKET_H

// Packet identifiers
const BYTE PKT_ACCOUNT_LOGIN	= 0xfe; // Char select login
const BYTE PKT_NEW_HERO			= 0x40; // Character creation
const BYTE PKT_DEL_HERO			= 0x41; // Character deletion
const BYTE PKT_LOGIN			= 0x01; // Game login
const BYTE PKT_LOGOUT			= 0x02; // Game Logout
const BYTE PKT_USERMODIFY		= 0X09; // User add / delete
const BYTE PKT_CHARACTER_DATA	= 0x50;	// User stat data
const BYTE PKT_CHAT				= 0x07;
const BYTE PKT_SET_TIME			= 0x29;
const BYTE PKT_GAMESTART		= 0x0b;
const BYTE PKT_MOVEFIRST		= 0x03;
const BYTE PKT_MOVEMIDDLE		= 0x04;
const BYTE PKT_MOVEEND			= 0x06;
const BYTE PKT_RUN_MOVEFIRST	= 0x15;
const BYTE PKT_RUN_MOVEMIDDLE	= 0x16;
const BYTE PKT_RUN_MOVEEND		= 0x17;
const BYTE PKT_RESTARTGAME		= 0x0c; // Restart Game (Goes to char select)
const BYTE PKT_INV_ALL			= 0x30;
const BYTE PKT_CHANGE_ITEM_INDEX = 0x32;
const BYTE PKT_ITEM_INFO	    = 0x33;
const BYTE PKT_ITEM_CHANGE_INFO = 0x3d;
const BYTE PKT_BATTLEMODE		= 0x18;
const BYTE PKT_CHANGEBELTINDEX	= 0x89;
const BYTE PKT_PUTITEM_INV		= 0x88;
const BYTE PKT_PUTITEM_BELT		= 0x87;
const BYTE PKT_ITEM_THROW		= 0x34;
const BYTE PKT_WEIGHT			= 0x3a;
const BYTE PKT_ITEM_FIELD_INFO	= 0x36;
const BYTE PKT_ITEM_DUMP		= 0x3e;
const BYTE PKT_MONEYCHANGE		= 0x39;
const BYTE PKT_ITEM_PICKUP		= 0x35;
const BYTE PKT_MAGIC_ALL		= 0x26;
const BYTE PKT_MAGIC_READY		= 0x13; 
const BYTE PKT_ITEM_USE			= 0x3c;
const BYTE PKT_EFFECT			= 0xaa;
const BYTE PKT_SETRUNMODE		= 0xda;
const BYTE PKT_CHANGEDIR		= 0xa7;
const BYTE PKT_LIFE				= 0x25;
const BYTE PKT_HPMP_RECOVERY	= 0x96; 
const BYTE PKT_CLIENTEVENT		= 0x60;
const BYTE PKT_EVENTSELBOX		= 0x92;
const BYTE PKT_CLASSPOINTBUY	= 0xe5;
const BYTE PKT_GETITEM			= 0x37;
const BYTE PKT_ZONECHANGE		= 0x0a;
const BYTE PKT_EVENTOKBOX		= 0x93;
const BYTE PKT_EVENTNORMAL		= 0x94;
const BYTE PKT_REPAIR_OPEN		= 0x99;
const BYTE PKT_SKILL_ABILITY	= 0xd4; // Crafting skills
const BYTE PKT_MAKEROPEN		= 0x74;	
const BYTE PKT_OUTITEM			= 0x38;
const BYTE PKT_SHOW_MAGIC		= 0x2a;
const BYTE PKT_STORAGEOPEN		= 0x76;
const BYTE PKT_SHOPOPEN			= 0x70;
const BYTE PKT_TOWNPORTALREQ	= 0xa8;
const BYTE PKT_TOWNPORTALEND	= 0xa9;
const BYTE PKT_SHOP_ITEM_COUNT	= 0x71;
const BYTE PKT_SELL_SHOP		= 0x72;
const BYTE PKT_BUY_SHOP			= 0x73;
const BYTE PKT_SAVEITEM			= 0x77;
const BYTE PKT_TAKEBACKITEM		= 0x78;
const BYTE PKT_SAVEMONEY		= 0x79;
const BYTE PKT_TAKEBACKMONEY	= 0x7a;
const BYTE PKT_ATTACK			= 0x10;
const BYTE PKT_ATTACK_ARROW		= 0x11; // Magic Attack not bow
const BYTE PKT_ATTACK_MAGIC_RAIL = 0X12;  
const BYTE PKT_MAGIC_CIRCLE    	=  0x14;  
const BYTE PKT_STATUS_INFO		=  0X19;  
const BYTE PKT_POISON			=  0x20; 
const BYTE PKT_ENCRYPTION_START_REQ = 0xfd;
const BYTE ENCRYPTION_PKT		= 0xf9;
const BYTE PKT_DUMMY_ATTACK		= 0xc7;
const BYTE PKT_DEAD				= 0x23;
const BYTE PKT_SPECIALATTACKCANCEL = 0xc3;
const BYTE PKT_PARTY_DENY		= 0xcc;
const BYTE PKT_PARTY_INVITE		= 0xcd;
const BYTE PKT_PARTY_INVITE_RESULT = 0xce;
const BYTE PKT_SETGRAY			= 0xad; // gray name for pker etc
const BYTE PKT_RESURRECTION_SCROLL = 0xd1;
const BYTE PKT_MAGIC_BELT		= 0x27;
const BYTE PKT_CHANGE_OTHER_ITEM = 0xe1;
const BYTE PKT_BBS_OPEN			= 0x65;
const BYTE PKT_BBS_NEXT			= 0x66;
const BYTE PKT_BBS_READ			= 0x67;
const BYTE PKT_BBS_WRITE		= 0x68;
const BYTE PKT_BBS_EDIT			= 0x69;
const BYTE PKT_BBS_DELETE		= 0x6A;
const BYTE PKT_TRADEREXCHANGE	= 0xDB;
const BYTE PKT_GUILD_OPEN		= 0x97;
const BYTE PKT_GUILD_LIST		= 0x98;
const BYTE PKT_GUILD_MEMBER_LIST	= 0xa2;
const BYTE PKT_GUILD_NEW		= 0x95;
const BYTE PKT_GUILD_INFO		= 0x9f;
const BYTE PKT_GUILD_CHANGE_POS	= 0xa6;
const BYTE PKT_GUILD_EDIT_INFO_REQ = 0xae;
const BYTE PKT_GUILD_EDIT_INFO = 0xaf;
const BYTE PKT_GUILD_APPLICATION_LIST = 0xa4;
const BYTE PKT_GUILD_REQ_USER_INFO = 0xa1;
const BYTE PKT_GUILD_REQ		= 0x9a;
const BYTE PKT_GUILD_MOVE_REQ	= 0x9b;
const BYTE PKT_GUILD_REMOVE_REQ = 0x9c;
const BYTE PKT_GUILD_MOVE_REJECT = 0xa3;
const BYTE PKT_GUILD_CHANGE_RANK = 0xa5;
const BYTE PKT_GUILD_GIVE_DONATION = 0xbd;
const BYTE PKT_GUILD_REMOVE_USER = 0x9e;
const BYTE PKT_GUILD_DISSOLVE = 0x9d;
const BYTE PKT_ATTACK_SPECIALARROW = 0xbe;
const BYTE PKT_REPAIR_REQ = 0xa0;
const BYTE PKT_MESSAGE = 0xc8;
const BYTE PKT_ALLCHAT_STATUS = 0xcb;
const BYTE PKT_MAGICPRECAST = 0xdc;
const BYTE PKT_QUEST_VIEW = 0xdf;
const BYTE PKT_ATTACK_SPECIALRAIL = 0xbf;
const BYTE PKT_ATTACK_SPECIALCIRCLE = 0xc0;
const BYTE PKT_ATTACK_SPECIALCIRCLE_2 = 0xc1;
const BYTE PKT_HAIRSHOPOK = 0xd2;
const BYTE PKT_GUILDWAR_REQ = 0xb2;
const BYTE PKT_GUILDWAR_REQ_RESULT = 0xb3;
const BYTE PKT_GUILDWAR_REQ_CANCEL = 0xb4;
const BYTE PKT_GUILDWAR_OK = 0xb5;
const BYTE PKT_GUILD_STORAGEOPEN = 0xb6;
const BYTE PKT_GUILD_SAVEITEM =	0xb7;
const BYTE PKT_GUILD_TAKEBACKITEM =	0xb8;
const BYTE PKT_GUILD_STORAGECLOSE = 0xb9;
const BYTE PKT_GUILD_SYMBOL_CHANGE_REQ = 0xb0;
const BYTE PKT_GUILD_SYMBOL_CHANGE = 0xb1;
const BYTE PKT_GUILD_SYMBOL_DATA = 0xd3;
const BYTE PKT_TRADE_REQ = 0x7b;
const BYTE PKT_TRADE_ACK = 0x7c;
const BYTE PKT_TRADE_ADDITEM = 0x7d;
const BYTE PKT_TRADE_SETMONEY = 0x7e;
const BYTE PKT_TRADE_OK = 0x7f;
const BYTE PKT_TRADE_CANCEL = 0x80;
const BYTE PKT_TRADE_RESULT = 0x81;
const BYTE PKT_GUILDTOWNSTONE_OPEN = 0xd9;
const BYTE PKT_SPECIAL_MOVE = 0xd6;

// Length constants
const int ACCOUNT_LENGTH = 12; // Length of the login id and password
const int NAME_LENGTH = 12; // Length of character names
const int NPC_NAME_LENGTH = 50; // Length of NPC names

// Error codes
const BYTE SUCCESS				=	1;
const BYTE FAIL					=	2;

const BYTE ERR_1				=	1;
const BYTE ERR_2				=	2;
const BYTE ERR_3				=	3;
const BYTE ERR_4				=	4;
const BYTE ERR_5				=	5;
const BYTE ERR_6				=	6;
const BYTE ERR_7				=	7;
const BYTE ERR_8				=	8;
const BYTE ERR_9				=	9;
const BYTE ERR_10				=	10;
const BYTE ERR_11				=	11;
const BYTE ERR_12				=	12;
const BYTE UNKNOWN_ERR			=	255;

// Item constants
const int EQUIP_ITEM_NUM		= 10;
const int INV_ITEM_NUM			= 50;
const int BELT_ITEM_NUM			= 4;
const int STORAGE_ITEM_NUM		= 80;
const int GUILD_STORAGE_ITEM_NUM = 40;

// Char constants
const int GENDER_FEMALE			= 0;
const int GENDER_MALE			= 1;

// UserModify packet constants
const BYTE INFO_MODIFY			= 1;
const BYTE INFO_DELETE			= 2;

// Character Data packet constants
const BYTE INFO_NAMES			= 0x01;
const BYTE INFO_BASICVALUE		= 0x02;			
const BYTE INFO_EXTVALUE		= 0x04;			
const BYTE INFO_WEAPONEXP		= 0x08;			
const BYTE INFO_MAKEEXP			= 0x10;			
const BYTE INFO_MAGICEXP		= 0x20;			
const BYTE INFO_ALL				= 0xFF;				

// Chat type packet constants
const BYTE NORMAL_CHAT			=	1;
const BYTE WHISPER_CHAT			=	2;
const BYTE ZONE_CHAT			=	3;
const BYTE WHISPER_ME_CHAT		=	4;
const BYTE SYSTEM_CHAT			=	5;
const BYTE GM_NOTICE_CHAT		=	6;
const BYTE GUILD_CHAT			=	7;
const BYTE SHOUT_CHAT			=	8;
const BYTE YELLOW_STAT			=	9;
const BYTE BLUE_STAT			=	10;
const BYTE PARTY_CHAT			=	11;
const BYTE GM_SCROLL_NOTICE_CHAT =	12;

// Time Set packet constants
const BYTE  WEATHER_FINE		= 0x00;
const BYTE  WEATHER_RAIN		= 0x01;
//const BYTE  WEATHER_SNOW		= 0x03;

// Field item info packet constants
const BYTE ITEM_INFO_MODIFY		=	1;
const BYTE ITEM_INFO_DELETE		=	2;

// Magic packet constants
const BYTE MAGIC_TYPE_MAGIC		= 1;
const BYTE MAGIC_TYPE_SPECIAL	= 0;
const BYTE MAGIC_TYPE_ABILITY	= 4;

// Party Invite packet constants
const BYTE INVITE_FAIL			= 0;
const BYTE INVITE_SUCCESS		= 1;
const BYTE DELETE_MEMBER		= 2;

#endif // PACKET_H
