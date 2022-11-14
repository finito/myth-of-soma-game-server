#pragma once

#include "IOCPSocket2.h"
#include "MAP.h"

#include "Npc.h"
#include "EVENT.h"
#include "EVENT_DATA.h"
#include "LOGIC_ELSE.h"
#include "EXEC.h"

#include "SEncryption.h"

#include "GuildMember.h" // Defines

// Forward declarations
class COM;
class CCircularBuffer;
class CNpc;
class CStore;
class CMagicTable;
class CSpecialTable;
class CTraderExchangeTable;
class Guild;

class USER : public CIOCPSocket2
{
public:
	USER();
	virtual ~USER();

	friend class CNpc; // THIS IS HERE SO NPC Class CAN HAVE ACCESS

private:
	// Structs
	struct GuildApplicantUserInfo
	{
		short sLevel;
		int iMoral;
		short sMageType;
		int iWeaponExp[6];
		int iMakeExp[5];
	};

// Member Function
public:
	void CloseProcess();

	void Initialize();	// Called when the socket is enabled (User connects)
						// Any initilisation that should be done after a user connects and
						// before anything else should be put in here
	//
	// Packet Send Functions
	//
	void Send(TCHAR *pBuf, int nLength, bool bRaw = false); // Send a packet to the client
	void SendAll(TCHAR *pBuf, int nLength);
	void SendZone(TCHAR *pBuf, int nLength);
	void SendInsight(TCHAR *pBuf, int nLength);
	void SendToRange(char *pBuf, int index, int min_x, int min_y, int max_x, int max_y);

	//
	// Saves user data to database
	//
	void SaveAllData();

private:
	//
	// Account login functions
	//
	bool IsDoubleAccount(char *account);
	bool CheckSessionLogin(TCHAR* strAccount, TCHAR* strPassword);
	bool CheckAccountLogin(TCHAR* strAccount);
	bool LoadCharData(TCHAR *id);
	bool GetLastChar(TCHAR *id);
	bool SendCharInfo(TCHAR *strChar, TCHAR* pBuf, int& index);

	//
	// Packet Recv(Client) Functions
	//
	void Parsing(int len, TCHAR *pBuf, bool &bEncryption); // Parses a packet that came from the client!!
	void AccountLoginReq(TCHAR *pBuf);
	void NewHero(TCHAR *pBuf); // Creates a new character on the account!
	void DelHero(TCHAR *pBuf); // Deletes a character from the account!
	void LoginReq(TCHAR *pBuf); // Gets user ready for going ingame (Loads data etc...)
	void GameStart(TCHAR *pBuf); // Puts user on map etc...
	void LogOut(TCHAR *pBuf); // Full Logout
	void MoveReq(TCHAR *pBuf, BYTE PacketType); // Walk movement of user
	void RunReq(TCHAR *pBuf, BYTE PacketType); // Run movement of user
	void Chat(TCHAR *pBuf);
	void SendCharItemData();
	void ChangeItemBeltIndex(TCHAR *pBuf);
	void ChangeItemIndex(TCHAR* pBuf);
	void GetItemBelt(TCHAR *pBuf); // PKT_PUTITEM_INV
	void PutItemBelt(TCHAR *pBuf); // PKT_PUTITEM_BELT
	void ThrowItem(TCHAR* pBuf);
	void ThrowMoney(TCHAR *pBuf);
	void PickupItem(TCHAR *pBuf);
	void SetBattleMode(TCHAR* pBuf);
	void MagicReady(TCHAR* pBuf);
	void ItemUseReq(TCHAR *pBuf);
	void SetRunModeReq(TCHAR *pBuf);
	void ChangeDir(TCHAR *pBuf);
	void ClientEvent(TCHAR *pBuf);
	void SelectMsgResult(TCHAR* pBuf);
	void ClassStoreBuyReq(TCHAR *pBuf);
	void ReqUserInfo(TCHAR* pBuf);
	void BuyItem(TCHAR *pBuf);
	void SellItem(TCHAR *pBuf);
	void SaveMoney(TCHAR* pBuf);
	void TakeBackMoney(TCHAR* pBuf);
	void SaveItem(TCHAR* pBuf);
	void TakeBackItem(TCHAR* pBuf);
	void Attack(TCHAR* pBuf);
	void EncryptionStartReq(TCHAR *pBuf);
	void MagicArrow(TCHAR *pBuf);
	void SetBeLive(TCHAR *pBuf);
	void PartyDeny(TCHAR *pBuf);
	void PartyInviteResult(TCHAR *pBuf);
	void SetMagicBelt(TCHAR *pBuf);
	void ChangeOtherItem(TCHAR *pBuf);
	void MagicRail(TCHAR *pBuf);
	void MagicCircle(TCHAR *pBuf);
	void TraderExchange(TCHAR* pBuf);
	void GuildList(TCHAR* pBuf);
	void GuildMemberList(TCHAR* pBuf);
	void GuildNew(TCHAR* pBuf);
	void GuildInfo(TCHAR* pBuf);
	void GuildChangeCallName(TCHAR* pBuf);
	void GuildEditInfoReq(TCHAR* pBuf);
	void GuildEditInfo(TCHAR* pBuf);
	void GuildReqUserList(TCHAR* pBuf);
	void GuildReqUserInfo(TCHAR* pBuf);
	void GuildReq(TCHAR* pBuf);
	void GuildMoveReq(TCHAR* pBuf);
	void GuildRemoveReq(TCHAR* pBuf);
	void GuildRemoveReqBoss(TCHAR* pBuf);
	void GuildChangeRank(TCHAR* pBuf);
	void GuildContribution(TCHAR* pBuf);
	void GuildRemoveUser(TCHAR* pBuf);
	void GuildDissolve(TCHAR* pBuf);
	void SpecialAttackArrow(TCHAR* pBuf);
	void RepairItem(TCHAR *pBuf);
	void MagicCasting(TCHAR* pBuf);
	void EventView(TCHAR* pBuf);
	void SpecialAttackRail(TCHAR* pBuf);
	void SpecialAttackCircle(TCHAR* pBuf);
	void HairShopOK(TCHAR* pBuf);
	void GuildWarReqResult(TCHAR* pBuf);
	void GuildWarReqCancel(TCHAR* pBuf);
	void GuildWarOK();
	void GuildStorageOpen();
	void GuildStorageClose();
	void GuildSaveItem(TCHAR* pBuf);
	void GuildTakeBackItem(TCHAR* pBuf);
	void GuildSymbolReq();
	void GuildSymbol(TCHAR* pBuf);
	void GuildSymbolData(TCHAR* pBuf);
	void ExchangeResult(TCHAR* pBuf);
	void ExchangeItem(TCHAR* pBuf);
	void ExchangeMoney(TCHAR* pBuf);
	void ExchangeOk();
	void ExchangeCancel(TCHAR* pBuf);
	void SpecialMove(TCHAR* pBuf);

	//
	// NPC Connection
	//
	CNpc* GetNpc(int nid);
	void AddRangeInfoToMe(CNpc *pNpc, BYTE tMode);
	int MakeRangeInfoToMe(CNpc* pNpc, BYTE tMode, TCHAR *pData);

	//
	// Misc Functions
	//
	CPoint FindNearAvailablePoint_S(int x, int y, int iDistance);
	CPoint FindNearAvailablePoint_C(int x, int y, int iDistance);
	CPoint FindRandPointInRect_C(int z, int x, int y, int x2, int y2); // Finds a random empty spot on
																	  // map using given passed params
	bool IsMovable_S(int x, int y);
	bool Move_C(int x, int y, int nDist = 1); // Checks if user can move to new location
	CPoint ConvertToServer(int x, int y, int z = -1); // Converts client co ordinates to server co ordinates!
	CPoint ConvertToClient(int x, int y, int z = -1);
	bool IsMovable_C(int x, int y, int z = -1); // Check tile can be moved onto (Client Co ordinate)
	int GetZoneIndex(int iZone); // Gets the index into the zones array for a zone number
	void SetZoneIndex(int zone);
	short GetStm();
	void IncStm(short sInc);
	short DecStm(short sDec); // Decreases stamina by an amount
	bool GetDistance(int xpos, int ypos, int dist, int* ret = NULL);
	void SendTime();
	void LinkToSameZone(short nX, short nY);
	void SendHPMP();
	void ShowCurrentUser();
	bool LinkToOtherZone(int new_z, int moveX, int moveY);
	void SendZoneChange(bool bResult);
	bool ChangeGender();
	bool ChangeSkin();
	short GetMaxHP();
	short GetMaxMP();
	short GetMaxWgt();
	short GetMaxStm();
	bool CheckMyJob(int nClass);	// Dsoma characters have different classes so need a way of checking.
	void SendDummyAttack(int iTargetId, int iDamage);
	bool CheckHaveSpecialNumInBody(short sSpecialNo);
	void NpcDeadEvent(CNpc* pNpc);
	void SendItemStatusMessage(ItemList* pItem);
	void SendUserStatus(int iStatusType);
	bool IsMapRange(int iAreaNumber);
	bool IsMapRange(int iAreaNumber, short sX, short sY);
	void CheckRangeAbility();
	bool CheckDistance(USER *pUser, int iDistance);
	bool CheckAttackSuccess(USER* pTarget);
	int GetDefense();
	bool CheckCanPK(USER* pTarget);
	bool CheckSafetyZone();
	bool CheckGuildWarArmy(USER* pTarget);
	int GetDefenseHuman(USER* pTarget);
	int GetAttackHuman(USER* pTarget);
	void CheckClassPoint(USER* pTarget);
	void IsDetecterRange();
	bool CheckGray(USER* pTarget);
	void SetGrayUser(BYTE byGrayMode);
	BYTE GetGrayUser();
	void CheckGrayCount();
	void SendGrayUser(BYTE byGrayMode);
	void Dead(short sClass);
	void SendSetLive();
	bool CheckPK(USER* pTarget);
	int CompareOtherValue(int iIncType);
	CPoint RandNearPoint_S(int iDistance);
	int GetInsurancePolicy();
	void SetBeLiveInDuel();
	void SetBeLiveInPKZone();
	bool CheckRevive();
	void SetBeLiveByDetecter();
	bool IsZoneInThisServer(int zone);
	bool OpenChangeOtherItem(short sSpecialNum);
	bool CheckMinus();
	bool IsServerRank(int iRank);
	int GetMinWeaponExp(int iIncType);
	void MinusWeaponExp(int iIncType, int iInc);
	int GetMinMagicExp(int iIncType);
	void MinusMagicExp(int iIncType, int iInc);
	bool CheckOtherJobLimit(int iClass);
	void UserTimer();
	void ParseUserStatusData(TCHAR *pBuf);
	void FillUserStatusData(TCHAR *pBuf);
	void PlusHpMp(DWORD dwTime);
	void CheckNewEvent();
	bool ArrowPathFind(CPoint ptSource, CPoint ptTarget);
	bool IsMapMovable_C(int x, int y);
	bool ChangeRandItem(short sGroup);
	void GuildTownWarStart();
	void GuildTownWarEnd();

	//
	// Basic user info and sight functions
	//
	void InitUser(bool bLoggedOut); // Sets default values
	bool SetUid(short x, short y, int id); // Puts user on the map at x, y
	int GetUid(short x, short y, int z = -1);
	USER* GetUserId(TCHAR* id);
	void FillUserInfo(char *pBuf, int &index, BYTE type);
	void SendMyInfo(BYTE towho, BYTE type);
	void SendCharData(BYTE bFlag);
	void SightRecalc(); // Used to work out what data should be sent to user based on 'sight'
	void AddRangeInfoToMe(USER *pUser, BYTE type);
	int MakeRangeInfoToMe(USER* pUser, BYTE type, TCHAR *pData);
	void SendUserInfoBySightChange(short dir_x, short dir_y, short prex, short prey);
	void SendRangeInfoToMe(int min_x, int min_y, int max_x, int max_y, BYTE type); // Gathers entity's of server to user
	void SendRangeInfoToMe(); // Sends the gathered entity's data to user client
	bool IsInSight(int x, int y, int z);
	USER* GetUser(int uid);

	//
	// Database Functions
	//
	bool LoadUserData(TCHAR *szID);
	bool LoadItemData(TCHAR *strChar);
	bool IsExistCharId(TCHAR* strChar);
	bool LoadStorageData(TCHAR *strChar);
	bool UpdateUserData();
	bool InsertGuild(Guild* pGuild, BYTE& bySubResult);
	bool ChangeGuildCallName(short sRank, TCHAR* strGuildName, TCHAR* strCallName, BYTE& bySubResult);
	bool EditGuildInfo(short sGuildNum, TCHAR* strGuildInfo);
	bool GetGuildReqUserInfo(TCHAR* strGuildName, TCHAR* strApplicantName, GuildApplicantUserInfo& UserInfo, BYTE& bySubResult);
	bool InsertGuildReq(TCHAR* strApplicantName, TCHAR* strGuildName, BYTE& bySubResult);
	bool RemoveGuildReq(TCHAR* strApplicantName, TCHAR* strGuildName, BYTE& bySubResult);
	bool MoveGuildReq(TCHAR* strApplicantName, TCHAR* strGuildName, BYTE& bySubResult);
	bool ChangeGuildRank(TCHAR* strMemberName, TCHAR* strGuildName, short from, short to, BYTE& bySubResult);
	bool UpdateGuildMoney(short sGuildNum, DWORD dwGuildBarr);
	bool RemoveGuildMember(TCHAR* strMemberName, TCHAR* strGuildName, BYTE& bySubResult);
	bool RemoveGuild(TCHAR* strChiefName, TCHAR* strGuildName, BYTE& bySubResult);
	bool UpdateItemAll();
	bool UpdateBeltAll();
	bool UpdateStorageAll();
	bool GetStatus(CString& strStatus);
	bool SetStatus(CString strStatus);
	bool UpdateGuildSymbol(short sGuildNum, short sSymbolVersion, TCHAR* strSymbol);
	bool LoadGuildStorageData(TCHAR *strGuildName);
	bool UpdateGuildStorage(TCHAR *strGuildName, short sGuildSlot, short sUserSlot);
	bool UpdateGuildTownChange(short sTownNum, short sGuildNum, TCHAR* strGuildName);
	bool UpdateGuildTownLevel(short sTownNum, short sLevel);

	//
	// Chat Functions
	//
	void SendServerChatMessage(TCHAR* pMsg, BYTE towho);
	void SendServerChatMessage(UINT strID, BYTE towho);
	void NormalChat(TCHAR *pBuf);
	void ShoutChat(TCHAR *pBuf);
	void ZoneChat(TCHAR *pBuf);
	void WhisperChat(TCHAR *pBuf, bool bFirst = false);
	void GuildChat(TCHAR *pBuf, short sGroup, bool bSystem = true);
	void PartyChat(TCHAR *pBuf);
	void CommandChat(TCHAR *pBuf);
	void SendSpecialMsg(TCHAR *pMsg, BYTE type, int nWho);
	void SendSpecialMsg(UINT strID, BYTE type, int nWho);
	void SendServerMessageGameMaster(TCHAR* pMsg, BYTE towho);
	void SendServerMessageGameMaster(UINT strID, BYTE towho);
	void SendAllChatStatus();

	//
	// Item Functions
	//
	ItemList StrToUserItem(TCHAR *pBuf);
	short GetItemClass(short sNum);
	void RecalcWgt();
	bool GetEquipItemPid(TCHAR *pItemBuf, TCHAR *pBuf); // Gets picture id of an item
	int GetSendItemData(ItemList item, TCHAR* pData, BYTE bFlags);
	int GetSpecialItemData(short nItemNum, TCHAR* pData);
	int AddPlusSpecialData(short sIndex, TCHAR* pData);
	short PushItemInventory(ItemList* pItem);
	void PlusItemDur(ItemList* pItem, short Dura, bool bSubtract = false);
	void SetPlusSpecial(ItemList* item);
	bool UserThrowItem(ItemList* pThrowItem, BYTE  MaxArea);
	void SendItemInfo(short nSlot);
	void SendItemInfoChange(BYTE Belt, short Slot, BYTE Type);
	void SendChangeWgt();
	void SendMoneyChanged();
	void SendItemFieldInfoToMe();
	void AddItemFieldInfoToMe(ItemList *pItem, BYTE type, int x, int y);
	void SendItemFieldInfo(BYTE type, BYTE towho, ItemList *pItem, int x, int y);
	bool SpecialItemUse(short sItemNum, short sSlot, short sSpecial, short sOp1, short sOp2, short sOp3, bool &bItemUsed, int &iLightItemTime);
	void EffectPotion(BYTE byR, BYTE byG, BYTE byB);
	void SendGetItem(short sSlot);
	int GetSameItem(ItemList Item, int nSlot);
	int GetEmptySlot(int nSlot);
	void SendDeleteItem(BYTE Belt, short Slot);
	int GetMaxStorageCount();
	CStore* GetStore(int nStore);
	int GetItemWeight(int sNum, int sCount);
	bool CanEquipItem(ItemList* pItem);
	int PlusFromItem(int specialNo);
	void NpcThrowItem(CNpc *pNpc);
	void NpcThrowMoney(CNpc *pNpc);
	void GiveBasicItem();
	void DeadThrowItem(bool bCheckMoral, bool bSendItemDelete);
	void DeadThrowMoney(bool bSendMoneyChanged);
	bool CanTradeItem(const ItemList& item);
	void UserItemToStr(ItemList* list, TCHAR *pBuf);
	void SendRepairItem(BYTE bySuccess, BYTE bySubResult=0);
	void EffectFire();
	int CheckRemainFire();
	void SendExchangeItemFail();
	void SendExchangeItem(USER* pTradeUser, short sSlot, short sCount);

	//
	// Magic Functions
	//
	void SendCharMagicData();
	int GetSendMagicData(int iMid, TCHAR* pData);
	int GetSendSpecialAttackData(int iMid, TCHAR* pData);
	int GetSendAbilityData(int iMid, TCHAR* pData);
	void ParseHaveMagicData(TCHAR *pBuf);
	bool IsHaveMagic(short sMid, BYTE byMagicType);
	bool IsHaveMagicMageType(int iMageType);
	DWORD CheckRemainMagic(int iMagicEffectValue); // Returns the remaining time of a magic effect.
	bool CheckRemainMagicAll(); // Checks if there are any remaining magic effects on the user.
	void InitRemainMagic(int iMagicClass);
	void InitRemainSpecial();
	bool CheckMagicDelay(CMagicTable* pMagic);
	int GetMagicDamage(CMagicTable* pMagic, CNpc* pNpc);
	int GetMagicDamage(CMagicTable* pMagic, USER* pUser);
	int GetMagicDefense(CMagicTable* pMagic, CNpc* pNpc);
	int GetMagicDefense(CMagicTable* pMagic, USER* pUser);
	void SendMagicReady(BYTE bySuccess, int iTargetId, short sMagicNo, BYTE byDir, DWORD dwCastDelay=0);
	void SendMagicAttack(BYTE bySuccess, int iTargetId, short sMagicNo=0, short sTargetHP=0, short sTargetMaxHP=0, int iContinueTime=0);
	void SendMagicRailAttack(BYTE bySuccess, BYTE byDistance=0, short sTargetCount=0, int* iTargetsId=0, short sMagicNo=0, short* sTargetsHP=0, short* sTargetsMaxHP=0, int iContinueTime=0);
	void SendMagicCircleAttack(BYTE bySuccess, BYTE byDistance=0, short sTargetCount=0, int* iTargetsId=0, short sMagicNo=0, short* sTargetsHP=0, short* sTargetsMaxHP=0, int iContinueTime=0);
	bool CheckPlusType(CMagicTable* pMagic, int iType);
	int DoMagicEffect(CMagicTable* pMagic, CNpc* pNpc);
	int DoMagicEffect(CMagicTable* pMagic, USER* pUser);
	DWORD CheckRemainSpecialAttack(int iSpecialEffectValue);
	void EventMagic(short sMagicNo);
	void EventMagicArrow(CMagicTable* pMagic);
	void SetMagicExp(CMagicTable* pMagic);
	void PlusMagicExp(int iMagicClass, int iPlusMagicExp);
	void FillMagicData(TCHAR* pBuf);
	bool CheckSuccessSpecialAttackByItem(CSpecialTable* pSpecial);
	bool CheckPlusType(CSpecialTable* pSpecial, int iType);
	void SendSpecialAttackArrow(BYTE bySuccess, short sSpecialNo=0, int iContinueTime=0, int iTargetId=0, short sTargetHP=0, short sX=-1, short sY=-1);
	bool CheckPlusValueByHeal(int iHeal, int iDamage);
	void SendMagicCasting(BYTE bySuccess, short sMagic = 0, int iStartTime = 0);
	bool CheckSuccessSpecialAttack(CSpecialTable* pSpecial, USER* pTarget);
	bool CheckSpecialAttackDelay(CSpecialTable* pSpecial);
	void SendSpecialAttackCancel(int iSpecialEffectValue);
	void EventSAttack(short sSpecialNo);
	void EventSAttackArrow(CSpecialTable* pSpecial);
	int CalcStopTime(short sLevelSource, short sLevelTarget);
	void SendSpecialAttackRail(BYTE bySuccess, int iTargetCount=0, int* iTargetsId=0, short sSpecialNo=0, short* sTargetsHP=0, int iContinueTime=0);
	void SendSpecialAttackCircle(BYTE bySuccess, int iTargetCount=0, int* iTargetsId=0, short sSpecialNo=0, short* sTargetsHP=0, int iContinueTime=0);

	//
	// Npc Event (EXEC)
	//
	void NpcEvent(TCHAR *pBuf);
	bool CheckEventLogic(EVENT_DATA *pEventData);
	bool RunNpcEvent(CNpc *pNpc, EXEC *pExec);
	void SendSelectMsg(int* iSelMsg);
	int GetNpcChatIndex(int iChatNum);
	EVENT* GetEventInCurrentZone();
	void AddMyEventNum(int sEventNum);
	void ChangeCha(int iAmount);
	void ChangeDex(int iAmount);
	void ChangeInt(int iAmount);
	void ChangeStr(int iAmount);
	void ChangeWis(int iAmount);
	void SendClassStoreOpen(int sStore);
	void DelMyEventNum(int iEventNum);
	void GiveMoney(int money);
	void GiveItem(int sNum, int iCount);
	void GiveMagic(short sMid, BYTE byMagicType);
	void SendMakerOpen(int* iMakers);
	void SendMsgBox(int iType, int nChatNum);
	void SendOpenSpecialMake(int iMake);
	void RepairItemOpenReq();
	void RobMoney(int money);
	void RobItem(int sNum, int num);
	void RobMagic(short sMid, BYTE byMagicType);
	void ShowMagic(short sPostMagic, short sPreMagic = -1);
	void SendNpcSay(CNpc *pNpc, int nChatNum);
	void StorageOpen();
	void SendStoreOpen(int nStore, int nBuyRate, int nSellRate);
	void ChangeExtStat(int &iStat, int iAmount);
	void ChangeExtStat(short &sStat, int iAmount);
	void OpenTrader(int* iTypes);
	void GuildOpen();
	void KillNpc();
	void LiveNpc(short sMId, int iCount, short sX, short sY);
	void OpenHairShop(int* iPrices);
	void ChangeMageType(int iMageType);
	void OpenGuildTownStone(short sTownNum);

	//
	// NPC Event (LOGIC)
	//
	bool FindEvent(int event_num);
	int FindItem(int sNum);
	bool CheckStatMinMax(int iStat, int min, int max);
	bool CheckItemSlot(int say, LOGIC_ELSE* pLE);
	bool CheckMoneyMinMax(int min, int max);
	bool CheckItemMinMax(int sNum, int min, int max);
	bool CheckLevel(int min, int max);
	bool CheckRandom(int rand);
	bool CheckItemWeight(int say, int iSid1, int iNum1, int iSid2, int iNum2,
							int iSid3, int iNum3, int iSid4, int iNum4, int iSid5, int iNum5);
	bool CheckGameTime(int iMonth, int iDay, int iHourStart, int iHourEnd);
	bool CheckFirstClick();
	bool CheckClassTown();
	bool CheckClassTownWar();
	bool CheckGuildRank(short sGuildRank);
	bool CheckAliveNpc(short sMId, int iCheck);
	bool CheckMageType(int iMageType1, int iCheck);
	bool CheckGuildTown(short sTownNum);
	bool CheckGuildTownAlly(short sTownNum);
	bool CheckGuildTownWar(short sTownNum);
	bool CheckGuildTownLevel(short sTownNum, short sLevel);

	//
	//  EVENT Data
	//
	void InitEventSlot();
	void StrToHaveEventData(TCHAR *pBuf);
	void UserHaveEventDataToStr(TCHAR *pBuf);

	//
	// Store
	//
	int GetStoreItemData(short ItemNum, short Quantity, TCHAR* pData, int iPosValue = -1, CString strName = "");
	int GetTraderItemData(CTraderExchangeTable* pTraderExchange, TCHAR* pData);

	//
	// Warp / Teleport
	//
	bool CheckTownPortal(short nItemNum);
	bool CheckTownPortalFixed();
	bool TownPortal(short nItemNum);
	bool TownPortalFixed();
	void TownPortalStart(BYTE byType, short nItemNum = 0);
	void TownPortalReqSend();
	void TownPortalEnd(TCHAR* pBuf);
	void TownPortalEndReq(TCHAR* pBuf);

	//
	// Attack
	//
	bool CheckAttackDelay();
	void SendAttackFail(int tuid);
	void SendAttackMiss(int tuid);
	void SendAttackSuccess(int tuid, short sHP, short sMaxHP);
	bool CheckAttackSuccess(CNpc* pNpc);
	int GetAttack();
	void DecreaseWeaponItemDura();
	void DecreaseArmorItemDura();
	void SetDamage(int iDamage, short sClass, bool bStopMagicCast = true);

	//
	// Stat / Skill / Inc / Dec
	//
	void CalcPlusValue(int iType, int& iPlus, bool bCheckJob = true);
	int GetInc(int nIncType);
	void PlusWeaponExpByAttack(int nWeaponExp);
	void PlusWeaponExp(int nWeaponType, int nPlusWeaponExp);
	int GetSkillExpTableIndex(int nLevel);
	int GetStatExpTableIndex(int nLevel);
	int GetConExpTableIndex(int nLevel);
	int GetChaExpTableIndex(int nLevel);
	int GetSkillExpTableDIndex(int nLevel);
	int GetStatExpTableDIndex(int nLevel);
	int GetConExpTableDIndex(int nLevel);
	int GetChaExpTableDIndex(int nLevel);
	void PlusCon(int iPlus);
	void PlusStr(int iPlus);
	void PlusDex(int iPlus);
	void PlusInt(int iPlus);
	void PlusWis(int iPlus);
	void PlusCha(int iPlus);
	int GetPlusValueCount(CNpc* pNpc, int iDamage, bool bNpcDead, int nIncType);
	void PlusMoralByNpc(int iMoral);
	void CheckLevelUp();
	void LevelUp();

	//
	// Party
	//
	bool CheckPartyLeader();
	void PartyDissolve(USER *pUser);
	void PartyWithdraw();
	void PartyWithdraw(char* strUserId);
	int GetPartyMemberCount(USER* pLeader);
	void PartyCreate(char* strUserId);
	void PartyInvite(char* strUserId);

	//
	// BBS
	//
	void BBSOpen(int iBBSNum);
	void BBSNext(TCHAR *pBuf);
	void BBSRead(TCHAR *pBuf);
	void BBSWrite(TCHAR *pBuf);
	void BBSEdit(TCHAR *pBuf);
	void BBSDelete(TCHAR *pBuf);

	//
	// Moral
	//
	bool SetMoral(int iPlusMoral);
	CString GetMoralName(int iMoral);
	bool SetMoralByPK(int iTargetMoral);

	//
	// Class War
	//
	void ClassWarStart();
	void ClassWarEnd();
	void DeleteOtherClass();

	//
	// Guild
	//
	Guild* GetGuild(short sGuildNum);
	Guild* GetGuildByName(TCHAR* strGuildName);
	int	GetEmptyGuildNum();
	void CheckGuildBossDead();
	void GuildWarEnd();
	short GetGuildNumByMember(TCHAR* strUserId);
	short GetGuildReqNumByMember(TCHAR* strUserId);
	void ReleaseGuild();
	bool CheckGuildWar();

	//
	// Debug
	//
	CString EventExecToString(BYTE byExec);
	CString EventLogicToString(BYTE byExec);

	//
	// Inline Functions
	//
	inline bool CheckDemon(short sClass)
	{
		if (sClass < DEVIL)
		{
			return false;
		}
		return true;
	}

// Member Variable
public:
	bool m_UserFlag;				// is this even needed?? hmmm

	//volatile DWORD m_RecvSeqValue;	// Packets contain a 4 byte number that helps to prevent the packet
								    // from being resent constantly. This variable stores the previous value
								    // and then is checked against the value in the next recieved packet.
									// If there is a big difference the packet is ignored.

	bool m_UserConnected;			// Used to know if the user is connected on the socket.

private:
	// Constants
	static const int CORE_BUFF_SIZE = 6000;
	static const int HUMAN = 0;
	static const int DEVIL = 10;

	//
	// Network
	//

	CCircularBuffer *m_pCB;			// Used to store packet data that is yet to be parsed.
	TCHAR m_RData[CORE_BUFF_SIZE+1];// Retrieved packet data buffer. Filled by the PullOutCore function.
	COM* m_pCom;					// Communication between other threads, users , etc???
	friend class COM;				// Allow COM class access to private stuff
	int m_Uid;						// Unique identifier for the user across the server.
	int m_iModSid;					// Something to do with index to an array of threads
									// Probably used to split the users onto different threads for some
									// things such as database access
	BYTE m_State;					// State of the user e.g. Connected, Logout, GameStarted.

	TCHAR m_strAccount[ACCOUNT_LENGTH+1];	// Same as the strUserId in NGSCUSER table
	bool m_bAccountLoggedIn; // Will be set upon a successful AccountLoginReq to true
	TCHAR m_TempBuf[MAX_PACKET_SIZE];	// Hold packet data that will eventually be sent to client

	bool m_bLogout; // Used to check if the user is logging out or not

	int m_dwLastSendTime;

	//
	// Character select
	//
	int	m_nCharNum;					// Number of characters
	TCHAR m_strCharacters[3][NAME_LENGTH+1]; // Character Names
	TCHAR m_strLastCharacter[NAME_LENGTH+1]; // Last Character Name

	//
	// User data
	//
	TCHAR m_strUserId[NAME_LENGTH+1];
	TCHAR m_strDenyMsgUserId[5][NAME_LENGTH+1];// PM Deny Msg List
	short m_sClass;
	short m_sLevel;
	int m_iSTR;
	int m_iSTRInc;
	int m_iDEX;
	int m_iDEXInc;
	int m_iINT;
	int m_iINTInc;
	int m_iCHA;
	int m_iCHAInc;
	int m_iWIS;
	int m_iWISInc;
	int m_iCON;
	int m_iCONInc;
	short m_sHP;
	short m_sMaxHP;
	short m_sMP;
	short m_sMaxMP;
	short m_sStm;
	short m_sMaxStm;
	short m_sWgt;
	short m_sMaxWgt;
	short m_sAge; // Not realy used tbh
	int m_iMoral;
	short m_sGender;
	short m_sHair;
	short m_sHairMode;
	short m_sSkin;
	short m_sGroup; // Guild number
	short m_sGroupReq; // Guild number apply list
	short m_sFame;	// What is this for????
	short m_sMageType;
	short m_sRank; // 0 Normal 1 Full Game Master etc...
	int m_iSwordExp;
	int m_iSpearExp;
	int m_iBowExp;
	int m_iAxeExp;
	int m_iKnuckleExp;
	int m_iStaffExp;
	int m_iWeaponMakeExp;
	int m_iArmorMakeExp;
	int m_iAccMakeExp;
	int m_iPotionMakeExp;
	int m_iCookingExp;
	int m_iWMagicExp; // White Magic
	int m_iBMagicExp; // Blue Magic
	int m_iDMagicExp; // Black Magic
	DWORD m_dwExp;
	DWORD m_dwMaxExp;
	DWORD m_dwBarr;
	DWORD m_dwBank;
	short m_sStartStr;
	short m_sStartDex;
	short m_sStartInt;
	short m_sStartCha;
	short m_sStartWis;
	short m_sStartCon;
	int m_iClassPoint;
	int m_iTotClassPoint;
	int m_iMoralCount;
	int m_iGrayCount;
	int m_iEventCount;
	int m_iPlayCount;
	bool m_bAllChatRecv;
	bool m_bPrivMsgRecv;
	BYTE m_byDir; // Direction user is facing
	bool m_bPreventChat; // This is set to true by police chat
	bool m_bLive;
	BYTE m_BattleMode;

	//
	// Item
	//
	ItemList m_InvItem[INV_ITEM_NUM];
	ItemList m_BeltItem[BELT_ITEM_NUM];
	ItemList m_StorageItem[STORAGE_ITEM_NUM];
	ItemList m_GuildItem[GUILD_STORAGE_ITEM_NUM];

	//
	// Magic
	//
	struct	MagicList
	{
		short sMid;
		BYTE byType;
	};
	MagicList	m_UserMagic[MAX_MAGIC_NUM];
	struct	MagicBeltList
	{
		short sMid;
		BYTE byType;
	};
	MagicBeltList m_UserMagicBelt[MAGIC_BELT_SIZE];

	int m_nHaveMagicNum;

	short m_sRemainMagicNo[MAX_MAGIC_EFFECT];
	DWORD m_dwRemainMagicTime[MAX_MAGIC_EFFECT];

	bool m_bMagicCasted;
	bool m_bPreMagicCasted;
	short m_sMagicNoCasted;
	DWORD m_dwMagicCastedTime;

	// Special
	struct SpecialList
	{
		short sMid;
		short sDamage;
		DWORD dwTime;
	};
	SpecialList m_RemainSpecial[MAX_SPECIAL_EFFECT];
	DWORD m_dwLastSpecialTime;
	DWORD m_dwSpecialAttackDelay;
	short m_sSpecialAttackNo;
	DWORD m_dwSpecialAttackCastedTime;

	//
	// Party
	//
	bool m_bPartyDeny;
	bool m_bInParty;
	bool m_bPartyInvite;
	int m_iPartyInviteUid;
	struct PartyList
	{
		int		uid;
		TCHAR	m_strUserId[NAME_LENGTH+1];
	};
	PartyList m_PartyMembers[MAX_PARTY_USER_NUM]; // Only the leader holds all the party member information
												  // members only hold the leader information.

	//
	// Map
	//
	int m_iZoneIndex;
	short m_sZ;
	short m_sX;
	short m_sY;
	short m_sPrevX;	// Used for sight recalc
	short m_sPrevY; // Used for sight recalc

	//
	//	Item Map Info Data
	//
	int	m_ItemFieldInfoCount;
	int	m_ItemFieldInfoIndex;
	TCHAR m_ItemFieldInfoBuf[8192];

	//
	//	User Map Info Data
	//
	int	m_UserInfoIndex;
	TCHAR m_UserInfoBuf[8192];

	//
	// NPC
	//
	int	m_iSelectMsgResult[4];
	int	m_iTalkToNpc;

	//
	// Event Data
	//
	CTypedPtrArray <CPtrArray, int*> m_arEventNum;
	int m_iEventNpcId; // The id of the npc that user clicked therefore activating an event.

	// Attack
	DWORD m_dwNoDamageTime;
	DWORD m_dwLastNoDamageTime;
	DWORD m_dwLastAttackTime;

	// Warp / Teleport
	bool m_bIsUsingWarpItem;
	bool m_bIsWarping;
	short m_nWarpZone;
	short m_nWarpX;
	short m_nWarpY;

	// Stat calculations
	double m_dPlusValueCountRemainder;	// Remainder of a conversion from double to int (GetPlusValueCount).
										// Therefore accumulating previous PlusValueCount that are < 1,
										// that will be added next time.

	int m_iHealPlusValue;				// Current amount of damage done when healing
										// when it reaches magic healing damage * 4 it
										// will remove that from this total and then return
										// true indicating that stats can be gained from the heal.

	// Give Basic Item
	bool m_bNewUser;

	// Hidden
	bool m_bHidden;

	// Gray from PK
	DWORD m_dwGrayTime;
	bool m_bLiveGray;	// Set when the user is killed and they are grey

	// Death Type
	int m_iDeathType;		// Used to know the type of process to use when reviving the user

	// Movement
	bool m_bRunMode;
	bool m_bCanRecoverStm;
	DWORD m_dwLastRecoverStmTime;

	// Change Other Item
	int m_sChangeOtherItemSpecialNum;
	short m_sChangeOtherItemSlot;

	// Store Rates
	int m_nStoreBuyRate;
	int m_nStoreSellRate;

	// Guild War
	bool m_bGuildWar;
	bool m_bGuildWarOk;
	bool m_bInGuildWar;
	short m_sGuildWar;
	bool m_bGuildWarDead;
	int m_iGuildWarUid;
	TCHAR m_strGuildWarUser[NAME_LENGTH+1];

	// Guild
	short m_sGuildRank;
	TCHAR m_strGuildName[GUILD_NAME_LENGTH+1];
	short m_sGuildSymbolVersion;

	// Light (Torch / Candle etc)
	int m_iLightItemNo;
	DWORD m_dwLightItemTime;

	// HP / MP Recovery
	DWORD m_dwLastRecoverHPTime;
	DWORD m_dwLastRecoverMPTime;

	// Dead (Used to ensure that user will only die once because of multi threading in NPC's)
	long m_lDeadUsed;

	// Hair Shop
	int m_iHairShopPrices[HAIR_SHOP_PRICE_COUNT];

	// Trade (Item Exchange)
	struct ExchangeItemList
	{
		short sSlot;
		short sCount;
	};
	bool m_bTradeWaiting;
	bool m_bTrading;
	bool m_bExchangeOk;
	int  m_iTradeUid;
	TCHAR m_strTradeUser[NAME_LENGTH+1];
	DWORD m_dwTradeMoney;
	CTypedPtrArray <CPtrArray, ExchangeItemList*> m_arExchangeItem;
	CRITICAL_SECTION m_CS_ExchangeItem;

	// NPC Recall
	int m_iRecallNPCThreadOffset;
	int m_iRecallNPCThreadNPCOffset;

	// Other User Inventory
	TCHAR m_strInventoryOtherUserId[NAME_LENGTH+1];
	bool m_bHasInventoryOther;
	ItemList m_OtherInvItem[INV_ITEM_NUM];
	ItemList m_OtherBeltItem[BELT_ITEM_NUM];

	// Guild Town War
	short m_sGuildTownWarType;
};
