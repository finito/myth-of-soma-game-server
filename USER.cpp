// USER.cpp: implementation of the USER class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "1p1emu.h"
#include "Extern.h"
#include "USER.h"
#include "COM.h"
#include "CircularBuffer.h"
#include "Search.h"
#include "1p1emudlg.h"
#include "BufferEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CSearch *g_pUserList;
extern CMy1p1EmuDlg *g_pMainDlg;

USER::USER()
{
	m_pCB = new CCircularBuffer(20000);
	InitializeCriticalSection(&m_CS_ExchangeItem);
}

USER::~USER()
{
	InitEventSlot(); // Memory cleanup
	EnterCriticalSection(&m_CS_ExchangeItem);
	for (int i = 0; i < m_arExchangeItem.GetSize(); i++)
	{
		if (m_arExchangeItem[i])
		{
			delete m_arExchangeItem[i];
			m_arExchangeItem[i] = NULL;
		}
	}
	m_arExchangeItem.RemoveAll();
	LeaveCriticalSection(&m_CS_ExchangeItem);
	if (m_pCB) delete m_pCB;
	DeleteCriticalSection(&m_CS_ExchangeItem);
}

void USER::CloseProcess()
{
	if (!m_UserConnected)
	{
		return;
	}

	CheckGuildBossDead(); // FIXME : Does this need moving elsewhere?

	m_UserConnected = false;

	// Log the user out of the game!
	if (m_State == STATE_GAMESTARTED)
	{
		LogOut(0);
	}

	m_State = STATE_DISCONNECTED;

	CIOCPSocket2::CloseProcess();

	g_pMainDlg->UserFree(m_Uid);

	m_UserFlag = false;
}

void USER::Initialize()
{
	// Set the uid of the user and thread index
	if (m_Sid == -1)
	{
		m_Sid = 0;
		m_iModSid = AUTOMATA_THREAD;
		m_Uid = m_Sid;
	}
	else
	{
		m_iModSid = m_Sid % AUTOMATA_THREAD;
		m_Uid = m_Sid;
	}

	// Add the user to the global user list
	if (m_Uid >= 0)
	{
		g_pUserList->SetUserUid(m_Uid,this);
	}

	m_CryptionFlag = 0;
	m_Rec_val = 0;

	m_UserConnected = true;
	m_pCom = g_pMainDlg->GetCOM();
	m_UserFlag = false;

	InitUser(1);
}

void USER::InitUser(bool bLoggedOut)
{
	if (bLoggedOut)
	{
		// Things that should only be altered when user completely logs out of the game
		// e.g. not a restart to character select etc...
		m_bAccountLoggedIn = false;
		ZeroMemory(m_strAccount, sizeof(m_strAccount));
		m_State = STATE_DISCONNECTED;
	}

	ZeroMemory(m_strUserId, sizeof(m_strUserId));
	ZeroMemory(m_strDenyMsgUserId, sizeof(m_strDenyMsgUserId));

	m_nCharNum = 0;

	m_iZoneIndex = 0;
	m_sZ = 0;
	m_sX = 0;
	m_sY = 0;
	m_sPrevX = -1;
	m_sPrevY = -1;

	m_sClass = 0;
	m_sLevel = 0;

	m_iSTR = 0;
	m_iDEX = 0;
	m_iINT = 0;
	m_iCHA = 0;
	m_iWIS = 0;
	m_iCON = 0;

	m_iSTRInc = 0;
	m_iDEXInc = 0;
	m_iINTInc = 0;
	m_iCHAInc = 0;
	m_iWISInc = 0;
	m_iCONInc = 0;

	m_sHP = 0;
	m_sMaxHP = 0;
	m_sMP = 0;
	m_sMaxMP = 0;
	m_sStm = 0;
	m_sMaxStm = 0;
	m_sWgt = 0;
	m_sMaxWgt = 0;

	m_sAge = 0;
	m_iMoral = 0;
	m_sGender = 0;
	m_sHair = 0;
	m_sHairMode = 0;
	m_sSkin = 0;
	m_sGroup = -1;
	m_sGroupReq = -1;
	m_sFame = 0;
	m_sMageType = 0;

	m_iSwordExp = 0;
	m_iSpearExp = 0;
	m_iBowExp = 0;
	m_iAxeExp = 0;
	m_iKnuckleExp = 0;
	m_iStaffExp = 0;

	m_iWeaponMakeExp = 0;
	m_iArmorMakeExp = 0;
	m_iAccMakeExp = 0;
	m_iPotionMakeExp = 0;
	m_iCookingExp = 0;

	m_iWMagicExp = 0;
	m_iBMagicExp = 0;
	m_iDMagicExp = 0;

	m_dwExp = 0;
	m_dwMaxExp = 0;
	m_dwBarr = 0;
	m_dwBank = 0;

	m_sStartStr = 10;
	m_sStartDex = 10;
	m_sStartInt = 10;
	m_sStartCha = 10;
	m_sStartWis = 10;
	m_sStartCon = 10;

	m_iClassPoint = 0;
	m_iTotClassPoint = 0;
	m_iMoralCount = 0;
	m_iGrayCount = 0;
	m_iEventCount = 0;
	m_iPlayCount = 0;
	m_bAllChatRecv = true;
	m_bPrivMsgRecv = true;

	m_byDir = 0;
	m_sRank = 0;
	m_bLive = true;
	m_bLiveGray = false;

	m_bCanRecoverStm = false;
	m_bPreventChat = false;

	m_BattleMode = BATTLEMODE_NORMAL;

	m_bPartyDeny = false;
	m_bInParty = false;
	m_bPartyInvite = false;
	m_iPartyInviteUid = -1;
	for (int i = 0; i < MAX_PARTY_USER_NUM; i++)
	{
		m_PartyMembers[i].uid = -1;
		ZeroMemory(m_PartyMembers[i].m_strUserId, sizeof(m_PartyMembers[i].m_strUserId));
	}

	for (int i = 0; i < INV_ITEM_NUM; i++)
	{
		m_InvItem[i].Init();
	}

	for (int i = 0; i < BELT_ITEM_NUM; i++)
	{
		m_BeltItem[i].Init();
	}

	for (int i = 0; i < STORAGE_ITEM_NUM; i++)
	{
		m_StorageItem[i].Init();
	}

	for (int i = 0; i < GUILD_STORAGE_ITEM_NUM; i++)
	{
		m_GuildItem[i].Init();
	}

	m_ItemFieldInfoCount = 0;
	memset(m_ItemFieldInfoBuf, NULL, 8192);
	m_ItemFieldInfoIndex = 0;

	memset(m_UserInfoBuf, NULL, 8192);
	m_UserInfoIndex = 0;

	for (int i = 0; i < MAX_MAGIC_NUM; i++)
	{
		m_UserMagic[i].sMid = -1;
		m_UserMagic[i].byType = 1;
	}

	for (int i = 0; i < MAGIC_BELT_SIZE; i++)
	{
		m_UserMagicBelt[i].sMid = -1;
		m_UserMagicBelt[i].byType = 1;
	}

	m_nHaveMagicNum = 0;

	DWORD dwCurrTime = GetTickCount();
	m_dwLastRecoverStmTime = dwCurrTime;
	m_dwLastAttackTime = dwCurrTime;

	m_iTalkToNpc = -1; // Reset the Npc user talking to
	for (int i = 0; i < 4; i++) // Reset Select Msg
	{
		m_iSelectMsgResult[i] = -1;
	}

	InitEventSlot();

	m_dwNoDamageTime		= 0;
	m_dwLastNoDamageTime	= dwCurrTime;

	m_bIsUsingWarpItem = false;
	m_bIsWarping = false;
	m_nWarpZone = -1;
	m_nWarpX = -1;
	m_nWarpY = - 1;

	m_dPlusValueCountRemainder = 0;

	m_iEventNpcId = 0;

	m_bNewUser = false;

	m_bHidden = false;

	m_dwGrayTime = dwCurrTime;

	m_iDeathType = 0;

	m_bMagicCasted = false;
	m_bPreMagicCasted = false;
	m_sMagicNoCasted = -1;
	m_dwMagicCastedTime = dwCurrTime;

	m_bRunMode = false;

	m_sChangeOtherItemSpecialNum = 0;
	m_sChangeOtherItemSlot = -1;

	InitRemainMagic(MAGIC_CLASS_ALL);
	InitRemainSpecial();

	m_nStoreBuyRate = 100;
	m_nStoreSellRate = 100;

	m_bGuildWar = false;
	m_bGuildWarOk = false;
	m_bInGuildWar = false;
	m_sGuildWar = -1;
	m_bGuildWarDead = true;
	m_iGuildWarUid = -1;
	ZeroMemory(m_strGuildWarUser, sizeof(m_strGuildWarUser));

	m_sGuildRank = -1;
	ZeroMemory(m_strGuildName, sizeof(m_strGuildName));
	m_sGuildSymbolVersion = 0;

	m_iHealPlusValue = 0;

	m_iLightItemNo = -1;
	m_dwLightItemTime = 0;

	m_dwLastRecoverHPTime = dwCurrTime;
	m_dwLastRecoverMPTime = dwCurrTime;
	m_dwLastSendTime = dwCurrTime;

	m_lDeadUsed = 0;

	m_dwLastSpecialTime = dwCurrTime;
	m_dwSpecialAttackDelay = DEFAULT_ATTACK_DELAY;
	m_sSpecialAttackNo = -1;
	m_dwSpecialAttackCastedTime = dwCurrTime;

	for (int i = 0; i < HAIR_SHOP_PRICE_COUNT; i++)
	{
		m_iHairShopPrices[i] = 0;
	}

	m_bTradeWaiting = false;
	m_bTrading = false;
	m_bExchangeOk = false;
	m_iTradeUid = -1;
	ZeroMemory(m_strTradeUser, sizeof(m_strTradeUser));
	EnterCriticalSection(&m_CS_ExchangeItem);
	for (int i = 0; i < m_arExchangeItem.GetSize(); i++)
	{
		if (m_arExchangeItem[i] != NULL)
		{
			delete m_arExchangeItem[i];
			m_arExchangeItem[i] = NULL;
		}
	}
	m_arExchangeItem.RemoveAll();
	LeaveCriticalSection(&m_CS_ExchangeItem);

	m_iRecallNPCThreadOffset = 0;
	m_iRecallNPCThreadNPCOffset = 0;

	ZeroMemory(m_strInventoryOtherUserId, sizeof(m_strInventoryOtherUserId));
	m_bHasInventoryOther = false;

	m_sGuildTownWarType = -1;

	// KEEP THIS AT THE BOTTOM!!
	m_State = STATE_INITED;
}

void USER::Send(TCHAR *pBuf, int nLength, bool bRaw)
{
	CIOCPSocket2::Send(pBuf, nLength, bRaw);

	DWORD dwCurrTime = GetTickCount();
	if ((dwCurrTime - m_dwLastSendTime) > 1000)
	{
		m_dwLastSendTime = dwCurrTime;
		if (m_State == STATE_GAMESTARTED)
		{
			PlusHpMp(dwCurrTime);
			GetStm(); // TODO: Should be calling PlusRange here but we have not yet implemented it.
			CheckGrayCount();
		}
	}

	if (m_State == STATE_GAMESTARTED)
	{
		//if (m_bInParty)
		//{
		//	SendCharData(INFO_EXTVALUE);
		//}
	}
}

void USER::SendAll(TCHAR *pBuf, int nLength)
{
	if (nLength <= 0 || nLength >= MAX_PACKET_SIZE) return;

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if (!pNewData)return;

	pNewData->flag = SEND_ALL;
	pNewData->len = nLength;

	CopyMemory(pNewData->pBuf, pBuf, nLength);

	m_pCom->Send(pNewData);
	if (pNewData) delete pNewData;
}

void USER::SendZone(TCHAR *pBuf, int nLength)
{
	if (nLength <= 0 || nLength >= MAX_PACKET_SIZE) return;

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if (!pNewData)return;

	pNewData->flag = SEND_ZONE;
	pNewData->len = nLength;

	CopyMemory(pNewData->pBuf, pBuf, nLength);

	pNewData->z = m_sZ;

	m_pCom->Send(pNewData);
	if (pNewData) delete pNewData;
}

void USER::SendInsight(TCHAR *pBuf, int nLength)
{
	if (nLength <= 0 || nLength >= MAX_PACKET_SIZE) return;

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;
	if (pNewData == NULL) return;

	pNewData->flag = SEND_INSIGHT;
	pNewData->len = nLength;

	CopyMemory(pNewData->pBuf, pBuf, nLength);

	pNewData->uid = 0;
	pNewData->x = m_sX;
	pNewData->y = m_sY;
	pNewData->z = m_sZ;
	pNewData->zone_index = m_iZoneIndex;

	m_pCom->Send(pNewData);
	if (pNewData) delete pNewData;
}

void USER::SendToRange(char *pBuf, int index, int min_x, int min_y, int max_x, int max_y)
{
	if (index <= 0 || index >= MAX_PACKET_SIZE) return;

	SEND_DATA* pNewData = NULL;
	pNewData = new SEND_DATA;

	if (!pNewData) return;

	pNewData->flag = SEND_RANGE;
	pNewData->len = index;

	::CopyMemory(pNewData->pBuf, pBuf, index);

	pNewData->uid = 0;
	pNewData->z = m_sZ;
	pNewData->rect.left		= min_x;
	pNewData->rect.right	= max_x;
	pNewData->rect.top		= min_y;
	pNewData->rect.bottom	= max_y;
	pNewData->zone_index = m_iZoneIndex;

	m_pCom->Send(pNewData);
	delete pNewData;
}

void USER::SendMyInfo(BYTE towho, BYTE type)
{
	if (m_bHidden && towho != TO_ME) return;

	CBufferEx	TempBuf;
	int i;

	CPoint pos = ConvertToClient(m_sX, m_sY);
	if (pos.x == -1 || pos.y == -1){ pos.x = 1; pos.y = 1; }

	TempBuf.Add(PKT_USERMODIFY);
	TempBuf.Add(type);
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add((short)m_sClass); // class
	TempBuf.Add((short)pos.x);
	TempBuf.Add((short)pos.y);

	if (type == INFO_MODIFY)
	{
		TempBuf.Add((BYTE)0); // bDead Enable
		TempBuf.AddString(m_strUserId);
		TempBuf.Add((BYTE)0); // not sure has something to do with monster soma and some kind of name or w/e
		TempBuf.AddString(m_strGuildName);

		TempBuf.Add(m_sGuildSymbolVersion); // Guild pic id
		TempBuf.Add((BYTE)m_sGuildRank); // Guild Rank
		TempBuf.Add((BYTE)m_bInGuildWar); // Guild War .. 1 = in war 0 = no war
		TempBuf.Add((short)m_sGuildTownWarType); // How to display name at GVW
		TempBuf.Add((short)m_sGroup); // Guild Num
		TempBuf.Add((short)m_sGuildWar); // The guild the user is fighting against
		TempBuf.Add((BYTE)m_bGuildWarDead); // Will turn name black during GW if 1

		TempBuf.AddString(m_PartyMembers[0].m_strUserId);

		TempBuf.Add((BYTE)g_bClassWar); // 1 if in a place where wotw is on
		TempBuf.Add((short)GetMaxHP());
		TempBuf.Add((short)m_sHP);
		TempBuf.Add((short)m_sHair);
		TempBuf.Add((short)m_sHairMode); // hair mode
		TempBuf.Add((short)m_sSkin);
		TempBuf.Add((short)m_sGender);
		TempBuf.Add((short)(m_iMoral / CLIENT_MORAL)); // Moral // TODO : Check Moral value is correctly being sent
		TempBuf.Add((BYTE)m_bLive); // 1 if alive 0 = dead
		TempBuf.Add((BYTE)1); // PKMODE 1 = Enabled 0 = Disabled
		TempBuf.Add((BYTE)m_BattleMode); // battlemode
		if (CheckRemainSpecialAttack(SPECIAL_EFFECT_SPEED))
		{
			TempBuf.Add(m_RemainSpecial[SPECIAL_EFFECT_SPEED].sDamage);
		}
		else
		{
			if (m_InvItem[ARM_RHAND].IsEmpty())
			{
				TempBuf.Add((short)DEFAULT_ATTACK_DELAY);
			}
			else
			{
				TempBuf.Add((short)m_InvItem[ARM_RHAND].sTime);
			}
		}
		TempBuf.Add((short)m_byDir); // Direction
		TempBuf.Add(GetGrayUser()); // Grey mode

		for (i = 0; i < EQUIP_ITEM_NUM; i++)
		{
			TempBuf.Add((short)m_InvItem[i].sPicNum);
			TempBuf.Add((short)m_InvItem[i].bType); // B Type
			TempBuf.Add((short)m_InvItem[i].bArm); // B Arm
		}

		TempBuf.Add((short)0); // no idea

		for (i = 0; i < MAX_MAGIC_EFFECT; i++)
		{
			DWORD dwConTime = CheckRemainMagic(i);
			if (dwConTime)
			{
				TempBuf.Add((BYTE)1); // Tell client there is a magic!
				TempBuf.Add(m_sRemainMagicNo[i]);
				TempBuf.Add(dwConTime);
			}
		}
		TempBuf.Add((BYTE)0); // tells client there are no more magic to read
		TempBuf.Add((short)CheckRemainFire()); // Remain light item
		for (i = 0; i < MAX_SPECIAL_EFFECT; i++)
		{
			DWORD dwConTime = CheckRemainSpecialAttack(i);
			if (dwConTime)
			{
				TempBuf.Add((BYTE)1); // Tell client there is a magic!
				TempBuf.Add(m_RemainSpecial[i].sMid);
				TempBuf.Add(dwConTime);
			}
		}
		TempBuf.Add((BYTE)0);
	}

	switch (towho)
	{
	case TO_ALL:
		SendAll(TempBuf, TempBuf.GetLength());
		break;
	case TO_ME:
		Send(TempBuf, TempBuf.GetLength());
		break;
	case TO_ZONE:
		SendZone(TempBuf, TempBuf.GetLength());
		break;
	case TO_INSIGHT:
	default:
		SendInsight(TempBuf, TempBuf.GetLength());
		break;
	}
}

void USER::SendCharData(BYTE bFlag)
{
	if (bFlag == 0) return;
	CBufferEx TempBuf;

	TempBuf.Add(PKT_CHARACTER_DATA);
	TempBuf.Add(m_Uid+USER_BAND);
	TempBuf.Add(bFlag);

	// Info names
	if (bFlag & INFO_NAMES)
	{
		TempBuf.AddString(m_strUserId);
		TempBuf.Add(m_iClassPoint); // User pos
		TempBuf.AddString(m_strGuildName);
		TempBuf.Add((BYTE)0); // Designation string
	}
	// info basic value
	if (bFlag & INFO_BASICVALUE)
	{
		TempBuf.Add(m_dwBarr); // Money
		TempBuf.Add((short)(m_iMoral/CLIENT_MORAL)); // Morale
		TempBuf.Add(m_sAge); // age
		TempBuf.Add(m_sGender);
		TempBuf.Add((short)(m_iSTR/100000));
		TempBuf.Add((short)(m_iDEX/1000000));
		TempBuf.Add((short)(m_iINT/100000));
		TempBuf.Add((short)(m_iWIS/1000000));
		TempBuf.Add((short)(m_iCHA/1000000)); // Charisma
		TempBuf.Add((short)(m_iCON/100000));
	}
	// info ext value
	if (bFlag & INFO_EXTVALUE)
	{
		TempBuf.Add(m_sLevel);
		TempBuf.Add(m_dwMaxExp / 100);	// max exp
		TempBuf.Add(m_dwExp / 100); // cur exp
		TempBuf.Add(GetMaxHP());
		TempBuf.Add(m_sHP);
		TempBuf.Add(GetMaxMP()); // max mp
		TempBuf.Add(m_sMP);
		TempBuf.Add(GetMaxWgt()); // max weight....
		TempBuf.Add(m_sWgt);
		TempBuf.Add(GetMaxStm()); // max stam....
		TempBuf.Add(m_sStm);
	}
	// info wep exp
	if (bFlag & INFO_WEAPONEXP)
	{
		TempBuf.Add(m_iSwordExp/10000);
		TempBuf.Add(m_iSpearExp/10000);
		TempBuf.Add(m_iAxeExp/10000);
		TempBuf.Add(m_iKnuckleExp/10000);
		TempBuf.Add(m_iBowExp/10000);
		TempBuf.Add(m_iStaffExp/10000);
	}
	// info make exp
	if (bFlag & INFO_MAKEEXP)
	{
		TempBuf.Add(m_iWeaponMakeExp/10000);
		TempBuf.Add(m_iArmorMakeExp/10000);
		TempBuf.Add(m_iAccMakeExp/10000);
		TempBuf.Add(m_iPotionMakeExp/10000);
		TempBuf.Add(m_iCookingExp/10000);
	}
	// info magic exp
	if (bFlag & INFO_MAGICEXP)
	{
		TempBuf.Add(m_iDMagicExp/10000);
		TempBuf.Add(m_iWMagicExp/10000);
		TempBuf.Add(m_iBMagicExp/10000);
	}
	if (TempBuf.GetLength() > 6) // Packet code(1) + user id(4) + flag(1)
		Send(TempBuf, TempBuf.GetLength());
}

void USER::SightRecalc()
{
	short sx, sy;
	sx = m_sX / SIGHT_SIZE_X;
	sy = m_sY / SIGHT_SIZE_Y;

	short dir_x = 0;
	short dir_y = 0;

	if (sx == m_sPrevX && sy == m_sPrevY) return;

	if (m_sPrevX == -1 || m_sPrevY == -1)
	{
		dir_x = 0;
		dir_y = 0;
	}
	else if (m_sPrevX == -2 || m_sPrevY == -2)
	{
		dir_x = DIR_OUTSIDE;
		dir_y = DIR_OUTSIDE;
	}
	else
	{
		if (sx > m_sPrevX && abs(sx-m_sPrevX) == 1)		dir_x = DIR_H;
		if (sx < m_sPrevX && abs(sx-m_sPrevX) == 1)		dir_x = DIR_L;
		if (sy > m_sPrevY && abs(sy-m_sPrevY) == 1)		dir_y = DIR_H;
		if (sy < m_sPrevY && abs(sy-m_sPrevY) == 1)		dir_y = DIR_L;
		if (abs(sx-m_sPrevX) > 1)						dir_x = DIR_OUTSIDE;
		if (abs(sy-m_sPrevY) > 1)						dir_y = DIR_OUTSIDE;
	}

	short prex = m_sPrevX;
	short prey = m_sPrevY;
	m_sPrevX = sx;
	m_sPrevY = sy;

	SendUserInfoBySightChange(dir_x, dir_y, prex, prey);
}

void USER::SendUserInfoBySightChange(short dir_x, short dir_y, short prex, short prey)
{
	short min_x = 0, min_y = 0;
	short max_x = 0, max_y = 0;

	short sx = m_sPrevX;
	short sy = m_sPrevY;

	int modify_index = 0;
	TCHAR modify_send[10000];	::ZeroMemory(modify_send, sizeof(modify_send));
	FillUserInfo(modify_send, modify_index, INFO_MODIFY);

	int delete_index = 0;
	TCHAR delete_send[4096];	::ZeroMemory(delete_send, sizeof(delete_send));
	FillUserInfo(delete_send, delete_index, INFO_DELETE);

	if (prex == -1 || prey == -1)
	{
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendItemFieldInfoToMe();
		SendRangeInfoToMe();
		return;
	}
	if (dir_x == DIR_OUTSIDE || dir_y == DIR_OUTSIDE)
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_DELETE);
		SendToRange(delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendToRange(modify_send, modify_index, min_x, min_y, max_x, max_y);
		SendItemFieldInfoToMe();
		SendRangeInfoToMe();
		return;
	}
	if (dir_x > 0)
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_DELETE);
		SendToRange(delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx+1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendToRange(modify_send, modify_index, min_x, min_y, max_x, max_y);
		SendItemFieldInfoToMe();
	}
	if (dir_y > 0)
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_DELETE);
		SendToRange(delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy+1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendToRange(modify_send, modify_index, min_x, min_y, max_x, max_y);
		SendItemFieldInfoToMe();
	}
	if (dir_x < 0)
	{
		min_x = (prex+1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_DELETE);
		SendToRange(delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendToRange(modify_send, modify_index, min_x, min_y, max_x, max_y);
		SendItemFieldInfoToMe();
	}
	if (dir_y < 0)
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey+1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_DELETE);
		SendToRange(delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy)*SIGHT_SIZE_Y;
		SendRangeInfoToMe(min_x, min_y, max_x, max_y, INFO_MODIFY);
		SendToRange(modify_send, modify_index, min_x, min_y, max_x, max_y);
		SendItemFieldInfoToMe();
	}

	SendRangeInfoToMe();
}

void USER::SendRangeInfoToMe(int min_x, int min_y, int max_x, int max_y, BYTE type)
{
	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize())return;

	int i, j, tuid;
	int throwindex;

	CPoint t;

	USER *pUser = NULL;
	CNpc *pNpc = NULL;
	ItemList* pThrowItem = NULL;

	MAP* pMap = g_Zones[m_iZoneIndex];
	if (!pMap) return;

	int index = 0;
	int index_comp = 0;
	int count = 0;

	for (i = min_y; i < max_y; i++)
	{
		if (i >= pMap->m_sizeMap.cy || i < 0)
		{
			continue;
		}

		for (j = min_x; j < max_x; j++)
		{
			if (j >= pMap->m_sizeMap.cx || j < 0)
			{
				continue;
			}

			tuid = pMap->m_pMap[j][i].m_lUser;
			if (tuid != 0)
			{
				if (tuid != m_Uid + USER_BAND)
				{
					if (tuid >= USER_BAND && tuid < NPC_BAND)
					{
						pUser = g_pUserList->GetUserUid(tuid - USER_BAND);
						if (!pUser) continue;

						if (pUser->m_State != STATE_GAMESTARTED)
						{
							continue;
						}

						if (j != pUser->m_sX || i != pUser->m_sY)
						{
							SetUid(j, i, 0);
						}
						else
						{
							AddRangeInfoToMe(pUser, type);
						}
					}
					else if (tuid >= NPC_BAND && tuid < INVALID_BAND)
					{
						pNpc = GetNpc(tuid - NPC_BAND);

						if (pNpc)
						{
							if (j != pNpc->m_sCurX || i != pNpc->m_sCurY)
							{
								SetUid(j, i, 0);
							}
							else
							{
//								SendNpcInfo(pNpc, flag);
								AddRangeInfoToMe(pNpc, type);
							}
						}
					}
				}
			}

			throwindex = pMap->m_pMap[j][i].iIndex;

			if (throwindex >= 0 && throwindex < MAX_THROW_ITEM)
			{
				if (m_pCom->m_ThrowItemArray[throwindex] == NULL)
				{
					return;
				}

				EnterCriticalSection(&m_pCom->m_critThrowItem);

				pThrowItem = m_pCom->m_ThrowItemArray[throwindex]->m_pItem;

				if (pThrowItem)
				{
					t = ConvertToClient(j, i);

					if (t.x != -1 && t.y != -1)
					{
						AddItemFieldInfoToMe(pThrowItem, type, t.x, t.y);
					}
				}

				LeaveCriticalSection(&m_pCom->m_critThrowItem);
			}
		}
	}
}

bool USER::SetUid(short x, short y, int id)
{
	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize())return false;
	MAP* pMap = g_Zones[m_iZoneIndex];
	if (!pMap) return false;

	if (pMap->m_pMap[x][y].m_bMove == MAP_NON_MOVEABLE) return false;
	if (pMap->m_pMap[x][y].m_lUser != 0 && pMap->m_pMap[x][y].m_lUser != id)return false;

	pMap->m_pMap[x][y].m_lUser = id;

	return true;
}

int USER::GetUid(short x, short y, int z /* = -1 */)
{
	if (z != -1) z = GetZoneIndex(z);
	else z = m_iZoneIndex;

	if (z < 0 || z >= g_Zones.GetSize())return false;
	MAP* pMap = g_Zones[z];
	if (!pMap) return false;

	return static_cast<int>(pMap->m_pMap[x][y].m_lUser);
}

void USER::AddRangeInfoToMe(USER *pUser, BYTE type)
{
	int index = 0;
	TCHAR pData[1024];
	index = MakeRangeInfoToMe(pUser, type, pData);
	if (index)
	{
		MYSHORT slen;
		m_UserInfoBuf[m_UserInfoIndex++] = (char) PACKET_START1;
		m_UserInfoBuf[m_UserInfoIndex++] = (char) PACKET_START2;

		slen.i = index;

		m_UserInfoBuf[m_UserInfoIndex++] = (char) (slen.b[0]);
		m_UserInfoBuf[m_UserInfoIndex++] = (char) (slen.b[1]);

		if (m_CryptionFlag == 1)
		{
			SomaEncryption.Encode_Decode((BYTE*)m_UserInfoBuf + m_UserInfoIndex, (BYTE*)pData, index);
		}
		else
		{
			memcpy(m_UserInfoBuf + index, pData, index);
		}
		m_UserInfoIndex += index;

		m_UserInfoBuf[m_UserInfoIndex++] = (char) PACKET_END1;
		m_UserInfoBuf[m_UserInfoIndex++] = (char) PACKET_END2;

		if (m_UserInfoIndex >= 8000)
			SendRangeInfoToMe();
	}
}

int USER::MakeRangeInfoToMe(USER* pUser, BYTE type, TCHAR *pData)
{
	if (!pUser) return 0;
	int index = 0;
	pUser->FillUserInfo(pData, index, type);
	return index;
}

void USER::SendRangeInfoToMe()
{
	if (m_UserInfoIndex <= 0) return;
	Send(m_UserInfoBuf, m_UserInfoIndex, true);
	memset(m_UserInfoBuf, NULL, 8192);
	m_UserInfoIndex = 0;
}

void USER::FillUserInfo(char *pBuf, int &index, BYTE type)
{
	int nLen = 0, i;

	if (m_State != STATE_GAMESTARTED && type == INFO_MODIFY) return;
	if (m_bHidden) return;

	CPoint t = ConvertToClient(m_sX, m_sY);
	if (t.x == -1 || t.y == -1) { t.x = 1; t.y = 1; }

	SetByte(pBuf, PKT_USERMODIFY, index);
	SetByte(pBuf, type, index);
	SetInt(pBuf, m_Uid + USER_BAND, index);
	SetShort(pBuf, m_sClass, index); // class
	SetShort(pBuf, t.x, index);
	SetShort(pBuf, t.y, index);

	if (type != INFO_MODIFY)
	{
		return;
	}

	SetByte(pBuf, 0, index); // bDead Enable
	SetVarString(pBuf, m_strUserId, strlen(m_strUserId), index);
	SetByte(pBuf, 0, index); // not sure
	SetVarString(pBuf, m_strGuildName, strlen(m_strGuildName), index);		// Add Guild Name
	SetShort(pBuf, m_sGuildSymbolVersion, index); // Guild pic id
	SetByte(pBuf, static_cast<BYTE>(m_sGuildRank), index); // Guild Rank
	SetByte(pBuf, m_bInGuildWar, index); // Guild War .. 1 = in war 0 = no war
	SetShort(pBuf, m_sGuildTownWarType, index); // How to display name at GVW
	SetShort(pBuf, m_sGroup, index);
	SetShort(pBuf, m_sGuildWar, index); // The guild the user is fighting against
	SetByte(pBuf, m_bGuildWarDead, index); // Will turn name black during GW if 1
	SetVarString(pBuf, m_PartyMembers[0].m_strUserId, strlen(m_PartyMembers[0].m_strUserId), index); // Party leader name
	SetByte(pBuf, (BYTE)g_bClassWar, index); // 1 if in a place where wotw is on
	SetShort(pBuf, GetMaxHP(), index);
	SetShort(pBuf, m_sHP, index);
	SetShort(pBuf, (short)m_sHair, index);
	SetShort(pBuf, (short)m_sHairMode, index); // hair mode
	SetShort(pBuf, (short)m_sSkin, index);
	SetShort(pBuf, m_sGender, index);
	SetShort(pBuf, (short)(m_iMoral/CLIENT_MORAL), index); // Morale
	SetByte(pBuf, m_bLive, index); // 1 if alive 0 = dead
	SetByte(pBuf, 1, index); // PKMODE 1 = Enabled 0 = Disabled
	SetByte(pBuf, m_BattleMode, index); // battlemode

	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_SPEED))
	{
		SetShort(pBuf, m_RemainSpecial[SPECIAL_EFFECT_SPEED].sDamage, index);
	}
	else
	{
		if (m_InvItem[ARM_RHAND].IsEmpty())
		{
			SetShort(pBuf, DEFAULT_ATTACK_DELAY, index);
		}
		else
		{
			SetShort(pBuf, m_InvItem[ARM_RHAND].sTime, index);
		}
	}
	SetShort(pBuf, (short)m_byDir, index);//SetShort(pBuf, (short)m_tDir, index);
	SetByte(pBuf, GetGrayUser(), index); // Grey mode

	for (i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		SetShort(pBuf, m_InvItem[i].sPicNum, index);
		SetShort(pBuf, m_InvItem[i].bType, index); // B Type
		SetShort(pBuf, m_InvItem[i].bArm, index); // B Arm
	}

	SetShort(pBuf, (short)0, index); // no idea

	for (i = 0; i < MAX_MAGIC_EFFECT; i++)
	{
		DWORD dwConTime = CheckRemainMagic(i);
		if (dwConTime)
		{
			SetByte(pBuf, 1, index); // Tell client there is a magic!
			SetShort(pBuf, m_sRemainMagicNo[i], index);
			SetDWORD(pBuf, dwConTime, index);
		}
	}
	SetByte(pBuf, 0, index); // tells client there are no more magic to read
	SetShort(pBuf, (short)CheckRemainFire(), index); // no idea
	for (i = 0; i < MAX_SPECIAL_EFFECT; i++)
	{
		DWORD dwConTime = CheckRemainSpecialAttack(i);
		if (dwConTime)
		{
			SetByte(pBuf, 1, index); // Tell client there is a magic!
			SetShort(pBuf, m_RemainSpecial[i].sMid, index);
			SetDWORD(pBuf, dwConTime, index);
		}
	}
	SetByte(pBuf, 0, index);
}

bool USER::IsInSight(int x, int y, int z)
{
	if (m_sZ != z)return false;

	int min_x, max_x;
	int min_y, max_y;

	int sx = m_sX / SIGHT_SIZE_X;
	int sy = m_sY / SIGHT_SIZE_Y;

	min_x = (sx-1)*SIGHT_SIZE_X; if (min_x < 0)min_x = 0;
	max_x = (sx+2)*SIGHT_SIZE_X;
	min_y = (sy-1)*SIGHT_SIZE_Y; if (min_y < 0)min_y = 0;
	max_y = (sy+2)*SIGHT_SIZE_Y;

	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize())return false;

	MAP* pMap = g_Zones[m_iZoneIndex];
	if (!pMap)return false;

	if (max_x >= pMap->m_sizeMap.cx)max_x = pMap->m_sizeMap.cx - 1;
	if (max_y >= pMap->m_sizeMap.cy)max_y = pMap->m_sizeMap.cy - 1;

	if (min_x > x || max_x < x)return false;
	if (min_y > y || max_y < y)return false;

	return true;
}

void USER::InitEventSlot()
{
	for (int i = 0; i < m_arEventNum.GetSize(); i++)
	{
		if (m_arEventNum[i])
		{
			delete m_arEventNum[i];
			m_arEventNum[i] = NULL;
		}
	}
	m_arEventNum.RemoveAll();
}

void USER::StrToHaveEventData(TCHAR *pBuf)
{
	int index = 0;
	int eventnum;

	InitEventSlot();

	if (!pBuf[0])return;

	short HaveEventNum = GetShort(pBuf, index);

	if (HaveEventNum > MAX_EVENT_NUM) HaveEventNum = MAX_EVENT_NUM;

	int* pEventNum;

	for (int i = 0; i < HaveEventNum; i++)
	{
		eventnum = GetShort(pBuf, index);

		pEventNum = new int;

		*(pEventNum) = eventnum;

		m_arEventNum.Add(pEventNum);
	}
}

void USER::UserHaveEventDataToStr(TCHAR *pBuf)
{
	int index = 0;
	int num = m_arEventNum.GetSize();

	if (num >= MAX_EVENT_NUM) num = MAX_EVENT_NUM;

	SetShort(pBuf, num, index);

	int temp_int;

	for (int i = 0; i < num; i++)
	{
		temp_int = *(m_arEventNum[i]);

		SetShort(pBuf, temp_int, index);
	}
}

bool USER::CheckTownPortal(short nItemNum)
{
	for (int i = 0; i < g_arTownPortal.GetSize(); i++)
	{
		if (g_arTownPortal[i]->nItemNum == nItemNum)
		{
			return true;
		}
	}
	return false;
}


bool USER::CheckTownPortalFixed()
{
	for (int i = 0; i < g_arTownPortalFixed.GetSize(); i++)
	{
		if (g_arTownPortalFixed[i]->nZone == m_sZ)
		{
			return true;
		}
	}
	return false;
}

bool USER::TownPortal(short nItemNum)
{
	for (int i = 0; i < g_arTownPortal.GetSize(); i++)
	{
		if (g_arTownPortal[i]->nItemNum == nItemNum)
		{
			m_nWarpZone = g_arTownPortal[i]->nMoveZone;
			m_nWarpX = g_arTownPortal[i]->nX;
			m_nWarpY = g_arTownPortal[i]->nY;
			return true;
		}
	}

	return false;
}

bool USER::TownPortalFixed()
{
	for (int i = 0; i < g_arTownPortalFixed.GetSize(); i++)
	{
		if (g_arTownPortalFixed[i]->nZone == m_sZ)
		{
			m_nWarpZone = g_arTownPortalFixed[i]->nMoveZone;
			m_nWarpX = g_arTownPortalFixed[i]->nX;
			m_nWarpY = g_arTownPortalFixed[i]->nY;
			break;
		}
	}

	bool bDemon = CheckDemon(m_sClass);
	int nTownZone = bDemon ? 9 : 1;
	if (bDemon != CheckDemon(g_mapZoneClass[m_nWarpZone]))
	{
		for (int i = 0; i < g_arTownPortalFixed.GetSize(); i++)
		{
			if (g_arTownPortalFixed[i]->nZone == nTownZone)
			{
				m_nWarpZone = g_arTownPortalFixed[i]->nMoveZone;
				m_nWarpX = g_arTownPortalFixed[i]->nX;
				m_nWarpY = g_arTownPortalFixed[i]->nY;
				return true;
			}
		}
	}
	else
	{
		return true;
	}

	return false;
}

void USER::TownPortalStart(BYTE byType, short nItemNum)
{
	m_nWarpZone = -1;
	m_nWarpX = -1;
	m_nWarpY = -1;

	if (byType == 0) // Town Portal
	{
		if (!TownPortal(nItemNum)) return;
		if (m_nWarpZone == -1 || m_nWarpX == -1 || m_nWarpY == -1) return;
	}
	else if (byType == 1) // Fixed Town Portal
	{
		if (!TownPortalFixed()) return;
		if (m_nWarpZone == -1 || m_nWarpX == -1 || m_nWarpY == -1) return;
	}
	else
	{
		return;
	}
	TownPortalReqSend();
}

void USER::TownPortalReqSend()
{
	m_bIsWarping = true;
	CBufferEx TempBuf;
	TempBuf.Add(PKT_TOWNPORTALREQ);
	TempBuf.Add(m_Uid + USER_BAND);
	SendInsight(TempBuf, TempBuf.GetLength());
}

USER* USER::GetUser(int uid)
{
	if (uid < 0 || uid + USER_BAND >= NPC_BAND)return NULL;

	return m_pCom->GetUserUid(uid);
}

void USER::SaveAllData()
{
	if (m_State != STATE_GAMESTARTED) return;

	UpdateUserData();
	UpdateItemAll();
	UpdateBeltAll();
	UpdateStorageAll();
}
