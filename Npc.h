#pragma once

#include "User.h"
#include "Com.h"
#include "PathFind.h"
#include "Define.h"

#define CHANGE_PATH_STEP	2

#define MAX_MAP_SIZE		1000

// Npc Types
#define NPCTYPE_NPC			0
#define NPCTYPE_NORMAL		1
#define NPCTYPE_BOSS		2
#define NPCTYPE_SUBBOSS		3

// Npc AI Types
#define NPCAITYPE_NORMAL	0
#define NPCAITYPE_DUMMY		1
#define NPCAITYPE_GUILD		2
#define NPCAITYPE_STONE_GUARD	3

// Npc Regen Types
#define NPCREGENTYPE_NORMAL	0	// Normal
#define NPCREGENTYPE_FORCE	1	// Commands / Group recall
#define NPCREGENTYPE_TIME	2	// Game Time

// Npc Class id bands
#define NPC_CLASS_BAND_HUMAN	10000
#define NPC_CLASS_BAND_DEVIL	20000

// Misc
#define MAX_NPC_ITEM_NUM	15
#define DEFAULT_MOVE_DISTANCE 3
#define DEFAULT_MOB_ATTACK_SPEED 1500
#define NPC_HAVE_USER_LIST 10
#define MAX_NPC_CHAT_NUM 3

// Npc Attack Types
#define NPC_AT_PASSIVE 0
#define NPC_AT_AGGRO 1

typedef CArray <CPoint, CPoint> RandMoveArray;
typedef CTypedPtrArray <CPtrArray, ItemList*> NpcItemArray;

class CMagicTable;
class CNpc
{
public:
	CNpc();
	virtual ~CNpc();

	// NPC Movement
	bool RandomMove(COM *pCom);
	bool PathFind(CPoint start, CPoint end);
	bool IsMovingEnd();
	bool StepMove(COM* pCom, int nStep);
	void ClearPathFindData();
	bool GetTargetPath(COM* pCom); // Path to NPC Target
	bool GetTargetPos(COM *pCom, CPoint &pt); // Targets current location
	bool IsChangePath(COM* pCom, int nStep = CHANGE_PATH_STEP); // Has target changed location?
	bool ResetPath(COM* pCom); // Creates a new path to target
	bool IsStepEnd();
	BOOL IsInRange(); // Checks if NPC is within the defined co-ords of the database
	bool IsInMovableRange(short sX, short sY); // Checks the place NPC wants to move to is valid
	CPoint FindNearAvailablePoint_S(int x, int y, int iDistance); // Returns a valid location for npc if found
	void MoveNpc(short x, short y, COM *pCom); // Moves npc to a location on map

	// NPC Actions
	void NpcLive(COM *pCom);
	bool SetLive(COM* pCom);
	void NpcStanding(COM *pCom);
	void NpcMoving(COM *pCom);
	void NpcAttacking(COM *pCom);
	void NpcTracing(COM *pCom);
	void NpcFighting(COM *pCom);
	void NpcDead(COM *pCom);
	void SetDead(COM* pCom, int iUid, bool bBroadCast); // Called when the npc has no hp left.
	void NpcRecall(COM *pCom);

	// NPC Fighting
	bool FindEnemy(COM *pCom);
	void Attack(COM *pCom);
	void SendAttackSuccess(COM *pCom, int tuid, short sHP, short sMaxHP);
	void SendAttackFail(COM *pCom, int tuid);
	void SendAttackMiss(COM *pCom, int tuid);
	int GetDefense();
	int GetAttack();
	bool IsAttackList(USER* pUser); // Used by npc to check if a user has attacked it
	void AttackListAdd(USER* pUser);
	void AttackListRemove(USER* pUser);

	// Client Connection
	void Send(USER *pUser, TCHAR *pBuf, int nLength);
	void SightRecalc(COM* pCom);
	void SendUserInfoBySightChange(int dir_x, int dir_y, int prex, int prey, COM *pCom);
	void SendToRange(COM *pCom, char *temp_send, int index, int min_x, int min_y, int max_x, int max_y);
	void SendInsight(COM* pCom, TCHAR *pBuf, int nLength);
	void FillNpcInfo(char *temp_send, int &index, BYTE flag, COM* pCom);

	// MISC
	bool Init();
	bool SetUid(int x, int y, int id);
	CNpc* GetNpc(int nid);
	CPoint ConvertToClient(int x, int y);
	CPoint ConvertToServer(int x, int y);
	bool IsCloseTarget(COM* pCom, int nRange);
	bool GetDistance(int xpos, int ypos, int dist);
	bool IsSurround(int targetx, int targety);
	bool IsMovable(int x, int y);
	USER* GetUser(COM* pCom, int uid);
	void NpcTrace(TCHAR *pMsg);
	bool CheckUser(USER* pUser); // Checks a USER is valid
	bool IsDetecter(); // Is the npc a normal guard.
	bool IsStone(); // Is the npc a stone guard.
	bool CheckAIType(int iAIType);
	bool FindUserInSight(COM* pCom); // Searches for a player within npc sight range
	bool CheckCanNpcLive(); // Check the npc can actualy become alive (basicly checks for a RegenEvent)
	bool ArrowPathFind(CPoint ptSource, CPoint ptTarget);
	bool IsMapMovable_C(int x, int y);
	BYTE GetDirection(int x1, int y1, int x2, int y2);

	// Misc Attack
	void InitTarget();
	void InitUserList();
	void InitAttackType();
	short SelectAttackType();
	short SelectMagicType();
	bool CheckAttackSuccess(USER* pUser);

	// Magic Attack
	void Magic(COM *pCom, short sMagicNo);
	void MagicArrow(COM *pCom, short sMagicNo); // Handles the magic arrow type of magic attack
	int GetMagicDamage(CMagicTable *pMagic, USER *pUser);
	int GetMagicDefense(CMagicTable *pMagic, USER *pUser);
	void SendMagicArrowAttackFail(COM *pCom, int iTargetId, short sMagicNo);
	void SendMagicArrowAttackSuccess(COM *pCom, int iTargetId, short sMagicNo, DWORD dwConTime,
		short sHP, short sMaxHP);
	bool CheckDistance(USER *pUser, int iDistance);
	DWORD CheckRemainMagic(int iMagicEffectValue);
	bool CheckRemainMagicAll();
	void InitRemainMagic(int iMagicClass);
	void SendNpcStatus(COM *pCom); // Sends status of poison magic afaik the source client does
								   // not even use or need this but included it anyway.
	void PoisonMagic(COM *pCom);

	// Special Attack
	DWORD CheckRemainSpecialAttack(COM *pCom, int iSpecialEffectValue);
	void InitRemainSpecial();
	void SendSpecialAttackCancel(COM *pCom);
	void SendSpecialAttackArrow(COM* pCom, BYTE bySuccess, BYTE byDir=0, short sSpecialNo=0, int iContinueTime=0, int iTargetId=0, short sTargetHP=0);

	// Chat
	void Talking(COM *pCom);

// Member Fields
	int		m_sNid;

	// int		m_nInitX;
	// int		m_nInitY;

	int		m_nInitMinX;
	int		m_nInitMinY;
	int		m_nInitMaxX;
	int		m_nInitMaxY;

	int		m_presx;
	int		m_presy;

	int		m_sCurZ;			// Current Zone;
	int		m_sCurX;			// Current X Position;
	int		m_sCurY;			// Current Y Position;

	// int		m_sOrgZ;			// DB Zone
	// int		m_sOrgX;			// DB X Pos
	// int		m_sOrgY;			// DB Y Pos

	int		m_TableZoneIndex;
	// int		m_sTableOrgZ;
	// int		m_sTableOrgX;
	// int		m_sTableOrgY;

	bool	m_bFirstLive;		// Is this the first time the NPC was revived?
	BYTE	m_NpcState;			// Decides what the NPC will do next CPU cycle, i.e. Attack, Move, etc
	int		m_ZoneIndex;

	short	m_sClientSpeed;

	DWORD	m_dwStepDelay;

	int		m_Delay;			// Used in NPC Thread
	DWORD	m_dwLastThreadTime;	// The last time this NPC was run in the NPC Thread

	// Recall
	short m_sRecallNid;	// Id of the NPC being recalled to
	short m_sRecallX;	// Location of recall
	short m_sRecallY;

	// Enemy Target
	DWORD m_dwLastFind;
	int m_iAttackedUid; // Who last attacked the NPC
	struct Target
	{
		int	id;
		int x;
		int y;
	};
	Target	m_Target;

	struct AttackUserList
	{
		TCHAR strUserID[NAME_LENGTH + 1];
	};
	int m_iAttackUserListIndex;
	AttackUserList m_AttackUserList[NPC_HAVE_USER_LIST]; // Holds 10 users names that have attacked the npc

	short m_sAttackType[100];		// Array of 0 or magic numbers used by npc attack selection
	int m_iAttackTypeMagicRange;	// How much of the AttackType array holds magic numbers

	// Magic Remain
	short m_sRemainMagicNo[MAX_MAGIC_EFFECT];
	DWORD m_dwRemainMagicTime[MAX_MAGIC_EFFECT];

	short m_sPoisonMagicNo;
	DWORD m_dwPoisonMagicCount;

	// Special Remain
	short m_sRemainSpecialNo[MAX_SPECIAL_EFFECT];
	DWORD m_dwRemainSpecialTime[MAX_SPECIAL_EFFECT];

	// Timer
	DWORD m_dwLastTimeCount;

	// Item
	NpcItemArray m_arItem;

	// Event
	bool m_bFirstClick;

	// Path finding
	CPoint	m_ptDest;
	int		m_min_x;
	int		m_min_y;
	int		m_max_x;
	int		m_max_y;

	// TODO: Sort out the issue of using m_pOrgMap or g_Zones to access the mobs map
	MapInfo	**m_pOrgMap; // Pointer to mobs current Map

	long	m_lMapUsed;

	int		m_pMap[MAX_MAP_SIZE];

	CSize	m_vMapSize;
	CPoint	m_vStartPoint, m_vEndPoint;

	CPathFind m_vPathFind;

	NODE	*m_pPath;

	bool			m_bRandMove;
	RandMoveArray	m_arRandMove;
	bool	m_bFirstMove;


	// FIELDS THAT I THINK SHOULD BE IN THE DATABASE

	int		m_sMoveDistance;

	//----------------------------------------------------------------
	//	MONSTER DB
	//----------------------------------------------------------------
	int		m_sSid;
	int		m_sPid;
 	BYTE	m_tNpcType;
	int		m_sTypeAI;
	int		m_sMinDamage;
	short	m_sClass;
	TCHAR	m_strName[50];		// MONSTER(NPC) Name
	int		m_sBlood;
	int		m_sLevel;
	long	m_iMaxExp;
	int		m_sStr;
	int		m_sWStr;
	int		m_sAStr;
	int		m_sDex_at;
	int		m_sDex_df;
	int		m_sIntel;
	int		m_sCharm;
	int		m_sWis;
	int		m_sCon;
	int		m_sMaxHp;
	int		m_sMaxMp;
	long	m_iStatus;
	long	m_iMoral;
	BYTE	m_tNpcAttType;
	BYTE	m_can_escape;
	BYTE	m_can_find_enemy;

	BYTE	m_can_find_our;
	BYTE	m_have_item_num;
	BYTE	m_haved_item;
	BYTE	m_have_magic_num;
	BYTE	m_haved_magic;
	BYTE	m_have_skill_num;
	BYTE	m_haved_skill;
	BYTE	m_search_range;
	BYTE	m_movable_range;
	int		m_sSpeed; // Move Speed
	int		m_standing_time;
	int		m_regen_time;
	int		m_bmagicexp;
	int		m_wmagicexp;
	int		m_dmagicexp;
	int		m_sRangeRate;
	int		m_sBackRate;
	int		m_sHowTarget;
	int		m_sMoneyRate;
	int		m_sMagicNum[3];
	int		m_sMagicRate[3];
	int		m_sDistance;

	short	m_sHP;				// HP
	short	m_sMP;				// MP

	DWORD	m_dwExp;				// Monsters EXP

	//----------------------------------------------------------------
	//	MONSTER_POS DB
	//----------------------------------------------------------------
	int		m_sMid;
	short	m_sZone;
	int		m_sMinX;
	int		m_sMinY;
	int		m_sMaxX;
	int		m_sMaxY;
	short	m_sNum;
	short	m_sTableNum;
	short	m_sRegenType;
	int		m_sRegenTime;
	short	m_sRegenEvent;
	short	m_sGroup;
	short	m_sGuild;
	short	m_sGuildOpt;
	short	m_sChat[MAX_NPC_CHAT_NUM];
	short	m_sMaxItemNum;
	short	m_sarrItem[MAX_NPC_ITEM_NUM]; // item
	short	m_sarrItemRand[MAX_NPC_ITEM_NUM]; // percent
	short	m_sEventNum;
	short	m_sMoneyType;
	short	m_sMoneyMin;
	short	m_sMoneyMax;

	// Stone guard / guard
	short m_sMinDetecterX;
	short m_sMaxDetecterX;
	short m_sMinDetecterY;
	short m_sMaxDetecterY;

	// Guild Town
	short m_sTownGuildNum;
	short m_sTownGuildPicId;
	TCHAR m_strTownGuildName[GUILD_NAME_LENGTH];
};

struct NPC_TYPE_NOLIVE
{
	short sNId;	// NPC Server Id
	short sMId;	// Monster Set
	short sGroup;
};

struct NPC_TYPE_TIME
{
	short sNId;	// NPC Server Id
	short sMId;	// Monster Set
	short sRegenTime;
};

struct NPC_TYPE_CLASS
{
	short sNId;	// NPC Server Id
	short sMId;	// Monster Set
	short sClass;
};

struct NPC_TYPE_GUILD
{
	short sNId;	// NPC Server Id
	short sMId;	// Monster Set
	short sGuild;
	short sGuildOpt;
};

struct NPC_TYPE_DETECTER
{
	short sNId; // NPC Server Id
};
