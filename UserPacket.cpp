// UserPacket.cpp: implementation of the client packet handling part of USER class.
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

extern CSearch* g_pUserList;
extern CMy1p1EmuDlg* g_pMainDlg;
extern NpcThreadArray g_arNpcThread;

// Devil soma character basic start stat table
short DevilStartStatTable[6][10] = {
//	STR		DEX		INT		CHA		WIS		CON		HP		MP		STM		WGT
	14,		13,		5,		10,		5,		13,		34,		8,		29,		112,	// Swordsman
	15,		10,		5,		10,		5,		15,		40,		8,		20,		115,	// Knight
	10,		17,		5,		10,		5,		13,		34,		8,		41,		100,	// Archer
	17,		9,		5,		10,		5,		14,		37,		8,		17,		121,	// Warrior
	13,		15,		5,		10,		5,		12,		31,		8,		35,		105,	// Fighter
	6,		8,		15,		10,		14,		7,		22,		25,		15,		85};	// Wizard

void USER::Parsing(int len, TCHAR* pBuf, bool &bEncryption)
{
	if (g_bShutDown)
	{
		return;
	}

	int index = 0;

	if (bEncryption)
	{
		DWORD dwValue = GetDWORD(pBuf + index, index);
		DWORD tempVer = dwValue - m_Rec_val;
		GetByte(pBuf, index); // no idea but should be DF ;p (version number)
		if (tempVer > 100) return;
		m_Rec_val = dwValue;
	}

	BYTE command = GetByte(pBuf, index);

	// Before GameStart
	if (command == PKT_ENCRYPTION_START_REQ) EncryptionStartReq(pBuf + index);
	if (!bEncryption) return;

	if (m_State != STATE_GAMESTARTED)
	{
		switch (command)
		{
		case PKT_ACCOUNT_LOGIN:
			AccountLoginReq(pBuf + index);
			break;
		case PKT_NEW_HERO:
			if (m_bAccountLoggedIn) NewHero(pBuf + index);
			break;
		case PKT_DEL_HERO:
			if (m_bAccountLoggedIn) DelHero(pBuf + index);
			break;
		case PKT_LOGIN:
			if (m_bAccountLoggedIn) LoginReq(pBuf + index);
			break;
		case PKT_GAMESTART:
			if (m_bAccountLoggedIn && m_State == STATE_CONNECTED) GameStart(pBuf + index);
			break;
		}
	}

	if (m_State != STATE_GAMESTARTED) return;
	if (m_bLogout) return;

	switch (command)
	{
	case PKT_LOGOUT:
		LogOut(pBuf + index);
		break;
	case PKT_CHAT:
		Chat(pBuf + index);
		break;
	case PKT_INV_ALL:
		SendCharItemData();
		break;
	case PKT_USERMODIFY:
		ReqUserInfo(pBuf + index);
		break;
	case PKT_ITEM_USE:
		ItemUseReq(pBuf+index);
		break;
	case PKT_LIFE: // SetLive
		SetBeLive(pBuf + index);
		break;
	}

	if (!m_bLive) return;

	// GameStart
	switch (command)
	{
	case PKT_MOVEFIRST:
	case PKT_MOVEMIDDLE:
	case PKT_MOVEEND:
		MoveReq(pBuf + index, command);
		break;
	case PKT_RUN_MOVEFIRST:
	case PKT_RUN_MOVEMIDDLE:
	case PKT_RUN_MOVEEND:
		RunReq(pBuf + index, command);
		break;
	case PKT_CHANGE_ITEM_INDEX:
		ChangeItemIndex(pBuf + index);
		break;
	case PKT_BATTLEMODE:
		SetBattleMode(pBuf + index);
		break;
	case PKT_CHANGEBELTINDEX:
		ChangeItemBeltIndex(pBuf + index);
		break;
	case PKT_PUTITEM_BELT:
		PutItemBelt(pBuf + index);
		break;
	case PKT_PUTITEM_INV:
		GetItemBelt(pBuf + index);
		break;
	case PKT_ITEM_THROW:
		ThrowItem(pBuf + index);
		break;
	case PKT_ITEM_DUMP:
		ThrowMoney(pBuf + index);
		break;
	case PKT_ITEM_PICKUP:
		PickupItem(pBuf + index);
		break;
	case PKT_MAGIC_READY:
		MagicReady(pBuf + index);
		break;
	case PKT_SETRUNMODE:
		SetRunModeReq(pBuf + index);
		break;
	case PKT_CHANGEDIR:
		ChangeDir(pBuf + index);
		break;
	case PKT_CLIENTEVENT:
		ClientEvent(pBuf+index);
		break;
	case PKT_EVENTSELBOX:
		SelectMsgResult(pBuf+index);
		break;
	case PKT_CLASSPOINTBUY:
		{
			BYTE byType = GetByte(pBuf, index);
			switch (byType)
			{
			case 2: // Buy
				ClassStoreBuyReq(pBuf+index);
				break;
			}
		}
		break;
	case PKT_TOWNPORTALREQ:
		TownPortalEnd(pBuf+index);
		break;
	case PKT_TOWNPORTALEND:
		TownPortalEndReq(pBuf+index);
		break;
	case PKT_BUY_SHOP:
		BuyItem(pBuf+index);
		break;
	case PKT_SELL_SHOP:
		SellItem(pBuf+index);
		break;
	case PKT_SAVEMONEY:
		SaveMoney(pBuf+index);
		break;
	case PKT_TAKEBACKMONEY:
		TakeBackMoney(pBuf+index);
		break;
	case PKT_SAVEITEM:
		SaveItem(pBuf+index);
		break;
	case PKT_TAKEBACKITEM:
		TakeBackItem(pBuf+index);
		break;
	case PKT_ATTACK:
		Attack(pBuf+index);
		break;
	case PKT_ATTACK_ARROW:
		MagicArrow(pBuf+index);
		break;
	case PKT_ATTACK_MAGIC_RAIL:
		MagicRail(pBuf+index);
		break;
	case PKT_MAGIC_CIRCLE:
		MagicCircle(pBuf+index);
		break;
	case PKT_PARTY_DENY:
		PartyDeny(pBuf+index);
		break;
	case PKT_PARTY_INVITE_RESULT:
		PartyInviteResult(pBuf+index);
		break;
	case PKT_MAGIC_BELT:
		SetMagicBelt(pBuf+index);
		break;
	case PKT_CHANGE_OTHER_ITEM:
		ChangeOtherItem(pBuf+index);
		break;
	case PKT_BBS_NEXT:
		BBSNext(pBuf+index);
		break;
	case PKT_BBS_READ:
		BBSRead(pBuf+index);
		break;
	case PKT_BBS_WRITE:
		BBSWrite(pBuf+index);
		break;
	case PKT_BBS_EDIT:
		// BBSEdit(pBuf+index);
		break;
	case PKT_BBS_DELETE:
		// BBSDelete(pBuf+index);
		break;
	case PKT_TRADEREXCHANGE:
		TraderExchange(pBuf+index);
		break;
	case PKT_GUILD_LIST:
		GuildList(pBuf+index);
		break;
	case PKT_GUILD_MEMBER_LIST:
		GuildMemberList(pBuf+index);
		break;
	case PKT_GUILD_NEW:
		GuildNew(pBuf+index);
		break;
	case PKT_GUILD_INFO:
		GuildInfo(pBuf+index);
		break;
	case PKT_GUILD_CHANGE_POS:
		GuildChangeCallName(pBuf+index);
		break;
	case PKT_GUILD_EDIT_INFO_REQ:
		GuildEditInfoReq(pBuf+index);
		break;
	case PKT_GUILD_EDIT_INFO:
		GuildEditInfo(pBuf+index);
		break;
	case PKT_GUILD_APPLICATION_LIST:
		GuildReqUserList(pBuf+index);
		break;
	case PKT_GUILD_REQ_USER_INFO:
		GuildReqUserInfo(pBuf+index);
		break;
	case PKT_GUILD_REQ:
		GuildReq(pBuf+index);
		break;
	case PKT_GUILD_MOVE_REQ:
		GuildMoveReq(pBuf+index);
		break;
	case PKT_GUILD_REMOVE_REQ:
		GuildRemoveReq(pBuf+index);
		break;
	case PKT_GUILD_MOVE_REJECT:
		GuildRemoveReqBoss(pBuf+index);
		break;
	case PKT_GUILD_CHANGE_RANK:
		GuildChangeRank(pBuf+index);
		break;
	case PKT_GUILD_GIVE_DONATION:
		GuildContribution(pBuf+index);
		break;
	case PKT_GUILD_REMOVE_USER:
		GuildRemoveUser(pBuf+index);
		break;
	case PKT_GUILD_DISSOLVE:
		GuildDissolve(pBuf+index);
		break;
	case PKT_ATTACK_SPECIALARROW:
		SpecialAttackArrow(pBuf+index);
		break;
	case PKT_ATTACK_SPECIALRAIL:
		SpecialAttackRail(pBuf+index);
		break;
	case PKT_ATTACK_SPECIALCIRCLE:
	case PKT_ATTACK_SPECIALCIRCLE_2:
		SpecialAttackCircle(pBuf+index);
		break;
	case PKT_REPAIR_REQ:
		RepairItem(pBuf+index);
		break;
	case PKT_MAGICPRECAST:
		MagicCasting(pBuf+index);
		break;
	case PKT_QUEST_VIEW:
		EventView(pBuf+index);
		break;
	case PKT_HAIRSHOPOK:
		HairShopOK(pBuf+index);
		break;
	case PKT_GUILDWAR_REQ_RESULT:
		GuildWarReqResult(pBuf+index);
		break;
	case PKT_GUILDWAR_REQ_CANCEL:
		GuildWarReqCancel(pBuf+index);
		break;
	case PKT_GUILDWAR_OK:
		GuildWarOK();
		break;
	case PKT_GUILD_STORAGEOPEN:
		GuildStorageOpen();
		break;
	case PKT_GUILD_STORAGECLOSE:
		GuildStorageClose();
		break;
	case PKT_GUILD_SAVEITEM:
		GuildSaveItem(pBuf+index);
		break;
	case PKT_GUILD_TAKEBACKITEM:
		GuildTakeBackItem(pBuf+index);
		break;
	case PKT_GUILD_SYMBOL_CHANGE_REQ:
		GuildSymbolReq();
		break;
	case PKT_GUILD_SYMBOL_CHANGE:
		GuildSymbol(pBuf+index);
		break;
	case PKT_GUILD_SYMBOL_DATA:
		GuildSymbolData(pBuf+index);
		break;
	case PKT_TRADE_ACK:
		ExchangeResult(pBuf+index);
		break;
	case PKT_TRADE_ADDITEM:
		ExchangeItem(pBuf+index);
		break;
	case PKT_TRADE_SETMONEY:
		ExchangeMoney(pBuf+index);
		break;
	case PKT_TRADE_OK:
		ExchangeOk();
		break;
	case PKT_TRADE_CANCEL:
		ExchangeCancel(pBuf+index);
		break;
	case PKT_SPECIAL_MOVE:
		SpecialMove(pBuf+index);
	default:
		break;
	}

	if (m_State != STATE_GAMESTARTED) return;

	CheckRemainSpecialAttack(SPECIAL_EFFECT_PARALYZE);
	CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION);

	int nHour = m_sMageType % 100;
	if (nHour != g_pMainDlg->m_nHour)
	{
		int iMagicClass = m_sMageType / 100;
		switch (iMagicClass)
		{
		case MAGIC_CLASS_WHITE:
			m_iDMagicExp -= 100000;
			if (m_iDMagicExp < 0) m_iDMagicExp = 0;
			m_sMageType = 200 + g_pMainDlg->m_nHour;
			break;
		case MAGIC_CLASS_BLACK:
			m_iWMagicExp -= 100000;
			if (m_iWMagicExp < 0) m_iWMagicExp = 0;
			m_sMageType = 300 + g_pMainDlg->m_nHour;
			break;
		}
	}
}

void USER::AccountLoginReq(TCHAR* pBuf)
{
	int		index = 0;
	BYTE	result = FAIL, error_code = 0;
	int		old_index = 0;
	TCHAR	szTemp[8096];
	TCHAR	szPass[ACCOUNT_LENGTH+1];
	ZeroMemory(szPass, sizeof(szPass));

//	TRACE("AccountLoginReq Request...Check");

	// Validation checks for the account
	int	nAccountLen = GetVarString(sizeof(m_strAccount), m_strAccount, pBuf, index);
	int nPassLen = GetVarString(sizeof(szPass), szPass, pBuf, index);

	if (nAccountLen <= 0 || nAccountLen > ACCOUNT_LENGTH)
	{
		error_code = 1;
		goto result_send;
	}

	if (nPassLen <= 0 || nPassLen > ACCOUNT_LENGTH)
	{
		error_code = 1;
		goto result_send;
	}

	// Check the login details are correct
	if (!CheckSessionLogin(m_strAccount, szPass))
	{
		error_code = 1;
		goto result_send;
	}

	// Check if the account is already logged in.
	if (IsDoubleAccount(m_strAccount))
	{
		error_code = 1;
		goto result_send;
	}

	// Retrieve last character logged into with
	if (!GetLastChar(m_strAccount))
	{
		error_code = UNKNOWN_ERR;
		goto result_send;
	}

	// Retrieve character data on the account
	if (!LoadCharData(m_strAccount))
	{
		// Load Character Data Fail...
		// ErrorLog("Error Loading Char Data\n");
		error_code = UNKNOWN_ERR;
		goto result_send;
	}

	m_bAccountLoggedIn = true;
	result = SUCCESS;

result_send:

	index = 0;
	SetByte(m_TempBuf, PKT_ACCOUNT_LOGIN, index);
	SetByte(m_TempBuf, result, index);

	old_index = index;
	::CopyMemory(szTemp, m_TempBuf, old_index);

	if (result == SUCCESS)
	{
		int nLastCharNum = 0;
		for (int i = 0; i < 3; i++)
		{
			if (_stricmp(m_strCharacters[i], m_strLastCharacter) == 0)
			{
				nLastCharNum = i;
				break;
			}
		}
		SetByte(m_TempBuf, nLastCharNum, index); // Last logged in character index???
		SetByte(m_TempBuf, (BYTE)m_nCharNum, index);
	}
	else
	{
		SetByte(m_TempBuf, error_code, index);
		Send(m_TempBuf, index);
		Close();
		return;
	}

	if (m_nCharNum != 0 && result == SUCCESS)
	{
		for (int i = 0; i < 3; i++)
		{
			if (m_strCharacters[i][0])
			{
				if (!SendCharInfo(m_strCharacters[i], m_TempBuf, index))
				{
					// ErrorLog("Error Loading Char Info\n");
					result = FAIL;
					error_code = UNKNOWN_ERR;
					index = 0;
					SetByte(m_TempBuf, PKT_ACCOUNT_LOGIN, index);
					SetByte(m_TempBuf, result, index);
					SetByte(m_TempBuf, error_code, old_index);
					Send(m_TempBuf, old_index);
					Close();
					return;
				}
			}
		}
	}

//	TODO: UpdateCurrentUserTable();
//	ErrorLog(m_TempBuf, index);

	Send(m_TempBuf, index);
}

void USER::NewHero(TCHAR* pBuf)
{
	// Check if there is room for another character
	if (m_nCharNum >= 3)
	{
		goto fail_send;
	}

	int index = 0;

	// Get and validate the name of new character
	TCHAR strChar[NAME_LENGTH+1];

	int	nNameLen = GetVarString(sizeof(strChar), strChar, pBuf, index);
	if (nNameLen <= 0 || nNameLen > NAME_LENGTH ||
	   !strChar)
	{
		goto fail_send;
	}

	// Basic stats
	short sSTR = 0;
	short sDEX = 0;
	short sINT = 0;
	short sCHA = 0;
	short sWIS = 0;
	short sCON = 0;
	short sHP  = 0;
	short sMP  = 0;
	short sStm = 0;
	short sWgt = 0;

	// Class of new character
	short sClass = GetShort(pBuf, index);

	if (!CheckDemon(sClass))
	{
		// Stats for Hsoma characters
		sSTR = GetShort(pBuf, index);
		sDEX = GetShort(pBuf, index);
		sINT = GetShort(pBuf, index);
		sCHA = GetShort(pBuf, index);
		sWIS = GetShort(pBuf, index);
		sCON = GetShort(pBuf, index);

		// Validate maximum stat points used and stat ranges
		short sTotalStatPoints = sSTR + sDEX + sINT + sCHA + sWIS + sCON;
		if (sTotalStatPoints > MAX_TOTAL_START_STAT ||
			(sSTR < MIN_START_STAT || sSTR > MAX_START_STAT) ||
			(sDEX < MIN_START_STAT || sDEX > MAX_START_STAT) ||
			(sINT < MIN_START_STAT || sINT > MAX_START_STAT) ||
			(sCHA < MIN_START_STAT || sCHA > MAX_START_STAT) ||
			(sWIS < MIN_START_STAT || sWIS > MAX_START_STAT) ||
			(sCON < MIN_START_STAT || sCON > MAX_START_STAT))
		{
			goto fail_send;
		}

		sHP  = (sCON * 3) - 5;
		sMP  = (sWIS * 3) - 20;
		sStm = (sINT * 3) - 10;
		sWgt = (sSTR * 3) + 70;
	}
	else
	{
		// Stats for Dsoma characters
		int idx = sClass - DEVIL - 1;
		if (idx > 5) goto fail_send; // Stops invalid idx into array...

		sSTR = DevilStartStatTable[idx][0];
		sDEX = DevilStartStatTable[idx][1];
		sINT = DevilStartStatTable[idx][2];
		sCHA = DevilStartStatTable[idx][3];
		sWIS = DevilStartStatTable[idx][4];
		sCON = DevilStartStatTable[idx][5];
		sHP	 = DevilStartStatTable[idx][6];
		sMP	 = DevilStartStatTable[idx][7];
		sStm = DevilStartStatTable[idx][8];
		sWgt = DevilStartStatTable[idx][9];
	}

	short sGender = GetShort(pBuf, index);
	short sHair = GetShort(pBuf, index);
	short sHairMode = GetShort(pBuf, index);
	short sSkin = GetShort(pBuf, index);

	// Check if character already exists in database
	if (IsExistCharId(strChar))
	{
		goto fail_send;
	}

	// Check for empty slot in character name array
	bool found = false;
	for (int i = 0; i < 3; i++)
	{
		if (m_strCharacters[i])
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		// Not found empty slot!
		goto fail_send;
	}

	// Find a location to place new character on starting zone of race
	short sX = 0;
	short sY = 0;
	short sZ = 0;
	if (!CheckDemon(sClass))
	{
		CPoint ptLocation = FindRandPointInRect_C(1, 277, 93, 300, 120);
		if (ptLocation.x == -1 || ptLocation.y == -1)
		{
			ptLocation.x = 291;
			ptLocation.y = 113;
		}
		// Convert sX and sY to server co ordinates for saving in database
		CPoint pt = ConvertToServer(ptLocation.x, ptLocation.y);
		sZ = 1;
		sX = static_cast<short>(pt.x);
		sY = static_cast<short>(pt.y);
	}
	else
	{
		CPoint ptLocation = FindRandPointInRect_C(9, 393, 903, 433, 903);
		if (ptLocation.x == -1 || ptLocation.y == -1)
		{
			ptLocation.x = 393;
			ptLocation.y = 903;
		}
		// Convert sX and sY to server co ordinates for saving in database
		CPoint pt = ConvertToServer(ptLocation.x, ptLocation.y);
		sZ = 9;
		sX = static_cast<short>(pt.x);
		sY = static_cast<short>(pt.y);
	}

	// Add character to database
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	// Convert to stat exp values ;o
	int iSTR = static_cast<int>(sSTR*1000000);
	int iDEX = static_cast<int>(sDEX*10000000);
	int iINT = static_cast<int>(sINT*1000000);
	int iCHA = static_cast<int>(sCHA*10000000);
	int iWIS = static_cast<int>(sWIS*10000000);
	int iCON = static_cast<int>(sCON*1000000);

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call NEW_CHAR (\'%s\',\'%s\',%d,%d,%d,%d,%d,%d,"
		"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d, %d,%d,%d,%d,%d,%d,%d,%d,%d)}"),
		m_strAccount, strChar, 0, sClass, iSTR, iDEX, iINT, iCHA, iWIS, iCON, sGender, sHair,
		sHairMode, sSkin, sHP, sMP, sWgt, sStm, sSTR, sDEX, sINT, sCHA, sWIS, sCON, sX, sY, sZ);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) goto fail_send;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("COULD NOT ALLOCATE HANDLE!\n");
		goto fail_send;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		TRACE("Could not exec query!\n");
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		goto fail_send;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	// Re-load select characters
	if (!LoadCharData(m_strAccount))
	{
		goto fail_send;
	}

	// Send success and Re-send select character data
	index = 0;
	SetByte(m_TempBuf, PKT_NEW_HERO, index);
	SetByte(m_TempBuf, SUCCESS, index);

	for (int i = 0; i < 3; i++)
	{
		if (_stricmp(m_strCharacters[i], strChar) == 0)
		{
			if (!SendCharInfo(m_strCharacters[i], m_TempBuf, index))
			{
				// ErrorLog("Error Loading Char Info\n");
				goto fail_send;
			}
		}
	}

	Send(m_TempBuf, index);
	return;

fail_send:
	index = 0;
	SetByte(m_TempBuf, PKT_NEW_HERO, index);
	SetByte(m_TempBuf, FAIL, index);
	Send(m_TempBuf, index);
}

void USER::DelHero(TCHAR* pBuf)
{
	int index = 0;

	// Get and validate the name of character to delete
	TCHAR strChar[NAME_LENGTH+1];

	int	nNameLen = GetVarString(sizeof(strChar), strChar, pBuf, index);
	if (nNameLen <= 0 || nNameLen > NAME_LENGTH ||
	   !strChar)
	{
		return; // Any point sending a fail packet?
	}

	// Make sure character is on the account!
	int idx = -1;
	for (int i = 0; i < 3; i++)
	{
		if (_stricmp(m_strCharacters[i], strChar) == 0)
		{
			idx = i;
			break;
		}
	}

	if (idx == -1)
	{
		// Not on account get us out of here!
		// (Any point sending a fail packet?)
		return;
	}

	TCHAR			szSQL[1024]; ::ZeroMemory(szSQL, sizeof(szSQL));
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call DEL_CHAR (\'%s\', \'%s\', %d, ?)}"), m_strAccount, strChar, idx);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index); // There is currently only going to be 1 database
	if (!pDB) return;

	SQLHSTMT hstmt = NULL;
	SQLRETURN retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		return;
	}

	SQLSMALLINT	sRet = 0;
	SQLINTEGER sRetInd = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1 ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sRet, 0, &sRetInd);
	if (retcode != SQL_SUCCESS)
	{
		SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		return;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}
	else
	{
		while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
		{
			SQLFetch(hstmt);
		}
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	// TODO: if (sRet == 0) return false; sRet will be used to send to client how many days
	// until there character can be deleted...

	BYTE result = FAIL;

	// Re-load select characters
	if (LoadCharData(m_strAccount))
	{
		// Check that the character has gone!!
		int idx = -1;
		for (int i = 0; i < 3; i++)
		{
			if (_stricmp(m_strCharacters[i], strChar) == 0)
			{
				idx = i;
				break;
			}
		}
		if (idx == -1)
		{
			result = SUCCESS;

			GetGuild(-1); // Enters critical section
			for (int i = 0; i < g_arGuild.GetSize(); i++)
			{
				Guild* pGuild = g_arGuild[i];
				if (pGuild && pGuild->m_sNum != -1)
				{
					pGuild->RemoveMember(strChar);
					pGuild->RemoveApplicant(strChar);
				}
			}
			ReleaseGuild(); // Leaves critical section
		}
	}

	// Send result to client
	index = 0;
	SetByte(m_TempBuf, PKT_DEL_HERO, index);
	SetByte(m_TempBuf, result, index);
	if (result == SUCCESS)
	{
		// For some odd reason the client would like to know
		// the name of character deleted even tho the client
		// told us in the first place o___O
		SetVarString(m_TempBuf, strChar, nNameLen, index);
	}
	Send(m_TempBuf, index);
}

void USER::LoginReq(TCHAR* pBuf)
{
	BYTE result = FAIL;
	int index = 0;

	// Get and validate the name of character
	TCHAR strChar[NAME_LENGTH+1];
	int	nNameLen = GetVarString(sizeof(strChar), strChar, pBuf, index);
	if (nNameLen <= 0 || nNameLen > NAME_LENGTH ||
	   !strChar)
	{
		return; // Any point sending a fail packet?
	}

	// Init user with default values
	InitUser(0);

	// Not logging out
	m_bLogout = false;

	// Check the account is logged in
	if (!CheckAccountLogin(m_strAccount))
	{
		goto result_send;
	}

	// See if character already in memory
	// if it can't be found or logged out or disconnected then its ok
	// to carry on.s
	USER* pDoubleUser = GetUserId(strChar);
	if (pDoubleUser != NULL)
	{
		if (pDoubleUser->m_State != STATE_DISCONNECTED && pDoubleUser->m_State != STATE_LOGOUT)
		{
			pDoubleUser->SendServerChatMessage(IDS_USER_DOUBLE_CHAR, TO_ME);
			pDoubleUser->Close();
			goto result_send;
		}
	}

	// Load character data
	if (!LoadUserData(strChar))
	{
		CString entry;
		entry.Format("Could not load character data for %s", strChar);
		g_pMainDlg->LogToFile(entry);
		goto result_send;
	}

	// Zone is in this server?
	if (!IsZoneInThisServer(m_sZ))
	{
		CString entry;
		entry.Format("Zone %d is not in this server. Character %s", m_sZ, strChar);
		g_pMainDlg->LogToFile(entry);
		goto result_send;
	}

	// Load item data
	if (!LoadItemData(strChar))
	{
		CString entry;
		entry.Format("Could not load item data for %s", strChar);
		g_pMainDlg->LogToFile(entry);
		goto result_send;
	}

	// Load storage data
	if (!LoadStorageData(strChar))
	{
		CString entry;
		entry.Format("Could not load storage item data for %s", strChar);
		g_pMainDlg->LogToFile(entry);
		goto result_send;
	}

	SetZoneIndex(m_sZ);

	// User state should now be connected.
	m_State = STATE_CONNECTED;
	m_UserFlag = true;
	result = SUCCESS;

result_send:
	index = 0;
	SetByte(m_TempBuf, PKT_LOGIN, index);
	SetByte(m_TempBuf, result, index);
	if (result != SUCCESS)
	{
		Send(m_TempBuf, index);
		return;
	}
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	CPoint pt = ConvertToClient(m_sX, m_sY);
	SetShort(m_TempBuf, pt.x, index);
	SetShort(m_TempBuf, pt.y, index);
	SetShort(m_TempBuf, m_sZ, index);
	SetByte(m_TempBuf, m_bNewUser, index);
	SetByte(m_TempBuf, g_mapZoneDetails[m_sZ].unknown, index);
	SetByte(m_TempBuf, g_mapZoneDetails[m_sZ].nightType, index);
	SetShort(m_TempBuf, g_mapZoneDetails[m_sZ].hasMusic, index);
	if (IsMapRange(MAP_AREA_WOTW_MAP))
	{
		SetShort(m_TempBuf, g_iClassWar, index); // wotw map
	}
	else
	{
		SetShort(m_TempBuf, -1, index); // Not wotw map
	}
	SetByte(m_TempBuf, 0, index);
	Send(m_TempBuf, index);

	m_sPrevX = -1;
	m_sPrevY = -1;
	SightRecalc();// - Sends this user other things!!!

	SendMyInfo(TO_ME, INFO_MODIFY);
	SendCharData(INFO_ALL);

	if (m_bNewUser)
	{
		GiveBasicItem();
	}

	SendCharItemData();	// InventoryAllReq
	SendTime();
	SendCharMagicData(); // MagicAllReq

	//MagicAllReq - 1
	//MagicAllReq - 2
	//MagicAllReq - 3

	// TODO: SendContributionStatus
	SendAllChatStatus();

	if (!m_bAllChatRecv)
	{
		SendServerChatMessage(IDS_BLOCKING_SHOUT, TO_ME);
	}

	if (!m_bPrivMsgRecv)
	{
		SendServerChatMessage(IDS_BLOCKING_PM, TO_ME);
	}

	if (m_bPartyDeny)
	{
		SendServerChatMessage(IDS_BLOCKING_PARTY, TO_ME);
	}

	SendServerChatMessage(IDS_WELCOME_MSG, TO_ME);

	// Guild Town warp stuff
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (!g_bGuildTownWar[i])
			continue;

		GuildTownData* GuildTown = g_arGuildTownData[i];
		if (GuildTown == NULL)
			continue;

		CRect GuildTownRect;
		switch (GuildTown->sTownNum)
		{
		case 1:
			{
			CPoint pt1 = ConvertToServer(223, 281, 11);
			CPoint pt2 = ConvertToServer(277, 315, 11);
			GuildTownRect.SetRect(pt1.x, pt1.y, pt2.x, pt2.y);
			}
			break;
		case 2:
			{
			CPoint pt1 = ConvertToServer(162, 794, 1);
			CPoint pt2 = ConvertToServer(267, 793, 1);
			GuildTownRect.SetRect(pt1.x, pt1.y, pt2.x, pt2.y);
			}
			break;
		default:
			break;
		}

		if (GuildTown->sTownNum == 1)
		{
			if (m_sZ != 1 && m_sZ != 8)
				continue;

			if (m_sZ == 1 && !GuildTownRect.PtInRect(CPoint(m_sX, m_sY)))
				continue;
		}
		else if (GuildTown->sTownNum == 2)
		{
			if (m_sZ != 11 && m_sZ != 13)
				continue;

			if (m_sZ == 11 && !GuildTownRect.PtInRect(CPoint(m_sX, m_sY)))
				continue;
		}

		if (!CheckGuildTown(GuildTown->sTownNum) && !CheckGuildTownAlly(GuildTown->sTownNum))
		{
			if (GuildTown->sTownNum == 1)
			{
				CPoint pt = FindRandPointInRect_C(1, 250, 812, 278, 812);
				if (pt.x == -1 || pt.y == -1)
				{
					pt.x = 242;
					pt.y = 812;
				}
				if (m_sZ != 1)
					LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
				else
					LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
			}
			else if (GuildTown->sTownNum == 2)
			{
				if (CheckDemon(m_sClass))
				{
					CPoint pt = FindRandPointInRect_C(11, 276, 324, 302, 324);
					if (pt.x == -1 || pt.y == -1)
					{
						pt.x = 276;
						pt.y = 324;
					}
					if (m_sZ != 11)
						LinkToOtherZone(11, static_cast<short>(pt.x), static_cast<short>(pt.y));
					else
						LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
				}
				else
				{
					CPoint pt = FindRandPointInRect_C(1, 277, 93, 300, 120);
					if (pt.x == -1 || pt.y == -1)
					{
						pt.x = 291;
						pt.y = 113;
					}
					LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
				}
			}
		}
	}

	if (IsMapRange(MAP_AREA_ON_WOTW_MAP))
	{
		bool bInTown = true;
		int iMoral = m_iMoral / CLIENT_MORAL;
		if (!CheckDemon(m_sClass))
		{
			if (iMoral <= -31)
			{
				bInTown = false;
			}
		}
		else
		{
			if (iMoral >= 31)
			{
				bInTown = false;
			}
		}

		if (CheckDemon(g_iClassWar) != CheckDemon(m_sClass))
		{
			if (bInTown)
			{
				if (!CheckDemon(m_sClass))
				{
					CPoint pt = FindRandPointInRect_C(1, 277, 93, 300, 120);
					if (pt.x == -1 || pt.y == -1)
					{
						pt.x = 291;
						pt.y = 113;
					}
					LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
				}
				else
				{
					CPoint pt = FindRandPointInRect_C(9, 393, 903, 433, 903);
					if (pt.x == -1 || pt.y == -1)
					{
						pt.x = 393;
						pt.y = 903;
					}
					LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
				}
			}
			else
			{
				if (!CheckDemon(m_sClass))
				{
					CPoint pt = FindRandPointInRect_C(1, 32, 126, 40, 126);
					if (pt.x == -1 || pt.y == -1)
					{
						pt.x = 32;
						pt.y = 126;
					}
					LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
				}
				else
				{
					CPoint pt = FindRandPointInRect_C(9, 307, 959, 333, 959);
					if (pt.x == -1 || pt.y == -1)
					{
						pt.x = 307;
						pt.y = 959;
					}
					LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
				}
			}
		}
	}

	if (CheckDemon(m_sClass) != CheckDemon(g_mapZoneClass[m_sZ]))
	{
		if (!CheckDemon(m_sClass))
		{
			CPoint pt = FindRandPointInRect_C(1, 277, 93, 300, 120);
			if (pt.x == -1 || pt.y == -1)
			{
				pt.x = 291;
				pt.y = 113;
			}
			LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
		else
		{
			pt = FindRandPointInRect_C(9, 393, 903, 433, 903);
			if (pt.x == -1 || pt.y == -1)
			{
				pt.x = 393;
				pt.y = 903;
			}
			LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
	}
}

void USER::GameStart(TCHAR* pBuf)
{
	m_State = STATE_GAMESTARTED;
	CPoint pt = FindNearAvailablePoint_S(m_sX, m_sY, 10);
	if (pt.x != -1 && pt.y != -1)
	{
		m_sX = static_cast<short>(pt.x);
		m_sY = static_cast<short>(pt.y);
	}

	SetUid(m_sX, m_sY, m_Uid + USER_BAND);

	m_bInGuildWar = CheckGuildWar();
	if (m_bInGuildWar)
	{
		Guild* pGuild = GetGuildByName(m_strGuildName);
		if (pGuild != NULL)
		{
			m_sGuildWar = pGuild->m_sState;
		}

		ReleaseGuild();
	}

	SendMyInfo(TO_INSIGHT, INFO_MODIFY);

	CheckRangeAbility();
}

void USER::LogOut(TCHAR* pBuf)
{
	if (m_bLogout) return;
	if (m_State != STATE_GAMESTARTED) return;

	m_bLogout = true;

	// If this player was part of a trade
	// End the trade and inform the other player involved.
	if (m_bTrading)
	{
		m_bTrading = false;
		USER* pTradeUser = GetUser(m_iTradeUid);
		if (pTradeUser)
		{
			pTradeUser->m_bTrading = false;
			int index = 0;
			SetByte(m_TempBuf, PKT_TRADE_CANCEL, index);
			pTradeUser->Send(m_TempBuf, index);
			pTradeUser->SendServerChatMessage(IDS_TRADE_CANCEL, TO_ME);
		}
	}

	// If this player was part of a trade request
	// End the trade request and inform the other player involved.
	if (m_bTradeWaiting)
	{
		m_bTradeWaiting = false;
		USER* pTradeUser = GetUser(m_iTradeUid);
		if (pTradeUser)
		{
			pTradeUser->m_bTradeWaiting = false;
			int index = 0;
			SetByte(m_TempBuf, PKT_TRADE_CANCEL, index);
			pTradeUser->Send(m_TempBuf, index);
			pTradeUser->SendServerChatMessage(IDS_TRADE_CANCEL, TO_ME);
		}
	}

	// If this player was part of pending party invitation
	// find the other player involved and inform them that the party invitation failed
	// because the player has now logged out of the game.
	if (m_bPartyInvite)
	{
		m_bPartyInvite = false;
		USER* pInvite = GetUser(m_iPartyInviteUid);
		if (pInvite)
		{
			pInvite->m_bPartyInvite = false;
			int index = 0;
			SetByte(m_TempBuf, PKT_MESSAGE, index);
			SetShort(m_TempBuf, 0x18, index);
			pInvite->Send(m_TempBuf, index);
		}
	}

	// If this player is in a party inform the other party members that they have
	// now left the party because have logged out of the game.
	// If this player is the leader of the party then the party is dissolved
	// Else the player is withdrawn from the party.
	if (m_bInParty)
	{
		if (m_PartyMembers[0].uid == m_Uid &&
			strcmp(m_PartyMembers[0].m_strUserId, m_strUserId) == 0) // Party leader so dissolve the party
		{
			PartyDissolve(this);
		}
		else	// Not the party leader so withdraw
		{
			PartyWithdraw();
		}
	}

	// Release guild storage
	GuildStorageClose();

	// end the guild war (only ends if the logging out user is guild leader)
	GuildWarEnd();

	// Warp the user if they have died when logging out
	TCHAR byType[1];
	byType[0] = LIVE_TYPE_WARP;
	SetBeLive(byType);

	// Save user data
	UpdateUserData();
	UpdateItemAll();
	UpdateBeltAll();
	UpdateStorageAll();

	// Remove the user from the map
	if (m_iZoneIndex > 0 && m_iZoneIndex < g_Zones.GetSize())
	{
		MAP* pMap = g_Zones[m_iZoneIndex];
		if (pMap &&
			m_sX > 0 && m_sX < pMap->m_sizeMap.cx &&
			m_sY > 0 && m_sY < pMap->m_sizeMap.cy)
		{
			if (g_Zones[m_iZoneIndex]->m_pMap[m_sX][m_sY].m_lUser == m_Uid + USER_BAND)
			{
				InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[m_sX][m_sY].m_lUser, 0);
			}
		}
	}

	m_State = STATE_LOGOUT; // NOTE DO NOT PUT THIS BEFORE HERE OR CHARACTERS WILL NOT SAVE!
	SendMyInfo(TO_INSIGHT, INFO_DELETE);

	if (pBuf)
	{
		int index = 0;
		short LogoutType = GetShort(pBuf, index);
		if (LogoutType == 1) // Restart
		{
			index = 0;
			SetByte(m_TempBuf, PKT_RESTARTGAME, index);
			SetByte(m_TempBuf, 0, index);
			Send(m_TempBuf, index);
			InitUser(0);
		}
	}
}

void USER::MoveReq(TCHAR* pBuf, BYTE PacketType)
{
	BYTE result = FAIL;
	int will_x = -1, will_y = -1, x = -1, y = -1;
	int dir = 0;
	int index = 0;

	// Fail to walk if currently using reflection magic
	if (CheckRemainMagic(MAGIC_EFFECT_REFLECT))
	{
		goto fail_result;
	}

	// Fail to walk if currently warping
	if (m_bIsWarping)
	{
		goto fail_result;
	}

	// Fail to walk if currently paralyzed
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_PARALYZE))
	{
		goto fail_result;
	}

	// Fail to walk if currently concussioned
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION))
	{
		goto fail_result;
	}

	x = will_x = GetShort(pBuf, index);
	y = will_y = GetShort(pBuf, index);
	dir = GetShort(pBuf, index);

	short sPrevStm = m_sStm;

	GetStm();

	if (Move_C(x, y)) result = SUCCESS;
	else m_sStm = sPrevStm;

	IncStm(1);

	m_bCanRecoverStm = false;
	m_dwLastRecoverStmTime = GetTickCount();

	m_byDir = dir;

	index = 0;
	SetByte(m_TempBuf, PacketType, index);
	SetByte(m_TempBuf, result, index);

	if (result == FAIL)
	{
fail_result:
		SetInt(m_TempBuf, m_Uid + USER_BAND, index);
		if (IsMapRange(MAP_AREA_WOTW_MAP))
		{
			SetShort(m_TempBuf, g_iClassWar, index); // wotw map
		}
		else
		{
			SetShort(m_TempBuf, -1, index); // Not wotw map
		}
		CPoint t = ConvertToClient(m_sX, m_sY);
		SetShort(m_TempBuf, t.x, index);
		SetShort(m_TempBuf, t.y, index);
		SetShort(m_TempBuf, m_sStm, index);
		SetShort(m_TempBuf, m_byDir, index);
		SendInsight(m_TempBuf, index);
		return;
	}

	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	if (IsMapRange(MAP_AREA_WOTW_MAP))
	{
		SetShort(m_TempBuf, g_iClassWar, index); // wotw map
	}
	else
	{
		SetShort(m_TempBuf, -1, index); // Not wotw map
	}
	SetShort(m_TempBuf, will_x, index);
	SetShort(m_TempBuf, will_y, index);
	SetShort(m_TempBuf, m_sStm, index);
	SetShort(m_TempBuf, m_byDir, index);
	SendInsight(m_TempBuf, index);

	if (!m_bPreMagicCasted)
	{
		m_bMagicCasted = false;
	}
	SightRecalc();

	GuildTownWarStart();
	GuildTownWarEnd();
	ClassWarStart();
	ClassWarEnd();
}

void USER::RunReq(TCHAR* pBuf, BYTE PacketType)
{
	BYTE result = FAIL;
	int index = 0;
	int wx_1 = -1, wy_1 = -1, x1 = -1, y1 = -1;
	int wx_2 = -1, wy_2 = -1, x2 = -1, y2 = -1;
	int wx_3 = -1, wy_3 = -1, x3 = -1, y3 = -1;
	int dir = 0;
	CPoint ptOld;

	// Guild War Leader Check (Leaders cannot run during guild war)
	if (m_bInGuildWar && m_sGuildRank == Guild::GUILD_CHIEF)
	{
		goto fail_result;
	}

	// Fail to run if currently using reflection magic
	if (CheckRemainMagic(MAGIC_EFFECT_REFLECT))
	{
		goto fail_result;
	}

	// Fail to run if currently warping
	if (m_bIsWarping)
	{
		goto fail_result;
	}

	// Fail to run if currently paralyzed
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_PARALYZE))
	{
		goto fail_result;
	}

	// Fail to run if currently concussioned
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION))
	{
		goto fail_result;
	}

	if (GetStm() <= 0)
	{
		goto fail_result;
	}

	if (PacketType == PKT_RUN_MOVEFIRST)
	{
		x3 = wx_3 = GetShort(pBuf, index);
		y3 = wy_3 = GetShort(pBuf, index);
	}
	x1 = wx_1 = GetShort(pBuf, index);
	y1 = wy_1 = GetShort(pBuf, index);
	x2 = wx_2 = GetShort(pBuf, index);
	y2 = wy_2 = GetShort(pBuf, index);
	dir = GetShort(pBuf, index);

//	checkTick = GetDWORD(pBuf, index);
//	if (CheckRunSpeedHack(checkTick, x1 == x2)) return;

	if (PacketType == PKT_RUN_MOVEFIRST ||
		PacketType == PKT_RUN_MOVEMIDDLE)
	{
		if (IsMovable_C(x1, y1) == FALSE) goto fail_result;
		if (Move_C(x2, y2, 2) == FALSE)
		{
			if (Move_C(x1, y1, 1) == FALSE) goto fail_result;
			wx_2 = wy_2 = -1;
		}
	}
	else if (PacketType == PKT_RUN_MOVEEND)
	{
		if (x1 == x2 && y1 == y2)
		{
			if (Move_C(x1, y1, 1) == FALSE) goto fail_result;
			wx_2 = wy_2 = -1;
		}
		else
		{
			if (IsMovable_C(x1, y1) == FALSE) goto fail_result;
			if (Move_C(x2, y2, 2) == FALSE)
			{
				if (Move_C(x1, y1, 1) == FALSE) goto fail_result;
				wx_2 = wy_2 = -1;
			}
		}
	}

	DecStm(1);

	m_byDir = dir;

	// Reset stamina timer because user is running they can no longer gain it
	m_dwLastRecoverStmTime = GetTickCount();

	index = 0;
	result = SUCCESS;
	SetByte(m_TempBuf, PacketType, index);
	SetByte(m_TempBuf, result, index);
	SetInt	(m_TempBuf, m_Uid + USER_BAND, index);
	if (IsMapRange(MAP_AREA_WOTW_MAP))
	{
		SetShort(m_TempBuf, g_iClassWar, index); // wotw map
	}
	else
	{
		SetShort(m_TempBuf, -1, index); // Not wotw map
	}
	if (PacketType == PKT_RUN_MOVEFIRST)
	{
		SetShort(m_TempBuf, wx_3, index);
		SetShort(m_TempBuf, wy_3, index);
	}
	SetShort(m_TempBuf, wx_1, index);
	SetShort(m_TempBuf, wy_1, index);
	SetShort(m_TempBuf, wx_2, index);
	SetShort(m_TempBuf, wy_2, index);
	SetShort(m_TempBuf, m_sStm, index);
	SetShort(m_TempBuf, m_byDir, index);
	SendInsight(m_TempBuf, index);

	if (!m_bPreMagicCasted)
	{
		m_bMagicCasted = false;
	}
	SightRecalc();
	return;

fail_result:
	index = 0;
	ptOld = ConvertToClient(m_sX, m_sY);
	if (ptOld.x < 0 || ptOld.y < 0) return;

	SetByte(m_TempBuf, PacketType, index);
	SetByte(m_TempBuf, FAIL, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	if (IsMapRange(MAP_AREA_WOTW_MAP))
	{
		SetShort(m_TempBuf, g_iClassWar, index); // wotw map
	}
	else
	{
		SetShort(m_TempBuf, -1, index); // Not wotw map
	}
	SetShort(m_TempBuf, ptOld.x, index);
	SetShort(m_TempBuf, ptOld.y, index);
	SetShort(m_TempBuf, wx_2, index);
	SetShort(m_TempBuf, wy_2, index);
	SetShort(m_TempBuf, m_sStm, index);
	SetShort(m_TempBuf, m_byDir, index);
	SendInsight(m_TempBuf, index);
}

void USER::Chat(TCHAR* pBuf)
{
	if (m_State != STATE_GAMESTARTED)
	{
		return;
	}

	// Cannot chat when giggled.
	if (CheckRemainMagic(MAGIC_EFFECT_GIGGLE))
	{
		return;
	}

	// compare iEventCount with 0
	if (m_iEventCount <= 0)
	{
		// SetEventCount(-1);
	}

	//if (!m_bPreventChat) Doesn't even work properly yet
	//{
	//	SendServerChatMessage(IDS_POLICE_CHAT, TO_ME);
	//	return;
	//}

	int index = 0;
	BYTE type = GetByte(pBuf, index);

	if (type == NORMAL_CHAT)
	{
		switch (pBuf[index])
		{
		case '/':
			CommandChat(pBuf + index + 1);
			break;
		case '!':
			if (pBuf[index + 1] == '!')
			{
				ShoutChat(pBuf + index + 2);
			}
			else
			{
				ZoneChat(pBuf + index + 1);
			}
			break;
		case '@':
			WhisperChat(pBuf + index + 1, true);
			break;
		case '#':
			GuildChat(pBuf + index + 1, m_sGroup, false);
			break;
		case '>':
			PartyChat(pBuf + index + 1);
			break;
		default:
			NormalChat(pBuf + index);
			break;
		}
	}
	else if (type == WHISPER_CHAT)
	{
		WhisperChat(pBuf + index);
	}
}

void USER::SendCharItemData()
{
	ItemList InvItem[INV_ITEM_NUM];
	for (int i = 0; i < INV_ITEM_NUM; i++)
	{
		InvItem[i] = m_InvItem[i];
	}

	ItemList BeltItem[BELT_ITEM_NUM];
	for (int i = 0; i < BELT_ITEM_NUM; i++)
	{
		BeltItem[i] = m_BeltItem[i];
	}

	if (m_bHasInventoryOther)
	{
		USER* pUser = GetUserId(m_strInventoryOtherUserId);
		if (pUser != NULL && pUser->m_State == STATE_GAMESTARTED)
		{
			for (int i = 0; i < INV_ITEM_NUM; i++)
			{
				InvItem[i] = m_OtherInvItem[i];
			}

			for (int i = 0; i < BELT_ITEM_NUM; i++)
			{
				BeltItem[i] = m_OtherBeltItem[i];
			}
		}
	}

	CBufferEx TempBuf;
	TempBuf.Add(PKT_INV_ALL);
	TempBuf.Add(m_Uid+USER_BAND);

	short sInvCount = 0;
	for (int i = 0; i < INV_ITEM_NUM; i++)
	{
		if (InvItem[i].sUsage > 0) sInvCount++;
	}

	short sBeltCount = 0;
	for (int i = 0; i < BELT_ITEM_NUM; i++)
	{
        if (BeltItem[i].sUsage > 0) sBeltCount++;
	}

	TempBuf.Add(sInvCount);
	TempBuf.Add(sBeltCount);

	for (int i = 0; i < INV_ITEM_NUM; i++)
	{
		if (InvItem[i].sUsage > 0)
		{
			TCHAR pData[150];
			TempBuf.Add((short)i);
			TempBuf.AddData(pData, GetSendItemData(InvItem[i], pData, ITEM_TYPE_ME));
		}
	}
	for (int i = 0; i < BELT_ITEM_NUM; i++)
	{
        if (BeltItem[i].sUsage > 0)
		{
			TCHAR pData[150];
			TempBuf.Add((short)i);
			TempBuf.AddData(pData, GetSendItemData(BeltItem[i], pData, ITEM_TYPE_ME));
		}
	}
	Send(TempBuf, TempBuf.GetLength());
}

void USER::ChangeItemBeltIndex(TCHAR* pBuf)
{
	// TODO: Add database saving stuff!!!
	int	index = 0;
	short nOldSlot = -1, nNewSlot = -1;
	BYTE Result = FAIL;
	CItemTable* pOldItem = NULL;
	ItemList TempItem;

	nOldSlot  = GetShort(pBuf, index);
	nNewSlot = GetShort(pBuf, index);

	if (nOldSlot < 0 || nOldSlot >= BELT_ITEM_NUM) goto result_send;
	if (nNewSlot < 0 || nNewSlot >= BELT_ITEM_NUM) goto result_send;

	if (!g_mapItemTable.Lookup(m_BeltItem[nOldSlot].sNum, pOldItem)) goto result_send;

	TempItem = m_BeltItem[nOldSlot];
	m_BeltItem[nOldSlot] = m_BeltItem[nNewSlot];
	// TODO: Update data in belt index of oldslot in database here
	m_BeltItem[nNewSlot] = TempItem;
	// TODO: Update data in belt index of newslot in database here
	Result = SUCCESS;

result_send:
	CBufferEx TempBuf;
	TempBuf.Add(PKT_CHANGEBELTINDEX);
	TempBuf.Add(Result);

	if (Result == SUCCESS)
	{
		TempBuf.Add(nOldSlot);
		TempBuf.Add(nNewSlot);
	}

	Send(TempBuf, TempBuf.GetLength());
}

void USER::ChangeItemIndex(TCHAR* pBuf)
{
	int	index = 0;
	short sOldNum = 0, sNewNum = 0, nOldSlot = -1, nNewSlot = -1, nEquipmentSlotChanged = -1;
	BYTE byArm = -1, Result = FAIL;

	ItemList TempItem;
	bool HasEquipmentChanged = false;
	bool IsItemNeedSwap = false;

	if (m_bTrading) goto result_send;

	nOldSlot  = GetShort(pBuf, index);
	nNewSlot = GetShort(pBuf, index);

	if (nOldSlot < 0 || nOldSlot >= INV_ITEM_NUM) goto result_send;

	CItemTable* pItemOldSlot = NULL;
	if (!g_mapItemTable.Lookup(m_InvItem[nOldSlot].sNum, pItemOldSlot)) goto result_send;

	if ((nOldSlot >= 0 && nOldSlot < EQUIP_ITEM_NUM) && (nNewSlot >= 0 && nNewSlot < EQUIP_ITEM_NUM)) // E -> E
	{
		TRACE("Equip to Equip");
	}
	else if (nNewSlot == -1 && nOldSlot >= 0 && nOldSlot < EQUIP_ITEM_NUM) // E -> I
	{
		nNewSlot = PushItemInventory(&m_InvItem[nOldSlot]);
		if (nNewSlot == -1) goto result_send;

		m_InvItem[nOldSlot].Init();
		nEquipmentSlotChanged = nOldSlot;
		HasEquipmentChanged = true;

		// Reset special attacks (auras)
		// FIXME should really be using a function to do this
		for (int i = 0; i < MAX_SPECIAL_EFFECT; i++)
		{
			if (i == SPECIAL_EFFECT_PARALYZE) continue;
			if (i == SPECIAL_EFFECT_CONCUSSION) continue;
			m_RemainSpecial[i].sMid = -1;
			m_RemainSpecial[i].sDamage = 0;
			m_RemainSpecial[i].dwTime = GetTickCount();
		}

		// If user is not wearing a staff or double hand weapon and doesn't have magic avail special
		// then the user cannot use the casted magic.
		if (!CheckHaveSpecialNumInBody(SPECIAL_MAGIC_AVAIL))
		{
			if ((!m_InvItem[ARM_RHAND].IsEmpty() && m_InvItem[ARM_RHAND].bType != TYPE_STAFF) ||
				!m_InvItem[ARM_LHAND].IsEmpty())
			{
				m_bMagicCasted = false;
			}
		}
		if (!m_InvItem[ARM_RHAND].IsEmpty())
		{
			m_bMagicCasted = false;
		}

		// TODO : ChangeItemIndex - UpdateItemIndex here...
	}
	else if ((nOldSlot >= EQUIP_ITEM_NUM && nOldSlot < INV_ITEM_NUM) && (nNewSlot >= 0 && nNewSlot < EQUIP_ITEM_NUM)) // I -> E
	{
		// Make sure that the slot the item is being put into is correct
		if (m_InvItem[nOldSlot].bType > TYPE_ACC) goto result_send;

		// Check item can be equipped
		if (!CanEquipItem(&m_InvItem[nOldSlot]))
		{
			SendServerChatMessage(IDS_USER_ITEM_CANNOT_EQUIP, TO_ME);
			goto result_send;
		}

		BYTE bArm = m_InvItem[nOldSlot].bArm;
		if (bArm == ARM_HANDS)
		{
			if (!m_InvItem[ARM_LHAND].IsEmpty() &&
				!m_InvItem[ARM_RHAND].IsEmpty())
			{
				goto result_send;
			}

			if (!m_InvItem[ARM_LHAND].IsEmpty())
			{
				short nInvSlot = PushItemInventory(&m_InvItem[ARM_LHAND]);
				if (nInvSlot == -1) goto result_send;
				m_InvItem[ARM_LHAND].Init();
				SendDeleteItem(BASIC_INV, ARM_LHAND);
				SendItemInfoChange(BASIC_INV, nInvSlot, INFO_ALL);
				SendItemInfo(ARM_LHAND);
			}

			bArm = ARM_RHAND;
		}
		else if (bArm == ARM_LHAND)
		{
			if (m_InvItem[ARM_RHAND].bArm == ARM_HANDS)
			{
				short nInvSlot = PushItemInventory(&m_InvItem[ARM_RHAND]);
				if (nInvSlot == -1) goto result_send;
				m_InvItem[ARM_RHAND].Init();
				SendDeleteItem(BASIC_INV, ARM_RHAND);
				SendItemInfoChange(BASIC_INV, nInvSlot, INFO_ALL);
				SendItemInfo(ARM_RHAND);
			}
		}
		else if (bArm == ARM_WRIST)
		{
			if (!m_InvItem[ARM_HANDS].IsEmpty() && !m_InvItem[ARM_WRIST].IsEmpty())
			{
				bArm = ARM_WRIST; // Left Accessory item
			}
			else if (!m_InvItem[ARM_HANDS].IsEmpty())
			{
				bArm = ARM_WRIST; // Right Accessory item
			}
			else
			{
				bArm = ARM_HANDS; // Left Accessory item
			}
		}

		// Don't allow items to be put into wrong equipment slot e.g. weapon into boots
		if (nNewSlot != bArm)
		{
			TRACE("This item cannot equip in this slot");
			goto result_send;
		}

		nEquipmentSlotChanged = bArm;
		HasEquipmentChanged = true;
		IsItemNeedSwap = true;

		// Reset special attacks (auras)
		// FIXME should really be using a function to do this
		for (int i = 0; i < MAX_SPECIAL_EFFECT; i++)
		{
			if (i == SPECIAL_EFFECT_PARALYZE) continue;
			if (i == SPECIAL_EFFECT_CONCUSSION) continue;
			m_RemainSpecial[i].sMid = -1;
			m_RemainSpecial[i].sDamage = 0;
			m_RemainSpecial[i].dwTime = GetTickCount();
		}

		// If user is not wearing a staff or double hand weapon and doesn't have magic avail special
		// then the user cannot use the casted magic.
		if (!CheckHaveSpecialNumInBody(SPECIAL_MAGIC_AVAIL))
		{
			if ((!m_InvItem[ARM_RHAND].IsEmpty() && m_InvItem[ARM_RHAND].bType != TYPE_STAFF) ||
				!m_InvItem[ARM_LHAND].IsEmpty())
			{
				m_bMagicCasted = false;
			}
		}
		if (!m_InvItem[ARM_RHAND].IsEmpty())
		{
			m_bMagicCasted = false;
		}
	}
	else if ((nOldSlot >=EQUIP_ITEM_NUM && nOldSlot < INV_ITEM_NUM) // I -> I
		&& (nNewSlot >= EQUIP_ITEM_NUM && nNewSlot < INV_ITEM_NUM))
	{
		// do stuff if needed...nothing here yet
		IsItemNeedSwap = true;
	}
	else // Fail
	{
		goto result_send;
	}

	// should only be reached if all checks was successful
	if (IsItemNeedSwap)
	{
		TempItem = m_InvItem[nOldSlot];
		m_InvItem[nOldSlot] = m_InvItem[nNewSlot];
		m_InvItem[nNewSlot] = TempItem;
	}

	Result = SUCCESS;

result_send:
	CBufferEx TempBuf;
	TempBuf.Add(PKT_CHANGE_ITEM_INDEX);
	TempBuf.Add(Result);

	if (Result == SUCCESS)
	{
		TempBuf.Add(nOldSlot);
		TempBuf.Add(nNewSlot);
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
	}

	if (HasEquipmentChanged)
	{
		//GetMagicItemSetting();
		SendItemInfo(nEquipmentSlotChanged);
		SendCharData(INFO_BASICVALUE | INFO_EXTVALUE);
	}

	Send(TempBuf, TempBuf.GetLength());
}

void USER::GetItemBelt(TCHAR* pBuf)
{
	// TODO: Add database saving stuff!!!
	int	index = 0;
	short nInvSlot = -1, nBeltSlot = -1, nInvSlotSame = -1;
	BYTE Result = FAIL;
	CItemTable* pBeltItem = NULL;
	ItemList TempItem;

	nBeltSlot = GetShort(pBuf, index);		// Belt slot to be moved to inventory
	nInvSlot  = GetShort(pBuf, index);		// Inventory slot that belt item moving to

	if (nInvSlot < EQUIP_ITEM_NUM || nInvSlot >= INV_ITEM_NUM) goto result_send;
	if (nBeltSlot < 0 || nBeltSlot >= BELT_ITEM_NUM) goto result_send;

	if (!g_mapItemTable.Lookup(m_BeltItem[nBeltSlot].sNum, pBeltItem)) goto result_send;

	if (m_InvItem[nInvSlot].IsEmpty() || m_InvItem[nInvSlot].bType == TYPE_POTION || m_InvItem[nInvSlot].bType == TYPE_FOOD)
	{
		for (int i = EQUIP_ITEM_NUM; i < INV_ITEM_NUM; i++)
		{
			if (m_InvItem[i].sNum == m_BeltItem[nBeltSlot].sNum)
			{
				nInvSlotSame = i;
				break;
			}
		}

		if (nInvSlotSame >= 0)
		{
			if (m_InvItem[nInvSlotSame].sUsage + m_BeltItem[nBeltSlot].sUsage > MAX_ITEM_DURA)
			{
				short sItemCountLeft = m_InvItem[nInvSlotSame].sUsage + m_BeltItem[nBeltSlot].sUsage - MAX_ITEM_DURA;

				m_BeltItem[nBeltSlot].sUsage = sItemCountLeft;
				m_InvItem[nInvSlotSame].sUsage = MAX_ITEM_DURA;
				SendItemInfoChange(BASIC_INV, nInvSlotSame, INFO_DUR);

				short sFoundInvSlot = PushItemInventory(&m_BeltItem[nBeltSlot]);
				if (sFoundInvSlot != -1)
				{
					m_BeltItem[nBeltSlot].Init();
					SendItemInfoChange(BELT_INV, nBeltSlot, INFO_ALL);
				}
				else
				{
					SendItemInfoChange(BELT_INV, nBeltSlot, INFO_DUR);
				}
			}
			else
			{
				PlusItemDur(&m_InvItem[nInvSlotSame], m_BeltItem[nBeltSlot].sUsage); // Add dura of inventory item to the belt
				// TODO: Update belt item data in database here
				m_BeltItem[nBeltSlot].Init();
				// TODO: Update inventory item data in database here
				SendItemInfoChange(BASIC_INV, nInvSlotSame, INFO_DUR);
				Result = SUCCESS + 2; // Tells client to delete item in belt
			}

		}
		else
		{
			TempItem = m_InvItem[nInvSlot];
			m_InvItem[nInvSlot] = m_BeltItem[nBeltSlot];
			m_BeltItem[nBeltSlot] = TempItem;
			// TODO: Update inventory item data in database here
			// TODO: Update belt item data in database here
			Result = SUCCESS;
		}
	}
	else
	{
		nInvSlot = PushItemInventory(&m_BeltItem[nBeltSlot]);
		if (nInvSlot != -1)
		{
			// TODO: Update item in database here
			m_BeltItem[nBeltSlot].Init();
			SendItemInfoChange(BASIC_INV, nInvSlot, INFO_ALL);
			Result = SUCCESS + 2; // Tells client to delete item in belt
		}
	}

result_send:
	CBufferEx TempBuf;
	TempBuf.Add(PKT_PUTITEM_INV);
	TempBuf.Add(Result);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::PutItemBelt(TCHAR* pBuf)
{
	int	index = 0;
	short nInvSlot = -1, nBeltSlot = -1, nBeltSlotSame = -1;
	BYTE Result = FAIL;
	CItemTable* pInvItem = NULL;
	ItemList TempItem;

	nInvSlot  = GetShort(pBuf, index);		// Inventory slot to be moved to belt
	nBeltSlot = GetShort(pBuf, index);		// Belt slot that inventory item moving to

	if (nInvSlot < 0 || nInvSlot >= INV_ITEM_NUM) goto result_send;
	if (nBeltSlot < 0 || nBeltSlot >= BELT_ITEM_NUM) goto result_send;

	if (!g_mapItemTable.Lookup(m_InvItem[nInvSlot].sNum, pInvItem)) goto result_send;

	if (m_InvItem[nInvSlot].bType == TYPE_POTION || m_InvItem[nInvSlot].bType == TYPE_FOOD)
	{
		for (int i = 0; i < BELT_ITEM_NUM; i++)
		{
			if (m_BeltItem[i].sNum == m_InvItem[nInvSlot].sNum)
			{
				nBeltSlotSame = i;
				break;
			}
		}

		if (nBeltSlotSame >= 0)
		{
			if (m_BeltItem[nBeltSlotSame].sUsage + m_InvItem[nInvSlot].sUsage > MAX_ITEM_DURA)
			{
				short sItemCountLeft = m_BeltItem[nBeltSlotSame].sUsage + m_InvItem[nInvSlot].sUsage - MAX_ITEM_DURA;
				m_InvItem[nInvSlot].sUsage = sItemCountLeft;
				m_BeltItem[nBeltSlotSame].sUsage = MAX_ITEM_DURA;
				SendItemInfoChange(BASIC_INV, nInvSlot, INFO_DUR);
				SendItemInfoChange(BELT_INV, nBeltSlotSame, INFO_DUR);
			}
			else
			{
				PlusItemDur(&m_BeltItem[nBeltSlotSame], m_InvItem[nInvSlot].sUsage); // Add dura of inventory item to the belt
				// TODO: Update belt item data in database here
				m_InvItem[nInvSlot].Init();
				// TODO: Update inventory item data in database here
				SendItemInfoChange(BELT_INV, nBeltSlotSame, INFO_DUR);
				Result = SUCCESS + 2; // Tells client to delete item in inventory
			}
		}
		else
		{
			TempItem = m_BeltItem[nBeltSlot];
			m_BeltItem[nBeltSlot] = m_InvItem[nInvSlot];
			m_InvItem[nInvSlot] = TempItem;
			// TODO: Update inventory item data in database here
			// TODO: Update belt item data in database here
			Result = SUCCESS;
		}
	}

result_send:
	CBufferEx TempBuf;
	TempBuf.Add(PKT_PUTITEM_BELT);
	TempBuf.Add(Result);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::ThrowItem(TCHAR* pBuf)
{
	int index = 0;
	BYTE Type = GetByte(pBuf, index);
	short Slot = GetShort(pBuf, index);
	short Count = GetShort(pBuf, index);
	short GiveX	= GetShort(pBuf, index);
	short GiveY = GetShort(pBuf ,index);
	CPoint	ptGive = ConvertToServer(GiveX, GiveY);

	ItemList* pMapItem = NULL;
	BYTE Result = FAIL;
	bool HasEquipmentChanged = false;

	if (m_bTrading) goto send_result_;

	if (Type == BELT_INV)
	{
		if (Slot < 0 || Slot >= BELT_ITEM_NUM) goto send_result_;
		short ItemNum = m_BeltItem[Slot].sNum;
		CItemTable* pItem = NULL;
		if (!g_mapItemTable.Lookup(ItemNum, pItem)) goto send_result_;
	}
	else if (Type == BASIC_INV)
	{
		if (Slot < 0 || Slot >= INV_ITEM_NUM) goto send_result_;
		short ItemNum = m_InvItem[Slot].sNum;
		CItemTable* pItem = NULL;
		if (!g_mapItemTable.Lookup(ItemNum, pItem)) goto send_result_;
	}
	else
	{
		goto send_result_;
	}

	if (ptGive.x < 0 || ptGive.y < 0) goto send_result_;
	if (ptGive.x >= g_Zones[m_iZoneIndex]->m_sizeMap.cx || ptGive.y >= g_Zones[m_iZoneIndex]->m_sizeMap.cy) goto send_result_;
	// NOT NEEDED because we want ppl to be able to drop while running if (ptGive.x != m_sX || ptGive.y != m_sY) goto send_result_;

	pMapItem = new ItemList;
	if (pMapItem == NULL) goto send_result_;

	if (Type == BASIC_INV)
	{
		*pMapItem = m_InvItem[Slot];
		if (m_InvItem[Slot].bType > TYPE_ACC)
		{
			if (Count > m_InvItem[Slot].sUsage) goto send_result_;
			pMapItem->sUsage = Count;
		}
		else
		{
			pMapItem->sUsage = m_InvItem[Slot].sUsage;
		}

		pMapItem->sWgt = m_InvItem[Slot].sWgt;
	}
	else if (Type == BELT_INV)
	{
		*pMapItem = m_BeltItem[Slot];
		if (m_BeltItem[Slot].bType > TYPE_ACC)
		{
			if (Count > m_BeltItem[Slot].sUsage) goto send_result_;
			pMapItem->sUsage = Count;
		}
		else
		{
			pMapItem->sUsage = m_BeltItem[Slot].sUsage;
		}

		pMapItem->sWgt = m_BeltItem[Slot].sWgt;
	}

	// Make sure the item is tradeable!
	if (!CanTradeItem(*pMapItem))
	{
		SendServerChatMessage(IDS_CANNOT_THROW_ITEM, TO_ME);
		goto send_result_;
	}

	// Remove special attacks when throwing weapon
	if (Slot == ARM_RHAND)
	{
		InitRemainSpecial();
	}

	if (UserThrowItem(pMapItem, 9))
	{
		if (Type == BELT_INV)
		{
			if (Count >= m_BeltItem[Slot].sUsage)
			{
				m_BeltItem[Slot].Init();
				Count = 0;
				// TODO: Update database here
			}
			else
			{
				PlusItemDur(&m_BeltItem[Slot], Count, true); // Sets dura of item minus the amount being droped
				if (m_BeltItem[Slot].sUsage <= 0)
				{
					m_BeltItem[Slot].Init();
					Count = 0;
				}
				else
				{
					Count = m_BeltItem[Slot].sUsage;
				}
				// TODO: Update database here
			}
		}
		else if (Type == BASIC_INV)
		{
			if (m_InvItem[Slot].bType <= TYPE_ACC) // Equipment
			{
				m_InvItem[Slot].Init();
				Count = 0;
				HasEquipmentChanged = true;
				// TODO: Update database here
			}
			else
			{
				if (Count >= m_InvItem[Slot].sUsage)
				{
					m_InvItem[Slot].Init();
					Count = 0;
					// TODO: Update database here
				}
				else
				{
					PlusItemDur(&m_InvItem[Slot], Count, true); // Sets dura of item minus the amount being droped
					if (m_InvItem[Slot].sUsage <= 0)
					{
						m_InvItem[Slot].Init();
						Count = 0;
					}
					else
					{
						Count = m_InvItem[Slot].sUsage;
					}
					// TODO: Update database here
				}
			}
		}
		TRACE("Throw item id: %d\n", pMapItem->sNum);
		Result = SUCCESS;
	}

send_result_:
	CBufferEx TempBuf;
	TempBuf.Add(PKT_ITEM_THROW);
	TempBuf.Add(Result);

	if (Result == SUCCESS)
	{
		TempBuf.Add(Type);
		TempBuf.Add(Slot);
		TempBuf.Add(Count);
	}
	else
	{
		if (pMapItem) { delete pMapItem; pMapItem = NULL; }
	}

	Send(TempBuf, TempBuf.GetLength());

	if (Result == SUCCESS)
	{
		SendChangeWgt();

		if (HasEquipmentChanged)
		{
			SendItemInfo(Slot);
		}
	}
}

void USER::ThrowMoney(TCHAR* pBuf)
{
	int index = 0;
	DWORD Money = GetDWORD(pBuf, index);
	short GiveX	= GetShort(pBuf, index);
	short GiveY = GetShort(pBuf ,index);

	CPoint ptGive = ConvertToServer(GiveX, GiveY);
	if (ptGive.x < 0 || ptGive.y < 0) goto send_fail_;
	if (ptGive.x >= g_Zones[m_iZoneIndex]->m_sizeMap.cx || ptGive.y >= g_Zones[m_iZoneIndex]->m_sizeMap.cy) goto send_fail_;
	// NOT NEEDED because we want ppl to be able to drop while running  if (ptGive.x != m_sX || ptGive.y != m_sY) goto send_fail_;

	if (m_bTrading) goto send_fail_;

	if (Money > m_dwBarr)  goto send_fail_;
	if (Money <= 0 || Money == _MAX_DWORD)  goto send_fail_;

	ItemList* pMapItem = new ItemList;
	if (pMapItem == NULL) goto send_fail_;

	// Create the money item from item table loaded from database
	if (!pMapItem->InitFromItemTable(TYPE_MONEY_NUM)) goto send_fail_;

	// Adds money to the money item to be put onto the map and checks that it doesn't go over the max etc
	CheckMaxValue((DWORD &)pMapItem->dwMoney, (DWORD)Money);

	if (UserThrowItem(pMapItem, 9))
	{
		if (m_dwBarr <= Money) m_dwBarr = 0;
		else m_dwBarr -= Money;

		SendMoneyChanged();
		TRACE("Throw money: %d\n", pMapItem->dwMoney);
		return;
	}

send_fail_:
	CBufferEx TempBuf;
	TempBuf.Add(PKT_ITEM_DUMP);
	TempBuf.Add(FAIL);
	Send(TempBuf, TempBuf.GetLength());
	if (pMapItem) { delete pMapItem; pMapItem = NULL; }
}

void USER::PickupItem(TCHAR* pBuf)
{
	MAP* pMap = g_Zones[m_iZoneIndex];
	if (!pMap) return;

	int index = 0, i;
	short PickX	= GetShort(pBuf, index);
	short PickY = GetShort(pBuf ,index);
	short sFoundInvSlot = -1, sFoundBeltSlot = -1, sWeight = 0, sItemCountRemain = 0, sItemCount = 0;
	ItemList* pPickupItem = NULL;
	CItemTable* pItem = NULL;
	CBufferEx TempBuf;
	CString str; // for pickup msgs
	DWORD dwBarr = 0;
	DWORD dwMyMoney = m_dwBarr;

	int mapIndex = -1;
	BYTE bType = 0;

	CPoint ptPick = ConvertToServer(PickX, PickY);
	if (ptPick.x < 0 || ptPick.y < 0) goto send_fail_;
	PickX = (short)ptPick.x;
	PickY = (short)ptPick.y;

	if (m_bTrading) goto send_fail_;

	if (PickX < 0 || PickY < 0) goto send_fail_;
	if (PickX >= g_Zones[m_iZoneIndex]->m_sizeMap.cx || PickY >= g_Zones[m_iZoneIndex]->m_sizeMap.cy) goto send_fail_;
	if (!GetDistance(PickX, PickY, 1)) goto send_fail_;

	mapIndex = pMap->m_pMap[PickX][PickY].iIndex;
	if (mapIndex < 0) goto send_fail_;

	EnterCriticalSection(&m_pCom->m_critThrowItem);
	if (m_pCom->m_ThrowItemArray[mapIndex] == NULL)
	{
		LeaveCriticalSection(&m_pCom->m_critThrowItem);
		goto send_fail_;
	}
	pPickupItem = m_pCom->m_ThrowItemArray[mapIndex]->m_pItem;
	LeaveCriticalSection(&m_pCom->m_critThrowItem);
	//
	if (!pPickupItem)
	{
		pMap->m_pMap[PickX][PickY].iIndex = -1;
		goto send_fail_;
	}

	if (InterlockedCompareExchangePointer((PVOID*)&g_Zones[m_iZoneIndex]->m_pMap[PickX][PickY].m_FieldUse,(PVOID)1, (PVOID)0) == (PVOID*)0)
	{
		bType = pPickupItem->bType;
		if (pPickupItem->bType == TYPE_MONEY)
		{
			if (pPickupItem->dwMoney <= 0 || pPickupItem->dwMoney == _MAX_DWORD)
			{
				::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[PickX][PickY].m_FieldUse, 0);
				goto send_fail_;
			}
			if (m_dwBarr == _MAX_DWORD)
			{
				::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[PickX][PickY].m_FieldUse, 0);
				goto send_fail_;
			}

			if (!CheckMaxValueReturn(dwMyMoney, pPickupItem->dwMoney))
			{
				int iBarr = 0;
				CheckMaxValue(dwMyMoney, pPickupItem->dwMoney);

				iBarr = (int)dwMyMoney;
				dwBarr = (DWORD)abs(iBarr - (int)pPickupItem->dwMoney);
			}

			CheckMaxValue(dwMyMoney, pPickupItem->dwMoney);

			if (dwBarr > 0)
			{
				pPickupItem->dwMoney = dwBarr;
				SendItemFieldInfo(ITEM_INFO_MODIFY, TO_INSIGHT, pPickupItem, PickX, PickY);

				EnterCriticalSection(&m_pCom->m_critThrowItem);
				if (m_pCom->m_ThrowItemArray[mapIndex] != NULL)
				{
					m_pCom->m_ThrowItemArray[mapIndex]->m_pItem->dwMoney = dwBarr;
				}
				LeaveCriticalSection(&m_pCom->m_critThrowItem);
			}
			else
			{
				pPickupItem->sNum = 0;
				pPickupItem->dwMoney = 0;
				pPickupItem->sUsage = 0;
				SendItemFieldInfo(ITEM_INFO_DELETE, TO_INSIGHT, pPickupItem, PickX, PickY);
				pMap->m_pMap[PickX][PickY].iIndex = -1;

				EnterCriticalSection(&m_pCom->m_critThrowItem);
				if (m_pCom->m_ThrowItemArray[mapIndex] != NULL)
				{
					if (m_pCom->m_ThrowItemArray[mapIndex]->m_pItem)
					{
						delete m_pCom->m_ThrowItemArray[mapIndex]->m_pItem;
						m_pCom->m_ThrowItemArray[mapIndex]->m_pItem = NULL;
					}
				}
				LeaveCriticalSection(&m_pCom->m_critThrowItem);
				//
			}
		}
		else // TYPE IS ITEM
		{
			if (!g_mapItemTable.Lookup(pPickupItem->sNum, pItem))
			{
				::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[PickX][PickY].m_FieldUse, 0);
				goto send_fail_;
			}

			if (pPickupItem->bType > TYPE_ACC) // Stackable
			{
				sItemCount = pPickupItem->sUsage;
				for (i = 1; i < sItemCount + 1; i++)
				{
					if (i == 1)
					{
						sWeight = m_sWgt + (pPickupItem->sWgt * i);
						if (sWeight > GetMaxWgt())
						{
							SendServerChatMessage(IDS_PICKUP_WEIGHT_FULL, TO_ME);
							::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[PickX][PickY].m_FieldUse, 0);
							goto send_fail_;
						}
					}
					else
					{
						sWeight = m_sWgt + (pPickupItem->sWgt * i);
						if (sWeight > GetMaxWgt())
						{
							sItemCount = i - 1;
							break;
						}
					}
				}

				sItemCountRemain = pPickupItem->sUsage - sItemCount;
				pPickupItem->sUsage = sItemCount;

				if (pPickupItem->bType == TYPE_POTION || pPickupItem->bType == TYPE_FOOD)
				{
					for (i = 0; i < BELT_ITEM_NUM; i++)
					{
						if (m_BeltItem[i].sNum == pPickupItem->sNum)
						{
							sFoundBeltSlot = i;
							break;
						}
					}
				}

				if (sFoundBeltSlot == -1)
				{
					sFoundInvSlot = PushItemInventory(pPickupItem);
					if (sFoundInvSlot == -1)
					{
						::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[PickX][PickY].m_FieldUse, 0);
						goto send_fail_;
					}
					sItemCountRemain += pPickupItem->sUsage;
				}
				else
				{
					if (m_BeltItem[sFoundBeltSlot].sUsage + pPickupItem->sUsage > MAX_ITEM_DURA)
					{
						sItemCountRemain += m_BeltItem[sFoundBeltSlot].sUsage + pPickupItem->sUsage - MAX_ITEM_DURA;
						m_BeltItem[sFoundBeltSlot].sUsage = MAX_ITEM_DURA;
						SendServerChatMessage(IDS_BELT_FULL, TO_ME);
					}
					else
					{
						PlusItemDur(&m_BeltItem[sFoundBeltSlot], pPickupItem->sUsage);
						SendItemInfoChange(BELT_INV, sFoundBeltSlot, INFO_DUR);
					}
				}
			}
			else // Not stackable (Equipement)
			{
				sWeight = m_sWgt + pPickupItem->sWgt;
				if (sWeight > GetMaxWgt())
				{
					SendServerChatMessage(IDS_PICKUP_WEIGHT_FULL, TO_ME);
					::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[PickX][PickY].m_FieldUse, 0);
					goto send_fail_;
				}

				sFoundInvSlot = PushItemInventory(pPickupItem);
				if (sFoundInvSlot == -1)
				{
					::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[PickX][PickY].m_FieldUse, 0);
					goto send_fail_;
				}
			}

			if (sItemCountRemain > 0)
			{
				pPickupItem->sUsage = sItemCountRemain;
				SendItemFieldInfo(ITEM_INFO_MODIFY, TO_INSIGHT, pPickupItem, PickX, PickY);

				EnterCriticalSection(&m_pCom->m_critThrowItem);
				if (m_pCom->m_ThrowItemArray[mapIndex] != NULL)
				{
					m_pCom->m_ThrowItemArray[mapIndex]->m_pItem->sUsage = sItemCountRemain;
				}
				LeaveCriticalSection(&m_pCom->m_critThrowItem);
			}
			else
			{
				pPickupItem->sNum = 0;
				pPickupItem->dwMoney = 0;
				pPickupItem->sUsage = 0;
				SendItemFieldInfo(ITEM_INFO_DELETE, TO_INSIGHT, pPickupItem, PickX, PickY);
				pMap->m_pMap[PickX][PickY].iIndex = -1;

				EnterCriticalSection(&m_pCom->m_critThrowItem);
				if (m_pCom->m_ThrowItemArray[mapIndex] != NULL)
				{
					if (m_pCom->m_ThrowItemArray[mapIndex]->m_pItem)
					{
						delete m_pCom->m_ThrowItemArray[mapIndex]->m_pItem;
						m_pCom->m_ThrowItemArray[mapIndex]->m_pItem = NULL;
					}
				}
				LeaveCriticalSection(&m_pCom->m_critThrowItem);
			}
		}

		::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[PickX][PickY].m_FieldUse, 0);
	}

	if (bType == TYPE_MONEY)
	{
		DWORD dwMoneyPickedUp = dwMyMoney - m_dwBarr;
		m_dwBarr = dwMyMoney;

		CString str;
		str.Format(IDS_PICKUP_BARR, dwMoneyPickedUp);
		SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
		SendMoneyChanged();
	}
	else if (sFoundInvSlot != -1)
	{
		TempBuf.Add(PKT_ITEM_PICKUP);
		TempBuf.Add(SUCCESS);
		TempBuf.Add((BYTE)BASIC_INV); // Belt type 0 = Belt 1 = Inv
		TempBuf.Add(sFoundInvSlot);
		TempBuf.Add(m_InvItem[sFoundInvSlot].bType);
		TempBuf.Add(m_InvItem[sFoundInvSlot].bArm);
		TempBuf.Add(GetItemClass(m_InvItem[sFoundInvSlot].sNum)); // class
		TempBuf.Add(m_InvItem[sFoundInvSlot].sPicNum);
		TempBuf.Add(m_InvItem[sFoundInvSlot].bGender);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sAb1);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sAb2);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sWgt);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sTime);
		TempBuf.Add(m_InvItem[sFoundInvSlot].dwMoney);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sNeedStr);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sNeedInt);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sNeedDex);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sNeedSkill);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sHP);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sMP);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sUsage);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sMaxUsage);
		TempBuf.Add(m_InvItem[sFoundInvSlot].sDur);
		TempBuf.Add((BYTE)m_InvItem[sFoundInvSlot].strNewName.GetLength());
		TempBuf.AddData((LPTSTR)(LPCTSTR)m_InvItem[sFoundInvSlot].strNewName, m_InvItem[sFoundInvSlot].strNewName.GetLength());
		TempBuf.Add(m_InvItem[sFoundInvSlot].byUpgradeLevel); // Upgrade level
		int index = 0;
		index += GetSpecialItemData(m_InvItem[sFoundInvSlot].sNum, m_TempBuf);
		TempBuf.AddData(m_TempBuf, index);

		Send(TempBuf, TempBuf.GetLength());

		if (m_InvItem[sFoundInvSlot].bType > TYPE_ACC)
		{
			str.Format(IDS_PICKUP_ITEMS, sItemCount, m_InvItem[sFoundInvSlot].strNewName);
		}
		else
		{
			str.Format(IDS_PICKUP_ITEM, m_InvItem[sFoundInvSlot].strNewName);
		}

		SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}
	else if (sFoundBeltSlot != -1)
	{
		if (m_BeltItem[sFoundBeltSlot].bType > TYPE_ACC)
		{
			str.Format(IDS_PICKUP_ITEMS, sItemCount, m_BeltItem[sFoundBeltSlot].strNewName);
		}
		else
		{
			str.Format(IDS_PICKUP_ITEM, m_BeltItem[sFoundBeltSlot].strNewName);
		}

		SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}

	SendChangeWgt();

	return;

send_fail_:
	TempBuf.Add(PKT_ITEM_PICKUP);
	TempBuf.Add(FAIL);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::SetBattleMode(TCHAR* pBuf)
{
	int index = 0;
	BYTE BattleMode = GetByte(pBuf, index);

	if (BattleMode < BATTLEMODE_NORMAL || BattleMode > BATTLEMODE_PK) return;

	if (BattleMode == BATTLEMODE_PK)
	{
		if (m_sLevel < LEVEL_CAN_PK)
		{
			BattleMode = BATTLEMODE_NORMAL;
		}
	}

	m_BattleMode = BattleMode;

	DWORD dwTick = GetTickCount();
	m_dwLastRecoverHPTime = dwTick;
	m_dwLastRecoverMPTime = dwTick;

	CBufferEx SendBuf;
	SendBuf.Add(PKT_BATTLEMODE);
	SendBuf.Add(m_Uid+USER_BAND);
	SendBuf.Add(m_BattleMode);
	SendInsight(SendBuf, SendBuf.GetLength());
}

void USER::MagicReady(TCHAR* pBuf)
{
	int index = 0;
	int iTargetId = GetInt(pBuf , index);
	short sMagicNo = GetShort(pBuf, index);
	BYTE byDir = GetByte(pBuf, index);
	short sDir = GetShort(pBuf, index);

	// Set direction the user is now facing
	m_byDir = static_cast<BYTE>(sDir);

	DWORD dwCurrTime = GetTickCount();

	// Validate the magic being used
	if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
	{
		SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
		return;
	}

	CMagicTable *pMagic = g_arMagicTable.GetAt(sMagicNo-1);
	if (!pMagic)
	{
		SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
		return;
	}

	if (!IsHaveMagic(sMagicNo, MAGIC_TYPE_MAGIC))
	{
		TRACE("User does not have magic\n");
		SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
		return;
	}

	// Fail casting if reflection still remains
	if (CheckRemainMagic(MAGIC_EFFECT_REFLECT))
	{
		SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
		return;
	}

	// Cannot attack when warping
	if (m_bIsWarping)
	{
		SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
		return;
	}

	// Cannot attack when in peace mode
	if (m_BattleMode == BATTLEMODE_NORMAL)
	{
		SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
		return;
	}

	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION))
	{
		SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
		return;
	}

	short sMpDec = pMagic->m_sMpdec;
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_MPSAVE))
	{
		sMpDec = (sMpDec * m_RemainSpecial[SPECIAL_EFFECT_MPSAVE].sDamage) / 100;
	}
	if (sMpDec > m_sMP)
	{
		SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
		return;
	}

	if (!CheckDemon(m_sClass))
	{
		int iMoral = m_iMoral / CLIENT_MORAL;
		if (iMoral < pMagic->m_sMinMoral)
		{
			SendServerChatMessage(IDS_USER_MAGIC_MORAL_LOW, TO_ME);
			SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
			return;
		}

		if (iMoral > pMagic->m_sMaxMoral)
		{
			SendServerChatMessage(IDS_USER_MAGIC_MORAL_HIGH, TO_ME);
			SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
			return;
		}
	}

	int iINT = m_iINT / CLIENT_BASE_STATS;

	if (iINT < pMagic->m_sMinInt)
	{
		SendServerChatMessage(IDS_USER_MAGIC_SKILL_LOW, TO_ME);
		SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
		return;
	}

	int iMagicExp = 0;
	switch (pMagic->m_tClass)
	{
	case 1:
		iMagicExp = m_iBMagicExp / CLIENT_SKILL;
		break;
	case 2:
		iMagicExp = m_iWMagicExp / CLIENT_SKILL;
		break;
	case 3:
		iMagicExp = m_iDMagicExp / CLIENT_SKILL;
		break;
	default:
		break;
	}

	if (iMagicExp < pMagic->m_sMinMxp)
	{
		SendServerChatMessage(IDS_USER_MAGIC_SKILL_LOW, TO_ME);
		SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
		return;
	}

	// If user is not wearing a staff and doesn't have magic avail special
	// then the user cannot cast the magic.
	if (!CheckHaveSpecialNumInBody(SPECIAL_MAGIC_AVAIL))
	{
		if ((!m_InvItem[ARM_RHAND].IsEmpty() && m_InvItem[ARM_RHAND].bType != TYPE_STAFF) ||
			!m_InvItem[ARM_LHAND].IsEmpty())
		{
			SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
			return;
		}
	}

	// Cannot cast a magic that doesn't belong to your mage type
	if (pMagic->m_tClass != 1)
	{
		if (IsServerRank(SERVER_RANK_NORMAL))
		{
			if (CheckMageType(pMagic->m_tClass, 0))
			{
				SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
				return;
			}
		}
	}

	// See if the user has already got a magic casted
	if (m_bMagicCasted)
	{
		if (!IsHaveMagic(m_sMagicNoCasted, MAGIC_TYPE_MAGIC))
		{
			// Dismiss the casted magic because the user does not have it
			m_bMagicCasted = false;
			m_bPreMagicCasted = false;
		}
		else
		{
			if ((dwCurrTime - m_dwMagicCastedTime) <= static_cast<DWORD>(pMagic->m_sStartTime + 1000))
			{
				// Fail the Magic Ready because a magic is still being casted
				SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
				return;
			}

			// Enough time has passed so the cast can be dismissed
			m_bMagicCasted = false;
			m_bPreMagicCasted = false;
		}
	}

	if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
	{
		USER *pTarget = g_pUserList->GetUserUid(iTargetId - USER_BAND);
		if (!pTarget)
		{
			SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
			return;
		}

		// Certain things need to be checked only if the magic does damage
		if (pMagic->m_sDamage > 0)
		{
			// Must check that the player can be PKed
			if (!CheckCanPK(pTarget))
			{
				CString strMessage;
				strMessage.Format(IDS_USER_CANNOT_PK_LEVEL_BELOW, LEVEL_CAN_PK);
				SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
				SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
				return;
			}

			// Check for safety zone
			if (CheckSafetyZone() || pTarget->CheckSafetyZone())
			{
				SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
				return;
			}
		}

		// Skip the code after this because it apply only to magic type arrow
		if (pMagic->m_tType01 == MAGIC_TYPE_ARROW)
		{
			// TODO: Comment me
			if (pMagic->m_sDamage > 0 && pMagic->m_tTarget != MAGIC_TARGET_ALL)
			{
				if (CheckDemon(m_sClass) == CheckDemon(pTarget->m_sClass) &&
					m_BattleMode == BATTLEMODE_ATTACK &&
					pTarget->m_iGrayCount == 0 &&
					!CheckGuildWarArmy(pTarget))
				{
					SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
					return;
				}
			}

			// Certain maps have walls and these need to be checked for
			// and prevent attacks through them.
			CPoint ptPos = ConvertToClient(m_sX, m_sY);
			CPoint ptPosTarget = ConvertToClient(pTarget->m_sX, pTarget->m_sY);
			if (!ArrowPathFind(ptPos, ptPosTarget))
			{
				SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
				return;
			}
		}
	}
	else if (iTargetId >= NPC_BAND)				// NPC
	{
		CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
		if (!pNpc)
		{
			SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
			return;
		}

		// Check if the npc is able to die.
		// This allows for preventing attack of a peaceful npc.
		if (pNpc->m_tNpcType == NPCTYPE_NPC)
		{
			SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
			return;
		}

		if (pNpc->m_NpcState == NPC_DEAD || pNpc->m_NpcState == NPC_LIVE)
		{
			SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
			return;
		}

		// Guards cannot be attacked by players when they are in battle mode (PK mode allows attack).
		if (pNpc->IsDetecter() && m_BattleMode == BATTLEMODE_ATTACK)
		{
			SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
			return;
		}

		// You cannot use magic on npc that is for user targets only
		if (pMagic->m_tTarget == MAGIC_TARGET_USER)
		{
			SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
			return;
		}

		// Certain maps have walls and these need to be checked for
		// and prevent attacks through them.
		CPoint ptPos = ConvertToClient(m_sX, m_sY);
		CPoint ptPosTarget = ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);
		if (!ArrowPathFind(ptPos, ptPosTarget))
		{
			SendMagicReady(FAIL, iTargetId, sMagicNo, byDir);
			return;
		}
	}

	if (pMagic->m_sStartTime > 0)
	{
		m_bMagicCasted = true;
		m_sMagicNoCasted = sMagicNo;
		m_dwMagicCastedTime = dwCurrTime;
	}

	SendMagicReady(SUCCESS, iTargetId, sMagicNo, byDir, pMagic->m_sStartTime);
}

/////////////////////////////////////////////////////////////////////////////////
//	Uses an item that belongs to a user to give a desired effect..
//
void USER::ItemUseReq(TCHAR* pBuf)
{
	int index = 0;
	BYTE byBelt = GetByte(pBuf, index);
	short nSlot = GetShort(pBuf, index);
	short nAddHP = 0;
	short nAddMP = 0;
	CBufferEx TempBuf;
	ItemList Item;
	bool bItemUsed = true;
	int iLightItemTime = -1;

	if (byBelt == BELT_INV)
	{
		if (nSlot < 0 || nSlot >= BELT_ITEM_NUM) goto send_fail_;
		Item = m_BeltItem[nSlot];
	}
	else if (byBelt == BASIC_INV)
	{
		if (nSlot < EQUIP_ITEM_NUM || nSlot >= INV_ITEM_NUM) goto send_fail_;
		Item = m_InvItem[nSlot];
	}
	else
	{
		return;
	}

	CItemTable* pItemTable;
	if (!g_mapItemTable.Lookup(Item.sNum, pItemTable)) goto send_fail_;

	// If the item is a crafting material, item can't be used or the dura is less than 1 then send fail packet
	if (pItemTable->m_bType == TYPE_MTR || pItemTable->m_bArm != ARM_USE || Item.sUsage < 1) goto send_fail_;

	// Prevent usage of item that does not blong to the class! e.g. prevent human using devil item...
	if (CheckDemon(m_sClass) != Item.IsDemonItem()) goto send_fail_;

	if (!SpecialItemUse(Item.sNum, nSlot, pItemTable->m_sSpecial, pItemTable->m_sSpOpt[0], pItemTable->m_sSpOpt[1],
						pItemTable->m_sSpOpt[2], bItemUsed, iLightItemTime))
	{
		goto send_fail_;
	}

	nAddHP = pItemTable->m_sHP;
	nAddMP = pItemTable->m_sMP;

	if (bItemUsed)
	{
		if (Item.sUsage <= 1)
		{
			Item.Init();
		}
		else
		{
			PlusItemDur(&Item, 1, true);
		}
	}

	if (byBelt == BELT_INV)
	{
		m_BeltItem[nSlot] = Item;
		// update belt item index
	}
	else if (byBelt == BASIC_INV)
	{
		m_InvItem[nSlot] = Item;
		// update inv item index
	}

	TempBuf.Add(PKT_ITEM_USE);
	TempBuf.Add(SUCCESS);
	TempBuf.Add(byBelt);
	TempBuf.Add(nSlot);
	TempBuf.Add(pItemTable->m_bType);
	TempBuf.Add(Item.sUsage);
	TempBuf.Add(static_cast<short>(iLightItemTime)); // torch -1 = no torch
	Send(TempBuf, TempBuf.GetLength());

	SendChangeWgt();

	if (nAddHP > 0)
	{
		short nCurrentHP = m_sHP;
		nCurrentHP += nAddHP;
		if (nCurrentHP >= GetMaxHP()) nCurrentHP = GetMaxHP();
		m_sHP = nCurrentHP;
	}

	if (nAddMP > 0)
	{
		short nCurrentMP = m_sMP;
		nCurrentMP += nAddMP;
		if (nCurrentMP >= GetMaxMP()) nCurrentMP = GetMaxMP();
		m_sMP = nCurrentMP;
	}

	SendCharData(INFO_EXTVALUE);

	return;

send_fail_:
	TempBuf.Add(PKT_ITEM_USE);
	TempBuf.Add(FAIL);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::SetRunModeReq(TCHAR* pBuf)
{
	// Recover any stamina before changing run mode
	GetStm();

	int index = 0;
	bool bRunMode = false;

	bRunMode = GetByte(pBuf, index) > 0 ? true : false;

	// Add Validation here to stop user going into run mode if there in a guild war etc
	if (m_bInGuildWar && m_sGuildRank == Guild::GUILD_CHIEF)
	{
		bRunMode = false;
	}

	m_bRunMode = bRunMode;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_SETRUNMODE);
	TempBuf.Add(bRunMode);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::ChangeDir(TCHAR* pBuf)
{
	int	index = 0;

	int uid = GetInt(pBuf, index);
	if (uid != m_Uid + USER_BAND) return;

	BYTE dir = GetByte(pBuf, index);
	if (dir < 0 || dir > 7) return;
	m_byDir = dir;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_CHANGEDIR);
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add((short)m_byDir);
	SendInsight(TempBuf, TempBuf.GetLength());
}

void USER::ClientEvent(TCHAR* pBuf)
{
	int index = 0;

	BYTE bType = GetByte(pBuf, index);

	if (bType == 2) // Meet NPC
	{
		NpcEvent(pBuf + index);
	}
	else if (bType == 3) // Request Trade
	{
		int iTradeUid = GetInt(pBuf, index);
		if (iTradeUid < 0 || iTradeUid >= NPC_BAND) return;
		if (iTradeUid - USER_BAND == m_Uid) return;

		// Get and validate the name of character that going to trade with
		TCHAR strTradeCharName[NAME_LENGTH+1];
		int	nNameLen = GetVarString(sizeof(strTradeCharName), strTradeCharName, pBuf, index);
		if (nNameLen <= 0 || nNameLen > NAME_LENGTH || !strTradeCharName) return;

		USER* pTradeUser = GetUser(iTradeUid - USER_BAND);
		if (pTradeUser == NULL) return;
		if (!CheckDistance(pTradeUser, 2)) return;

		// Make sure not already trading or waiting for a response to a trade request
		if (m_bTrading || m_bTradeWaiting || pTradeUser->m_bTrading || pTradeUser->m_bTradeWaiting)
		{
			// SendTradeAck(FAIL);
			return;
		}

		// Name of player trading with must match that sent in the request
		if (_stricmp(pTradeUser->m_strUserId, strTradeCharName) != 0)
		{
			// SendTradeAck(FAIL);
			return;
		}

		// Cannot trade with other race
		if (CheckDemon(m_sClass) != CheckDemon(pTradeUser->m_sClass))
			return;

		// Set trade variables
		m_iTradeUid = iTradeUid;
		m_bTradeWaiting = true;
		pTradeUser->m_iTradeUid = m_Uid;
		pTradeUser->m_bTradeWaiting = true;

		index = 0;
		SetByte(m_TempBuf, PKT_TRADE_REQ, index);
		SetInt(m_TempBuf, m_Uid + USER_BAND, index);
		SetVarString(m_TempBuf, m_strUserId, strlen(m_strUserId), index);
		pTradeUser->Send(m_TempBuf, index);
	}
}

void USER::SelectMsgResult(TCHAR* pBuf)
{
	if (m_bTrading) return;

    CNpc* pNpc = GetNpc(m_iTalkToNpc);
	if (!pNpc) return;

	m_iTalkToNpc = -1; // Reset the Npc there talking to.. if the event continues this will be
					   // Set again

	int index = 0;

	int SelBoxOp = GetShort(pBuf, index);
	if (SelBoxOp < 0 || SelBoxOp > 3) return;
	int SelBoxEvent = m_iSelectMsgResult[SelBoxOp];

	for (int i = 0; i < 4; i++) // Reset Select Msg
	{
		m_iSelectMsgResult[i] = -1;
	}

	if (SelBoxEvent == -1) return;
	EVENT* pEvent = GetEventInCurrentZone();

	if (!pEvent) return;

	// TODO: Possible bug here, SelBoxEvent could be > .GetSize because event numbers are not forced to be sequential
	if (SelBoxEvent > pEvent->m_arEvent.GetSize()) return;

	if (!pEvent->m_arEvent[SelBoxEvent]) return;

	if (!CheckEventLogic(pEvent->m_arEvent[SelBoxEvent])) return;

	for (int i = 0; i < pEvent->m_arEvent[SelBoxEvent]->m_arExec.GetSize(); i++)
	{
		if (!RunNpcEvent(pNpc, pEvent->m_arEvent[SelBoxEvent]->m_arExec[i]))
		{
			return;
		}
	}
}

void USER::ClassStoreBuyReq(TCHAR* pBuf)
{
	int index = 0, i;
	short sItemNo = GetShort(pBuf, index);
	short sItemAmount = GetShort(pBuf, index);
	int iItemShopIndex = -1;
	ItemList buyItem;
	int iPosCost = 0;
	short sWeight;
	short sSlot;
	BYTE byResult = FAIL;

	if (sItemAmount < 1 || sItemAmount > MAX_ITEM_DURA) goto result_send_;
	if (sItemNo <= 0) goto result_send_;

	for (i = 0; i < g_arClassStoreTable.GetSize(); i++)
	{
		if (g_arClassStoreTable[i]->m_sItemNum == sItemNo)
		{
			iItemShopIndex = i;
			break;
		}
	}

	if (iItemShopIndex == -1) goto result_send_;
	if (g_arClassStoreTable[iItemShopIndex]->m_sNum <= 0) goto result_send_;

	if (!buyItem.InitFromItemTable(sItemNo)) goto result_send_;

	if (buyItem.bType > TYPE_ACC)
	{
		sItemAmount = buyItem.sUsage = sItemAmount * g_arClassStoreTable[iItemShopIndex]->m_sNum;
	}

	iPosCost = (g_arClassStoreTable[iItemShopIndex]->m_iPoint * sItemAmount);
	if (m_iClassPoint < iPosCost)
	{
		SendServerChatMessage(IDS_BUY_NOT_ENOUGH_POS, TO_ME);
		goto result_send_;
	}

	sWeight = m_sWgt + (sItemAmount * buyItem.sWgt);
	if (sWeight > GetMaxWgt())
	{
		SendServerChatMessage(IDS_BUY_NOT_ENOUGH_WEIGHT, TO_ME);
		goto result_send_;
	}

	if (buyItem.bType == TYPE_POTION || buyItem.bType == TYPE_FOOD)
	{
		for (i = 0; i < BELT_ITEM_NUM; i++)
		{
			if (m_BeltItem[i].sNum == buyItem.sNum)
			{
				if (m_BeltItem[i].sUsage >= MAX_ITEM_DURA)
				{
					SendServerChatMessage(IDS_BELT_FULL, TO_ME);
					goto result_send_;
				}
				else
				{
					if (m_BeltItem[i].sUsage + sItemAmount > MAX_ITEM_DURA)
					{
						buyItem.sUsage = sItemAmount - ((m_BeltItem[i].sUsage + sItemAmount) - MAX_ITEM_DURA);
					}
				}
			}
		}
	}

	sSlot = PushItemInventory(&buyItem);
	if (sSlot == -1) goto result_send_;

	if (buyItem.sUsage > 0) // sUsage of item will holds the amount left over if
	{						  // more than 0 user cannot buy these so reduce cost
		iPosCost -= (g_arClassStoreTable[iItemShopIndex]->m_iPoint * buyItem.sUsage);
	}

	m_iClassPoint -= iPosCost;

	SendCharData(INFO_NAMES); // Updates the POS

	SendChangeWgt();

	byResult = SUCCESS;

result_send_:

	if (byResult == SUCCESS)
	{
		SendGetItem(sSlot);
	}

	CBufferEx TempBuf;
	TempBuf.Add(PKT_CLASSPOINTBUY);
	TempBuf.Add(byResult);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::TownPortalEnd(TCHAR* pBuf)
{
	if (!m_bIsUsingWarpItem) return;

	CPoint ptWarp = FindRandPointInRect_C(m_nWarpZone, m_nWarpX - 10, m_nWarpY - 10, m_nWarpX + 10, m_nWarpY + 10);
	if (ptWarp.x == -1 || ptWarp.y == -1)
	{
		ptWarp.x = m_nWarpX;
		ptWarp.y = m_nWarpY;
	}

	if (m_sZ == m_nWarpZone)
	{
		LinkToSameZone(static_cast<short>(ptWarp.x), static_cast<short>(ptWarp.y));
	}
	else
	{

		LinkToOtherZone(m_nWarpZone, ptWarp.x, ptWarp.y);
	}

	CBufferEx TempBuf;
	TempBuf.Add(PKT_TOWNPORTALEND);
	TempBuf.Add(m_Uid + USER_BAND);
	SendInsight(TempBuf, TempBuf.GetLength());
}

void USER::TownPortalEndReq(TCHAR* pBuf)
{
	int index = 0;
	int uid = GetDWORD(pBuf, index);
	USER* pUser = g_pUserList->GetUserUid(uid - USER_BAND);
	if (!pUser) return;
	pUser->m_bIsUsingWarpItem = false;
	pUser->m_bIsWarping = false;
}

void USER::ReqUserInfo(TCHAR* pBuf)
{
	int index = 0;
	int uid = GetInt(pBuf, index);
	if (uid >= USER_BAND && uid < NPC_BAND)	// USER
	{
		USER* pUser = g_pUserList->GetUserUid(uid - USER_BAND);
		if (!pUser) return;
		int index = 0;
		pUser->FillUserInfo(m_TempBuf, index, INFO_MODIFY);
		Send(m_TempBuf, index);
	}
	else if (uid >= NPC_BAND)				// NPC
	{
		CNpc* pNpc = GetNpc(uid - NPC_BAND);
		if (!pNpc) return;
		int index = 0;
		pNpc->FillNpcInfo(m_TempBuf, index, INFO_MODIFY, m_pCom);
		Send(m_TempBuf, index);
	}
}

void USER::BuyItem(TCHAR* pBuf)
{
	int index = 0;
	short StoreNo, ItemNo, ItemAmount;
	StoreNo = GetShort(pBuf, index);
	ItemNo = GetShort(pBuf, index);
	ItemAmount = GetShort(pBuf, index);
	BYTE Result = FAIL;
	bool HasItemQuantityChanged = false;

	CStore* pStore = NULL;
	int ItemShopIndex = -1;
	ItemList buyItem;
	DWORD Cost = 0;
	short Weight = 1;
	short Slot = 0;
	int i = 0;

	if (m_bTrading) goto result_send_;

	if (ItemAmount < 1 || ItemAmount > MAX_ITEM_DURA) goto result_send_;
	if (StoreNo < 0 || ItemNo <= 0) goto result_send_;

	pStore = GetStore(StoreNo);
	if (pStore == NULL) goto result_send_;

	for (i = 0; i < pStore->m_arItems.GetSize(); i++)
	{
		if (pStore->m_arItems[i]->sItemNo == ItemNo)
		{
			ItemShopIndex = i;
			break;
		}
	}

	if (ItemShopIndex == -1) goto result_send_;

	if (pStore->m_arItems[ItemShopIndex]->sQuantity != -1 &&
		pStore->m_arItems[ItemShopIndex]->sQuantity < ItemAmount)
	{
		goto result_send_;
	}

	if (!buyItem.InitFromItemTable(ItemNo)) goto result_send_;
	if (buyItem.bType > TYPE_ACC)
	{
		buyItem.sUsage = ItemAmount;
	}
	else
	{
		ItemAmount = 1;
	}

	Cost = static_cast<DWORD>((buyItem.dwMoney * ((double)m_nStoreBuyRate/100.0)) * ItemAmount);
	if (m_dwBarr < Cost)
	{
		SendServerChatMessage(IDS_BUY_NOT_ENOUGH_BARR, TO_ME);
		goto result_send_;
	}

	Weight = m_sWgt + (ItemAmount * buyItem.sWgt);
	if (Weight > GetMaxWgt())
	{
		SendServerChatMessage(IDS_BUY_NOT_ENOUGH_WEIGHT, TO_ME);
		goto result_send_;
	}

	// TODO : Maybe place item onto belt if its
	// potion / food and there already is same item on belt

	// FIXME: Is it wrong to assume that the player would like to receive only a portion of the total amount of the item they asked for...?
	// Would it be better to just check if they do have room and if not to give an error and let them choose another amount or free up inventory space?
	Slot = PushItemInventory(&buyItem);
	if (Slot == -1) goto result_send_;

	if (buyItem.sUsage > 0) // sUsage of item will holds the amount left over if
	{					    // more than 0 user cannot buy these so reduce cost
		Cost -= static_cast<DWORD>((buyItem.dwMoney * ((double)m_nStoreBuyRate/100.0)) * buyItem.sUsage);
		ASSERT(ItemAmount >= buyItem.sUsage);
		ItemAmount -= buyItem.sUsage;
	}

	if (pStore->m_arItems[ItemShopIndex]->sQuantity != -1)
	{
		ASSERT(pStore->m_arItems[ItemShopIndex]->sQuantity >= ItemAmount);
		pStore->m_arItems[ItemShopIndex]->sQuantity -= ItemAmount;
		HasItemQuantityChanged = true;
	}

	m_dwBarr -= Cost;
	SendMoneyChanged();

	SendChangeWgt();

	Result = SUCCESS;

result_send_:
	CBufferEx TempBuf;
	TempBuf.Add(PKT_BUY_SHOP);
	TempBuf.Add(Result);
	Send(TempBuf, TempBuf.GetLength());

	if (Result == SUCCESS)
	{
		SendGetItem(Slot);
		if (HasItemQuantityChanged)
		{
			CBufferEx TempBuf;
			TempBuf.Add(PKT_SHOP_ITEM_COUNT);
			TempBuf.Add(StoreNo);
			TempBuf.Add((short)ItemShopIndex);
			TempBuf.Add((short)pStore->m_arItems[ItemShopIndex]->sItemNo);
			TempBuf.Add((short)pStore->m_arItems[ItemShopIndex]->sQuantity);
			SendInsight(TempBuf, TempBuf.GetLength());
		}
	}
}

void USER::SellItem(TCHAR* pBuf)
{
	int index = 0;
	short StoreNo, ItemSlot, ItemAmount;
	StoreNo = GetShort(pBuf, index);
	ItemSlot = GetShort(pBuf, index);
	ItemAmount = GetShort(pBuf, index);
	BYTE Result = FAIL;
	DWORD dwMyBarr = m_dwBarr;

	CStore* pStore = NULL;
	CItemTable* pItem = NULL;
	DWORD dwSellCost = 0;

	if (m_bTrading) goto result_send_;

	if (ItemAmount < 1 || ItemAmount > MAX_ITEM_DURA) goto result_send_;
	if (StoreNo < 0) goto result_send_;
	if (ItemSlot < 0 || ItemSlot >= INV_ITEM_NUM) goto result_send_;

	pStore = GetStore(StoreNo);
	if (pStore == NULL) goto result_send_;

	if (!g_mapItemTable.Lookup(m_InvItem[ItemSlot].sNum, pItem)) goto result_send_;

	// Check for untradeable items.
	if (!CanTradeItem(m_InvItem[ItemSlot]))
	{
		SendServerChatMessage(IDS_USER_CANNOT_SELL_ITEM, TO_ME);
		goto result_send_;
	}

	if (m_InvItem[ItemSlot].bType > TYPE_ACC)
	{
		if (m_InvItem[ItemSlot].sUsage < ItemAmount) goto result_send_;

		dwSellCost = static_cast<DWORD>((((double)pItem->m_iCost/2.0) * ((double)m_nStoreSellRate/100.0)) * ItemAmount);

		if (m_InvItem[ItemSlot].sUsage == ItemAmount)
		{
			m_InvItem[ItemSlot].Init();
			// TODO: Update database here
			SendDeleteItem(BASIC_INV, ItemSlot);
		}
		else
		{
			PlusItemDur(&m_InvItem[ItemSlot], ItemAmount, true);
			// TODO: Update database here
			SendItemInfoChange(BASIC_INV, ItemSlot, INFO_DUR);
		}
	}
	else
	{
		dwSellCost = static_cast<DWORD>((((double)pItem->m_iCost/2.0) * ((double)m_nStoreSellRate/100.0)) *
			((double)pItem->m_sDur/m_InvItem[ItemSlot].sMaxUsage) * ((double)m_InvItem[ItemSlot].sUsage/m_InvItem[ItemSlot].sMaxUsage));

		m_InvItem[ItemSlot].Init();
		// TODO: Update database here
		SendDeleteItem(BASIC_INV, ItemSlot);
		ItemAmount = 1;
	}

	if (!CheckMaxValueReturn((DWORD &)dwMyBarr, (DWORD)(dwSellCost))) goto result_send_;
	CheckMaxValue((DWORD &)m_dwBarr, (DWORD)dwSellCost);

	SendMoneyChanged();

	SendChangeWgt();

	Result = SUCCESS;

result_send_:
	CBufferEx TempBuf;
	TempBuf.Add(PKT_SELL_SHOP);
	TempBuf.Add(Result);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::SaveMoney(TCHAR* pBuf)
{
	if (m_bTrading) goto result_send_;

	int index = 0;
	DWORD dwMoney = GetDWORD(pBuf, index);

	if (dwMoney <= 0 || dwMoney > MAX_STORAGE_MONEY) goto result_send_;
	if (m_dwBarr - dwMoney < 0) goto result_send_;
	if (m_dwBank >= MAX_STORAGE_MONEY || m_dwBank + dwMoney > MAX_STORAGE_MONEY) goto result_send_;

	m_dwBank += dwMoney;
	m_dwBarr -= dwMoney;
	// Update user database here

result_send_:
	CBufferEx TempBuf;
	TempBuf.Add(PKT_SAVEMONEY);
	TempBuf.Add(m_dwBarr);
	TempBuf.Add(m_dwBank);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::TakeBackMoney(TCHAR* pBuf)
{
	if (m_bTrading) goto result_send_;

	int index = 0;
	DWORD dwMoney = GetDWORD(pBuf, index);

	if (dwMoney <= 0 || dwMoney > MAX_STORAGE_MONEY) goto result_send_;
	if (m_dwBank - dwMoney < 0) goto result_send_;

	m_dwBank -= dwMoney;

	if (!CheckMaxValueReturn((DWORD &)m_dwBarr, (DWORD)(dwMoney))) goto result_send_;
	CheckMaxValue((DWORD &)m_dwBarr, (DWORD)dwMoney);
	// Update user database here

result_send_:
	CBufferEx TempBuf;
	TempBuf.Add(PKT_TAKEBACKMONEY);
	TempBuf.Add(m_dwBarr);
	TempBuf.Add(m_dwBank);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::SaveItem(TCHAR* pBuf)
{
	int index = 0, i;
	short sSlot = GetShort(pBuf, index);
	short sItemCount = GetShort(pBuf, index);
	short sMaxStorageCount = 0, sBankSlot = -1, sFoundSameSlot = -1, sFoundEmptySlot = -1, sItemCountRemain = 0;
	CItemTable* pItem = NULL;
	CBufferEx TempBuf;
	ItemList InventoryItem, StorageItem;

	if (m_bTrading) goto send_fail_;

	if (sSlot < 0 || sSlot >= INV_ITEM_NUM) goto send_fail_;
	if (sItemCount <= 0 || sItemCount > MAX_ITEM_DURA) goto send_fail_;
	if (!g_mapItemTable.Lookup(m_InvItem[sSlot].sNum, pItem)) goto send_fail_;
	if (m_InvItem[sSlot].IsEmpty()) goto send_fail_;

	InventoryItem = m_InvItem[sSlot];
	StorageItem = m_InvItem[sSlot];

	sMaxStorageCount = GetMaxStorageCount();
	if (sMaxStorageCount <= 0) goto send_fail_;

	if (InventoryItem.bType > TYPE_ACC) // Non equipment
	{
		if (InventoryItem.sUsage > sItemCount)
		{
			sItemCountRemain = InventoryItem.sUsage - sItemCount;
			PlusItemDur(&InventoryItem, sItemCount, true);
		}
		else if (InventoryItem.sUsage == sItemCount)
		{
			InventoryItem.Init();
		}
		else
		{
			goto send_fail_;
		}

		for (i = 0; i < sMaxStorageCount; i++)
		{
			if (m_StorageItem[i].sNum == StorageItem.sNum && m_StorageItem[i].sUsage < MAX_ITEM_DURA)
			{
				sFoundSameSlot = i;
				break;
			}
		}
		// Add feature to store left over items in a new slot..
		if (sFoundSameSlot >= 0)
		{
			if (m_StorageItem[sFoundSameSlot].sUsage + sItemCount > MAX_ITEM_DURA)
			{
				short sItemCountLeft = m_StorageItem[sFoundSameSlot].sUsage + sItemCount - MAX_ITEM_DURA;
				InventoryItem.sUsage += sItemCountLeft;
				sItemCountRemain += sItemCountLeft;
				m_StorageItem[sFoundSameSlot].sUsage = MAX_ITEM_DURA;
				sBankSlot = sFoundSameSlot;
			}
			else
			{
				PlusItemDur(&m_StorageItem[sFoundSameSlot], sItemCount);
				sBankSlot = sFoundSameSlot;
			}
		}
		else
		{
			StorageItem.sUsage = sItemCount;
		}
	}
	else
	{
		InventoryItem.Init();
	}

	if (sFoundSameSlot == -1) // Item either equipment or didn't find same item in storage
	{
		for (i = 0; i < sMaxStorageCount; i++)
		{
			if (m_StorageItem[i].IsEmpty())
			{
				sFoundEmptySlot = i;
				break;
			}
		}

		if (sFoundEmptySlot == -1)
		{
			SendServerChatMessage(IDS_USER_WAREHOUSE_FULL, TO_ME);
			goto send_fail_;
		}
		else
		{
			sBankSlot = sFoundEmptySlot;
			m_StorageItem[sBankSlot] = StorageItem;
		}
	}

	m_InvItem[sSlot] = InventoryItem;

	TempBuf.Add(PKT_SAVEITEM);
	TempBuf.Add(sSlot);
	TempBuf.Add(sItemCountRemain);
	TempBuf.Add(sBankSlot);
	TCHAR pData[150];
	TempBuf.AddData(pData, GetSendItemData(m_StorageItem[sBankSlot], pData, ITEM_TYPE_STORAGE));

	Send(TempBuf, TempBuf.GetLength());

	SendChangeWgt();

	return;

send_fail_:
	TempBuf.Add(PKT_SAVEITEM);
	TempBuf.Add((short)-1);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::TakeBackItem(TCHAR* pBuf)
{
	int index = 0;
	short sSlot = GetShort(pBuf, index);
	short sItemCount = GetShort(pBuf, index);
	short sMaxStorageCount = 0, sFoundSlot = -1, sItemCountRemain = 0, sWeight = 0;
	CItemTable* pItem = NULL;
	CBufferEx TempBuf;
	ItemList InventoryItem, StorageItem;

	if (m_bTrading) goto send_fail_;

	if (sSlot < 0 || sSlot >= STORAGE_ITEM_NUM) goto send_fail_;
	if (sItemCount <= 0 || sItemCount > MAX_ITEM_DURA) goto send_fail_;
	if (!g_mapItemTable.Lookup(m_StorageItem[sSlot].sNum, pItem)) goto send_fail_;

	StorageItem = m_StorageItem[sSlot];
	InventoryItem = m_StorageItem[sSlot];

	if (StorageItem.bType > TYPE_ACC) // Non equipment
	{
		sWeight = m_sWgt + (StorageItem.sWgt * sItemCount);
		if (sWeight > GetMaxWgt())
		{
			SendServerChatMessage(IDS_TAKESTORAGE_WEIGHT_FULL, TO_ME);
			goto send_fail_;
		}
		if (StorageItem.sUsage > sItemCount)
		{
			sItemCountRemain = StorageItem.sUsage - sItemCount;
			PlusItemDur(&StorageItem, sItemCount, true);
			InventoryItem.sUsage = sItemCount;
		}
		else if (StorageItem.sUsage == sItemCount)
		{
			StorageItem.Init();
		}
		else
		{
			goto send_fail_;
		}
	}
	else
	{
		sWeight = m_sWgt + StorageItem.sWgt;
		if (sWeight > GetMaxWgt())
		{
			SendServerChatMessage(IDS_TAKESTORAGE_WEIGHT_FULL, TO_ME);
			goto send_fail_;
		}

		StorageItem.Init();
	}

	sFoundSlot = PushItemInventory(&InventoryItem);
	if (sFoundSlot == -1)
	{
		goto send_fail_;
	}

	if (InventoryItem.sUsage > 0)
	{
		StorageItem = InventoryItem;
		sItemCountRemain = StorageItem.sUsage;
	}

	m_StorageItem[sSlot] = StorageItem;

	TempBuf.Add(PKT_TAKEBACKITEM);
	TempBuf.Add(sSlot);
	TempBuf.Add(sItemCountRemain);
	TempBuf.Add(sFoundSlot);

	TCHAR pData[150];
	TempBuf.AddData(pData, GetSendItemData(m_InvItem[sFoundSlot], pData, ITEM_TYPE_ME));

	Send(TempBuf, TempBuf.GetLength());

	SendChangeWgt();

	return;

send_fail_:
	TempBuf.Add(PKT_TAKEBACKITEM);
	TempBuf.Add((short)-1);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::Attack(TCHAR* pBuf)
{
	TRACE("User Requesting Attack\n");

	int index = 0;

	int iTargetId = GetInt(pBuf, index);
	m_byDir = static_cast<BYTE>(GetShort(pBuf, index));

	// Remove the casted magic but only if its not pre casted magic
	if (!m_bPreMagicCasted)
	{
		m_bMagicCasted = false;
	}

	// Fail to attack if currently using reflection magic
	if (CheckRemainMagic(MAGIC_EFFECT_REFLECT))
	{
		SendAttackFail(iTargetId);
		return;
	}

	// Cannot attack before the delay has finished.
	if (!CheckAttackDelay())
	{
		SendAttackFail(iTargetId);
		return;
	}

	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION))
	{
		SendAttackFail(iTargetId);
		return;
	}

	// Target id must be within a valid band.
	if (iTargetId < USER_BAND || iTargetId >= INVALID_BAND)
	{
		SendAttackFail(iTargetId);
		return;
	}

	// User cannot attack while warping.
	if (m_bIsWarping)
	{
		SendAttackFail(iTargetId);
		return;
	}

	// Cannot attack if in peace mode
	if (m_BattleMode == BATTLEMODE_NORMAL)
	{
		SendAttackFail(iTargetId);
		return;
	}

	if (iTargetId >= USER_BAND && iTargetId < NPC_BAND) // USER
	{
		USER* pTarget = GetUser(iTargetId - USER_BAND);
		if (!pTarget)
		{
			SendAttackFail(iTargetId);
			return;
		}

		// Must check that the player can be PKed
		if (!CheckCanPK(pTarget))
		{
			CString strMessage;
			strMessage.Format(IDS_USER_CANNOT_PK_LEVEL_BELOW, LEVEL_CAN_PK);
			SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
			SendAttackFail(iTargetId);
			return;
		}

		// Cannot PvP when either player is inside safe zone (wotw has a safe zone when its active)
		if (CheckSafetyZone() ||
			pTarget->CheckSafetyZone())
		{
			SendAttackFail(iTargetId);
			return;
		}

		if (CheckDemon(m_sClass) == CheckDemon(pTarget->m_sClass) &&
			m_BattleMode == BATTLEMODE_ATTACK &&
			pTarget->m_iGrayCount == 0 &&
			!CheckGuildWarArmy(pTarget))
		{
			SendAttackFail(iTargetId);
			return;
		}

		// TODO : Maybe move this Live check on the target closer to the first check to be made.
		if (!pTarget->m_bLive ||
			pTarget->m_State != STATE_GAMESTARTED ||
			pTarget->m_bHidden ||
			pTarget->m_bIsWarping)
		{
			SendAttackFail(iTargetId);
			return;
		}

		// Certain maps have walls and these need to be checked for
		// and prevent attacks through them.
		CPoint ptPos = ConvertToClient(m_sX, m_sY);
		CPoint ptPosTarget = ConvertToClient(pTarget->m_sX, pTarget->m_sY);
		if (!ArrowPathFind(ptPos, ptPosTarget))
		{
			SendAttackFail(iTargetId);
			return;
		}

		// Check the user is within the attack distance range of the user
		int nDistance = 1;
		if (!m_InvItem[ARM_RHAND].IsEmpty())
		{
			switch (m_InvItem[ARM_RHAND].bType)
			{
			case TYPE_BOW:
				nDistance = 9;
				break;
			case TYPE_SPEAR:
				nDistance = 2;
				break;
			default:
				nDistance = 1;
				break;
			}
		}

		if (!CheckDistance(pTarget, nDistance))
		{
			SendAttackFail(iTargetId);
			return;
		}

		// Lets see if the attack was a hit or miss!
		if (!CheckAttackSuccess(pTarget))
		{
			SendAttackMiss(iTargetId);

			// Decrease dura of bows because a hit was missed they still use arrows. :P
			if (!m_InvItem[ARM_RHAND].IsEmpty() && m_InvItem[ARM_RHAND].bType == TYPE_BOW)
			{
				DecreaseWeaponItemDura();
			}
			return;
		}

		// NOTE if attack missed below code is not executed.

		// Calculate the damage to be dealt
		int iDefense = pTarget->GetDefense();
		int iAttack = GetAttack();

		int iDamage = iAttack - iDefense;

		int iDefenseHuman = pTarget->GetDefenseHuman(this);
		iDamage -= iDefenseHuman;

		int iAttackHuman = GetAttackHuman(pTarget);
		iDamage += iAttackHuman;

		if (iDamage <= 0)
		{
			iDamage = myrand(0, 2);
		}
		if (iDamage > pTarget->m_sHP)
		{
			iDamage = pTarget->m_sHP;
		}

		// Check if the target can be pked
		bool bCanPKTarget = CheckPK(pTarget);

		// Do the damage!
		pTarget->SetDamage(iDamage, m_sClass);

		// If the target died check for any gained pos
		if (pTarget->m_lDeadUsed == 1)
		{
			CheckClassPoint(pTarget);
		}

		// Do extra things if the user pked another user
		if (bCanPKTarget)
		{
			if (pTarget->m_lDeadUsed == 1)
			{
				if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
					(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
					(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
				{
					if (SetMoralByPK(pTarget->m_iMoral))
					{
						SendCharData(INFO_BASICVALUE);
						SendMyInfo(TO_INSIGHT, INFO_MODIFY);
					}
				}
			}

			// Lets any guards know who are within area that a user has attacked another user
			IsDetecterRange();

			// Sets the user gray if they pass the checks
			if (CheckGray(pTarget))
			{
				SetGrayUser(GRAY_MODE_NORMAL);
			}
		}

		// The attack was a success! yay :D
		SendAttackSuccess(iTargetId, pTarget->m_sHP, pTarget->GetMaxHP());
	}
	else if (iTargetId >= NPC_BAND) // NPC
	{
		TRACE("Target is NPC\n");
		CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
		if (!pNpc)
		{
			SendAttackFail(iTargetId);
			return;
		}

		if (pNpc->m_NpcState == NPC_DEAD || pNpc->m_NpcState == NPC_LIVE)
		{
			SendAttackFail(iTargetId);
			return;
		}

		// Check if the npc is able to die.
		// This allows for preventing attack of a peaceful npc.
		if (pNpc->m_tNpcType == NPCTYPE_NPC)
		{
			SendAttackFail(iTargetId);
			return;
		}

		// Certain maps have walls and these need to be checked for
		// and prevent attacks through them.
		CPoint ptPosUser = ConvertToClient(m_sX, m_sY);
		CPoint ptPosNpc = ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);
		if (!ArrowPathFind(ptPosUser, ptPosNpc))
		{
			SendAttackFail(iTargetId);
			return;
		}

		// Guards cannot be attacked by players when they are in battle mode (PK mode allows attack).
		if (pNpc->IsDetecter() && m_BattleMode == BATTLEMODE_ATTACK)
		{
			SendAttackFail(iTargetId);
			return;
		}

		// If AI Type of NPC is 0 or 3 then do something with the AttackList of the NPC oO
		// Need to look at this in more detail before making an implementation.
		if (pNpc->m_tNpcAttType == NPC_AT_PASSIVE &&
			(pNpc->CheckAIType(NPCAITYPE_NORMAL) ||
			pNpc->CheckAIType(NPCAITYPE_STONE_GUARD)))
		{
			pNpc->AttackListAdd(this);
		}

		// Set the NPC target
		// Stones and Guard NPC types cannot have a target set
		// Target is only set if NPC state is standing, moving or attacking.
		if ((pNpc->m_NpcState == NPC_STANDING ||
			pNpc->m_NpcState == NPC_MOVING ||
			pNpc->m_NpcState == NPC_FIGHTING) &&
			!pNpc->IsDetecter() &&
			!pNpc->IsStone())
		{
			pNpc->m_iAttackedUid = m_Uid + USER_BAND;
			pNpc->m_NpcState = NPC_ATTACKING;
		}

		// Check the NPC is within the attack distance range of the user
		int nDistance = 1;
		if (!m_InvItem[ARM_RHAND].IsEmpty())
		{
			switch (m_InvItem[ARM_RHAND].bType)
			{
			case TYPE_BOW:
				nDistance = 9;
				break;
			case TYPE_SPEAR:
				nDistance = 2;
				break;
			default:
				nDistance = 1;
				break;
			}
		}
		if (!pNpc->GetDistance(m_sX, m_sY, nDistance))
		{
			SendAttackFail(iTargetId);
			return;
		}

		// Lets see if the attack was a hit or miss!
		if (!CheckAttackSuccess(pNpc))
		{
			SendAttackMiss(iTargetId);

			// Decrease dura of bows because a hit was missed they still use arrows. :P
			if (!m_InvItem[ARM_RHAND].IsEmpty() && m_InvItem[ARM_RHAND].bType == TYPE_BOW)
			{
				DecreaseWeaponItemDura();
			}
			return;
		}
		// NOTE if attack missed below code is not executed.

		// Calculate the damage to be dealt
		int iDefense = pNpc->GetDefense();
		int iAttack = GetAttack();

		int iDamage = iAttack - iDefense;
		if (iDamage <= 0)
		{
			iDamage = myrand(0, 2);
		}
		if (iDamage > pNpc->m_sHP)
		{
			iDamage = pNpc->m_sHP;
		}

		// Do the damage!
		pNpc->m_sHP -= iDamage;

		if (pNpc->CheckAIType(NPCAITYPE_DUMMY)) // Dummy
		{
			pNpc->m_sHP = pNpc->m_sMaxHp;
			SendDummyAttack(iTargetId, iDamage);
		}

		bool bNpcDied = false;

		// Do stuff if the npc has died
		if (pNpc->m_sHP <= 0)
		{
			NpcDeadEvent(pNpc);
			pNpc->SetDead(m_pCom, m_Uid, true);
			bNpcDied = true;
			PlusMoralByNpc(pNpc->m_iMoral);
			if (CheckPartyLeader())
			{
				int iPlus = GetInc(INC_CHA);
				iPlus *= pNpc->m_sCharm;
				if (!CheckDemon(m_sClass))
				{
					PlusCha(iPlus);
				}
				m_iCHAInc += iPlus;
			}
			NpcThrowItem(pNpc);
			NpcThrowMoney(pNpc);
		}

		DWORD dwGainedExp = static_cast<DWORD>(((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) * 100.0);
		m_dwExp += dwGainedExp;

		CString str;
		str.Format(IDS_GM_MSG_ATTACK_INFO, iDamage, dwGainedExp);
		SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

		if (m_dwExp > m_dwMaxExp)
		{
			CheckLevelUp();
		}

		// The attack was a success! yay :D
		SendAttackSuccess(iTargetId, pNpc->m_sHP, pNpc->m_sMaxHp);

		// GetInc functions to get the inc values for
		// str, dex, int, wis and con.
		int iSTRInc = GetInc(INC_STR);
		int iDEXInc = GetInc(INC_DEX);
		int iINTInc = GetInc(INC_INT);
		int iWISInc = GetInc(INC_WIS);
		int iCONInc = GetInc(INC_CON);

		// Alter Stat Gain Inc values.
		// Weapons give different stat %
		// Some stats cannot be gained
		if (!CheckDemon(m_sClass))
		{
			if (!m_InvItem[ARM_RHAND].IsEmpty())
			{
				// Calculate and change the str inc and dex inc
				// different weapons give different % on stat gain.
				CalcPlusValue(INC_STR, iSTRInc);
				CalcPlusValue(INC_DEX, iDEXInc);

				// Cannot gain these stats if wearing a weapon.
				iCONInc = 0;
				iINTInc = 0;
				iWISInc = 0;
			}
			else
			{
				// Cannot gain these stats.
				iSTRInc = 0;
				iDEXInc = 0;
				iINTInc = 0;
				iWISInc = 0;
			}
		}
		else
		{
			CalcPlusValue(INC_STR, iSTRInc);
			CalcPlusValue(INC_DEX, iDEXInc);
			CalcPlusValue(INC_INT, iINTInc);
			CalcPlusValue(INC_WIS, iWISInc);
			CalcPlusValue(INC_CON, iCONInc);
		}

		// Checks for stone guard and normal guards decreasing moral
		// Moral is decreased by 100000.
		if (pNpc->IsDetecter() || pNpc->IsStone())
		{
			bool bSkip = false;
			for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
			{
				if (g_bGuildTownWar[i] && pNpc->m_sGuild == g_arGuildTownData[i]->sTownNum)
				{
					bSkip = true;
					break;
				}
			}

			if (!bSkip && (CheckDemon(m_sClass) == CheckDemon(pNpc->m_sClass)))
			{
				bool bRet = true;
				if (!CheckDemon(m_sClass))
				{
					bRet = SetMoral(-100000);
				}
				else
				{
					bRet = SetMoral(100000);
				}

				if (bRet)
				{
					SendCharData(INFO_BASICVALUE);
					SendMyInfo(TO_INSIGHT, INFO_MODIFY);
				}
			}
		}
		else if (pNpc->CheckAIType(NPCAITYPE_NORMAL)) // Normal AI
		{
			if (!m_InvItem[ARM_RHAND].IsEmpty())
			{
				// Increase weapon skill exp
				// more exp is given to slower weapons.
				int iWeaponExpPercent = 100;
				if (m_InvItem[ARM_RHAND].sTime >= 1250)
				{
					iWeaponExpPercent = 115;
				}
				else if (m_InvItem[ARM_RHAND].sTime >= 1150)
				{
					iWeaponExpPercent = 110;
				}

				PlusWeaponExpByAttack(iWeaponExpPercent);
			}

			// Con for hsoma is increased seperately
			if (!CheckDemon(m_sClass))
			{
				PlusCon(iCONInc);
				m_iCONInc += iCONInc;
			}

			// Increase following stats by their inc value
			// STR, DEX, INT and WIS (and CON if dsoma)
			// GetPlusValueCount calculates how many times we add the inc value.
			int PlusValueCount = GetPlusValueCount(pNpc, iDamage, bNpcDied, INC_STR);
			for (int i = 0; i < PlusValueCount; i++)
			{
				if (!CheckDemon(m_sClass))
				{
					PlusStr(iSTRInc);
					PlusDex(iDEXInc);
					PlusInt(iINTInc);
					PlusWis(iWISInc);
				}

				m_iSTRInc += iSTRInc;
				m_iDEXInc += iDEXInc;
				m_iINTInc += iINTInc;
				m_iWISInc += iWISInc;

				if (CheckDemon(m_sClass))
				{
					m_iCONInc += iCONInc;
				}
			}
		}
	}

	// The End \o/!
}

void USER::EncryptionStartReq(TCHAR* pBuf)
{
	if (m_CryptionFlag != 0)
	{
		CString entry = "Encryption start request received and cryptography has already been initialized";
		g_pMainDlg->LogToFile(entry);
		return;
	}

	int index = 0;
	SetByte(m_TempBuf, PKT_ENCRYPTION_START_REQ, index);
	SetByte(m_TempBuf, 1, index);
	SetString(m_TempBuf, (char *)SomaEncryption.m_Key1, 8, index);
	SetString(m_TempBuf, (char *)SomaEncryption.m_Key2, 8, index);

	m_CryptionFlag = 0;

	Send(m_TempBuf, index);

	m_CryptionFlag = 1;
}

void USER::MagicArrow(TCHAR* pBuf)
{
	BYTE bSuccess = FAIL;
	int index = 0;
	short sDir = GetShort(pBuf, index);
	int iTargetId = GetInt(pBuf , index);
	short sMagicNo = GetShort(pBuf, index);

	// Set direction the user is now facing
	m_byDir = static_cast<BYTE>(sDir);

	// Validate the magic being used
	if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
	{
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	CMagicTable *pMagic = g_arMagicTable.GetAt(sMagicNo-1);
	if (!pMagic)
	{
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	if (!IsHaveMagic(sMagicNo, MAGIC_TYPE_MAGIC))
	{
		TRACE("User does not have magic\n");
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	// Fail casting if reflection still remains
	if (CheckRemainMagic(MAGIC_EFFECT_REFLECT))
	{
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	// Cannot attack when warping
	if (m_bIsWarping)
	{
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	// Cannot attack when in peace mode
	if (m_BattleMode == BATTLEMODE_NORMAL)
	{
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	// Target Id must be valid
	if (iTargetId < 0 || iTargetId >= INVALID_BAND)
	{
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	// Concussion means you are unable to attack
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION))
	{
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	// Set the amount of MP it will cost
	short sMpDec = pMagic->m_sMpdec;
	// sEDist
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_MPSAVE))
	{
		sMpDec = (sMpDec * m_RemainSpecial[SPECIAL_EFFECT_MPSAVE].sDamage) / 100;
	}

	// Moral Checks
	if (!CheckDemon(m_sClass))
	{
		int iMoral = m_iMoral / CLIENT_MORAL;
		if (iMoral < pMagic->m_sMinMoral)
		{
			SendServerChatMessage(IDS_USER_MAGIC_MORAL_LOW, TO_ME);
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		if (iMoral > pMagic->m_sMaxMoral)
		{
			SendServerChatMessage(IDS_USER_MAGIC_MORAL_HIGH, TO_ME);
			SendMagicAttack(FAIL, iTargetId);
			return;
		}
	}

	int iINT = m_iINT / CLIENT_BASE_STATS;
	if (iINT < pMagic->m_sMinInt)
	{
		SendServerChatMessage(IDS_USER_MAGIC_SKILL_LOW, TO_ME);
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	int iMagicExp = 0;
	switch (pMagic->m_tClass)
	{
	case 1:
		iMagicExp = m_iBMagicExp / CLIENT_SKILL;
		break;
	case 2:
		iMagicExp = m_iWMagicExp / CLIENT_SKILL;
		break;
	case 3:
		iMagicExp = m_iDMagicExp / CLIENT_SKILL;
		break;
	default:
		break;
	}

	if (iMagicExp < pMagic->m_sMinMxp)
	{
		SendServerChatMessage(IDS_USER_MAGIC_SKILL_LOW, TO_ME);
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	// If user is not wearing a staff and doesn't have magic avail special
	// then the user cannot cast the magic.
	if (!CheckHaveSpecialNumInBody(SPECIAL_MAGIC_AVAIL))
	{
		if ((!m_InvItem[ARM_RHAND].IsEmpty() && m_InvItem[ARM_RHAND].bType != TYPE_STAFF) ||
			!m_InvItem[ARM_LHAND].IsEmpty())
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}
	}

	// This function handles only arrow magic types
	if (pMagic->m_tType01 != MAGIC_TYPE_ARROW)
	{
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	// Check delay of the magic only if it has a delay
	if (pMagic->m_sStartTime > 0)
	{
		if (!m_bMagicCasted ||
			m_sMagicNoCasted != sMagicNo ||
			!CheckMagicDelay(pMagic))
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}
	}

	// GetInc functions to get the inc values for
	// str, dex, int, wis and con.
	// str, dex and con only increases for dsoma characters
	int iSTRInc = GetInc(INC_STR);
	int iDEXInc = GetInc(INC_DEX);
	int iINTInc = GetInc(INC_INT);
	int iWISInc = GetInc(INC_WIS);
	int iCONInc = GetInc(INC_CON);
	double dSTRIncRate = 0;
	double dDEXIncRate = 0;
	double dINTIncRate = 1;
	double dWISIncRate = 1;
	double dCONIncRate = 0;

	// Dsoma characters can gain str, dex and con
	// when using magic in here we set the rates
	if (CheckDemon(m_sClass))
	{
		dSTRIncRate = 0.2;
		if (pMagic->m_tType02 == 1)
		{
			dDEXIncRate = 0.4;
			dINTIncRate = 1.3;
			dWISIncRate = 0.8;
			dCONIncRate = 0.3;
		}
		else
		{
			dDEXIncRate = 0.3;
			dINTIncRate = 0.8;
			dWISIncRate = 1.3;
			dCONIncRate = 0.4;
		}
	}

	iSTRInc = static_cast<int>(iSTRInc * dSTRIncRate);
	iDEXInc = static_cast<int>(iDEXInc * dDEXIncRate);
	iINTInc = static_cast<int>(iINTInc * dINTIncRate);
	iWISInc = static_cast<int>(iWISInc * dWISIncRate);
	iCONInc = static_cast<int>(iCONInc * dCONIncRate);

	// Must have the MP needed to use the magic
	if (sMpDec > m_sMP)
	{
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	m_bMagicCasted = false;

	// Check if the magic cannot be casted on self
	if (pMagic->m_tTarget == MAGIC_TARGET_OTHER && m_Uid + USER_BAND == iTargetId)
	{
		SendMagicAttack(FAIL, iTargetId);
		return;
	}

	if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
	{
		USER* pTarget = GetUser(iTargetId - USER_BAND);
		if (!pTarget)
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		// Certain things need to be checked only if the magic does damage
		if (pMagic->m_sDamage > 0)
		{
			// Must check that the player can be PKed
			if (!CheckCanPK(pTarget))
			{
				CString strMessage;
				strMessage.Format(IDS_USER_CANNOT_PK_LEVEL_BELOW, LEVEL_CAN_PK);
				SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
				SendMagicAttack(FAIL, iTargetId);
				return;
			}

			// Check for safety zone
			if (CheckSafetyZone() || pTarget->CheckSafetyZone())
			{
				SendMagicAttack(FAIL, iTargetId);
				return;
			}

			// TODO: Comment me
			if (CheckDemon(m_sClass) == CheckDemon(pTarget->m_sClass) &&
				m_BattleMode == BATTLEMODE_ATTACK &&
				pTarget->m_iGrayCount == 0 &&
				!CheckGuildWarArmy(pTarget))
			{
				SendMagicAttack(FAIL, iTargetId);
				return;
			}
		}

		// TODO : Maybe move this Live check on the target closer to the first check to be made.
		if (!pTarget->m_bLive ||
			pTarget->m_State != STATE_GAMESTARTED ||
			pTarget->m_bHidden ||
			pTarget->m_bIsWarping)
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		// Certain maps have walls and these need to be checked for
		// and prevent attacks through them.
		CPoint ptPos = ConvertToClient(m_sX, m_sY);
		CPoint ptPosTarget = ConvertToClient(pTarget->m_sX, pTarget->m_sY);
		if (!ArrowPathFind(ptPos, ptPosTarget))
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		// Increase distance of magic spell for healing type magic when
		// the same party as the target
		int iDistance = pMagic->m_sEDist;
		if (pMagic->m_sDamage < 0)
		{
			if (m_bInParty && pTarget->m_bInParty)
			{
				if (m_PartyMembers[0].uid == pTarget->m_PartyMembers[0].uid &&
					strcmp(m_PartyMembers[0].m_strUserId, pTarget->m_PartyMembers[0].m_strUserId) == 0)
				{
					iDistance = 8;
				}
			}
		}

		if (!CheckDistance(pTarget, iDistance))
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		m_sMP -= sMpDec;

		// Check if the target can be pked
		bool bCanPKTarget = CheckPK(pTarget);

		// Perform the magic effect on the target
		int iDamage = DoMagicEffect(pMagic, pTarget);

		// Do things if user died
		if (pTarget->m_lDeadUsed == 1)
		{
			// If the target died check for any gained pos
			CheckClassPoint(pTarget);
		}

		// Do extra things if the user pked another user
		if (bCanPKTarget)
		{
			if (pTarget->m_lDeadUsed == 1)
			{
				if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
					(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
					(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
				{
					if (SetMoralByPK(pTarget->m_iMoral))
					{
						SendCharData(INFO_BASICVALUE);
						SendMyInfo(TO_INSIGHT, INFO_MODIFY);
					}
				}
			}

			if (pMagic->m_sDamage > 0)
			{
				// Lets any guards know who are within area that a user has attacked another user
				IsDetecterRange();

				// Sets the user gray if they pass the checks
				if (CheckGray(pTarget))
				{
					SetGrayUser(GRAY_MODE_NORMAL);
				}
			}
		}

		SendMagicAttack(SUCCESS, iTargetId, sMagicNo, pTarget->m_sHP, pTarget->GetMaxHP(), pMagic->m_iTime);
	}
	else if (iTargetId >= NPC_BAND)				// NPC
	{
		CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
		if (!pNpc)
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		// Check if the npc is able to die.
		// This allows for preventing attack of a peaceful npc.
		if (pNpc->m_tNpcType == NPCTYPE_NPC)
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		if (pNpc->m_NpcState == NPC_DEAD || pNpc->m_NpcState == NPC_LIVE)
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		// Guards cannot be attacked by players when they are in battle mode (PK mode allows attack).
		if (pNpc->IsDetecter() && m_BattleMode == BATTLEMODE_ATTACK)
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		// You cannot use magic on npc that is for user targets only
		if (pMagic->m_tTarget == MAGIC_TARGET_USER)
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		// Certain maps have walls and these need to be checked for
		// and prevent attacks through them.
		CPoint ptPos = ConvertToClient(m_sX, m_sY);
		CPoint ptPosTarget = ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);
		if (!ArrowPathFind(ptPos, ptPosTarget))
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		// If AI Type of npc is 0 or 3 then do something with the AttackList of the npc oO
		// Need to look at this in more detail before making an implementation.
		if (pNpc->m_tNpcAttType == NPC_AT_PASSIVE && pMagic->m_sDamage > 0)
		{
			if (pNpc->CheckAIType(NPCAITYPE_NORMAL) || pNpc->CheckAIType(NPCAITYPE_STONE_GUARD))
			{
				pNpc->AttackListAdd(this);
			}
		}

		// Set the NPC target
		// Stones and Guard NPC types cannot have a target set
		// Target is only set if NPC state is standing, moving or attacking.
		if ((pNpc->m_NpcState == NPC_STANDING ||
			pNpc->m_NpcState == NPC_MOVING ||
			pNpc->m_NpcState == NPC_FIGHTING) &&
			!pNpc->IsDetecter() &&
			!pNpc->IsStone())
		{
			pNpc->m_iAttackedUid = m_Uid + USER_BAND;
			pNpc->m_NpcState = NPC_ATTACKING;
		}

		if (pNpc->m_NpcState == NPC_LIVE || pNpc->m_NpcState == NPC_DEAD)
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		if (!pNpc->GetDistance(m_sX, m_sY, pMagic->m_sEDist))
		{
			SendMagicAttack(FAIL, iTargetId);
			return;
		}

		m_sMP -= sMpDec;

		// Perform the magic effect on the target
		int iDamage = DoMagicEffect(pMagic, pNpc);

		SendMagicAttack(SUCCESS, iTargetId, sMagicNo, pNpc->m_sHP, pNpc->m_sMaxHp, pMagic->m_iTime);

		// Do stuff if the npc has died
		bool bNpcDied = false;
		if (pNpc->m_sHP <= 0)
		{
			NpcDeadEvent(pNpc);
			pNpc->SetDead(m_pCom, m_Uid, true);
			bNpcDied = true;
			PlusMoralByNpc(pNpc->m_iMoral);
			if (CheckPartyLeader())
			{
				int iPlus = GetInc(INC_CHA);
				iPlus *= pNpc->m_sCharm;
				if (!CheckDemon(m_sClass))
				{
					PlusCha(iPlus);
				}
				m_iCHAInc += iPlus;
			}
			NpcThrowItem(pNpc);
			NpcThrowMoney(pNpc);
		}

		// Checks for stone guard and normal guards decreasing moral
		// Moral is decreased by 100000.
		if (pNpc->IsDetecter() || pNpc->IsStone())
		{
			bool bSkip = false;
			for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
			{
				if (g_bGuildTownWar[i] && pNpc->m_sGuild == g_arGuildTownData[i]->sTownNum)
				{
					bSkip = true;
					break;
				}
			}

			if (!bSkip && (CheckDemon(m_sClass) == CheckDemon(pNpc->m_sClass)))
			{
				bool bRet = true;
				if (!CheckDemon(m_sClass))
				{
					bRet = SetMoral(-100000);
				}
				else
				{
					bRet = SetMoral(100000);
				}

				if (bRet)
				{
					SendCharData(INFO_BASICVALUE);
					SendMyInfo(TO_INSIGHT, INFO_MODIFY);
				}
			}
		}
		else if (pNpc->CheckAIType(NPCAITYPE_NORMAL) && CheckPlusType(pMagic, 6)) // Normal AI
		{
			SetMagicExp(pMagic);

			if (!m_InvItem[ARM_RHAND].IsEmpty() &&
				m_InvItem[ARM_RHAND].bType == TYPE_STAFF)
			{
				// Increase Staff Weapon Skill Exp
				int iStaffInc = GetInc(INC_STAFF);
				PlusWeaponExp(6, iStaffInc);
			}

			// Increase following stats by their inc value
			// STR, DEX, INT and WIS (and CON if dsoma)
			// GetPlusValueCount calculates how many times we add the inc value.
			int PlusValueCount = GetPlusValueCount(pNpc, iDamage, bNpcDied, INC_INT);
			for (int i = 0; i < PlusValueCount; i++)
			{
				if (!CheckDemon(m_sClass))
				{
					PlusStr(iSTRInc);
					PlusDex(iDEXInc);
					PlusInt(iINTInc);
					PlusWis(iWISInc);
					PlusCon(iCONInc);
				}

				m_iSTRInc += iSTRInc;
				m_iDEXInc += iDEXInc;
				m_iINTInc += iINTInc;
				m_iWISInc += iWISInc;
				m_iCONInc += iCONInc;
			}
		}
	}
}

void USER::MagicRail(TCHAR* pBuf)
{
	BYTE bSuccess = FAIL;
	int index = 0;
	short sDir = GetShort(pBuf, index);
	short sMagicNo = GetShort(pBuf, index);
	BYTE byDir = GetByte(pBuf, index);

	// Set direction the user is now facing
	m_byDir = static_cast<BYTE>(sDir);

	// Validate the magic being used
	if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	CMagicTable *pMagic = g_arMagicTable.GetAt(sMagicNo-1);
	if (!pMagic)
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	if (!IsHaveMagic(sMagicNo, MAGIC_TYPE_MAGIC))
	{
		TRACE("User does not have magic\n");
		SendMagicRailAttack(FAIL);
		return;
	}

	// Fail casting if reflection still remains
	if (CheckRemainMagic(MAGIC_EFFECT_REFLECT))
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	// Cannot attack when warping
	if (m_bIsWarping)
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	// Cannot attack when in peace mode
	if (m_BattleMode == BATTLEMODE_NORMAL)
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	// Concussion means you are unable to attack
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION))
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	short sMpDec = pMagic->m_sMpdec;
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_MPSAVE))
	{
		sMpDec = (sMpDec * m_RemainSpecial[SPECIAL_EFFECT_MPSAVE].sDamage) / 100;
	}
	if (sMpDec > m_sMP)
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	// sEDist
	// sMpDec

	// CheckRemainSpecialAttack(SPECIAL_EFFECT_MPSAVE)

	// Moral Checks
	if (!CheckDemon(m_sClass))
	{
		int iMoral = m_iMoral / CLIENT_MORAL;
		if (iMoral < pMagic->m_sMinMoral)
		{
			SendServerChatMessage(IDS_USER_MAGIC_MORAL_LOW, TO_ME);
			SendMagicRailAttack(FAIL);
			return;
		}

		if (iMoral > pMagic->m_sMaxMoral)
		{
			SendServerChatMessage(IDS_USER_MAGIC_MORAL_HIGH, TO_ME);
			SendMagicRailAttack(FAIL);
			return;
		}
	}

	int iINT = m_iINT / CLIENT_BASE_STATS;
	if (iINT < pMagic->m_sMinInt)
	{
		SendServerChatMessage(IDS_USER_MAGIC_SKILL_LOW, TO_ME);
		SendMagicRailAttack(FAIL);
		return;
	}

	int iMagicExp = 0;
	switch (pMagic->m_tClass)
	{
	case 1:
		iMagicExp = m_iBMagicExp / CLIENT_SKILL;
		break;
	case 2:
		iMagicExp = m_iWMagicExp / CLIENT_SKILL;
		break;
	case 3:
		iMagicExp = m_iDMagicExp / CLIENT_SKILL;
		break;
	default:
		break;
	}

	if (iMagicExp < pMagic->m_sMinMxp)
	{
		SendServerChatMessage(IDS_USER_MAGIC_SKILL_LOW, TO_ME);
		SendMagicRailAttack(FAIL);
		return;
	}

	// If user is not wearing a staff and doesn't have magic avail special
	// then the user cannot cast the magic.
	if (!CheckHaveSpecialNumInBody(SPECIAL_MAGIC_AVAIL))
	{
		if ((!m_InvItem[ARM_RHAND].IsEmpty() && m_InvItem[ARM_RHAND].bType != TYPE_STAFF) ||
			!m_InvItem[ARM_LHAND].IsEmpty())
		{
			SendMagicRailAttack(FAIL);
			return;
		}
	}

	if (pMagic->m_tType01 != MAGIC_TYPE_RAIL)
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	if (pMagic->m_sStartTime > 0)
	{
		if (!m_bMagicCasted ||
			m_sMagicNoCasted != sMagicNo ||
			!CheckMagicDelay(pMagic))
		{
			SendMagicRailAttack(FAIL);
			return;
		}
	}

	// TODO:
	//if (pMagic->m_tType02 == 1)
	//		something = 150
	//
	//

	if (!CheckDemon(m_sClass))
	{

	}

	// TODO: Some kind of stat code goes between here

	m_bMagicCasted = false;

	if (pMagic->m_tTarget == MAGIC_TARGET_SELF)
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	if (pMagic->m_sEDist <= 0)
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	short sXAdd = 0;
	short sYAdd = 0;
	switch (byDir)
	{
	case DIR_DOWN:
		sYAdd = 1;
		break;
	case DIR_DOWN_LEFT:
		sXAdd = -1;
		sYAdd = 1;
		break;
	case DIR_LEFT:
		sXAdd = -1;
		break;
	case DIR_UP_LEFT:
		sXAdd = -1;
		sYAdd = -1;
		break;
	case DIR_UP:
		sYAdd = -1;
		break;
	case DIR_UP_RIGHT:
		sXAdd = 1;
		sYAdd = -1;
		break;
	case DIR_RIGHT:
		sXAdd = 1;
		break;
	case DIR_DOWN_RIGHT:
		sXAdd = 1;
		sYAdd = 1;
		break;
	}

	int iTargetCount = 0;

	int iTargetsId[50] = {};
	short sTargetsHP[50] = {};
	short sTargetsMaxHP[50] = {};

	for (int i = 1; i < pMagic->m_sEDist; i++)
	{
		short sX = m_sX + sXAdd * i;
		short sY = m_sY + sYAdd * i;

		ASSERT(m_iZoneIndex >= 0 && m_iZoneIndex < g_Zones.GetSize());
		ASSERT(g_Zones[m_iZoneIndex]);

		if (sX > g_Zones[m_iZoneIndex]->m_sizeMap.cx || sX < 0) continue;
		if (sY > g_Zones[m_iZoneIndex]->m_sizeMap.cy || sY < 0) continue;

		if (abs(m_sX - sX) > pMagic->m_sEDist) continue;
		if (abs(m_sY - sY) > pMagic->m_sEDist) continue;

		int iTargetId = GetUid(sX, sY);
		if (iTargetId < USER_BAND || iTargetId == m_Uid + USER_BAND) continue;

		if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
		{
			USER* pTarget = GetUser(iTargetId - USER_BAND);
			if (!pTarget) continue;

			// Certain things need to be checked only if the magic does damage
			if (pMagic->m_sDamage > 0)
			{
				// Must check that the player can be PKed
				if (!CheckCanPK(pTarget))
				{
					CString strMessage;
					strMessage.Format(IDS_USER_CANNOT_PK_LEVEL_BELOW, LEVEL_CAN_PK);
					SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
					continue;
				}

				// Check for safety zone
				if (CheckSafetyZone() || pTarget->CheckSafetyZone())
				{
					continue;
				}

				// TODO: Comment me
				if (CheckDemon(m_sClass) == CheckDemon(pTarget->m_sClass) &&
					m_BattleMode == BATTLEMODE_ATTACK &&
					pTarget->m_iGrayCount == 0 &&
					!CheckGuildWarArmy(pTarget))
				{
					continue;
				}
			}

			if (!pTarget->m_bLive ||
				pTarget->m_State != STATE_GAMESTARTED ||
				pTarget->m_bHidden ||
				pTarget->m_bIsWarping)
			{
				continue;
			}

			// Certain maps have walls and these need to be checked for
			// and prevent attacks through them.
			CPoint ptPos = ConvertToClient(m_sX, m_sY);
			CPoint ptPosTarget = ConvertToClient(pTarget->m_sX, pTarget->m_sY);
			if (!ArrowPathFind(ptPos, ptPosTarget))
			{
				continue;
			}

			// Remove the target from the location on map if for some reason they should no longer be there
			if (sX != pTarget->m_sX || sY != pTarget->m_sY)
			{
				g_Zones[m_iZoneIndex]->m_pMap[sX][sY].m_lUser = 0;
				continue;
			}

			iTargetsId[iTargetCount] = iTargetId;
			sTargetsMaxHP[iTargetCount] = pTarget->GetMaxHP();

			TRACE("MagicRail - Found USER Target: %d\n", iTargetsId[iTargetCount]);
			++iTargetCount;
		}
		else if (iTargetId >= NPC_BAND)						// NPC
		{
			CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
			if (!pNpc) continue;

			// Check if the npc is able to die.
			// This allows for preventing attack of a peaceful npc.
			if (pNpc->m_tNpcType == NPCTYPE_NPC) continue;

			if (pNpc->m_NpcState == NPC_DEAD || pNpc->m_NpcState == NPC_LIVE) continue;

			// Guards cannot be attacked by players when they are in battle mode (PK mode allows attack).
			if (pNpc->IsDetecter() && m_BattleMode == BATTLEMODE_ATTACK) continue;

			// You cannot use magic on npc that is for user targets only
			if (pMagic->m_tTarget == MAGIC_TARGET_USER) continue;

			// Certain maps have walls and these need to be checked for
			// and prevent attacks through them.
			CPoint ptPos = ConvertToClient(m_sX, m_sY);
			CPoint ptPosTarget = ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);
			if (!ArrowPathFind(ptPos, ptPosTarget))
			{
				SendMagicAttack(FAIL, iTargetId);
				return;
			}

			// If AI Type of npc is NPCAITYPE_NORMAL or NPCAITYPE_STONE_GUARD then do something with the AttackList of the npc oO
			// Need to look at this in more detail before making an implementation.
			if (pNpc->m_tNpcAttType == NPC_AT_PASSIVE && pMagic->m_sDamage > 0)
			{
				if (pNpc->CheckAIType(NPCAITYPE_NORMAL) || pNpc->CheckAIType(NPCAITYPE_STONE_GUARD))
				{
					pNpc->AttackListAdd(this);
				}
			}

			// Set the NPC target
			// Stones and Guard NPC types cannot have a target set
			// Target is only set if NPC state is standing, moving or attacking.
			if ((pNpc->m_NpcState == NPC_STANDING ||
				pNpc->m_NpcState == NPC_MOVING ||
				pNpc->m_NpcState == NPC_FIGHTING) &&
				!pNpc->IsDetecter() &&
				!pNpc->IsStone())
			{
				pNpc->m_iAttackedUid = m_Uid + USER_BAND;
				pNpc->m_NpcState = NPC_ATTACKING;
			}

			// Remove the target from the location on map if for some reason they should no longer be there
			if (sX != pNpc->m_sCurX || sY != pNpc->m_sCurY)
			{
				g_Zones[m_iZoneIndex]->m_pMap[sX][sY].m_lUser = 0;
				continue;
			}

			iTargetsId[iTargetCount] = iTargetId;
			sTargetsMaxHP[iTargetCount] = pNpc->m_sMaxHp;

			TRACE("MagicRail - Found NPC Target: %d\n", iTargetsId[iTargetCount]);
			++iTargetCount;
		}

		if (iTargetCount >= 50) break;
	}

	// Make sure there are targets before continuing
	if (iTargetCount <= 0)
	{
		SendMagicRailAttack(FAIL);
		return;
	}

	// Decrease MP
	m_sMP -= sMpDec;

	for (int i = 0; i < iTargetCount; i++)
	{
		int iTargetId = iTargetsId[i];

		if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
		{
			USER* pTarget = GetUser(iTargetId - USER_BAND);
			if (!pTarget)
			{
				SendMagicRailAttack(FAIL);
				return;
			}

			// Check if the target can be pked
			bool bCanPKTarget = CheckPK(pTarget);

			// Perform the magic effect on the target
			int iDamage = DoMagicEffect(pMagic, pTarget);
			sTargetsHP[i] = pTarget->m_sHP;

			// If the target died check for any gained pos
			if (pTarget->m_lDeadUsed == 1)
			{
				CheckClassPoint(pTarget);
			}

			// Do extra things if the user pked another user
			if (bCanPKTarget)
			{
				if (pTarget->m_lDeadUsed == 1)
				{
					if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
						(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
						(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
					{
						if (SetMoralByPK(pTarget->m_iMoral))
						{
							SendCharData(INFO_BASICVALUE);
							SendMyInfo(TO_INSIGHT, INFO_MODIFY);
						}
					}
				}

				if (pMagic->m_sDamage > 0)
				{
					// Lets any guards know who are within area that a user has attacked another user
					IsDetecterRange();

					// Sets the user gray if they pass the checks
					if (CheckGray(pTarget))
					{
						SetGrayUser(GRAY_MODE_NORMAL);
					}
				}
			}
		}
		else if (iTargetId >= NPC_BAND)						// NPC
		{
			CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
			if (!pNpc)
			{
				SendMagicRailAttack(FAIL);
				return;
			}

			// Perform the magic effect on the target
			int iDamage = DoMagicEffect(pMagic, pNpc);
			sTargetsHP[i] = pNpc->m_sHP;

			// Do stuff if the npc has died
			bool bNpcDied = false;
			if (pNpc->m_sHP <= 0)
			{
				NpcDeadEvent(pNpc);
				pNpc->SetDead(m_pCom, m_Uid, true);
				bNpcDied = true;
				PlusMoralByNpc(pNpc->m_iMoral);
				if (CheckPartyLeader())
				{
					int iPlus = GetInc(INC_CHA);
					iPlus *= pNpc->m_sCharm;
					if (!CheckDemon(m_sClass))
					{
						PlusCha(iPlus);
					}
					m_iCHAInc += iPlus;
				}
				NpcThrowItem(pNpc);
				NpcThrowMoney(pNpc);
			}

			int iPlusValueCount = 0;

			// Checks for stone guard and normal guards decreasing moral
			// Moral is decreased by 100000.
			if (pNpc->IsDetecter() || pNpc->IsStone())
			{
				bool bSkip = false;
				for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
				{
					if (g_bGuildTownWar[i] && pNpc->m_sGuild == g_arGuildTownData[i]->sTownNum)
					{
						bSkip = true;
						break;
					}
				}

				if (!bSkip && (CheckDemon(m_sClass) == CheckDemon(pNpc->m_sClass)))
				{
					bool bRet = true;
					if (!CheckDemon(m_sClass))
					{
						bRet = SetMoral(-100000);
					}
					else
					{
						bRet = SetMoral(100000);
					}

					if (bRet)
					{
						SendCharData(INFO_BASICVALUE);
						SendMyInfo(TO_INSIGHT, INFO_MODIFY);
					}
				}
			}
			else if (pNpc->CheckAIType(NPCAITYPE_NORMAL) &&  // Normal AI
				CheckPlusType(pMagic, 6))
			{
				// Increase following stats by their inc value
				// STR, DEX, INT and WIS (and CON if dsoma)
				// GetPlusValueCount calculates how many times we add the inc value.
				iPlusValueCount = GetPlusValueCount(pNpc, iDamage, bNpcDied, INC_INT);
			}

			if (m_dwExp > m_dwMaxExp)
			{
				CheckLevelUp();
			}

			// GetInc functions to get the inc values for
			// str, dex, int, wis and con.
			// str, dex and con only increases for dsoma characters
			int iSTRInc = GetInc(INC_STR);
			int iDEXInc = GetInc(INC_DEX);
			int iINTInc = GetInc(INC_INT);
			int iWISInc = GetInc(INC_WIS);
			int iCONInc = GetInc(INC_CON);
			double dSTRIncRate = 0;
			double dDEXIncRate = 0;
			double dINTIncRate = 1;
			double dWISIncRate = 1;
			double dCONIncRate = 0;

			// Dsoma characters can gain str, dex and con
			// when using magic in here we set the rates
			if (CheckDemon(m_sClass))
			{
				dSTRIncRate = 0.2;
				if (pMagic->m_tType02 == 1)
				{
					dDEXIncRate = 0.4;
					dINTIncRate = 1.3;
					dWISIncRate = 0.8;
					dCONIncRate = 0.3;
				}
				else
				{
					dDEXIncRate = 0.3;
					dINTIncRate = 0.8;
					dWISIncRate = 1.3;
					dCONIncRate = 0.4;
				}
			}

			iSTRInc = static_cast<int>(iSTRInc * dSTRIncRate);
			iDEXInc = static_cast<int>(iDEXInc * dDEXIncRate);
			iINTInc = static_cast<int>(iINTInc * dINTIncRate);
			iWISInc = static_cast<int>(iWISInc * dWISIncRate);
			iCONInc = static_cast<int>(iCONInc * dCONIncRate);

			for (int i = 0; i < iPlusValueCount; i++)
			{
				if (!CheckDemon(m_sClass))
				{
					PlusStr(iSTRInc);
					PlusDex(iDEXInc);
					PlusInt(iINTInc);
					PlusWis(iWISInc);
					PlusCon(iCONInc);
				}

				m_iSTRInc += iSTRInc;
				m_iDEXInc += iDEXInc;
				m_iINTInc += iINTInc;
				m_iWISInc += iWISInc;
				m_iCONInc += iCONInc;
			}

			SetMagicExp(pMagic);

			// Increase Staff Weapon Skill Exp
			if (!m_InvItem[ARM_RHAND].IsEmpty() &&
				m_InvItem[ARM_RHAND].bType == TYPE_STAFF)
			{
				int iStaffInc = GetInc(INC_STAFF);
				PlusWeaponExp(6, iStaffInc);
			}
		}
	}

	SendMagicRailAttack(SUCCESS, static_cast<BYTE>(pMagic->m_sEDist), iTargetCount, iTargetsId, sMagicNo, sTargetsHP, sTargetsMaxHP, pMagic->m_iTime);
}

void USER::MagicCircle(TCHAR* pBuf)
{
	BYTE bSuccess = FAIL;
	int index = 0;
	int iCastTargetId = GetInt(pBuf, index);
	short sDir = GetShort(pBuf, index);
	short sMagicNo = GetShort(pBuf, index);

	// Set direction the user is now facing
	m_byDir = static_cast<BYTE>(sDir);

	// Validate the magic being used
	if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	CMagicTable *pMagic = g_arMagicTable.GetAt(sMagicNo-1);
	if (!pMagic)
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	if (!IsHaveMagic(sMagicNo, MAGIC_TYPE_MAGIC))
	{
		TRACE("User does not have magic\n");
		SendMagicCircleAttack(FAIL);
		return;
	}

	// Check the target (who the magic is being casted on) is within range of the magic
	// ignore if the target is same as this user
	// Set the target X and Y which is the centre point for the magic range.
	short sTargetX = -1;
	short sTargetY = -1;
	if (iCastTargetId != m_Uid)
	{
		if (iCastTargetId >= USER_BAND && iCastTargetId < NPC_BAND)	// USER
		{
			USER* pUser = GetUser(iCastTargetId - USER_BAND);
			if (!CheckDistance(pUser, pMagic->m_sEDist))
			{
				SendMagicCircleAttack(FAIL);
				return;
			}
			sTargetX = pUser->m_sX;
			sTargetY = pUser->m_sY;
		}
		else if (iCastTargetId >= NPC_BAND)						// NPC
		{
			CNpc* pNpc = GetNpc(iCastTargetId - NPC_BAND);
			if (!pNpc->GetDistance(m_sX, m_sY, pMagic->m_sEDist))
			{
				SendMagicCircleAttack(FAIL);
				return;
			}
			sTargetX = pNpc->m_sCurX;
			sTargetY = pNpc->m_sCurY;
		}
	}
	else
	{
		sTargetX = m_sX;
		sTargetY = m_sY;
	}

	if (sTargetX == -1 || sTargetY == -1)
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	// Fail casting if reflection still remains
	if (CheckRemainMagic(MAGIC_EFFECT_REFLECT))
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	// Cannot attack when warping
	if (m_bIsWarping)
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	// Cannot attack when in peace mode
	if (m_BattleMode == BATTLEMODE_NORMAL)
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	// Concussion means you are unable to attack
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION))
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	// Moral Checks
	if (!CheckDemon(m_sClass))
	{
		int iMoral = m_iMoral / CLIENT_MORAL;
		if (iMoral < pMagic->m_sMinMoral)
		{
			SendServerChatMessage(IDS_USER_MAGIC_MORAL_LOW, TO_ME);
			SendMagicCircleAttack(FAIL);
			return;
		}

		if (iMoral > pMagic->m_sMaxMoral)
		{
			SendServerChatMessage(IDS_USER_MAGIC_MORAL_HIGH, TO_ME);
			SendMagicCircleAttack(FAIL);
			return;
		}
	}

	int iINT = m_iINT / CLIENT_BASE_STATS;
	if (iINT < pMagic->m_sMinInt)
	{
		SendServerChatMessage(IDS_USER_MAGIC_SKILL_LOW, TO_ME);
		SendMagicCircleAttack(FAIL);
		return;
	}

	int iMagicExp = 0;
	switch (pMagic->m_tClass)
	{
	case 1:
		iMagicExp = m_iBMagicExp / CLIENT_SKILL;
		break;
	case 2:
		iMagicExp = m_iWMagicExp / CLIENT_SKILL;
		break;
	case 3:
		iMagicExp = m_iDMagicExp / CLIENT_SKILL;
		break;
	default:
		break;
	}

	if (iMagicExp < pMagic->m_sMinMxp)
	{
		SendServerChatMessage(IDS_USER_MAGIC_SKILL_LOW, TO_ME);
		SendMagicCircleAttack(FAIL);
		return;
	}

	// If user is not wearing a staff and doesn't have magic avail special
	// then the user cannot cast the magic.
	if (!CheckHaveSpecialNumInBody(SPECIAL_MAGIC_AVAIL))
	{
		if ((!m_InvItem[ARM_RHAND].IsEmpty() && m_InvItem[ARM_RHAND].bType != TYPE_STAFF) ||
			!m_InvItem[ARM_LHAND].IsEmpty())
		{
			SendMagicCircleAttack(FAIL);
			return;
		}
	}

	if (pMagic->m_tType01 != MAGIC_TYPE_CIRCLE)
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	if (pMagic->m_sStartTime > 0)
	{
		if (!m_bMagicCasted ||
			m_sMagicNoCasted != sMagicNo ||
			!CheckMagicDelay(pMagic))
		{
			SendMagicCircleAttack(FAIL);
			return;
		}
	}

	// TODO:
	//if (pMagic->m_tType02 == 1)
	//		something = 150
	//
	//

	// something = 50

	short sMpDec = pMagic->m_sMpdec;
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_MPSAVE))
	{
		sMpDec = (sMpDec * m_RemainSpecial[SPECIAL_EFFECT_MPSAVE].sDamage) / 100;
	}
	if (sMpDec > m_sMP)
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	m_bMagicCasted = false;

	if (pMagic->m_tTarget == MAGIC_TARGET_SELF)
	{
		// TODO: this needs changing to what the 1p1 is doing!
		// There are no magic in database that have a magic target of self and are circle.
		SendMagicCircleAttack(FAIL);
		return;
	}

	if (pMagic->m_sRange <= 0)
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	int iTargetCount = 0;

	int iTargetsId[50] = {};
	short sTargetsHP[50] = {};
	short sTargetsMaxHP[50] = {};

	ASSERT(m_iZoneIndex >= 0 && m_iZoneIndex < g_Zones.GetSize());
	ASSERT(g_Zones[m_iZoneIndex]);

	short sMinX = sTargetX - pMagic->m_sRange;
	if (sMinX < 0) sMinX = 0;

	short sMaxX = sTargetX + pMagic->m_sRange;
	if (sMaxX > g_Zones[m_iZoneIndex]->m_sizeMap.cx) sMaxX = static_cast<short>(g_Zones[m_iZoneIndex]->m_sizeMap.cx);

	short sMinY = sTargetY - pMagic->m_sRange;
	if (sMinY < 0) sMinY = 0;

	short sMaxY = sTargetY + pMagic->m_sRange;
	if (sMaxY > g_Zones[m_iZoneIndex]->m_sizeMap.cy) sMaxY = static_cast<short>(g_Zones[m_iZoneIndex]->m_sizeMap.cy);

	for (short sX = sMinX; sX <= sMaxX; sX++)
	{
		for (short sY = sMinY; sY <= sMaxY; sY++)
		{
			int iTargetId = GetUid(sX, sY);
			if (iTargetId < USER_BAND) continue;

			if (pMagic->m_tTarget == MAGIC_TARGET_OTHER && m_Uid + USER_BAND == iTargetId) continue;

			if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
			{
				USER* pTarget = GetUser(iTargetId - USER_BAND);
				if (!pTarget) continue;

				// Certain things need to be checked only if the magic does damage
				if (pMagic->m_sDamage > 0)
				{
					// Must check that the player can be PKed
					if (!CheckCanPK(pTarget))
					{
						CString strMessage;
						strMessage.Format(IDS_USER_CANNOT_PK_LEVEL_BELOW, LEVEL_CAN_PK);
						SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
						continue;
					}

					// Check for safety zone
					if (CheckSafetyZone() || pTarget->CheckSafetyZone())
					{
						continue;
					}

					// TODO: Comment me
					if (CheckDemon(m_sClass) == CheckDemon(pTarget->m_sClass) &&
						m_BattleMode == BATTLEMODE_ATTACK &&
						pTarget->m_iGrayCount == 0 &&
						!CheckGuildWarArmy(pTarget))
					{
						continue;
					}
				}

				if (!pTarget->m_bLive ||
					pTarget->m_State != STATE_GAMESTARTED ||
					pTarget->m_bHidden ||
					pTarget->m_bIsWarping)
				{
					continue;
				}

				// Certain maps have walls and these need to be checked for
				// and prevent attacks through them.
				CPoint ptPos = ConvertToClient(m_sX, m_sY);
				CPoint ptPosTarget = ConvertToClient(pTarget->m_sX, pTarget->m_sY);
				if (!ArrowPathFind(ptPos, ptPosTarget))
				{
					continue;
				}

				// Remove the target from the location on map if for some reason they should no longer be there
				if (sX != pTarget->m_sX || sY != pTarget->m_sY)
				{
					g_Zones[m_iZoneIndex]->m_pMap[sX][sY].m_lUser = 0;
					continue;
				}

				iTargetsId[iTargetCount] = iTargetId;
				sTargetsMaxHP[iTargetCount] = pTarget->GetMaxHP();

				TRACE("MagicCircle - Found USER Target: %d\n", iTargetsId[iTargetCount]);
				++iTargetCount;
			}
			else if (iTargetId >= NPC_BAND)						// NPC
			{
				CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
				if (!pNpc) continue;

				// Check if the npc is able to die.
				// This allows for preventing attack of a peaceful npc.
				if (pNpc->m_tNpcType == NPCTYPE_NPC) continue;

				if (pNpc->m_NpcState == NPC_DEAD || pNpc->m_NpcState == NPC_LIVE) continue;

				// Guards cannot be attacked by players when they are in battle mode (PK mode allows attack).
				if (pNpc->IsDetecter() && m_BattleMode == BATTLEMODE_ATTACK) continue;

				// You cannot use magic on npc that is for user targets only
				if (pMagic->m_tTarget == MAGIC_TARGET_USER) continue;

				// Certain maps have walls and these need to be checked for
				// and prevent attacks through them.
				CPoint ptPos = ConvertToClient(m_sX, m_sY);
				CPoint ptPosTarget = ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);
				if (!ArrowPathFind(ptPos, ptPosTarget))
				{
					continue;
				}

				// If AI Type of npc is NPCAITYPE_NORMAL or NPCAITYPE_STONE_GUARD then do something with the AttackList of the npc oO
				// Need to look at this in more detail before making an implementation.
				if (pNpc->m_tNpcAttType == NPC_AT_PASSIVE && pMagic->m_sDamage > 0)
				{
					if (pNpc->CheckAIType(NPCAITYPE_NORMAL) || pNpc->CheckAIType(NPCAITYPE_STONE_GUARD))
					{
						pNpc->AttackListAdd(this);
					}
				}

				// Set the NPC target
				// Stones and Guard NPC types cannot have a target set
				// Target is only set if NPC state is standing, moving or attacking.
				if ((pNpc->m_NpcState == NPC_STANDING ||
					pNpc->m_NpcState == NPC_MOVING ||
					pNpc->m_NpcState == NPC_FIGHTING) &&
					!pNpc->IsDetecter() &&
					!pNpc->IsStone())
				{
					pNpc->m_iAttackedUid = m_Uid + USER_BAND;
					pNpc->m_NpcState = NPC_ATTACKING;
				}

				// Remove the target from the location on map if for some reason they should no longer be there
				if (sX != pNpc->m_sCurX || sY != pNpc->m_sCurY)
				{
					g_Zones[m_iZoneIndex]->m_pMap[sX][sY].m_lUser = 0;
					continue;
				}

				iTargetsId[iTargetCount] = iTargetId;
				sTargetsMaxHP[iTargetCount] = pNpc->m_sMaxHp;

				TRACE("MagicCircle - Found NPC Target: %d\n", iTargetsId[iTargetCount]);
				++iTargetCount;
			}

			if (iTargetCount >= 50) break;
		}
	}

	// Make sure there are targets before continuing
	if (iTargetCount <= 0)
	{
		SendMagicCircleAttack(FAIL);
		return;
	}

	// Decrease MP
	m_sMP -= sMpDec;

	for (int i = 0; i < iTargetCount; i++)
	{
		int iTargetId = iTargetsId[i];

		if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
		{
			USER* pTarget = GetUser(iTargetId - USER_BAND);
			if (!pTarget)
			{
				SendMagicCircleAttack(FAIL);
				return;
			}

			// Check if the target can be pked
			bool bCanPKTarget = CheckPK(pTarget);

			// Perform the magic effect on the target
			int iDamage = DoMagicEffect(pMagic, pTarget);
			sTargetsHP[i] = pTarget->m_sHP;

			// If the target died check for any gained pos
			if (pTarget->m_lDeadUsed == 1)
			{
				CheckClassPoint(pTarget);
			}

			// Do extra things if the user pked another user
			if (bCanPKTarget)
			{
				if (pTarget->m_lDeadUsed == 1)
				{
					if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
						(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
						(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
					{
						if (SetMoralByPK(pTarget->m_iMoral))
						{
							SendCharData(INFO_BASICVALUE);
							SendMyInfo(TO_INSIGHT, INFO_MODIFY);
						}
					}
				}

				if (pMagic->m_sDamage > 0)
				{
					// Lets any guards know who are within area that a user has attacked another user
					IsDetecterRange();

					// Sets the user gray if they pass the checks
					if (CheckGray(pTarget))
					{
						SetGrayUser(GRAY_MODE_NORMAL);
					}
				}
			}
		}
		else if (iTargetId >= NPC_BAND)						// NPC
		{
			CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
			if (!pNpc)
			{
				SendMagicCircleAttack(FAIL);
				return;
			}

			// Perform the magic effect on the target
			int iDamage = DoMagicEffect(pMagic, pNpc);
			sTargetsHP[i] = pNpc->m_sHP;

			// Do stuff if the npc has died
			bool bNpcDied = false;
			if (pNpc->m_sHP <= 0)
			{
				NpcDeadEvent(pNpc);
				pNpc->SetDead(m_pCom, m_Uid, true);
				bNpcDied = true;
				PlusMoralByNpc(pNpc->m_iMoral);
				if (CheckPartyLeader())
				{
					int iPlus = GetInc(INC_CHA);
					iPlus *= pNpc->m_sCharm;
					if (!CheckDemon(m_sClass))
					{
						PlusCha(iPlus);
					}
					m_iCHAInc += iPlus;
				}
				NpcThrowItem(pNpc);
				NpcThrowMoney(pNpc);
			}

			int iPlusValueCount = 0;

			// Checks for stone guard and normal guards decreasing moral
			// Moral is decreased by 100000.
			if (pNpc->IsDetecter() || pNpc->IsStone())
			{
				bool bSkip = false;
				for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
				{
					if (g_bGuildTownWar[i] && pNpc->m_sGuild == g_arGuildTownData[i]->sTownNum)
					{
						bSkip = true;
						break;
					}
				}

				if (!bSkip && (CheckDemon(m_sClass) == CheckDemon(pNpc->m_sClass)))
				{
					bool bRet = true;
					if (!CheckDemon(m_sClass))
					{
						bRet = SetMoral(-100000);
					}
					else
					{
						bRet = SetMoral(100000);
					}

					if (bRet)
					{
						SendCharData(INFO_BASICVALUE);
						SendMyInfo(TO_INSIGHT, INFO_MODIFY);
					}
				}
			}
			else if (pNpc->CheckAIType(NPCAITYPE_NORMAL) &&  // Normal AI
				CheckPlusType(pMagic, 6))
			{
				// Increase following stats by their inc value
				// STR, DEX, INT and WIS (and CON if dsoma)
				// GetPlusValueCount calculates how many times we add the inc value.
				iPlusValueCount = GetPlusValueCount(pNpc, iDamage, bNpcDied, INC_INT);
			}

			if (m_dwExp > m_dwMaxExp)
			{
				CheckLevelUp();
			}

			// GetInc functions to get the inc values for
			// str, dex, int, wis and con.
			// str, dex and con only increases for dsoma characters
			int iSTRInc = GetInc(INC_STR);
			int iDEXInc = GetInc(INC_DEX);
			int iINTInc = GetInc(INC_INT);
			int iWISInc = GetInc(INC_WIS);
			int iCONInc = GetInc(INC_CON);
			double dSTRIncRate = 0;
			double dDEXIncRate = 0;
			double dINTIncRate = 1;
			double dWISIncRate = 1;
			double dCONIncRate = 0;

			// Dsoma characters can gain str, dex and con
			// when using magic in here we set the rates
			if (CheckDemon(m_sClass))
			{
				dSTRIncRate = 0.2;
				if (pMagic->m_tType02 == 1)
				{
					dDEXIncRate = 0.4;
					dINTIncRate = 1.3;
					dWISIncRate = 0.8;
					dCONIncRate = 0.3;
				}
				else
				{
					dDEXIncRate = 0.3;
					dINTIncRate = 0.8;
					dWISIncRate = 1.3;
					dCONIncRate = 0.4;
				}
			}

			iSTRInc = static_cast<int>(iSTRInc * dSTRIncRate);
			iDEXInc = static_cast<int>(iDEXInc * dDEXIncRate);
			iINTInc = static_cast<int>(iINTInc * dINTIncRate);
			iWISInc = static_cast<int>(iWISInc * dWISIncRate);
			iCONInc = static_cast<int>(iCONInc * dCONIncRate);

			for (int i = 0; i < iPlusValueCount; i++)
			{
				if (!CheckDemon(m_sClass))
				{
					PlusStr(iSTRInc);
					PlusDex(iDEXInc);
					PlusInt(iINTInc);
					PlusWis(iWISInc);
					PlusCon(iCONInc);
				}

				m_iSTRInc += iSTRInc;
				m_iDEXInc += iDEXInc;
				m_iINTInc += iINTInc;
				m_iWISInc += iWISInc;
				m_iCONInc += iCONInc;
			}

			SetMagicExp(pMagic);

			// Increase Staff Weapon Skill Exp
			if (!m_InvItem[ARM_RHAND].IsEmpty() &&
				m_InvItem[ARM_RHAND].bType == TYPE_STAFF)
			{
				int iStaffInc = GetInc(INC_STAFF);
				PlusWeaponExp(6, iStaffInc);
			}
		}
	}

	SendMagicCircleAttack(SUCCESS, static_cast<BYTE>(pMagic->m_sRange), iTargetCount, iTargetsId, sMagicNo, sTargetsHP, sTargetsMaxHP, pMagic->m_iTime);
}

void USER::SetBeLive(TCHAR* pBuf)
{
	if (m_bLive)
	{
		TRACE("User %s is alive so cannot perform this action.\n", m_strUserId);
		return;
	}

	int index = 0;
	BYTE byLiveType = GetByte(pBuf, index);

	if (m_iDeathType & DEATH_TYPE_GUARD)
	{
		m_iDeathType ^= DEATH_TYPE_GUARD;
		SetBeLiveByDetecter();
		return;
	}

	if (m_iDeathType & DEATH_TYPE_HSOMA_GV)
	{
		SetBeLiveInPKZone();
		m_iDeathType ^= DEATH_TYPE_HSOMA_GV;
		return;
	}

	if (m_iDeathType & DEATH_TYPE_DSOMA_GV)
	{
		SetBeLiveInPKZone();
		m_iDeathType ^= DEATH_TYPE_DSOMA_GV;
		return;
	}

	if (m_iDeathType & DEATH_TYPE_WOTW)
	{
		SetBeLiveInPKZone();
		m_iDeathType ^= DEATH_TYPE_WOTW;
		return;
	}

	if (m_iDeathType & DEATH_TYPE_CLASS)
	{
		SetBeLiveInPKZone();
		m_iDeathType ^= DEATH_TYPE_CLASS;
		return;
	}

	DWORD dwCurExp = m_dwExp;
	DWORD dwCurTime = GetTickCount();

	if (byLiveType == LIVE_TYPE_REVIVE)
	{
		// Check if the player is able to revive
		if (!CheckRevive())
		{
			byLiveType = LIVE_TYPE_WARP;
			SendServerChatMessage(IDS_USER_REVIVE_LOW_SKILL, TO_ME);
		}

		// If the class war is not on and player is on a WotW and the map
		// does not belong to their class then make them warp.
		if (!g_bClassWar &&
			IsMapRange(MAP_AREA_WOTW_MAP) &&
			(CheckDemon(g_iClassWar) != CheckDemon(m_sClass)))
		{
			byLiveType = LIVE_TYPE_WARP;
		}
	}
	else if (byLiveType == LIVE_TYPE_WARP)
	{
		if (m_bInParty)
		{
			// Dissolve or Withdraw the player from party
			if (m_PartyMembers[0].uid == m_Uid &&
				strcmp(m_PartyMembers[0].m_strUserId, m_strUserId) == 0)
			{
				PartyDissolve(this);
			}
			else
			{
				PartyWithdraw();
			}
		}
	}

	switch (byLiveType)
	{
	case LIVE_TYPE_REVIVE:
		{
			int iReviveItem = -1;

			if (m_bInGuildWar)
			{
				return;
			}

			if (m_iDeathType & DEATH_TYPE_DUEL)
			{
				m_iDeathType ^= DEATH_TYPE_DUEL;
				SetBeLiveInDuel();
				return;
			}
			else if (m_iDeathType & DEATH_TYPE_GUILDWAR)
			{
				m_iDeathType ^= DEATH_TYPE_GUILDWAR;
			}
			else
			{
				iReviveItem = GetInsurancePolicy();
				if (iReviveItem == -1)
				{
					DWORD dwRDExp  = 0;
					int iRDSTR = 0;
					int iRDDEX = 0;
					int iRDINT = 0;
					int iRDWIS = 0;
					int iRDCON = 0;
					int iRDCHA = 0;

					dwRDExp = GetInc(INC_LEVEL_RDEXP);
					iRDSTR = static_cast<int>(GetInc(INC_RDSTR) * (dwRDExp / 1000.0));
					iRDDEX = static_cast<int>(GetInc(INC_RDDEX) * (dwRDExp / 1000.0));
					iRDINT = static_cast<int>(GetInc(INC_RDINT) * (dwRDExp / 1000.0));
					iRDWIS = static_cast<int>(GetInc(INC_RDWIS) * (dwRDExp / 1000.0));
					iRDCON = static_cast<int>(GetInc(INC_RDCON));

					if (!CheckDemon(m_sClass))
					{
						iRDSTR = static_cast<int>((iRDSTR * CompareOtherValue(INC_STR)) / 100.0);
						iRDDEX = static_cast<int>((iRDDEX * CompareOtherValue(INC_DEX)) / 100.0);
						iRDINT = static_cast<int>((iRDINT * CompareOtherValue(INC_INT)) / 100.0);
						iRDWIS = static_cast<int>((iRDWIS * CompareOtherValue(INC_WIS)) / 100.0);
						iRDCON = static_cast<int>((iRDCON * CompareOtherValue(INC_CON)) / 100.0);
					}
					else
					{
						iRDCHA = GetInc(INC_RDCHA);
					}

					if (iRDSTR < 0) iRDSTR = 0;
					if (iRDDEX < 0) iRDDEX = 0;
					if (iRDINT < 0) iRDINT = 0;
					if (iRDWIS < 0) iRDWIS = 0;
					if (iRDCON < 0) iRDCON = 0;
					if (iRDCHA < 0) iRDCHA = 0;

					if (dwRDExp > m_dwExp)
					{
						m_dwExp = 0;
					}
					else
					{
						m_dwExp -= dwRDExp;
					}

					if (!CheckDemon(m_sClass))
					{
						if (m_iSTRInc < iRDSTR) m_iSTRInc = 0;
						else m_iSTRInc -= iRDSTR;

						if (m_iDEXInc < iRDDEX) m_iDEXInc = 0;
						else m_iDEXInc -= iRDDEX;

						if (m_iINTInc < iRDINT) m_iINTInc = 0;
						else m_iINTInc -= iRDINT;

						if (m_iWISInc < iRDWIS) m_iWISInc = 0;
						else m_iWISInc -= iRDWIS;

						if (m_iCONInc < iRDCON) m_iCONInc = 0;
						else m_iCONInc -= iRDCON;
					}

					PlusStr(-iRDSTR);
					PlusDex(-iRDDEX);
					PlusInt(-iRDINT);
					PlusWis(-iRDWIS);
					PlusCon(-iRDCON);
					PlusCha(-iRDCHA);

					if (!m_InvItem[ARM_RHAND].IsEmpty())
					{
						int iRDWeaponExp = 0;
						int iWeaponType = 0;
						switch (m_InvItem[ARM_RHAND].bType)
						{
						case TYPE_SWORD:
							iRDWeaponExp = GetInc(INC_RDSWORD);
							iWeaponType = 1;
							break;
						case TYPE_AXE:
							iRDWeaponExp = GetInc(INC_RDAXE);
							iWeaponType = 3;
							break;
						case TYPE_BOW:
							iRDWeaponExp = GetInc(INC_RDBOW);
							iWeaponType = 4;
							break;
						case TYPE_SPEAR:
							iRDWeaponExp = GetInc(INC_RDSPEAR);
							iWeaponType = 2;
							break;
						case TYPE_KNUCKLE:
							iRDWeaponExp = GetInc(INC_RDKNUCKLE);
							iWeaponType = 5;
							break;
						case TYPE_STAFF:
							iRDWeaponExp = GetInc(INC_RDSTAFF);
							iWeaponType = 6;
							break;
						}

						PlusWeaponExp(iWeaponType, -iRDWeaponExp);
					}

					int iRDMagicExp = 0;
					int iMagicClass = 0;
					switch (m_sMageType / 100)
					{
					case MAGIC_CLASS_BLUE:
						iRDMagicExp = GetInc(INC_RDBLUE_MAGIC);
						iMagicClass = MAGIC_CLASS_BLUE;
						break;
					case MAGIC_CLASS_WHITE:
						iRDMagicExp = GetInc(INC_RDWHITE_MAGIC);
						iMagicClass = MAGIC_CLASS_WHITE;
						break;
					case MAGIC_CLASS_BLACK:
						iRDMagicExp = GetInc(INC_RDBLACK_MAGIC);
						iMagicClass = MAGIC_CLASS_BLACK;
						break;
					}
					PlusMagicExp(iMagicClass, -iRDMagicExp);
				}
			}

			m_sHP = GetMaxHP() / 2;
			m_sStm = GetMaxStm();

			m_bLive = true;
			m_bLiveGray = false;
			InterlockedExchange(&m_lDeadUsed, (LONG)0);

			// Revival code if got a revival item GetInsurancePolicy(SPECIAL_REVIVE);
			if (iReviveItem != -1)
			{
				m_sHP = GetMaxHP();
				m_sMP = GetMaxMP();
				m_sStm = GetMaxStm();

				if (!m_InvItem[iReviveItem].IsEmpty())
				{
					PlusItemDur(&m_InvItem[iReviveItem], 1, true);

					int index = 0;
					SetByte(m_TempBuf, PKT_RESURRECTION_SCROLL, index);
					SetInt(m_TempBuf, m_Uid + USER_BAND, index);
					SetShort(m_TempBuf, iReviveItem, index);
					SetShort(m_TempBuf, m_InvItem[iReviveItem].sUsage, index);
					SetShort(m_TempBuf, 904, index); // Magic Number
					SetShort(m_TempBuf, 0, index);
					SendInsight(m_TempBuf, index);

					CString strRevive;
					strRevive.Format(IDS_USER_REVIVE_BY_ITEM, m_InvItem[iReviveItem].strNewName);
					SendServerChatMessage((LPTSTR)(LPCTSTR)strRevive, TO_ME);

					// Empty the item if there is no more left
					// need to do this after the above because it is being used.
					if (m_InvItem[iReviveItem].sUsage <= 0)
					{
						m_InvItem[iReviveItem].Init();
					}
				}
			}

			CPoint pt = RandNearPoint_S(5);
			if (pt.x == -1 || pt.y == -1)
			{
				pt.x = m_sX;
				pt.y = m_sY;
			}

			pt = ConvertToClient(pt.x, pt.y);
			if (pt.x != -1 && pt.y != -1)
			{
				SendCharData(INFO_ALL);
				LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
			}
		}
		break;
	case LIVE_TYPE_WARP:
		{
			if ((m_iDeathType & DEATH_TYPE_DUEL) == 0 &&
				(m_iDeathType & DEATH_TYPE_GUILDWAR) == 0)
			{
				DeadThrowItem(true, true);
				DeadThrowMoney(true);

				DWORD dwWDExp  = 0;
				int iWDSTR = 0;
				int iWDDEX = 0;
				int iWDINT = 0;
				int iWDWIS = 0;
				int iWDCON = 0;
				int iWDCHA = 0;

				dwWDExp = GetInc(INC_LEVEL_WDEXP);
				iWDSTR = static_cast<int>(GetInc(INC_WDSTR) * (dwWDExp / 1000.0));
				iWDDEX = static_cast<int>(GetInc(INC_WDDEX) * (dwWDExp / 1000.0));
				iWDINT = static_cast<int>(GetInc(INC_WDINT) * (dwWDExp / 1000.0));
				iWDWIS = static_cast<int>(GetInc(INC_WDWIS) * (dwWDExp / 1000.0));
				iWDCON = static_cast<int>(GetInc(INC_WDCON));

				if (!CheckDemon(m_sClass))
				{
					iWDSTR = static_cast<int>((iWDSTR * CompareOtherValue(INC_STR)) / 100.0);
					iWDDEX = static_cast<int>((iWDDEX * CompareOtherValue(INC_DEX)) / 100.0);
					iWDINT = static_cast<int>((iWDINT * CompareOtherValue(INC_INT)) / 100.0);
					iWDWIS = static_cast<int>((iWDWIS * CompareOtherValue(INC_WIS)) / 100.0);
					iWDCON = static_cast<int>((iWDCON * CompareOtherValue(INC_CON)) / 100.0);
				}
				else
				{
					iWDCHA = GetInc(INC_WDCHA);
				}

				if (iWDSTR < 0) iWDSTR = 0;
				if (iWDDEX < 0) iWDDEX = 0;
				if (iWDINT < 0) iWDINT = 0;
				if (iWDWIS < 0) iWDWIS = 0;
				if (iWDCON < 0) iWDCON = 0;
				if (iWDCHA < 0) iWDCHA = 0;

				if (dwWDExp > m_dwExp)
				{
					m_dwExp = 0;
				}
				else
				{
					m_dwExp -= dwWDExp;
				}

				if (!CheckDemon(m_sClass))
				{
					if (m_iSTRInc < iWDSTR) m_iSTRInc = 0;
					else m_iSTRInc -= iWDSTR;

					if (m_iDEXInc < iWDDEX) m_iDEXInc = 0;
					else m_iDEXInc -= iWDDEX;

					if (m_iINTInc < iWDINT) m_iINTInc = 0;
					else m_iINTInc -= iWDINT;

					if (m_iWISInc < iWDWIS) m_iWISInc = 0;
					else m_iWISInc -= iWDWIS;

					if (m_iCONInc < iWDCON) m_iCONInc = 0;
					else m_iCONInc -= iWDCON;
				}

				PlusStr(-iWDSTR);
				PlusDex(-iWDDEX);
				PlusInt(-iWDINT);
				PlusWis(-iWDWIS);
				PlusCon(-iWDCON);
				PlusCha(-iWDCHA);

				if (!m_InvItem[ARM_RHAND].IsEmpty())
				{
					int iWDWeaponExp = 0;
					int iWeaponType = 0;
					switch (m_InvItem[ARM_RHAND].bType)
					{
					case TYPE_SWORD:
						iWDWeaponExp = GetInc(INC_WDSWORD);
						iWeaponType = 1;
						break;
					case TYPE_AXE:
						iWDWeaponExp = GetInc(INC_WDAXE);
						iWeaponType = 3;
						break;
					case TYPE_BOW:
						iWDWeaponExp = GetInc(INC_WDBOW);
						iWeaponType = 4;
						break;
					case TYPE_SPEAR:
						iWDWeaponExp = GetInc(INC_WDSPEAR);
						iWeaponType = 2;
						break;
					case TYPE_KNUCKLE:
						iWDWeaponExp = GetInc(INC_WDKNUCKLE);
						iWeaponType = 5;
						break;
					case TYPE_STAFF:
						iWDWeaponExp = GetInc(INC_WDSTAFF);
						iWeaponType = 6;
						break;
					}

					PlusWeaponExp(iWeaponType, -iWDWeaponExp);
				}

				int iWDMagicExp = 0;
				int iMagicClass = 0;
				switch (m_sMageType / 100)
				{
				case MAGIC_CLASS_BLUE:
					iWDMagicExp = GetInc(INC_WDBLUE_MAGIC);
					iMagicClass = MAGIC_CLASS_BLUE;
					break;
				case MAGIC_CLASS_WHITE:
					iWDMagicExp = GetInc(INC_WDWHITE_MAGIC);
					iMagicClass = MAGIC_CLASS_WHITE;
					break;
				case MAGIC_CLASS_BLACK:
					iWDMagicExp = GetInc(INC_WDBLACK_MAGIC);
					iMagicClass = MAGIC_CLASS_BLACK;
					break;
				}
				PlusMagicExp(iMagicClass, -iWDMagicExp);
			}
			else if (m_iDeathType & DEATH_TYPE_DUEL)
			{
				m_iDeathType ^= DEATH_TYPE_DUEL;
			}
			else if (m_iDeathType & DEATH_TYPE_GUILDWAR)
			{
				m_iDeathType ^= DEATH_TYPE_GUILDWAR;
			}

			m_sHP = GetMaxHP();
			m_sStm = GetMaxStm() / 2;

			m_bLive = true;
			m_bLiveGray = false;
			InterlockedExchange(&m_lDeadUsed, (LONG)0);

			SendCharData(INFO_ALL);

			bool bInTown = true;
			int iMoral = m_iMoral / CLIENT_MORAL;
			if (!CheckDemon(m_sClass))
			{
				if (iMoral <= -31)
				{
					bInTown = false;
				}
			}
			else
			{
				if (iMoral >= 31)
				{
					bInTown = false;
				}
			}

			CPoint pt;
			if (CheckDemon(m_sClass) != CheckDemon(g_mapZoneClass[m_sZ]))
			{
				if (bInTown)
				{
					if (!CheckDemon(m_sClass))
					{
						pt = FindRandPointInRect_C(1, 277, 93, 300, 120);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 291;
							pt.y = 113;
						}
						LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					else
					{
						pt = FindRandPointInRect_C(9, 393, 903, 433, 903);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 393;
							pt.y = 903;
						}
						LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
				}
				else
				{
					if (!CheckDemon(m_sClass))
					{
						pt = FindRandPointInRect_C(1, 32, 126, 40, 126);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 32;
							pt.y = 126;
						}
						LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					else
					{
						pt = FindRandPointInRect_C(9, 307, 959, 333, 959);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 307;
							pt.y = 959;
						}
						LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
				}
			}
			else
			{
				switch (m_sZ)
				{
				case 1:
					if (bInTown)
					{
						pt = FindRandPointInRect_C(1, 277, 93, 300, 120);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 291;
							pt.y = 113;
						}
						LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					else
					{
						pt = FindRandPointInRect_C(1, 32, 126, 40, 126);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 32;
							pt.y = 126;
						}
						LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					break;
				case 4:
					if (bInTown)
					{
						pt = FindRandPointInRect_C(4, 204, 432, 226, 432);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 204;
							pt.y = 432;
						}
						LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					else
					{
						pt = FindRandPointInRect_C(4, 149, 495, 160, 496);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 149;
							pt.y = 495;
						}
						LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					break;
				case 5:
				case 12:
					if (bInTown)
					{
						pt = FindRandPointInRect_C(5, 24, 884, 41, 889);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 24;
							pt.y = 884;
						}
						LinkToOtherZone(5, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					else
					{
						pt = FindRandPointInRect_C(5, 369, 303, 384, 304);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 369;
							pt.y = 303;
						}
						LinkToOtherZone(5, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					break;
				case 9:
				case 10:
				case 11:
				case 13:
					if (bInTown)
					{
						pt = FindRandPointInRect_C(9, 393, 903, 433, 903);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 393;
							pt.y = 903;
						}
						LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					else
					{
						pt = FindRandPointInRect_C(9, 307, 959, 333, 959);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 307;
							pt.y = 959;
						}
						LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					break;
				case 14:
				case 15:
				case 16:
				case 17:
					if (bInTown)
					{
						pt = FindRandPointInRect_C(14, 28, 52, 42, 52);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 28;
							pt.y = 52;
						}
						LinkToOtherZone(14, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					else
					{
						pt = FindRandPointInRect_C(14, 4, 232, 12, 232);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 4;
							pt.y = 232;
						}
						LinkToOtherZone(14, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					break;
				default:
					if (bInTown)
					{
						if (!CheckDemon(m_sClass))
						{
							pt = FindRandPointInRect_C(1, 277, 93, 300, 120);
							if (pt.x == -1 || pt.y == -1)
							{
								pt.x = 291;
								pt.y = 113;
							}
							LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
						}
						else
						{
							pt = FindRandPointInRect_C(9, 393, 903, 433, 903);
							if (pt.x == -1 || pt.y == -1)
							{
								pt.x = 393;
								pt.y = 903;
							}
							LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
						}
					}
					else
					{
						if (!CheckDemon(m_sClass))
						{
							pt = FindRandPointInRect_C(1, 32, 126, 40, 126);
							if (pt.x == -1 || pt.y == -1)
							{
								pt.x = 32;
								pt.y = 126;
							}
							LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
						}
						else
						{
							pt = FindRandPointInRect_C(9, 307, 959, 333, 959);
							if (pt.x == -1 || pt.y == -1)
							{
								pt.x = 307;
								pt.y = 959;
							}
							LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
						}
					}
					break;
				}
			}
		}
		break;
	default:
		break;
	}

	SendSetLive();

	DWORD dwTick = GetTickCount();
	m_dwLastRecoverHPTime = dwTick;
	m_dwLastRecoverMPTime = dwTick;
}

void USER::PartyDeny(TCHAR* pBuf)
{
	if (m_bInParty)
	{
		if (m_Uid == m_PartyMembers[0].uid &&
			strcmp(m_strUserId, m_PartyMembers[0].m_strUserId) == 0)
		{
			PartyDissolve(this);
		}
		else
		{
			PartyWithdraw();
		}
	}

	if (m_bPartyDeny)
	{
		SendServerChatMessage(IDS_USER_ACCEPTING_PARTY, TO_ME);
		m_bPartyDeny = false;
	}
	else
	{
		SendServerChatMessage(IDS_USER_BLOCKING_PARTY, TO_ME);
		m_bPartyDeny = true;
	}
}

void USER::PartyInviteResult(TCHAR* pBuf)
{
	int index = 0;
	BYTE byType = GetByte(pBuf, index);

	USER* pLeader = GetUser(m_iPartyInviteUid);
	if (!pLeader)
	{
		// Party Invite failed cannot find the user
		m_bPartyInvite = false;
		m_iPartyInviteUid = -1;

		index = 0;
		SetByte(m_TempBuf, PKT_MESSAGE, index);
		SetShort(m_TempBuf, 24, index);
		Send(m_TempBuf, index);
		return;
	}

	if (byType == 0) // Refuse party invitation
	{
		m_bPartyInvite = false;
		m_iPartyInviteUid = -1;
		pLeader->m_bPartyInvite = false;
		pLeader->m_iPartyInviteUid = -1;

		index = 0;
		SetByte(m_TempBuf, PKT_MESSAGE, index);
		SetShort(m_TempBuf, 24, index);
		Send(m_TempBuf, index);
		pLeader->Send(m_TempBuf, index);
		return;
	}

	// Accepted party invitation
	m_bPartyInvite = false;
	m_iPartyInviteUid = -1;
	pLeader->m_bPartyInvite = false;
	pLeader->m_iPartyInviteUid = -1;

	if (!pLeader->m_bInParty) // Create the party
	{
		m_bInParty = true;
		pLeader->m_bInParty = true;

		strcpy(pLeader->m_PartyMembers[0].m_strUserId, pLeader->m_strUserId);
		pLeader->m_PartyMembers[0].uid = pLeader->m_Uid;
		strcpy(pLeader->m_PartyMembers[1].m_strUserId, m_strUserId);
		pLeader->m_PartyMembers[1].uid = m_Uid;

		strcpy(m_PartyMembers[0].m_strUserId, pLeader->m_strUserId);
		m_PartyMembers[0].uid = pLeader->m_Uid;

		int index = 0;
		SetByte(m_TempBuf, PKT_PARTY_INVITE_RESULT, index);
		SetByte(m_TempBuf, INVITE_SUCCESS, index);
		SetVarString(m_TempBuf, m_PartyMembers[0].m_strUserId, strlen(m_PartyMembers[0].m_strUserId), index);
		pLeader->Send(m_TempBuf, index);
		Send(m_TempBuf, index);

		index = 0;
		FillUserInfo(m_TempBuf, index, INFO_MODIFY);
		if (m_sZ == pLeader->m_sZ) // Don't need to our user info to leader if not on same zone
		{
			pLeader->Send(m_TempBuf, index);
		}
		Send(m_TempBuf, index);

		index = 0;
		pLeader->FillUserInfo(m_TempBuf, index, INFO_MODIFY);
		if (m_sZ == pLeader->m_sZ) // Don't need to leader user info to us if not on same zone
		{
			Send(m_TempBuf, index);
		}
		pLeader->Send(m_TempBuf, index);

		SendServerChatMessage(IDS_USER_PARTY_CREATED, TO_ME);
		pLeader->SendServerChatMessage(IDS_USER_PARTY_CREATED, TO_ME);
	}
	else // Party already exists
	{
		m_bInParty = true;

		for (int i = 0; i < MAX_PARTY_USER_NUM; i++)
		{
			if (pLeader->m_PartyMembers[i].uid == -1)
			{
				pLeader->m_PartyMembers[i].uid = m_Uid;
				strcpy(pLeader->m_PartyMembers[i].m_strUserId, m_strUserId);
				break;
			}
		}

		strcpy(m_PartyMembers[0].m_strUserId, pLeader->m_strUserId);
		m_PartyMembers[0].uid = pLeader->m_Uid;

		int index = 0;
		SetByte(m_TempBuf, PKT_PARTY_INVITE_RESULT, index);
		SetByte(m_TempBuf, INVITE_SUCCESS, index);
		SetVarString(m_TempBuf, m_PartyMembers[0].m_strUserId, strlen(m_PartyMembers[0].m_strUserId), index);
		Send(m_TempBuf, index);
		pLeader->Send(m_TempBuf, index);

		TCHAR m_MemberInfoBuf[MAX_PACKET_SIZE];
		int member_index = 0;
		FillUserInfo(m_MemberInfoBuf, member_index, INFO_MODIFY);

		CString strJoin = "";
		strJoin.Format(IDS_USER_PARTY_JOIN, m_strUserId);

		// Send message to all players within the party
		// informing them that someone has joined.
		for (int i = 0; i < MAX_PARTY_USER_NUM; i++)
		{
			if (pLeader->m_PartyMembers[i].uid != -1)
			{
				USER *pMember = g_pUserList->GetUserUid(pLeader->m_PartyMembers[i].uid);
				if (!pMember) continue;
				if (pMember->m_State != STATE_GAMESTARTED) continue;
				// Below check is done incase the uid has been reused for another user
				// and for some reason user is still in party list.
				if (strcmp(pMember->m_strUserId, pLeader->m_PartyMembers[i].m_strUserId) != 0) continue;

				pMember->SendServerChatMessage((LPTSTR)(LPCTSTR)strJoin, TO_ME);

				// Sends the new member user info
				pMember->Send(m_MemberInfoBuf, member_index);

				// Send a current party member user info to the new member
				// Don't bother sending if not on same zone
				if (m_sZ == pMember->m_sZ)
				{
					index = 0;
					pMember->FillUserInfo(m_TempBuf, index, INFO_MODIFY);
					Send(m_TempBuf, index);
				}
			}
		}

		// Create a message listing all the players currently in the party
		CString strInParty = "";
		for (int i = 0; i < MAX_PARTY_USER_NUM; i++)
		{
			if (pLeader->m_PartyMembers[i].uid != -1)
			{
				if (i == 0)
				{
					strInParty.AppendFormat("%s", pLeader->m_PartyMembers[i].m_strUserId);
				}
				else
				{
					strInParty.AppendFormat(", %s", pLeader->m_PartyMembers[i].m_strUserId);
				}
			}
		}
		CString strMessage = "";
		strMessage.Format(IDS_USER_CURRENTLY_PARTY_WITH, strInParty);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
	}
}

void USER::SetMagicBelt(TCHAR* pBuf)
{
	int index = 0;
	for (int i = 0; i < MAGIC_BELT_SIZE; i++)
	{
		BYTE byMagicType = GetByte(pBuf, index);
		short sMagicNo = GetShort(pBuf, index);

		if (byMagicType == MAGIC_TYPE_SPECIAL)
		{
			// Validate the magic being used
			if (sMagicNo <= 0 || sMagicNo > g_arSpecialAttackTable.GetSize())
			{
				m_UserMagicBelt[i].sMid = -1;
				continue;
			}

			CSpecialTable *pSpecial = g_arSpecialAttackTable.GetAt(sMagicNo-1);
			if (!pSpecial)
			{
				m_UserMagicBelt[i].sMid = -1;
				continue;
			}
		}
		else if (byMagicType == MAGIC_TYPE_ABILITY)
		{
			// Validate the magic being used
			if (sMagicNo <= 0 || sMagicNo > g_arMakeSkillTable.GetSize())
			{
				m_UserMagicBelt[i].sMid = -1;
				continue;
			}

			CMakeSkillTable *pSpecial = g_arMakeSkillTable.GetAt(sMagicNo-1);
			if (!pSpecial)
			{
				m_UserMagicBelt[i].sMid = -1;
				continue;
			}
		}
		else
		{
			// Validate the magic being used
			if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
			{
				m_UserMagicBelt[i].sMid = -1;
				continue;
			}

			CMagicTable *pMagic = g_arMagicTable.GetAt(sMagicNo-1);
			if (!pMagic)
			{
				m_UserMagicBelt[i].sMid = -1;
				continue;
			}

			if (pMagic->m_tClass != MAGIC_CLASS_BLUE && m_sMageType / 100 != pMagic->m_tClass)
			{
				m_UserMagicBelt[i].sMid = -1;
				continue;
			}
		}

		// FIX for European soma client
		// because it says magic as 3 different values
		// 1 for blue, 2 for white and 3 for black
		// so 2 and 3 need to be changed to a 1 for (MAGIC_TYPE_MAGIC) otherwise
		// the magic will not be found and it will get removed from the magic belt.
		if (byMagicType == 2 || byMagicType == 3)
		{
			byMagicType = MAGIC_TYPE_MAGIC;
		}

		if (!IsHaveMagic(sMagicNo, byMagicType))
		{
			m_UserMagicBelt[i].sMid = -1;
			continue;
		}

		m_UserMagicBelt[i].sMid = sMagicNo;
		m_UserMagicBelt[i].byType = byMagicType;
	}
}

void USER::ChangeOtherItem(TCHAR* pBuf)
{
	int index = 0;
	BYTE bySuccess = FAIL;
	short sItemIndex = -1;
	short sItemCount = 0;
	CChangeOtherItemTable* pOtherItem = NULL;
	ItemList giveItem;
	short sWgt = 0;
	short sSlot = -1;

	if (m_sChangeOtherItemSlot < 0 || m_sChangeOtherItemSlot >= INV_ITEM_NUM) goto send_fail_;
	if (m_InvItem[m_sChangeOtherItemSlot].IsEmpty()) goto send_fail_;
	if (m_InvItem[m_sChangeOtherItemSlot].sSpecial != SPECIAL_SPEC_GEM) goto send_fail_;
	if (m_InvItem[m_sChangeOtherItemSlot].sUsage < 1) goto send_fail_;

	bySuccess = GetByte(pBuf, index);
	if (bySuccess != SUCCESS)  goto send_fail_;

	sItemIndex = GetShort(pBuf, index);
	if (sItemIndex < 0 || sItemIndex > g_arChangeOtherItemTable.GetSize()) goto send_fail_;

	sItemCount = GetShort(pBuf, index);
	if (sItemCount < 1 || sItemCount > m_InvItem[m_sChangeOtherItemSlot].sUsage) goto send_fail_;

	pOtherItem = g_arChangeOtherItemTable[sItemIndex];
	if (!pOtherItem) goto send_fail_;

	if (pOtherItem->m_sSpecialNum != m_sChangeOtherItemSpecialNum) goto send_fail_;

	if (!giveItem.InitFromItemTable(pOtherItem->m_sItemNum)) goto send_fail_;

	if (giveItem.bType > TYPE_ACC)
	{
		giveItem.sUsage = sItemCount;
	}

	sWgt = m_sWgt + (sItemCount * giveItem.sWgt);
	if (sWgt > GetMaxWgt())
	{
		SendServerChatMessage(IDS_CHANGEOTHERITEM_WEIGHT_FULL, TO_ME);
		goto send_fail_;
	}

	sSlot = PushItemInventory(&giveItem);
	if (sSlot == -1)
	{
		SendServerChatMessage(IDS_INV_FULL, TO_ME);
		goto send_fail_;
	}

	SendItemInfoChange(BASIC_INV, sSlot, INFO_ALL);

	PlusItemDur(&m_InvItem[m_sChangeOtherItemSlot], sItemCount, true);
	SendItemInfoChange(BASIC_INV, m_sChangeOtherItemSlot, INFO_DUR);
	if (m_InvItem[m_sChangeOtherItemSlot].sUsage <= 0)
	{
		m_InvItem[m_sChangeOtherItemSlot].Init();
		SendDeleteItem(BASIC_INV, m_sChangeOtherItemSlot);
	}

	index = 0;
	SetByte(m_TempBuf, PKT_CHANGE_OTHER_ITEM, index);
	SetByte(m_TempBuf, 1, index);
	SetByte(m_TempBuf, SUCCESS, index);
	SetVarString(m_TempBuf, (LPTSTR)(LPCTSTR)pOtherItem->m_strItemName, pOtherItem->m_strItemName.GetLength(),index);
	Send(m_TempBuf, index);

	SendChangeWgt();

	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_CHANGE_OTHER_ITEM, index);
	SetByte(m_TempBuf, 1, index);
	SetByte(m_TempBuf, FAIL, index);
	Send(m_TempBuf, index);
}

void USER::TraderExchange(TCHAR* pBuf)
{
	int index = 0;
	BYTE byType = GetByte(pBuf, index);
	if (byType != 2) return; // 2 = Buy Item and is the only 1 that client sends and
							 // only 1 that server uses so why is it even being sent?

	short sItemIndex = -1;
	short sItemCount = 0;
	CTraderExchangeTable* pTraderExchange = NULL;
	DWORD dwCost = 0;
	CItemTable* pItemTable = NULL;
	ItemList giveItem;
	short sSlot = -1;
	int iEmptySlotCount = 0;
	bool bFoundAllItems = true;
	CString str = "";

	sItemIndex = GetShort(pBuf, index);
	if (sItemIndex < 1 || sItemIndex > g_arTraderExchangeTable.GetSize()) goto send_fail_;

	sItemCount = GetShort(pBuf, index);
	if (sItemCount < 1 || sItemCount > MAX_ITEM_DURA) goto send_fail_;

	pTraderExchange = g_arTraderExchangeTable[sItemIndex-1]; // Taking away 1 because its zero based.
	if (pTraderExchange == NULL)  goto send_fail_;

	if (pTraderExchange->m_sMoney > 0)
	{
		dwCost = static_cast<DWORD>(pTraderExchange->m_sMoney * sItemCount);
		if (m_dwBarr < dwCost)
		{
			SendServerChatMessage(IDS_BUY_NOT_ENOUGH_BARR, TO_ME);
			goto send_fail_;
		}
	}

	if (!g_mapItemTable.Lookup(pTraderExchange->m_sItem, pItemTable))
		goto send_fail_;

	// Need to check if the player has the needed amount of inventory space to perform the exchange
	for (int i = EQUIP_ITEM_NUM; i < INV_ITEM_NUM; i++)
	{
		if (m_InvItem[i].IsEmpty())
		{
			iEmptySlotCount++;
			if (pItemTable->m_bType > TYPE_ACC) // Non equipment (Stackable)
			{
				break;
			}
		}
	}
	if (pItemTable->m_bType <= TYPE_ACC &&
		iEmptySlotCount < sItemCount)
	{
		SendServerChatMessage(IDS_TRADER_EXCHANGE_NO_INV_SPACE, TO_ME);
		goto send_fail_;
	}

	// Go through the required items and check that user has got them.
	for (int i = 0; i < 5; i++)
	{
		if (pTraderExchange->m_sNeedItem[i] == -1)
		{
			continue;
		}

		// Search for the required item in the user inventory
		int iFoundItemCount = 0;
		int iRequiredItemCount = sItemCount * pTraderExchange->m_sNeedItemNum[i];
		for (int j = EQUIP_ITEM_NUM; j < INV_ITEM_NUM; j++)
		{
			if (!m_InvItem[j].IsEmpty())
			{
				if (m_InvItem[j].sNum == pTraderExchange->m_sNeedItem[i])
				{
					if (m_InvItem[j].bType > TYPE_ACC) // Non Equipment (Stackable)
					{
						iFoundItemCount += m_InvItem[j].sUsage;
						if (iFoundItemCount >= iRequiredItemCount)
						{
							break;
						}
					}
					else
					{
						iFoundItemCount++;
						if (iFoundItemCount >= iRequiredItemCount)
						{
							break;
						}
					}
				}
			}
		}
		if (iFoundItemCount < iRequiredItemCount)
		{
			bFoundAllItems = false;
			break;
		}
	}
	if (!bFoundAllItems)
	{
		SendServerChatMessage(IDS_TRADER_EXCHANGE_NOT_ENOUGH_ITEM, TO_ME);
		goto send_fail_;
	}

	// Now need to check user has got enough weight
	short sWgt = m_sWgt + (sItemCount * pItemTable->m_sWgt);
	if (sWgt > GetMaxWgt())
	{
		SendServerChatMessage(IDS_TRADER_EXCHANGE_WEIGHT_FULL, TO_ME);
		goto send_fail_;
	}

	// Give user the item
	if (!giveItem.InitFromItemTable(pTraderExchange->m_sItem)) goto send_fail_;
	if (giveItem.bType > TYPE_ACC)
	{
		giveItem.sUsage = sItemCount;
	}

	sSlot = PushItemInventory(&giveItem);
	if (sSlot == -1)
	{
		SendServerChatMessage(IDS_TRADER_EXCHANGE_NO_INV_SPACE, TO_ME);
		goto send_fail_;
	}
	SendItemInfoChange(BASIC_INV, sSlot, INFO_ALL);

	// Inform player that they have receieved an item
	if (giveItem.bType > TYPE_ACC)
	{
		str.Format(IDS_TRADER_EXCHANGE_RECEIEVE_ITEMS, sItemCount, giveItem.strNewName);
	}
	else
	{
		str.Format(IDS_TRADER_EXCHANGE_RECEIEVE_ITEM, giveItem.strNewName);
	}
	SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);

	// Update user money
	if (dwCost > 0)
	{
		m_dwBarr -= dwCost;
		SendMoneyChanged();
	}

	// Now delete all the items needed to exchange for the item
	for (int i = 0; i < 5; i++)
	{
		if (pTraderExchange->m_sNeedItem[i] == -1)
		{
			continue;
		}

		// Search for the item and then delete the amount that was needed
		int iDeletedItemCount = 0;
		int iDeleteItemCount = sItemCount * pTraderExchange->m_sNeedItemNum[i];
		for (int j = EQUIP_ITEM_NUM; j < INV_ITEM_NUM; j++)
		{
			if (!m_InvItem[j].IsEmpty())
			{
				if (m_InvItem[j].sNum == pTraderExchange->m_sNeedItem[i])
				{
					if (m_InvItem[j].bType > TYPE_ACC) // Non Equipment (Stackable)
					{
						int iCount = iDeleteItemCount;
						if (m_InvItem[j].sUsage < iDeleteItemCount)
						{
							iCount = m_InvItem[j].sUsage;
							iDeleteItemCount -= m_InvItem[j].sUsage;
						}
						PlusItemDur(&m_InvItem[j], iCount, true);
						SendItemInfoChange(BASIC_INV, j, INFO_DUR);
						if (m_InvItem[j].sUsage <= 0)
						{
							m_InvItem[j].Init();
							SendDeleteItem(BASIC_INV, j);
						}

						iDeletedItemCount += iCount;
						if (iDeletedItemCount >= iDeleteItemCount)
						{
							break;
						}
					}
					else
					{
						m_InvItem[j].Init();
						SendDeleteItem(BASIC_INV, j);
						iDeletedItemCount++;
						if (iDeletedItemCount >= iDeleteItemCount)
						{
							break;
						}
					}
				}
			}
		}
	}

	SendChangeWgt();

	index = 0;
	SetByte(m_TempBuf, PKT_TRADEREXCHANGE, index);
	SetByte(m_TempBuf, 2, index);
	SetByte(m_TempBuf, SUCCESS, index);
	Send(m_TempBuf, index);
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_TRADEREXCHANGE, index);
	SetByte(m_TempBuf, 2, index);
	SetByte(m_TempBuf, FAIL, index);
	Send(m_TempBuf, index);
}

void USER::GuildList(TCHAR* pBuf)
{
	// FIXME: GuildList this is kind of a bad way of sending a list
	// of guilds that player is able to join
	// because there could be loads it would create a realy big packet
	// could do with either replacing the whole guild joining system
	// via using commands and user interaction instead of an npc
	// or make it send pages for the guilds
	// e.g. max of 20 guilds per page or something.
	int index = 0;
	short sGuildCount = 0;
	GetGuild(-1);
	for (int i = 0; i < g_arGuild.GetSize(); i++)
	{
		Guild* pGuild = g_arGuild[i];
		if (pGuild != NULL)
		{
			if (pGuild->m_sNum >= 0)
			{
				if (CheckDemon(m_sClass) == CheckDemon(pGuild->m_sClass))
				{
					sGuildCount++;
					SetVarString(m_TempBuf, pGuild->m_strGuildName, strlen(pGuild->m_strGuildName), index);
				}
			}
		}
	}
	ReleaseGuild();

	CBufferEx TempBuf;
	TempBuf.Add(PKT_GUILD_LIST);
	TempBuf.Add(sGuildCount);
	TempBuf.AddData(m_TempBuf, index);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::GuildMemberList(TCHAR* pBuf)
{
	int index = 0;
	CBufferEx GuildMembers;
	CBufferEx GuildManagement;
	short sMemberCount = 0;
	BYTE byRank = Guild::GUILD_MEMBER;
	int iUserIndex = -1;
	int iUserRank = Guild::GUILD_MEMBER;

	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	iUserRank = pGuild->m_arMembers[iUserIndex].m_sGuildRank;

	// Guild management names and position names
	for (int i = Guild::GUILD_CHIEF; i <= Guild::GUILD_RANKS; i++)
	{
		int iIndex = pGuild->GetUserByRank(i);
		if (iIndex != -1)
		{
			GuildManagement.AddString(pGuild->m_arMembers[iIndex].m_strUserId);
			GuildManagement.AddString(pGuild->m_strCallName[i-1]);
		}
		else
		{
			GuildManagement.Add((BYTE)0);
			GuildManagement.AddString(pGuild->m_strCallName[i-1]);
		}
	}

	// Guild member names excludes the management
	for (int i = 0; i < MAX_GUILD_MEMBERS; i++)
	{
		if (pGuild->m_arMembers[i].m_lUsed > 0 &&
			pGuild->m_arMembers[i].m_sGuildRank == Guild::GUILD_MEMBER)
		{
			GuildMembers.AddString(pGuild->m_arMembers[i].m_strUserId);
			sMemberCount++;
		}
	}

	SetByte(m_TempBuf, PKT_GUILD_MEMBER_LIST, index);
	if (iUserRank == Guild::GUILD_CHIEF)
	{
		byRank = 1;
	}
	else if (iUserRank >= Guild::GUILD_FIRST && iUserRank < Guild::GUILD_SECOND)
	{
		byRank = 2;
	}
	else if (iUserRank >= Guild::GUILD_SECOND && iUserRank <= Guild::GUILD_RANKS)
	{
		byRank = 3;
	}
	else
	{
		byRank = Guild::GUILD_MEMBER;
	}

	SetByte(m_TempBuf, byRank, index);
	SetShort(m_TempBuf, sMemberCount, index);
	SetVarString(m_TempBuf, pGuild->m_strGuildName, strlen(pGuild->m_strGuildName), index);
	SetShort(m_TempBuf, pGuild->m_TaxRate, index);
	SetDWORD(m_TempBuf, pGuild->m_dwBarr, index);
	SetString(m_TempBuf, GuildManagement, GuildManagement.GetLength(), index);
	SetString(m_TempBuf, GuildMembers, GuildMembers.GetLength(), index);
	ReleaseGuild();
	Send(m_TempBuf, index);
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_MEMBER_LIST, index);
	SetByte(m_TempBuf, Guild::GUILD_MEMBER, index);
	SetShort(m_TempBuf, -1, index);
	Send(m_TempBuf, index);
}

void USER::GuildNew(TCHAR* pBuf)
{
	int index = 0;
	BYTE bySubResult = Guild::GUILD_SYSTEM_ERROR;
	int iGuildNum = -1;
	TCHAR strGuildName[GUILD_NAME_LENGTH+1] = {0};
	TCHAR strGuildInfo[Guild::GUILD_INFO_SIZE+1] = {0};
	int iLen = 0;
	CString str = "";

	if (m_bInGuildWar) return; // Why should it check this?

	// Must check if the user creating a guild isn't already
	// in a guild.
	if (m_sGroup != -1)
	{
		bySubResult = Guild::GUILD_ALREADY_JOIN;
		goto send_fail_;
	}

	// User must have the requirements for creating a new guild
	if (m_dwBarr < GUILD_MAKE_BARR)
	{
		bySubResult = Guild::GUILD_SMALL_MONEY;
		goto send_fail_;
	}

	// TODO: for human characters we check if they have a crafting skill of 50 or total of 90 and
	// therefore they don't need to be level 25 to make guild
	int iLevelReq = CheckDemon(m_sClass) ? GUILD_MAKE_LEVEL_D : GUILD_MAKE_LEVEL_H;
	if (m_sLevel < iLevelReq)
	{
		bySubResult = Guild::GUILD_SMALL_LEVEL;
		goto send_fail_;
	}

	// Dsoma characters have extra requirement of charisma to make a guild.
	if (CheckDemon(m_sClass))
	{
		int iCurCha = m_iCHA / CLIENT_EXT_STATS;
		if (iCurCha < GUILD_MAKE_CHARISMA)
		{
			bySubResult = Guild::GUILD_SMALL_LEVEL;
			goto send_fail_;
		}
	}

	// Get guild name and info from client packet
	iLen = GetVarString(sizeof(strGuildName), strGuildName, pBuf, index);
	if (iLen <= 0 || iLen > GUILD_NAME_LENGTH)
	{
		bySubResult = Guild::GUILD_INVALID_GUILD_NAME;
		goto send_fail_;
	}

	iLen = GetVarString(sizeof(strGuildInfo), strGuildInfo, pBuf, index);
	if (iLen <= 0 || iLen > Guild::GUILD_INFO_SIZE)
	{
		bySubResult = Guild::GUILD_INVALID_GUILD_INFO;
		goto send_fail_;
	}

	// Find an empty place to put the new guild
	// it can return -1 which means no more guilds can be created.
	iGuildNum = GetEmptyGuildNum();
	if (iGuildNum == -1)
	{
		bySubResult = Guild::GUILD_FULL;
		ReleaseGuild();
		goto send_fail_;
	}

	Guild* pGuild = g_arGuild[iGuildNum];
	ASSERT(pGuild);
	pGuild->Initialize(); // Make sure everything is at their default values!

	// Now start filling in the details for the new guild
	pGuild->m_sNum = iGuildNum;
	strcpy(pGuild->m_strGuildName, strGuildName);
	strcpy(pGuild->m_strInfo, strGuildInfo);
	pGuild->m_sClass = CheckDemon(m_sClass) ? DEVIL : HUMAN;
	if (!pGuild->AddMember(m_strUserId, Guild::GUILD_CHIEF))
	{
		bySubResult = Guild::GUILD_SYSTEM_ERROR;
		pGuild->Initialize();
		ReleaseGuild();
		goto send_fail_;
	}
	strcpy(pGuild->m_strCallName[0], "Leader");
	if (!InsertGuild(pGuild, bySubResult))
	{
		pGuild->Initialize();
		ReleaseGuild();
		goto send_fail_;
	}

	m_sGroup = iGuildNum;
	m_sGroupReq = -1;
	m_sGuildRank = Guild::GUILD_CHIEF;
	strcpy(m_strGuildName, pGuild->m_strGuildName);
	m_sGuildSymbolVersion = 0;
	m_dwBarr -= GUILD_MAKE_BARR;
	SendMoneyChanged();

	ReleaseGuild();

	str.Format(IDS_MAKE_GUILD, strGuildName);
	SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ALL); // FIXME make TO_ALL only send to hsoma or dsoma

	SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	SendCharData(INFO_NAMES); // Normal 1p1 sends INFO_NAMES | INFO_BASICVALUE but money has already been updated so no need.

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_NEW, index);
	SetByte(m_TempBuf, SUCCESS, index);
	Send(m_TempBuf, index);
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_NEW, index);
	SetByte(m_TempBuf, FAIL, index);
	SetByte(m_TempBuf, bySubResult, index);
	Send(m_TempBuf, index);
}

void USER::GuildInfo(TCHAR* pBuf)
{
	int index = 0;
	TCHAR strGuildName[GUILD_NAME_LENGTH+1] = {0};
	int iLen = 0;

	// Get guild name and info from client packet
	iLen = GetVarString(sizeof(strGuildName), strGuildName, pBuf, index);
	if (iLen <= 0 || iLen > GUILD_NAME_LENGTH) return;

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_INFO, index);
	Guild* pGuild = GetGuildByName(strGuildName);
	if (pGuild != NULL)
	{
		SetVarString(m_TempBuf, pGuild->m_strInfo, strlen(pGuild->m_strInfo), index);
	}
	else
	{
		SetByte(m_TempBuf, 0, index);
	}
	ReleaseGuild();
	Send(m_TempBuf, index);
}

void USER::GuildChangeCallName(TCHAR* pBuf)
{
	int index = 0;
	int iLen = 0;
	TCHAR strPos[Guild::GUILD_CALL_LENGTH+1] = {0};
	CBufferEx TempBuf;
	Guild *pGuild = NULL;
	BYTE bySubResult = 0;
	int iPos = 0;
	int iUserIndex = -1;

	if (m_sGroup < 0)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		goto send_result_;
	}

	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		m_sGroup = -1;
		ReleaseGuild();
		goto send_result_;
	}

	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		ReleaseGuild();
		goto send_result_;
	}

	if (pGuild->m_arMembers[iUserIndex].m_sGuildRank != Guild::GUILD_CHIEF)
	{
        bySubResult = Guild::GUILD_SMALL_RANK;
		ReleaseGuild();
		goto send_result_;
	}

	iPos = GetByte(pBuf, index);
	if (iPos < 1 || iPos > Guild::GUILD_RANKS)
	{
		bySubResult = Guild::GUILD_INVALID_RANK;
		ReleaseGuild();
		goto send_result_;
	}

	iLen = GetVarString(sizeof(strPos), strPos, pBuf, index);
	if (iLen < 0 || iLen > Guild::GUILD_CALL_LENGTH)
	{
		bySubResult = Guild::GUILD_INVALID_GUILD_CALL_NAME;
		ReleaseGuild();
		goto send_result_;
	}

	if (ChangeGuildCallName(iPos, pGuild->m_strGuildName, strPos, bySubResult))
	{
		pGuild->SetCallName(iPos, strPos);

		ReleaseGuild();

		index = 0;
		SetByte(m_TempBuf, PKT_GUILD_CHANGE_POS, index);
		SetByte(m_TempBuf, SUCCESS, index);
		Send(m_TempBuf, index);
		return;
	}

	ReleaseGuild();

send_result_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_CHANGE_POS, index);
	SetByte(m_TempBuf, FAIL, index);
	SetByte(m_TempBuf, bySubResult, index);
	Send(m_TempBuf, index);
}

void USER::GuildEditInfoReq(TCHAR* pBuf)
{
	// Cannot do this if in a guild war.
	if (m_bInGuildWar) return;

	// Must be in a guild
	if (m_sGroup < 0 || m_sGroup >= MAX_GUILD) return;

	// Fetch the guild details
	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		return;
	}

	// Must be the leader of the guild to do this
	int iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex >= 0 &&
		pGuild->m_arMembers[iUserIndex].m_sGuildRank != Guild::GUILD_CHIEF)
	{
		ReleaseGuild();
		return;
	}

	// Send the requested guild information
	int index = 0;
	SetByte(m_TempBuf, PKT_GUILD_EDIT_INFO_REQ, index);
	SetVarString(m_TempBuf, pGuild->m_strInfo, strlen(pGuild->m_strInfo), index);
	ReleaseGuild();
	Send(m_TempBuf, index);
}

void USER::GuildEditInfo(TCHAR* pBuf)
{
	// Cannot do this if in a guild war.
	if (m_bInGuildWar) return;

	// Must be in a guild
	if (m_sGroup < 0 || m_sGroup >= MAX_GUILD) return;

	// Fetch the guild details
	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		return;
	}

	// Must be the leader of the guild to do this
	int iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex >= 0 &&
		pGuild->m_arMembers[iUserIndex].m_sGuildRank != Guild::GUILD_CHIEF)
	{
		ReleaseGuild();
        return;
	}

	// Retrieve the new guild information
	int index = 0;
	TCHAR strGuildInfo[Guild::GUILD_INFO_SIZE+1] = {0};
	int iLen = GetVarString(sizeof(strGuildInfo), strGuildInfo, pBuf, index);
	if (iLen <= 0 || iLen > Guild::GUILD_INFO_SIZE)
	{
		ReleaseGuild();
		return;
	}

	// Set the new guild information
	BYTE byResult = FAIL;
	if (EditGuildInfo(pGuild->m_sNum, strGuildInfo))
	{
		strcpy(pGuild->m_strInfo, strGuildInfo);
		byResult = SUCCESS;
	}

	ReleaseGuild();

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_EDIT_INFO, index);
	SetByte(m_TempBuf, byResult, index);
	Send(m_TempBuf, index);
}

void USER::GuildReqUserList(TCHAR* pBuf)
{
	int sCount = 0;
	int index = 0;
	Guild* pGuild = NULL;
	int iUserIndex = -1;

	// Fetch the guild details
	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	// Must be guild leader or Guild::GUILD_FIRST of the guild to do this
	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	if (pGuild->m_arMembers[iUserIndex].m_sGuildRank < Guild::GUILD_CHIEF ||
		pGuild->m_arMembers[iUserIndex].m_sGuildRank >= Guild::GUILD_SECOND)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	// Build list of applicants
	SetByte(m_TempBuf, PKT_GUILD_APPLICATION_LIST, index);
	SetShort(m_TempBuf, pGuild->m_arApplicants.GetSize(), index);
	for (int i = 0; i < pGuild->m_arApplicants.GetSize(); i++)
	{
		SetVarString(m_TempBuf, (LPTSTR)(LPCTSTR)pGuild->m_arApplicants[i], pGuild->m_arApplicants[i].GetLength(), index);
	}
	ReleaseGuild();
	Send(m_TempBuf, index);
	return;

send_fail_:
	SetByte(m_TempBuf, PKT_GUILD_APPLICATION_LIST, index);
	SetShort(m_TempBuf, sCount, index);
	Send(m_TempBuf, index);
}

void USER::GuildReqUserInfo(TCHAR* pBuf)
{
	int index = 0;
	Guild* pGuild = NULL;
	int iUserIndex = -1;
	BYTE bySubResult = 0;
	TCHAR strApplicantName[NAME_LENGTH+1] = {0};
	int iLen = 0;
	CString strUserInfo = "";
	const CString strMajorWeapon[7] = {"None", "Sword", "Axe", "Bow", "Spear", "Knuckle", "Staff"};
	const CString strMajorSkill[6] = {"None", "Weapon", "Armor", "Access", "Potion", "Cooking"};

	// Fetch the guild details
	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		m_sGroup = -1;
		ReleaseGuild();
		goto send_fail_;
	}

	// Must be guild leader or Guild::GUILD_FIRST of the guild to do this
	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	if (pGuild->m_arMembers[iUserIndex].m_sGuildRank < Guild::GUILD_CHIEF ||
		pGuild->m_arMembers[iUserIndex].m_sGuildRank >= Guild::GUILD_SECOND)
	{
		bySubResult = Guild::GUILD_SMALL_RANK;
		ReleaseGuild();
		goto send_fail_;
	}

	// Retrieve the applicant's name
	iLen = GetVarString(sizeof(strApplicantName), strApplicantName, pBuf, index);
	if (iLen <= 0 || iLen > NAME_LENGTH)
	{
		bySubResult = Guild::GUILD_ABSENT_REQ_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	// Is the applicant in the application list for the guild?
	if (pGuild->GetApplicationUser(strApplicantName) < 0)
	{
		bySubResult = Guild::GUILD_ABSENT_REQ_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	// Get details of the applicant
	GuildApplicantUserInfo UserInfo;
	if (!GetGuildReqUserInfo(pGuild->m_strGuildName, strApplicantName, UserInfo, bySubResult))
	{
		ReleaseGuild();
		goto send_fail_;
	}

	ReleaseGuild();

	// Highest weapon skill name
	int iWeaponExp = 0;
	BYTE byWeaponType = 0;
	for (int i = 0; i < 6; i++)
	{
		if (UserInfo.iWeaponExp[i] > iWeaponExp)
		{
			byWeaponType = i + 1;
			iWeaponExp = UserInfo.iWeaponExp[i];
		}
	}

	// Highest crafting skill name
	int iMakeExp = 0;
	BYTE byMakeType = 0;
	for (int i = 0; i < 5; i++)
	{
		if (UserInfo.iMakeExp[i] > iMakeExp)
		{
			byMakeType = i + 1;
			iMakeExp = UserInfo.iMakeExp[i];
		}
	}

	strUserInfo.AppendFormat("Level:%d/Moral:%s/Major Weapon:%s/Major Skill:%s",
		UserInfo.sLevel, GetMoralName(UserInfo.iMoral / CLIENT_MORAL),
		strMajorWeapon[byWeaponType], strMajorSkill[byMakeType]);

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_REQ_USER_INFO, index);
	SetVarString(m_TempBuf, (LPTSTR)(LPCTSTR)strUserInfo, strUserInfo.GetLength(), index);
	Send(m_TempBuf, index);
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_REQ_USER_INFO, index);
	SetByte(m_TempBuf, 0, index);
	SetByte(m_TempBuf, bySubResult, index);
	Send(m_TempBuf, index);
}

void USER::GuildReq(TCHAR* pBuf)
{
	int index = 0;
	BYTE bySubResult = 0;
	TCHAR strGuildName[GUILD_NAME_LENGTH+1] = {0};
	int iLen = 0;

	// Cannot do this if in a guild war.
	if (m_bInGuildWar) return;

	// Must check if the user isn't already in a guild.
	if (m_sGroup != -1)
	{
		bySubResult = Guild::GUILD_ALREADY_JOIN;
		goto send_fail_;
	}

	// Must check for existing application request
	if (m_sGroupReq != -1)
	{
		bySubResult = Guild::GUILD_ALREADY_JOIN_REQ;
		goto send_fail_;
	}

	// Get guild name from client packet
	iLen = GetVarString(sizeof(strGuildName), strGuildName, pBuf, index);
	if (iLen <= 0 || iLen > GUILD_NAME_LENGTH)
	{
		bySubResult = Guild::GUILD_INVALID_GUILD_NAME;
		goto send_fail_;
	}

	// Fetch the guild details
	Guild* pGuild = GetGuildByName(strGuildName);
	if (pGuild == NULL)
	{
		bySubResult = Guild::GUILD_ABSENT_GUILD_NAME;
		ReleaseGuild();
		goto send_fail_;
	}

	// Make sure the guild isn't full and application list isn't full
	if (pGuild->GetMemberCount() >= MAX_GUILD_MEMBERS ||
		pGuild->m_arApplicants.GetSize() >= MAX_GUILD_APPLICATION_USER)
	{
		bySubResult = Guild::GUILD_ABSENT_GUILD_NAME; // FIXME Should be a guild user full
		ReleaseGuild();
		goto send_fail_;
	}

	// Add the user as a guild applicant
	if (!InsertGuildReq(m_strUserId, strGuildName, bySubResult))
	{
		ReleaseGuild();
		goto send_fail_;
	}
	pGuild->AddApplicant(m_strUserId);

	m_sGroup = -1;
	m_sGroupReq = pGuild->m_sNum;

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_REQ, index);
	SetByte(m_TempBuf, SUCCESS, index);
	Send(m_TempBuf, index);

	// Inform guild management that a user has applied to join
	for (int i = 1; i < Guild::GUILD_SECOND; i++)
	{
		int iIndex = pGuild->GetUserByRank(i);
		if (iIndex != -1)
		{
			USER* pUser = GetUserId(pGuild->m_arMembers[iIndex].m_strUserId);
			if (pUser != NULL)
			{
				if (pUser->m_State == STATE_GAMESTARTED)
				{
					CString str;
					str.Format(IDS_GUILD_USER_APPLY, m_strUserId);
					pUser->SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
				}
			}
		}
	}

	ReleaseGuild();
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_REQ, index);
	SetByte(m_TempBuf, FAIL, index);
	SetByte(m_TempBuf, bySubResult, index);
	Send(m_TempBuf, index);
}

void USER::GuildMoveReq(TCHAR* pBuf)
{
	int index = 0;
	BYTE bySubResult = 0;
	TCHAR strApplicantName[NAME_LENGTH+1] = {0};
	Guild* pGuild = NULL;
	int iLen = 0;
	int iUserIndex = -1;
	CString str = "";

	// Cannot do this if in a guild war.
	if (m_bInGuildWar) return;

	// Must be in a guild
	if (m_sGroup < 0 || m_sGroup >= MAX_GUILD)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		goto send_fail_;
	}

	// Retrieve the applicant's name
	iLen = GetVarString(sizeof(strApplicantName), strApplicantName, pBuf, index);
	if (iLen <= 0 || iLen > NAME_LENGTH)
	{
		bySubResult = Guild::GUILD_ABSENT_REQ_USER;
		goto send_fail_;
	}

	// Fetch the guild details
	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		m_sGroup = -1;
		ReleaseGuild();
		goto send_fail_;
	}

	// Must be guild leader or Guild::GUILD_FIRST of the guild to do this
	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	if (pGuild->m_arMembers[iUserIndex].m_sGuildRank < Guild::GUILD_CHIEF ||
		pGuild->m_arMembers[iUserIndex].m_sGuildRank >= Guild::GUILD_SECOND)
	{
		bySubResult = Guild::GUILD_SMALL_RANK;
		ReleaseGuild();
		goto send_fail_;
	}

	// Make sure the guild isn't full and application list isn't full
	if (pGuild->GetMemberCount() >= MAX_GUILD_MEMBERS)
	{
		bySubResult = Guild::GUILD_ABSENT_GUILD_NAME; // TODO: Should be a guild user full
		ReleaseGuild();
		goto send_fail_;
	}

	// Is the applicant in the application list for the guild?
	if (pGuild->GetApplicationUser(strApplicantName) < 0)
	{
		bySubResult = Guild::GUILD_ABSENT_REQ_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	// Add the applicant to the guild
	if (!MoveGuildReq(strApplicantName, pGuild->m_strGuildName, bySubResult))
	{
		ReleaseGuild();
		goto send_fail_;
	}

	// Get new guild user
	USER* pUser = GetUserId(strApplicantName);
	if (pUser != NULL && pUser->m_Uid != m_Uid)
	{
		pUser->m_sGroup = pGuild->m_sNum;
		pUser->m_sGroupReq = -1;
		pUser->m_sGuildRank = Guild::GUILD_MEMBER;
		pUser->m_sGuildSymbolVersion = pGuild->m_sSymbolVersion;
		strcpy(pUser->m_strGuildName, pGuild->m_strGuildName);
		pUser->SendMyInfo(TO_INSIGHT, INFO_MODIFY);
		pUser->SendCharData(INFO_NAMES | INFO_BASICVALUE);
		str.Format(IDS_GUILD_APPLICATION_ACCEPTED, pGuild->m_strGuildName);
		pUser->SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}

	pGuild->RemoveApplicant(strApplicantName);
	pGuild->AddMember(strApplicantName, Guild::GUILD_MEMBER);

	// Inform the guild
	str.Format(IDS_GUILD_USER_JOIN, strApplicantName);
	GuildChat((LPTSTR)(LPCTSTR)str, pGuild->m_sNum);

	ReleaseGuild();

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_MOVE_REQ, index);
	SetByte(m_TempBuf, SUCCESS, index);
	Send(m_TempBuf, index);
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_MOVE_REQ, index);
	SetByte(m_TempBuf, FAIL, index);
	SetByte(m_TempBuf, bySubResult, index);
	Send(m_TempBuf, index);
}

void USER::GuildRemoveReq(TCHAR* pBuf)
{
	// This is the 'Cancel Apply' option on guild master npcs.
	int index = 0;
	BYTE bySubResult = 0;

	// Cannot do this if in a guild war.
	if (m_bInGuildWar) return;

	// Must check if the user isn't already in a guild.
	if (m_sGroup != -1)
	{
		bySubResult = Guild::GUILD_ALREADY_JOIN;
		goto send_fail_;
	}

	// Must check for non existing application request
	if (m_sGroupReq == -1)
	{
		bySubResult = Guild::GUILD_ABSENT_JOIN_REQ;
		goto send_fail_;
	}

	// Fetch the details of guild user has applied to
	Guild* pGuild = GetGuild(m_sGroupReq);
	if (pGuild == NULL)
	{
		bySubResult = Guild::GUILD_ABSENT_JOIN_REQ;
		ReleaseGuild();
		goto send_fail_;
	}

	// Is the applicant in the application list for the guild?
	if (pGuild->GetApplicationUser(m_strUserId) < 0)
	{
		bySubResult = Guild::GUILD_ABSENT_JOIN_REQ;
		ReleaseGuild();
		goto send_fail_;
	}

	// Removes the user from guild application
	if (!RemoveGuildReq(m_strUserId, pGuild->m_strGuildName, bySubResult))
	{
		ReleaseGuild();
		goto send_fail_;
	}

	// TODO: UpdateUserData
	m_sGroup = -1;
	m_sGroupReq = -1;

	pGuild->RemoveApplicant(m_strUserId);

	ReleaseGuild();

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_REMOVE_REQ, index);
	SetByte(m_TempBuf, SUCCESS, index);
	Send(m_TempBuf, index);
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_REMOVE_REQ, index);
	SetByte(m_TempBuf, FAIL, index);
	SetByte(m_TempBuf, bySubResult, index);
	Send(m_TempBuf, index);
}

void USER::GuildRemoveReqBoss(TCHAR* pBuf)
{
	// This is the 'Reject' application action taken by guild mananagement.
	int index = 0;
	BYTE bySubResult = 0;
	TCHAR strApplicantName[NAME_LENGTH+1] = {0};
	int iLen = 0;
	Guild* pGuild = NULL;
	int iUserIndex = -1;
	CString str = "";

	// Cannot do this if in a guild war.
	if (m_bInGuildWar) return;

	// Must be in a guild
	if (m_sGroup < 0 || m_sGroup >= MAX_GUILD)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		goto send_fail_;
	}

	// Retrieve the applicant's name
	iLen = GetVarString(sizeof(strApplicantName), strApplicantName, pBuf, index);
	if (iLen <= 0 || iLen > NAME_LENGTH)
	{
		bySubResult = Guild::GUILD_ABSENT_REQ_USER;
		goto send_fail_;
	}

	// Fetch the guild details
	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		m_sGroup = -1;
		ReleaseGuild();
		goto send_fail_;
	}

	// Must be guild leader or Guild::GUILD_FIRST of the guild to do this
	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	if (pGuild->m_arMembers[iUserIndex].m_sGuildRank < Guild::GUILD_CHIEF ||
		pGuild->m_arMembers[iUserIndex].m_sGuildRank >= Guild::GUILD_SECOND)
	{
		bySubResult = Guild::GUILD_SMALL_RANK;
		ReleaseGuild();
		goto send_fail_;
	}

	// Is the applicant in the application list for the guild?
	if (pGuild->GetApplicationUser(strApplicantName) < 0)
	{
		bySubResult = Guild::GUILD_ABSENT_JOIN_REQ;
		ReleaseGuild();
		goto send_fail_;
	}

	// Removes the user from guild application
	if (!RemoveGuildReq(strApplicantName, pGuild->m_strGuildName, bySubResult))
	{
		ReleaseGuild();
		goto send_fail_;
	}

	// Get the application user
	USER* pUser = GetUserId(strApplicantName);
	if (pUser != NULL && pUser->m_Uid != m_Uid)
	{
		pUser->m_sGroup = -1;
		pUser->m_sGroupReq = -1;
		str.Format(IDS_GUILD_APPLICATION_REJECTED, pGuild->m_strGuildName);
		pUser->SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}
	pGuild->RemoveApplicant(strApplicantName);

	ReleaseGuild();

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_MOVE_REJECT, index);
	SetByte(m_TempBuf, SUCCESS, index);
	Send(m_TempBuf, index);
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_MOVE_REJECT, index);
	SetByte(m_TempBuf, FAIL, index);
	SetByte(m_TempBuf, bySubResult, index);
	Send(m_TempBuf, index);
}

void USER::GuildChangeRank(TCHAR* pBuf)
{
	int index = 0;
	BYTE bySubResult = 0;
	TCHAR strMemberName[NAME_LENGTH+1] = {0};
	int iLen = 0;
	Guild* pGuild = NULL;
	int iUserIndex;
	CString str = "";
	int from = -1;
	int to = -1;

	// Cannot do this if in a guild war.
	if (m_bInGuildWar) return;

	// Must be in a guild
	if (m_sGroup < 0 || m_sGroup >= MAX_GUILD)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		goto send_fail_;
	}

	// Fetch the guild details
	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		m_sGroup = -1;
		ReleaseGuild();
		goto send_fail_;
	}

	// Must be guild leader of the guild to do this
	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	if (pGuild->m_arMembers[iUserIndex].m_sGuildRank != Guild::GUILD_CHIEF)
	{
		bySubResult = Guild::GUILD_SMALL_RANK;
		ReleaseGuild();
		goto send_fail_;
	}

	// Retrieve the member's name
	iLen = GetVarString(sizeof(strMemberName), strMemberName, pBuf, index);
	if (iLen <= 0 || iLen > NAME_LENGTH)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	// Fetch guild member
	iUserIndex = pGuild->GetUser(strMemberName);
	if (iUserIndex < 0)
	{
		bySubResult = Guild::GUILD_ABSENT_JOIN;
		ReleaseGuild();
		goto send_fail_;
	}

	// Change the rank
	from = GetByte(pBuf, index);
	to = GetByte(pBuf, index);

	// No point moving to the same rank
	if (from == to)
	{
		bySubResult = Guild::GUILD_INVALID_RANK;
		ReleaseGuild();
		goto send_fail_;
	}

	// Not allowed to remove the leader
	// can only replace the leader.
	if (from == Guild::GUILD_CHIEF)
	{
		bySubResult = Guild::GUILD_SMALL_RANK;
		ReleaseGuild();
		goto send_fail_;
	}

	// Need to do some extra checking for the to be new leader
	// to make sure they have the requirements needed.
	if (to == Guild::GUILD_CHIEF)
	{
		// TODO: for human characters we check if they have a crafting skill of 50 or total of 90 and
		// therefore they don't need to be level 25 to make guild
		int iLevelReq = CheckDemon(m_sClass) ? GUILD_MAKE_LEVEL_D : GUILD_MAKE_LEVEL_H;
		if (m_sLevel < iLevelReq)
		{
			bySubResult = Guild::GUILD_SMALL_LEVEL; // 15h 21
			ReleaseGuild();
			goto send_fail_;
		}

		// Dsoma characters have extra requirement of charisma to make a guild.
		if (CheckDemon(m_sClass))
		{
			int iCurCha = m_iCHA / CLIENT_EXT_STATS;
			if (iCurCha < GUILD_MAKE_CHARISMA)
			{
				bySubResult = Guild::GUILD_SMALL_LEVEL;
				ReleaseGuild();
				goto send_fail_;
			}
		}
	}

	// Need to make sure there is space to change the rank
	// GUILD_CHIEF and GUILD_MEMBER ranks are excluded from this
	// because they will have a space always (GUILD_CHIEF can be replaced).
	if (to != Guild::GUILD_CHIEF && to != Guild::GUILD_MEMBER)
	{
		if (!pGuild->IsRankEmpty(to))
		{
			bySubResult = Guild::GUILD_NEED_EMPTY_RANK;
			ReleaseGuild();
			goto send_fail_;
		}
	}

	// Change the rank of the guild member in database
	if (!ChangeGuildRank(strMemberName, pGuild->m_strGuildName, from, to, bySubResult))
	{
		ReleaseGuild();
		goto send_fail_;
	}

	// Handle extra requirements of guild leader changing
	if (to == Guild::GUILD_CHIEF)
	{
		// Set the current leader to a guild member
		int i = pGuild->GetUserByRank(Guild::GUILD_CHIEF);
		pGuild->m_arMembers[i].m_sGuildRank = Guild::GUILD_MEMBER;
		USER* pUser = GetUserId(pGuild->m_arMembers[i].m_strUserId);
		if (pUser != NULL)
		{
			pUser->m_sGuildRank = Guild::GUILD_MEMBER;
		}

		// Inform the guild
		str.Format(IDS_GUILD_LEADER_CHANGE, m_strUserId, strMemberName);
		GuildChat((LPTSTR)(LPCTSTR)str, pGuild->m_sNum);
	}

	// Change rank
	pGuild->m_arMembers[iUserIndex].m_sGuildRank = to;
	USER* pUser = GetUserId(strMemberName);
	if (pUser != NULL)
	{
		pUser->m_sGuildRank = to;
	}

	ReleaseGuild();

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_CHANGE_RANK, index);
	SetByte(m_TempBuf, SUCCESS, index);
	Send(m_TempBuf, index);
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_CHANGE_RANK, index);
	SetByte(m_TempBuf, FAIL, index);
	SetByte(m_TempBuf, bySubResult, index);
	Send(m_TempBuf, index);
}

void USER::GuildContribution(TCHAR* pBuf)
{
	// TODO: There is a known bug that if the server crashes and user data is not saved then guild money is duped
	// e.g. player puts all their barr into guild crashes comes back on has got their barr back
	// and its in the guild aswell

	int index = 0;
	BYTE byType = GetByte(pBuf, index);
	DWORD dwMoney = GetDWORD(pBuf, index);
	Guild* pGuild = NULL;

	if (dwMoney > GUILD_MAX_DONATION)
		goto send_fail_;

	// Cannot do this if in a guild war.
	// This check isn't in the original 1p1 but put it here anyway.
	if (m_bInGuildWar)
		goto send_fail_;

	if (m_bTrading)
		goto send_fail_;

	// Must be in a guild
	if (m_sGroup < 0 || m_sGroup >= MAX_GUILD)
		goto send_fail_;

	// Fetch the guild details
	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		m_sGroup = -1;
		ReleaseGuild();
		goto send_fail_;
	}

	// Give
	if (byType == 1)
	{
		DWORD dwMyMoney = m_dwBarr;
		DWORD dwGuildBarr = pGuild->m_dwBarr;

		if (dwMyMoney < dwMoney)
		{
			ReleaseGuild();
			goto send_fail_;
		}

		dwMyMoney -= dwMoney;

		if (!CheckMaxValueReturn(dwGuildBarr, dwMoney))
		{
			ReleaseGuild();
			goto send_fail_;
		}
		CheckMaxValue(dwGuildBarr, dwMoney);

		if (!UpdateGuildMoney(pGuild->m_sNum, dwGuildBarr))
		{
			ReleaseGuild();
			goto send_fail_;
		}

		m_dwBarr = dwMyMoney;
		pGuild->m_dwBarr = dwGuildBarr;
	}
	// Take
	else if (byType == 2)
	{
		DWORD dwMyMoney = m_dwBarr;
		DWORD dwGuildBarr = pGuild->m_dwBarr;

		if (dwGuildBarr < dwMoney)
		{
			ReleaseGuild();
			goto send_fail_;
		}

		int i = pGuild->GetUser(m_strUserId);
		if (i < 0 || pGuild->m_arMembers[i].m_sGuildRank != Guild::GUILD_CHIEF)
		{
			ReleaseGuild();
			goto send_fail_;
		}

		dwGuildBarr -= dwMoney;
		if (!UpdateGuildMoney(pGuild->m_sNum, dwGuildBarr))
		{
			ReleaseGuild();
			goto send_fail_;
		}

		if (!CheckMaxValueReturn(dwMyMoney, dwMoney))
		{
			ReleaseGuild();
			goto send_fail_;
		}
		CheckMaxValue(dwMyMoney, dwMoney);

		m_dwBarr = dwMyMoney;
		pGuild->m_dwBarr = dwGuildBarr;
	}
	else
	{
		ReleaseGuild();
		goto send_fail_;
	}

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_GIVE_DONATION, index);
	SetByte(m_TempBuf, byType, index);
	SetByte(m_TempBuf, SUCCESS, index);
	SetDWORD(m_TempBuf, pGuild->m_dwBarr, index);
	ReleaseGuild();
	Send(m_TempBuf, index);
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_GIVE_DONATION, index);
	SetByte(m_TempBuf, byType, index);
	SetByte(m_TempBuf, FAIL, index);
	Send(m_TempBuf, index);
}

void USER::GuildRemoveUser(TCHAR* pBuf)
{
	int index = 0;
	BYTE bySubResult = 0;
	Guild* pGuild = NULL;
	TCHAR strMemberName[NAME_LENGTH+1] = {0};
	int iLen = 0;
	int iUserIndex = -1;
	short sRank = Guild::GUILD_MEMBER;
	CString str = "";

	// Cannot do this if in a guild war.
	if (m_bInGuildWar)
		return;

	// Must be in a guild
	if (m_sGroup < 0 || m_sGroup >= MAX_GUILD)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		goto send_fail_;
	}

	// Fetch the guild details
	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		m_sGroup = -1;
		ReleaseGuild();
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		goto send_fail_;
	}

	// Check this user is in the guild
	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	// Retrieve the member's name that is going to be removed
	iLen = GetVarString(sizeof(strMemberName), strMemberName, pBuf, index);
	if (iLen <= 0 || iLen > NAME_LENGTH)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	// Remove the guild user
	sRank = pGuild->m_arMembers[iUserIndex].m_sGuildRank;

	// Leader
	if (sRank == Guild::GUILD_CHIEF)
	{
		// Leader cannot remove themselves!
		if (_stricmp(m_strUserId, strMemberName) == 0)
		{
			bySubResult = Guild::GUILD_SYSTEM_ERROR;
			ReleaseGuild();
			goto send_fail_;
		}

		// The member being removed must be in the guild
		int i = pGuild->GetUser(strMemberName);
		if (i < 0)
		{
			bySubResult = Guild::GUILD_ABSENT_JOIN;
			ReleaseGuild();
			goto send_fail_;
		}

		// Remove the user from within guild tables in database
		if (!RemoveGuildMember(strMemberName, pGuild->m_strGuildName, bySubResult))
		{
			ReleaseGuild();
			goto send_fail_;
		}

		// Remove user from the guild
		pGuild->RemoveMember(strMemberName);

		// Fetch user (if online) and change guild details.
		USER* pUser = GetUserId(strMemberName);
		if (pUser != NULL &&
			pUser->m_Uid != m_Uid)
		{
			pUser->m_sGroup = -1;
			pUser->m_sGroupReq = -1;
			pUser->m_sGuildSymbolVersion = 0;
			pUser->m_sGuildRank = 0;
			memset(pUser->m_strGuildName, 0, sizeof(pUser->m_strGuildName));
			pUser->SendMyInfo(TO_INSIGHT, INFO_MODIFY);
			pUser->SendCharData(INFO_NAMES); // Original 1p1 sends INFO_BASIC_VALUE also but its not needed
		}

		// Send success to client
		index = 0;
		SetByte(m_TempBuf, PKT_GUILD_REMOVE_USER, index);
		SetByte(m_TempBuf, SUCCESS, index);
		Send(m_TempBuf, index);

		// Inform the guild that someone got expelled.
		str.Format(IDS_GUILD_USER_EXPELLED, strMemberName);
		GuildChat((LPTSTR)(LPCTSTR)str, pGuild->m_sNum);

	}
	// Co-Leaders
	else if (sRank >= Guild::GUILD_FIRST && sRank < Guild::GUILD_SECOND)
	{
		// Handle removing themselves seperately!
		if (_stricmp(m_strUserId, strMemberName) == 0)
		{
			// Remove the user from within guild tables in database
			if (!RemoveGuildMember(strMemberName, pGuild->m_strGuildName, bySubResult))
			{
				ReleaseGuild();
				goto send_fail_;
			}

			// Remove user from the guild
			pGuild->RemoveMember(strMemberName);

			// Fetch user (if online) and change guild details.
			USER* pUser = GetUserId(strMemberName);
			if (pUser != NULL)
			{
				pUser->m_sGroup = -1;
				pUser->m_sGroupReq = -1;
				pUser->m_sGuildSymbolVersion = 0;
				pUser->m_sGuildRank = 0;
				memset(pUser->m_strGuildName, 0, sizeof(pUser->m_strGuildName));
				pUser->SendMyInfo(TO_INSIGHT, INFO_MODIFY);
				pUser->SendCharData(INFO_NAMES); // Original 1p1 sends INFO_BASIC_VALUE also but its not needed
			}

			// Send success to client
			index = 0;
			SetByte(m_TempBuf, PKT_GUILD_REMOVE_USER, index);
			SetByte(m_TempBuf, SUCCESS, index);
			Send(m_TempBuf, index);

			// Inform the guild that someone withdrawn.
			str.Format(IDS_GUILD_USER_WITHDRAW, strMemberName);
			GuildChat((LPTSTR)(LPCTSTR)str, pGuild->m_sNum);
		}
		else
		{
			// The member being removed must be in the guild
			int i = pGuild->GetUser(strMemberName);
			if (i < 0)
			{
				bySubResult = Guild::GUILD_ABSENT_JOIN;
				ReleaseGuild();
				goto send_fail_;
			}

			// Must be Guild::GUILD_SECOND rank or member
			if (pGuild->m_arMembers[i].m_sGuildRank < Guild::GUILD_SECOND)
			{
				bySubResult = Guild::GUILD_SMALL_RANK;
				ReleaseGuild();
				goto send_fail_;
			}

			// Remove the user from within guild tables in database
			if (!RemoveGuildMember(strMemberName, pGuild->m_strGuildName, bySubResult))
			{
				ReleaseGuild();
				goto send_fail_;
			}

			// Remove user from the guild
			pGuild->RemoveMember(strMemberName);

			// Fetch user (if online) and change guild details.
			USER* pUser = GetUserId(strMemberName);
			if (pUser != NULL &&
				pUser->m_Uid != m_Uid)
			{
				pUser->m_sGroup = -1;
				pUser->m_sGroupReq = -1;
				pUser->m_sGuildSymbolVersion = 0;
				pUser->m_sGuildRank = 0;
				memset(pUser->m_strGuildName, 0, sizeof(pUser->m_strGuildName));
				pUser->SendMyInfo(TO_INSIGHT, INFO_MODIFY);
				pUser->SendCharData(INFO_NAMES); // Original 1p1 sends INFO_BASIC_VALUE also but its not needed
			}

			// Send success to client
			index = 0;
			SetByte(m_TempBuf, PKT_GUILD_REMOVE_USER, index);
			SetByte(m_TempBuf, SUCCESS, index);
			Send(m_TempBuf, index);

			// Inform the guild that someone got expelled.
			str.Format(IDS_GUILD_USER_EXPELLED, strMemberName);
			GuildChat((LPTSTR)(LPCTSTR)str, pGuild->m_sNum);
		}
	}
	// Second Ranks and Members
	else
	{
		// Can only remove themselves!
		if (_stricmp(m_strUserId, strMemberName) != 0)
		{
			bySubResult = Guild::GUILD_SMALL_RANK;
			ReleaseGuild();
			goto send_fail_;
		}

		// Remove the user from within guild tables in database
		if (!RemoveGuildMember(strMemberName, pGuild->m_strGuildName, bySubResult))
		{
			ReleaseGuild();
			goto send_fail_;
		}

		// Remove user from the guild
		pGuild->RemoveMember(strMemberName);

		// Fetch user (if online) and change guild details.
		USER* pUser = GetUserId(strMemberName);
		if (pUser != NULL)
		{
			pUser->m_sGroup = -1;
			pUser->m_sGroupReq = -1;
			pUser->m_sGuildSymbolVersion = 0;
			pUser->m_sGuildRank = 0;
			memset(pUser->m_strGuildName, 0, sizeof(pUser->m_strGuildName));
			pUser->SendMyInfo(TO_INSIGHT, INFO_MODIFY);
			pUser->SendCharData(INFO_NAMES); // Original 1p1 sends INFO_BASIC_VALUE also but its not needed
		}

		// Send success to client
		index = 0;
		SetByte(m_TempBuf, PKT_GUILD_REMOVE_USER, index);
		SetByte(m_TempBuf, SUCCESS, index);
		Send(m_TempBuf, index);

		// Inform the guild that someone withdrawn.
		str.Format(IDS_GUILD_USER_WITHDRAW, strMemberName);
		GuildChat((LPTSTR)(LPCTSTR)str, pGuild->m_sNum);
	}

	ReleaseGuild();
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_REMOVE_USER, index);
	SetByte(m_TempBuf, FAIL, index);
	SetByte(m_TempBuf, bySubResult, index);
	Send(m_TempBuf, index);
}

void USER::GuildDissolve(TCHAR* pBuf) // AKA GuildRemoveUserBoss
{
	int index = 0;
	BYTE bySubResult = Guild::GUILD_SYSTEM_ERROR;
	Guild* pGuild = NULL;
	int iUserIndex = -1;

	// Cannot do this if in a guild war.
	if (m_bInGuildWar)
		return;

	// Must be in a guild
	if (m_sGroup < 0 || m_sGroup >= MAX_GUILD)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		goto send_fail_;
	}

	// Fetch the guild details
	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		m_sGroup = -1;
		ReleaseGuild();
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		goto send_fail_;
	}

	// Check this user is in the guild
	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		bySubResult = Guild::GUILD_NOT_GUILD_USER;
		ReleaseGuild();
		goto send_fail_;
	}

	// Must be the guild chief
	if (pGuild->m_arMembers[iUserIndex].m_sGuildRank != Guild::GUILD_CHIEF)
	{
		bySubResult = Guild::GUILD_SMALL_RANK;
		ReleaseGuild();
		goto send_fail_;
	}

	// Remove the guild from within guild tables in database
	if (!RemoveGuild(m_strUserId, pGuild->m_strGuildName, bySubResult))
	{
		ReleaseGuild();
		goto send_fail_;
	}

	// TODO: This could be improved!
	for (int i = 0; i < MAX_USER; i++)
	{
		USER *pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) continue;
		if (pGuild->m_sNum == pUser->m_sGroup ||
			pGuild->m_sNum == pUser->m_sGroupReq)
		{
			pUser->m_sGroup = -1;
			pUser->m_sGroupReq = -1;
			memset(pUser->m_strGuildName, 0, sizeof(pUser->m_strGuildName));
			pUser->m_sGuildSymbolVersion = 0;
			pUser->m_sGuildRank = Guild::GUILD_MEMBER;
		}
	}

	// TODO: Handling Guild Money Status

	short sTownNum = -1;
	if (!CheckDemon(pGuild->m_sClass))
		sTownNum = 1;
	else
		sTownNum = 2;

	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		GuildTownData* GuildTown = g_arGuildTownData[i];
		if (GuildTown == NULL)
			continue;

		if (GuildTown->sTownNum == sTownNum)
		{
			for (int j = 0; j < 3; j++)
			{
				if (GuildTown->sAlly[j] == pGuild->m_sNum)
				{
					GuildTown->sAlly[j] = -1;
				}
			}

			if (GuildTown->sGuildNum == pGuild->m_sNum)
			{
				GuildTown->sGuildNum = -1;
				GuildTown->sLevel = 0;
				GuildTown->sTaxRate = 0;
				::ZeroMemory(GuildTown->strGuildName, sizeof(GuildTown->strGuildName));
				strcpy(GuildTown->strGuildName, "(No Guild)");

				for (int i = 0; i < g_arNpcTypeGuild.GetSize(); i++)
				{
					NPC_TYPE_GUILD* GuildNPC = g_arNpcTypeGuild[i];
					if (GuildNPC->sGuild == GuildTown->sTownNum)
					{
						if (GuildNPC->sGuildOpt == 1)
						{
							CNpc* pNpc = GetNpc(GuildNPC->sNId);
							if (!pNpc) continue;

							pNpc->m_sTownGuildNum = -1;
							pNpc->m_sTownGuildPicId = 0;
							::ZeroMemory(pNpc->m_strTownGuildName, sizeof(pNpc->m_strTownGuildName));
						}
					}
				}
			}
		}
	}


	// Inform everyone that a guild has disbanded.
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_DISSOLVE, index);
	SetByte(m_TempBuf, SUCCESS, index);
	SetByte(m_TempBuf, 2, index);
	SetShort(m_TempBuf, pGuild->m_sNum, index);
	SetVarString(m_TempBuf, pGuild->m_strGuildName, strlen(pGuild->m_strGuildName), index);
	SendAll(m_TempBuf, index);

	// Inform the guild chief that the disband was a success.
	// Same packet as above just different type :S
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_DISSOLVE, index);
	SetByte(m_TempBuf, SUCCESS, index);
	SetByte(m_TempBuf, 1, index);
	SetShort(m_TempBuf, pGuild->m_sNum, index);
	SetVarString(m_TempBuf, pGuild->m_strGuildName, strlen(pGuild->m_strGuildName), index);
	Send(m_TempBuf, index);

	// Reset the guild
	pGuild->Initialize();

	ReleaseGuild();
	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_DISSOLVE, index);
	SetByte(m_TempBuf, FAIL, index);
	SetByte(m_TempBuf, bySubResult, index);
	Send(m_TempBuf, index);
}

void USER::SpecialAttackArrow(TCHAR* pBuf)
{
	CString str;
	str.Format(IDS_GM_MSG_SPECIAL_ATTACK_INFO, m_sHP, m_sMP);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

	int index = 0;
	int iTargetId = GetDWORD(pBuf, index);
	short sDir = GetShort(pBuf, index);
	short sSpecialNo = GetShort(pBuf, index);

	// Validate the special attack being used
	if (sSpecialNo <= 0 || sSpecialNo > g_arSpecialAttackTable.GetSize())
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	CSpecialTable *pSpecial = g_arSpecialAttackTable.GetAt(sSpecialNo-1);
	if (!pSpecial)
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	if (!IsHaveMagic(sSpecialNo, MAGIC_TYPE_SPECIAL))
	{
		TRACE("User does not have this special attack.\n");
		SendSpecialAttackArrow(FAIL);
		return;
	}

	// Get the extra information required for trace special attacks.
	BYTE byTraceMode = TRACE_NONE;
	CString strTraceName;
	if (pSpecial->m_tEValue == SPECIAL_EFFECT_TRACE || pSpecial->m_tEValue == SPECIAL_EFFECT_MASTERTRACE)
	{
		byTraceMode = GetByte(pBuf, index);
		if (byTraceMode == TRACE_HUMAN)
		{
			int iLen = GetVarString(NAME_LENGTH, strTraceName.GetBuffer(NAME_LENGTH), pBuf, index);
			if (iLen <= 0 || iLen > NAME_LENGTH)
			{
				SendSpecialAttackArrow(FAIL);
				return;
			}
		}
		else if (byTraceMode == TRACE_MONSTER)
		{
			int iLen = GetVarString(NPC_NAME_LENGTH, strTraceName.GetBuffer(NPC_NAME_LENGTH), pBuf, index);
			if (iLen <= 0 || iLen > NPC_NAME_LENGTH)
			{
				SendSpecialAttackArrow(FAIL);
				return;
			}
		}
		else
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}
	}

	// Concussion means you are unable to attack
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION))
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	// Cannot attack when in peace mode
	if (m_BattleMode == BATTLEMODE_NORMAL)
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	// Invalid target id check
	if (iTargetId < 0 || iTargetId >= INVALID_BAND)
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	// This handles arrow attack only
	if (pSpecial->m_tType != SPECIAL_TYPE_ARROW)
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	// Validate the target for the special attack
	if (pSpecial->m_tTarget == MAGIC_TARGET_SELF)
	{
		if (m_Uid + USER_BAND != iTargetId)
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}
	}
	else if (pSpecial->m_tTarget == MAGIC_TARGET_OTHER)
	{
		if (m_Uid + USER_BAND == iTargetId)
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}
	}

	// Check have requirements for using the special attack
	if (pSpecial->m_sHpdec >= m_sHP ||
		pSpecial->m_sMpdec > m_sMP ||
		pSpecial->m_sStmdec > m_sStm)
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}
	if (!CheckSuccessSpecialAttackByItem(pSpecial))
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	// Some special attacks are buffs only and therefore
	// you should still be able to use them when within a safety zone
	bool bCheckSafetyZone = true;
	switch (pSpecial->m_tEValue)
	{
	case SPECIAL_EFFECT_HPREGEN:
	case SPECIAL_EFFECT_INCREASEATTACK:
	case SPECIAL_EFFECT_DEFENSE:
	case SPECIAL_EFFECT_SPEED:
	case SPECIAL_EFFECT_DODGE:
	case SPECIAL_EFFECT_MPREGEN:
	case SPECIAL_EFFECT_RAPIDCAST:
	case SPECIAL_EFFECT_MPSAVE:
	case SPECIAL_EFFECT_TRACE:
	case SPECIAL_EFFECT_MASTERTRACE:
		bCheckSafetyZone = false;
		break;
	}

	// GetInc functions to get the inc values for
	// str, dex, int, wis and con.
	int iSTRInc = GetInc(INC_STR);
	int iDEXInc = GetInc(INC_DEX);
	int iINTInc = GetInc(INC_INT);
	int iWISInc = GetInc(INC_WIS);
	int iCONInc = GetInc(INC_CON);

	// Alter Stat Gain Inc values.
	// Weapons give different stat %
	// Some stats cannot be gained
	if (!CheckDemon(m_sClass))
	{
		if (!m_InvItem[ARM_RHAND].IsEmpty())
		{
			// Calculate and change the str inc and dex inc
			// different weapons give different % on stat gain.
			CalcPlusValue(INC_STR, iSTRInc);
			CalcPlusValue(INC_DEX, iDEXInc);

			// Cannot gain these stats if wearing a weapon.
			iCONInc = 0;
			iINTInc = 0;
			iWISInc = 0;
		}
		else
		{
			// Cannot gain these stats.
			iINTInc = 0;
			iWISInc = 0;
		}
	}
	else
	{
		CalcPlusValue(INC_STR, iSTRInc);
		CalcPlusValue(INC_DEX, iDEXInc);
		CalcPlusValue(INC_INT, iINTInc);
		CalcPlusValue(INC_WIS, iWISInc);
		CalcPlusValue(INC_CON, iCONInc);
	}

	// Increase of stats is based on the sRate value of the special attack
	iSTRInc = (iSTRInc * pSpecial->m_sRate) / 100;
	iDEXInc = (iDEXInc * pSpecial->m_sRate) / 100;
	iINTInc = (iINTInc * pSpecial->m_sRate) / 100;
	iWISInc = (iWISInc * pSpecial->m_sRate) / 100;
	iCONInc = (iCONInc * pSpecial->m_sRate) / 100;

	// Handles the specifics of the special attack depending on the target
	// USER or NPC
	if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
	{
		USER* pTarget = GetUser(iTargetId - USER_BAND);
		if (!pTarget)
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}

		// Must check that the player can be PKed
		if (!CheckCanPK(pTarget))
		{
			// Ignore any special attacks that are buffs
			// Abuse of the safety zone check variable (maybe rename?)
			if (bCheckSafetyZone)
			{
				CString strMessage;
				strMessage.Format(IDS_USER_CANNOT_PK_LEVEL_BELOW, LEVEL_CAN_PK);
				SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
				SendSpecialAttackArrow(FAIL);
				return;
			}
		}

		// Cannot PvP when either player is inside safe zone (wotw has a safe zone when its active)
		if (bCheckSafetyZone &&
			(CheckSafetyZone() || pTarget->CheckSafetyZone()))
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}

		// Must be within the distance of special attack from the target!
		if (!CheckDistance(pTarget, pSpecial->m_sEDist))
		{
			if (pSpecial->m_sEDist > 0)
			{
				SendSpecialAttackArrow(FAIL);
				return;
			}
		}

		if (!CheckSuccessSpecialAttack(pSpecial, pTarget))
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}

		if (!CheckSpecialAttackDelay(pSpecial))
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}

		// Can't attack a warping target
		if (pTarget->m_bIsWarping)
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}

		m_dwSpecialAttackCastedTime = GetTickCount();

		int iContinueTime = pSpecial->m_iTime / 1000;
		short sX = -1;
		short sY = -1;
		switch (pSpecial->m_tEValue)
		{
		case SPECIAL_EFFECT_DOUBLEATTACK:
			{
			// Calculate the damage to be dealt
			int iDefense = pTarget->GetDefense();
			int iAttack = GetAttack();

			int iDamage = iAttack - iDefense;

			int iDefenseHuman = pTarget->GetDefenseHuman(this);
			iDamage -= iDefenseHuman;

			int iAttackHuman = GetAttackHuman(pTarget);
			iDamage += iAttackHuman;

			if (iDamage <= 0)
			{
				iDamage = myrand(0, 2);
			}
			iDamage +=  static_cast<int>(((static_cast<double>(myrand(90, 110)) * pSpecial->m_sDamage) / 100.0) + 0.5);

			// Check if the target can be pked
			bool bCanPKTarget = CheckPK(pTarget);

			// Do the damage!
			pTarget->SetDamage(iDamage, m_sClass);

			// If the target died check for any gained pos
			if (pTarget->m_lDeadUsed == 1)
			{
				CheckClassPoint(pTarget);
			}

			// Do extra things if the user pked another user
			if (bCanPKTarget)
			{
				if (pTarget->m_lDeadUsed == 1)
				{
					if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
						(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
						(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
					{
						if (SetMoralByPK(pTarget->m_iMoral))
						{
							SendCharData(INFO_BASICVALUE);
							SendMyInfo(TO_INSIGHT, INFO_MODIFY);
						}
					}
				}

				// Lets any guards know who are within area that a user has attacked another user
				IsDetecterRange();

				// Sets the user gray if they pass the checks
				if (CheckGray(pTarget))
				{
					SetGrayUser(GRAY_MODE_NORMAL);
				}
			}
			}
			break;
		case SPECIAL_EFFECT_HPREGEN:
		case SPECIAL_EFFECT_INCREASEATTACK:
		case SPECIAL_EFFECT_DEFENSE:
		case SPECIAL_EFFECT_SPEED:
		case SPECIAL_EFFECT_DODGE:
		case SPECIAL_EFFECT_MPREGEN:
		case SPECIAL_EFFECT_RAPIDCAST:
		case SPECIAL_EFFECT_MPSAVE:
			pTarget->m_RemainSpecial[pSpecial->m_tEValue].sMid = pSpecial->m_sMid;
			pTarget->m_RemainSpecial[pSpecial->m_tEValue].sDamage = pSpecial->m_sDamage;
			pTarget->m_RemainSpecial[pSpecial->m_tEValue].dwTime = GetTickCount();
			break;
		case SPECIAL_EFFECT_HPDRAIN:
			{
			// Calculate the damage to be dealt
			int iDefense = pTarget->GetDefense();
			int iAttack = GetAttack();

			int iDamage = iAttack - iDefense;

			int iDefenseHuman = pTarget->GetDefenseHuman(this);
			iDamage -= iDefenseHuman;

			int iAttackHuman = GetAttackHuman(pTarget);
			iDamage += iAttackHuman;

			if (iDamage <= 0)
			{
				iDamage = myrand(0, 2);
			}

			int iHpDrain = (pSpecial->m_sDamage * iDamage) / 100;
			if (iHpDrain > pTarget->m_sHP)
			{
				iHpDrain = pTarget->m_sHP;
			}
			pTarget->m_sHP -= iHpDrain;

			m_sHP += iHpDrain;
			if (m_sHP > GetMaxHP())
			{
				m_sHP = GetMaxHP();
			}

			// Check if the target can be pked
			bool bCanPKTarget = CheckPK(pTarget);

			// Do the damage!
			pTarget->SetDamage(iDamage, m_sClass);

			// If the target died check for any gained pos
			if (pTarget->m_lDeadUsed == 1)
			{
				CheckClassPoint(pTarget);
			}

			// Do extra things if the user pked another user
			if (bCanPKTarget)
			{
				if (pTarget->m_lDeadUsed == 1)
				{
					if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
						(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
						(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
					{
						if (SetMoralByPK(pTarget->m_iMoral))
						{
							SendCharData(INFO_BASICVALUE);
							SendMyInfo(TO_INSIGHT, INFO_MODIFY);
						}
					}
				}

				// Lets any guards know who are within area that a user has attacked another user
				IsDetecterRange();

				// Sets the user gray if they pass the checks
				if (CheckGray(pTarget))
				{
					SetGrayUser(GRAY_MODE_NORMAL);
				}
			}
			}
			break;
		case SPECIAL_EFFECT_PARALYZE:
		case SPECIAL_EFFECT_CONCUSSION:
			{
			pTarget->m_RemainSpecial[pSpecial->m_tEValue].sMid = pSpecial->m_sMid;
			pTarget->m_RemainSpecial[pSpecial->m_tEValue].sDamage = CalcStopTime(m_sLevel, pTarget->m_sLevel);
			pTarget->m_RemainSpecial[pSpecial->m_tEValue].dwTime = GetTickCount();
			iContinueTime = pTarget->m_RemainSpecial[pSpecial->m_tEValue].sDamage;

			if (CheckPK(pTarget))
			{
				// Lets any guards know who are within area that a user has attacked another user
				IsDetecterRange();

				// Sets the user gray if they pass the checks
				if (CheckGray(pTarget))
				{
					SetGrayUser(GRAY_MODE_NORMAL);
				}
			}
			}
			break;
		case SPECIAL_EFFECT_TRACE:
		case SPECIAL_EFFECT_MASTERTRACE:
			{
			pTarget->m_RemainSpecial[pSpecial->m_tEValue].sMid = pSpecial->m_sMid;
			pTarget->m_RemainSpecial[pSpecial->m_tEValue].sDamage = pSpecial->m_sDamage;
			pTarget->m_RemainSpecial[pSpecial->m_tEValue].dwTime = GetTickCount();

			if (byTraceMode == TRACE_HUMAN) // User
			{
				USER* pTrace = GetUserId((LPTSTR)(LPCTSTR)strTraceName);
				if (pTrace)
				{
					if (m_sZ == pTrace->m_sZ)
					{
						CPoint pt = ConvertToClient(pTrace->m_sX, pTrace->m_sY);
						sX = static_cast<short>(pt.x);
						sY = static_cast<short>(pt.y);
					}
				}
			}
			else if (byTraceMode == TRACE_MONSTER) // NPC
			{
				CNpc* pNpc = NULL;
				bool bFound = false;
				for (int i = 0; i < g_arNpcThread.GetSize(); i++)
				{
					if (bFound) break;

					for (int j  = 0; j < NPC_NUM; j++)
					{
						pNpc = g_arNpcThread[i]->m_pNpc[j];
						if (!pNpc) continue;
						if (pNpc->m_NpcState == NPC_LIVE || pNpc->m_NpcState == NPC_DEAD) continue;
						if (pNpc->m_sCurZ != m_sZ) continue;

						if (strTraceName.CompareNoCase(pNpc->m_strName) == 0)
						{
							CPoint pt = pNpc->ConvertToClient(pNpc->m_sCurX, pNpc->m_sCurY);
							sX = static_cast<short>(pt.x);
							sY = static_cast<short>(pt.y);
							bFound = true;
							break;
						}
					}
				}
			}
			}
			break;
		}

		// If either of users are not within the arena
		// stats and skill can be gained! (Needs checking!!!!!)
		if (!IsMapRange(MAP_AREA_DUEL) || !pTarget->IsMapRange(MAP_AREA_DUEL))
		{
			if ((m_Uid == pTarget->m_Uid && CheckPlusType(pSpecial, 4)) ||
				(m_Uid != pTarget->m_Uid && CheckPlusType(pSpecial, 5)))
			{
				// Increase weapon skill exp
				int iWeaponExpPercent = pSpecial->m_sPlusExpRate;
				PlusWeaponExpByAttack(iWeaponExpPercent);

				// Increase stats
				if (!CheckDemon(m_sClass))
				{
					PlusStr(iSTRInc);
					PlusDex(iDEXInc);
					PlusInt(iINTInc);
					PlusWis(iWISInc);
					PlusCon(iCONInc);
				}

				m_iSTRInc += iSTRInc;
				m_iDEXInc += iDEXInc;
				m_iINTInc += iINTInc;
				m_iWISInc += iWISInc;
				m_iCONInc += iCONInc;
			}
		}

		m_sHP -= pSpecial->m_sHpdec;
		if (m_sHP <= 0) m_sHP = 1;

		m_sMP -= pSpecial->m_sMpdec;
		if (m_sMP < 0) m_sMP = 0;

		m_sStm -= pSpecial->m_sStmdec;
		if (m_sStm < 0) m_sStm = 0;

		SendSpecialAttackArrow(SUCCESS, pSpecial->m_sMid, iContinueTime, iTargetId, pTarget->m_sHP, sX, sY);
	}
	else if (iTargetId >= NPC_BAND)						// NPC
	{
		CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
		if (!pNpc)
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}

		// Check if the npc is able to die.
		// This allows for preventing attack of a peaceful npc.
		if (pNpc->m_tNpcType == NPCTYPE_NPC)
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}

		// Npc cannot be attacked if it is dead or currently being respawned
		if (pNpc->m_NpcState == NPC_DEAD || pNpc->m_NpcState == NPC_LIVE)
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}

		// Must be within the distance of special attack from the target!
		if (!pNpc->GetDistance(m_sX, m_sY, pSpecial->m_sEDist))
		{
			if (pSpecial->m_sEDist > 0)
			{
				SendSpecialAttackArrow(FAIL);
				return;
			}
		}

		// Guards cannot be attacked by players when they are in battle mode (PK mode allows attack).
		if (pNpc->IsDetecter() && m_BattleMode == BATTLEMODE_ATTACK)
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}

		// Can the special attack be used at this time?
		if (!CheckSpecialAttackDelay(pSpecial))
		{
			SendSpecialAttackArrow(FAIL);
			return;
		}

		// Clear any existing special attacks being used
		InitRemainSpecial();

		// FIXME: Don't think this is needed because CheckSpecialAttackDelay sets its value
		m_dwLastSpecialTime = GetTickCount();

		int iContinueTime = pSpecial->m_iTime / 1000;
		int iDamage = 0;
		bool bNpcDied = false;
		switch (pSpecial->m_tEValue)
		{
		case SPECIAL_EFFECT_DOUBLEATTACK:
			{
			int iDefense = pNpc->GetDefense();
			int iAttack = GetAttack();
			iDamage = iAttack - iDefense;
			if (iDamage <= 0)
			{
				iDamage = myrand(0, 2);
			}
			iDamage +=  static_cast<int>(((static_cast<double>(myrand(90, 110)) * pSpecial->m_sDamage) / 100.0) + 0.5);

			if (iDamage > pNpc->m_sHP)
			{
				iDamage = pNpc->m_sHP;
			}

			// Do the damage!
			pNpc->m_sHP -= iDamage;

			if (pNpc->CheckAIType(NPCAITYPE_DUMMY)) // Dummy
			{
				pNpc->m_sHP = pNpc->m_sMaxHp;
				SendDummyAttack(iTargetId, iDamage);
			}

			// Do stuff if the npc has died
			if (pNpc->m_sHP <= 0)
			{
				NpcDeadEvent(pNpc);
				pNpc->SetDead(m_pCom, m_Uid, true);
				bNpcDied = true;
				PlusMoralByNpc(pNpc->m_iMoral);
				if (CheckPartyLeader())
				{
					int iPlus = GetInc(INC_CHA);
					iPlus *= pNpc->m_sCharm;
					if (!CheckDemon(m_sClass))
					{
						PlusCha(iPlus);
					}
					m_iCHAInc += iPlus;
				}
				NpcThrowItem(pNpc);
				NpcThrowMoney(pNpc);
			}

			DWORD dwGainedExp = static_cast<DWORD>(((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) * 100.0);
			m_dwExp += dwGainedExp;

			CString str;
			str.Format(IDS_GM_MSG_ATTACK_INFO, iDamage, dwGainedExp);
			SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

			if (m_dwExp > m_dwMaxExp)
			{
				CheckLevelUp();
			}
			}
			break;
		case SPECIAL_EFFECT_HPDRAIN:
			{
			int iDefense = pNpc->GetDefense();
			int iAttack = GetAttack();
			iDamage = iAttack - iDefense;
			if (iDamage <= 0)
			{
				iDamage = myrand(0, 2);
			}

			if (pNpc->CheckAIType(NPCAITYPE_DUMMY)) // Dummy
			{
				pNpc->m_sHP = pNpc->m_sMaxHp;
				SendDummyAttack(iTargetId, iDamage);
			}

			int iHpDrain = (pSpecial->m_sDamage * iDamage) / 100;
			if (iHpDrain > pNpc->m_sHP)
			{
				iHpDrain = pNpc->m_sHP;
			}
			pNpc->m_sHP -= iHpDrain;

			m_sHP += iHpDrain;
			if (m_sHP > GetMaxHP())
			{
				m_sHP = GetMaxHP();
			}

			// Do stuff if the npc has died
			if (pNpc->m_sHP <= 0)
			{
				NpcDeadEvent(pNpc);
				pNpc->SetDead(m_pCom, m_Uid, true);
				bNpcDied = true;
				PlusMoralByNpc(pNpc->m_iMoral);
				if (CheckPartyLeader())
				{
					int iPlus = GetInc(INC_CHA);
					iPlus *= pNpc->m_sCharm;
					if (!CheckDemon(m_sClass))
					{
						PlusCha(iPlus);
					}
					m_iCHAInc += iPlus;
				}
				NpcThrowItem(pNpc);
				NpcThrowMoney(pNpc);
			}

			DWORD dwGainedExp = static_cast<DWORD>(((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) * 100.0);
			m_dwExp += dwGainedExp;

			CString str;
			str.Format(IDS_GM_MSG_ATTACK_INFO, iDamage, dwGainedExp);
			SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

			if (m_dwExp > m_dwMaxExp)
			{
				CheckLevelUp();
			}
			}
			break;
		case SPECIAL_EFFECT_PARALYZE:
		case SPECIAL_EFFECT_CONCUSSION:
			pNpc->m_sRemainSpecialNo[pSpecial->m_tEValue] = pSpecial->m_sMid;
			pNpc->m_dwRemainSpecialTime[pSpecial->m_tEValue] = GetTickCount();
			iContinueTime = pNpc->m_sCon;
			break;
		}

		if (pNpc->m_tNpcAttType == NPC_AT_PASSIVE && pSpecial->m_sDamage > 0)
		{
			if (pNpc->CheckAIType(NPCAITYPE_NORMAL) || pNpc->CheckAIType(NPCAITYPE_STONE_GUARD))
			{
				pNpc->AttackListAdd(this);
			}
		}

		// Set the NPC target
		// Stones and Guard NPC types cannot have a target set
		// Target is only set if NPC state is standing, moving or attacking.
		if ((pNpc->m_NpcState == NPC_STANDING ||
			pNpc->m_NpcState == NPC_MOVING ||
			pNpc->m_NpcState == NPC_FIGHTING) &&
			!pNpc->IsDetecter() &&
			!pNpc->IsStone())
		{
			pNpc->m_iAttackedUid = m_Uid + USER_BAND;
			pNpc->m_NpcState = NPC_ATTACKING;
		}

		// Checks for stone guard and normal guards decreasing moral
		// Moral is decreased by 100000.
		if (pNpc->IsDetecter() || pNpc->IsStone())
		{
			bool bSkip = false;
			for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
			{
				if (g_bGuildTownWar[i] && pNpc->m_sGuild == g_arGuildTownData[i]->sTownNum)
				{
					bSkip = true;
					break;
				}
			}

			if (!bSkip && (CheckDemon(m_sClass) == CheckDemon(pNpc->m_sClass)))
			{
				bool bRet = true;
				if (!CheckDemon(m_sClass))
				{
					bRet = SetMoral(-100000);
				}
				else
				{
					bRet = SetMoral(100000);
				}

				if (bRet)
				{
					SendCharData(INFO_BASICVALUE);
					SendMyInfo(TO_INSIGHT, INFO_MODIFY);
				}
			}
		}
		else if (pNpc->CheckAIType(NPCAITYPE_NORMAL) && CheckPlusType(pSpecial, 6)) // Normal AI
		{
			PlusWeaponExpByAttack(pSpecial->m_sPlusExpRate);

			// Con for hsoma is increased seperately
			if (!CheckDemon(m_sClass))
			{
				PlusCon(iCONInc);
				m_iCONInc += iCONInc;
			}

			// Increase following stats by their inc value
			// STR, DEX, INT and WIS (and CON if dsoma)
			// GetPlusValueCount calculates how many times we add the inc value.
			int PlusValueCount = GetPlusValueCount(pNpc, iDamage, bNpcDied, INC_STR);
			for (int i = 0; i < PlusValueCount; i++)
			{
				if (!CheckDemon(m_sClass))
				{
					PlusStr(iSTRInc);
					PlusDex(iDEXInc);
					PlusInt(iINTInc);
					PlusWis(iWISInc);
				}

				m_iSTRInc += iSTRInc;
				m_iDEXInc += iDEXInc;
				m_iINTInc += iINTInc;
				m_iWISInc += iWISInc;

				if (CheckDemon(m_sClass))
				{
					m_iCONInc += iCONInc;
				}
			}
		}

		m_sHP -= pSpecial->m_sHpdec;
		if (m_sHP <= 0) m_sHP = 1;

		m_sMP -= pSpecial->m_sMpdec;
		if (m_sMP < 0) m_sMP = 0;

		m_sStm -= pSpecial->m_sStmdec;
		if (m_sStm < 0) m_sStm = 0;

		SendSpecialAttackArrow(SUCCESS, pSpecial->m_sMid, iContinueTime, iTargetId, pNpc->m_sHP);
	}
}

void USER::RepairItem(TCHAR* pBuf)
{
	int index = 0;

	// Item slot must be valid
	short sSlot = GetShort(pBuf, index);
	if (sSlot < 0 || sSlot >= INV_ITEM_NUM)
	{
		SendRepairItem(FAIL, REPAIR_FAIL_NONEED);
		return;
	}

	// Item must exist in inventory
	ItemList* repairItem = &m_InvItem[sSlot];
	if (repairItem->IsEmpty())
	{
		SendRepairItem(FAIL, REPAIR_FAIL_NONEED);
		return;
	}

	// Item must exist in database
	CItemTable* pRepairItemTable = NULL;
	if (!g_mapItemTable.Lookup(repairItem->sNum, pRepairItemTable))
	{
		SendRepairItem(FAIL, REPAIR_FAIL_NONEED);
		return;
	}

	// Cannot repair non-equipment
	if (repairItem->bType > TYPE_ACC)
	{
		SendRepairItem(FAIL, REPAIR_FAIL_NONEED);
		return;
	}

	// Cannot repair another races item
	if (CheckDemon(m_sClass) != repairItem->IsDemonItem())
	{
		SendRepairItem(FAIL, REPAIR_FAIL_NONEED);
		return;
	}

	// No need in repairing the item if the dura is at its maximum
	// TODO: Add a check to see if somehow sUsage > sMaxUsage (Shouldn't ever be but might happen due to a bug!)
	if (repairItem->sUsage == repairItem->sMaxUsage)
	{
		SendRepairItem(FAIL, REPAIR_FAIL_NONEED);
		return;
	}

	// Cannot repair the item if sMaxUsage < 10% of sDur
	if (repairItem->sMaxUsage <= repairItem->sDur * 0.10)
	{
		SendRepairItem(FAIL, REPAIR_FAIL_NOREPAIR);
		return;
	}

	// Calculate the cost of the repair
	DWORD dwRepairCost = (DWORD)(((double)repairItem->sMaxUsage - (double)repairItem->sUsage) /
		(double)repairItem->sDur * (double)repairItem->dwMoney * 0.5);
	if (dwRepairCost == 0 && repairItem->sMaxUsage != repairItem->sUsage)
	{
		dwRepairCost = 1;
	}

	// User doesn't have enough money for repair
	if (m_dwBarr < dwRepairCost)
	{
		SendRepairItem(FAIL, REPAIR_FAIL_NOMONEY);
		return;
	}

	// Check if the player has a SPECIAL_SPEC_REPAIR item in their inventory
	// This will cause the repair to be 100% success
	short sSpecialRepairSlot = -1;
	for (int i = 0; i < INV_ITEM_NUM; i++)
	{
		if (!m_InvItem[i].IsEmpty() &&
			m_InvItem[i].sSpecial == SPECIAL_SPEC_REPAIR &&
			CheckDemon(m_sClass) == m_InvItem[i].IsDemonItem())
		{
			sSpecialRepairSlot = i;
			break;
		}
	}

	// Calculate a percentage for those that don't get special repair
	// from an item.
	if (sSpecialRepairSlot == -1)
	{
		int iFailPercent = 0;
		if (repairItem->sUsage <= repairItem->sDur * 0.10 && repairItem->sUsage >= repairItem->sDur * 0.08)
		{
			iFailPercent = 2;
		}
		else if (repairItem->sUsage < repairItem->sDur * 0.08 && repairItem->sUsage > repairItem->sDur * 0.04)
		{
			iFailPercent = 5;
		}
		else if (repairItem->sUsage < repairItem->sDur * 0.04)
		{
			iFailPercent = 10;
		}

		if (CheckDemon(m_sClass))
		{
			iFailPercent *= 2;
		}

		if (iFailPercent >= myrand(1, 100))
		{
			repairItem->Init();
			// TODO: Update database here (UpdateItemIndex) does memory not db tho on euro 1p1 :P
			SendDeleteItem(BASIC_INV, sSlot);
			SendRepairItem(FAIL, REPAIR_FAIL_DESTROY);
			return;
		}
	}
	else
	{
		PlusItemDur(&m_InvItem[sSpecialRepairSlot], 1, true);
		// Empty the item if there is no more left
		if (m_InvItem[sSpecialRepairSlot].sUsage <= 0)
		{
			m_InvItem[sSpecialRepairSlot].Init();
		}

		// TODO: Update database here (UpdateItemIndex) does memory not db tho on original 1p1 :P
		SendItemInfoChange(BASIC_INV, sSpecialRepairSlot, INFO_DUR);
		SendChangeWgt();
	}

	// The player has to be charged...(blacksmith needs paying)
	m_dwBarr -= dwRepairCost;

	// Decrease the items maximum repair dura
	// Don't decrease for dsoma instead they get 2x the fail rate
	if (!CheckDemon(m_sClass))
	{
		repairItem->sMaxUsage -= (int)((double)repairItem->sMaxUsage * 0.05);
	}

	// Repair the item dura if current dura isn't already at 30k
	if (repairItem->sUsage != MAX_ITEM_DURA + 1) // 30k
	{
		repairItem->sUsage = repairItem->sMaxUsage;
	}

	SendMoneyChanged();
	// TODO: UpdateItemIndex for the item that got repaired
	SendItemInfoChange(BASIC_INV, sSlot, INFO_DUR | INFO_LIMIT);
	SendRepairItem(SUCCESS);
}

void USER::MagicCasting(TCHAR* pBuf)
{
	int index = 0;
	short sMagicNo = GetShort(pBuf, index);

	DWORD dwCurrTime = GetTickCount();

	// Validate the magic being used
	if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
	{
		SendMagicCasting(FAIL);
		return;
	}

	CMagicTable *pMagic = g_arMagicTable.GetAt(sMagicNo-1);
	if (!pMagic)
	{
		SendMagicCasting(FAIL);
		return;
	}

	if (!IsHaveMagic(sMagicNo, MAGIC_TYPE_MAGIC))
	{
		TRACE("User does not have magic\n");
		SendMagicCasting(FAIL);
		return;
	}

	// Fail casting if reflection still remains
	if (CheckRemainMagic(MAGIC_EFFECT_REFLECT))
	{
		SendMagicCasting(FAIL);
		return;
	}

	// Cannot attack when warping
	if (m_bIsWarping)
	{
		SendMagicCasting(FAIL);
		return;
	}

	// Cannot attack when in peace mode
	if (m_BattleMode == BATTLEMODE_NORMAL)
	{
		SendMagicCasting(FAIL);
		return;
	}

	short sMpDec = pMagic->m_sMpdec;
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_MPSAVE))
	{
		sMpDec = (sMpDec * m_RemainSpecial[SPECIAL_EFFECT_MPSAVE].sDamage) / 100;
	}
	if (sMpDec > m_sMP)
	{
		SendMagicCasting(FAIL);
		return;
	}

	// Moral Checks
	if (!CheckDemon(m_sClass))
	{
		int iMoral = m_iMoral / CLIENT_MORAL;
		if (iMoral < pMagic->m_sMinMoral)
		{
			SendServerChatMessage(IDS_USER_MAGIC_MORAL_LOW, TO_ME);
			SendMagicCasting(FAIL);
			return;
		}

		if (iMoral > pMagic->m_sMaxMoral)
		{
			SendServerChatMessage(IDS_USER_MAGIC_MORAL_HIGH, TO_ME);
			SendMagicCasting(FAIL);
			return;
		}
	}

	int iINT = m_iINT / CLIENT_BASE_STATS;
	if (iINT < pMagic->m_sMinInt)
	{
		SendServerChatMessage(IDS_USER_MAGIC_SKILL_LOW, TO_ME);
		SendMagicCasting(FAIL);
		return;
	}

	int iMagicExp = 0;
	switch (pMagic->m_tClass)
	{
	case 1:
		iMagicExp = m_iBMagicExp / CLIENT_SKILL;
		break;
	case 2:
		iMagicExp = m_iWMagicExp / CLIENT_SKILL;
		break;
	case 3:
		iMagicExp = m_iDMagicExp / CLIENT_SKILL;
		break;
	default:
		break;
	}

	if (iMagicExp < pMagic->m_sMinMxp)
	{
		SendServerChatMessage(IDS_USER_MAGIC_SKILL_LOW, TO_ME);
		SendMagicCasting(FAIL);
		return;
	}

	// If user is not wearing a staff and doesn't have magic avail special
	// then the user cannot cast the magic.
	if (!CheckHaveSpecialNumInBody(SPECIAL_MAGIC_AVAIL))
	{
		if ((!m_InvItem[ARM_RHAND].IsEmpty() && m_InvItem[ARM_RHAND].bType != TYPE_STAFF) ||
			!m_InvItem[ARM_LHAND].IsEmpty())
		{
			SendMagicCasting(FAIL);
			return;
		}
	}

	// Cannot cast a magic that doesn't belong to your mage type
	if (pMagic->m_tClass != 1)
	{
		if (IsServerRank(SERVER_RANK_NORMAL))
		{
			if (CheckMageType(pMagic->m_tClass, 0))
			{
				SendMagicCasting(FAIL);
				return;
			}
		}
	}

	// See if the user has already got a magic casted
	if (m_bMagicCasted)
	{
		if (!IsHaveMagic(m_sMagicNoCasted, MAGIC_TYPE_MAGIC))
		{
			// Dismiss the casted magic because the user does not have it
			m_bMagicCasted = false;
			m_bPreMagicCasted = false;
		}
		else
		{
			if ((dwCurrTime - m_dwMagicCastedTime) <= static_cast<DWORD>(pMagic->m_sStartTime + 1000))
			{
				// Fail the Magic Ready because a magic is still being casted
				SendMagicCasting(FAIL);
				return;
			}

			// Enough time has passed so the cast can be dismissed
			m_bMagicCasted = false;
			m_bPreMagicCasted = false;
		}
	}

	if (pMagic->m_sStartTime > 0)
	{
		m_bMagicCasted = true;
		m_bPreMagicCasted = true;
		m_sMagicNoCasted = sMagicNo;
		m_dwMagicCastedTime = dwCurrTime;
	}

	SendMagicCasting(SUCCESS, sMagicNo, pMagic->m_sStartTime);
}

void USER::EventView(TCHAR* pBuf)
{
	int index = 0;
	BYTE byMode = GetByte(pBuf, index);

	if (byMode == 1) // Quest Overviews
	{
		CBufferEx TempBuf;
		CBufferEx TempAboveBuf;
		int iCount = 0;
		int iCountAbove = 0;
		int iCountNotStarted = 0;
		for (int i = 0; i < g_arEventViewData.GetSize(); i++)
		{
			EventViewData* e = g_arEventViewData[i];
			if (e == NULL) continue;

			if (e->sClass != m_sClass) continue;

			if (e->sLevel > m_sLevel) continue;

			if (FindEvent(e->sEndEvent)) continue;

			if (e->sCheckEvent != -1)
			{
				if (!FindEvent(e->sCheckEvent))
				{
					continue;
				}
			}

			bool bHasStarted = true;
			if (!FindEvent(e->sStartEvent))
			{
				bHasStarted = false;
				iCountNotStarted++;
			}

			TempBuf.Add(e->sId);
			TempBuf.Add((BYTE)bHasStarted);
			TempBuf.AddString((LPTSTR)(LPCTSTR)e->strEventName);
			TempBuf.AddLongString((LPTSTR)(LPCTSTR)e->strEventInfo, e->strEventInfo.GetLength());
			iCount++;
		}

		// Because no quests was found that have not been started
		// look to see if there is any events that are > user level.
		if (iCountNotStarted == 0)
		{
			int iLevel = m_sLevel;
			int iHighestEventLevel = 0;
			while (iLevel < iHighestEventLevel && iCountAbove == 0)
			{
				iLevel++;
				int iCount = 0;
				for (int i = 0; i < g_arEventViewData.GetSize(); i++)
				{
					EventViewData* e = g_arEventViewData[i];
					if (!e) continue;

					if (e->sClass != m_sClass) continue;

					if (e->sLevel > iHighestEventLevel)
					{
						iHighestEventLevel = e->sLevel;
					}

					if (e->sLevel > m_sLevel) continue;

					TempAboveBuf.Add(e->sId);
					TempAboveBuf.AddString((LPTSTR)(LPCTSTR)e->strEventName);
					TempAboveBuf.AddLongString((LPTSTR)(LPCTSTR)e->strEventInfo, e->strEventInfo.GetLength());
					iCountAbove++;
				}
			}
		}

		// Send data
		index = 0;
		SetByte(m_TempBuf, PKT_QUEST_VIEW, index);
		SetByte(m_TempBuf, 1, index);
		SetShort(m_TempBuf, iCount, index);
		SetString(m_TempBuf, TempBuf, TempBuf.GetLength(), index);
		SetShort(m_TempBuf, iCountAbove, index);
		SetString(m_TempBuf, TempAboveBuf, TempAboveBuf.GetLength(), index);
		Send(m_TempBuf, index);
	}
	else if (byMode == 2) // Quest Descriptions
	{
		short sEventId = GetShort(pBuf, index);
		bool bFoundEvent = false;
		for (int i = 0; i < g_arEventViewData.GetSize(); i++)
		{
			EventViewData* e = g_arEventViewData[i];
			if (!e) continue;

			if (e->sId == sEventId)
			{
				bFoundEvent = true;

				index = 0;
				SetByte(m_TempBuf, PKT_QUEST_VIEW, index);
				SetByte(m_TempBuf, 2, index);
				SetByte(m_TempBuf, SUCCESS, index);
				SetShort(m_TempBuf, e->sId, index);
				SetShort(m_TempBuf, e->strEventAll.GetLength(), index);
				SetString(m_TempBuf, (LPTSTR)(LPCTSTR)e->strEventAll, e->strEventAll.GetLength(), index);
				Send(m_TempBuf, index);
				break;
			}
		}

		if (!bFoundEvent)
		{
			index = 0;
			SetByte(m_TempBuf, PKT_QUEST_VIEW, index);
			SetByte(m_TempBuf, 2, index);
			SetShort(m_TempBuf, FAIL, index);
			Send(m_TempBuf, index);

		}
	}
	else if (byMode == 3)
	{
		index = 0;
		SetByte(m_TempBuf, PKT_QUEST_VIEW, index);
		SetByte(m_TempBuf, 3, index);
		Send(m_TempBuf, index);
	}
}

void USER::SpecialAttackRail(TCHAR* pBuf)
{
	CString str;
	str.Format(IDS_GM_MSG_SPECIAL_ATTACK_INFO, m_sHP, m_sMP);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

	int index = 0;
	short sDir = GetShort(pBuf, index);
	BYTE byDir = GetByte(pBuf, index);
	short sSpecialNo = GetShort(pBuf, index);

	// Set direction the user is now facing
	m_byDir = static_cast<BYTE>(sDir);

	// Validate the special attack being used
	if (sSpecialNo <= 0 || sSpecialNo > g_arSpecialAttackTable.GetSize())
	{
		SendSpecialAttackRail(FAIL);
		return;
	}

	CSpecialTable *pSpecial = g_arSpecialAttackTable.GetAt(sSpecialNo-1);
	if (!pSpecial)
	{
		SendSpecialAttackRail(FAIL);
		return;
	}

	if (!IsHaveMagic(sSpecialNo, MAGIC_TYPE_SPECIAL))
	{
		TRACE("User does not have this special attack.\n");
		SendSpecialAttackRail(FAIL);
		return;
	}

	// Concussion means you are unable to attack
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION))
	{
		SendSpecialAttackRail(FAIL);
		return;
	}

	// Cannot attack when in peace mode
	if (m_BattleMode == BATTLEMODE_NORMAL)
	{
		SendSpecialAttackRail(FAIL);
		return;
	}

	// This handles rail attack only
	if (pSpecial->m_tType != SPECIAL_TYPE_RAIL)
	{
		SendSpecialAttackRail(FAIL);
		return;
	}

	// Check have requirements for using the special attack
	if (pSpecial->m_sHpdec >= m_sHP ||
		pSpecial->m_sMpdec > m_sMP ||
		pSpecial->m_sStmdec > m_sStm)
	{
		SendSpecialAttackRail(FAIL);
		return;
	}
	if (!CheckSuccessSpecialAttackByItem(pSpecial))
	{
		SendSpecialAttackRail(FAIL);
		return;
	}
	if (!CheckSpecialAttackDelay(pSpecial))
	{
		SendSpecialAttackRail(FAIL);
		return;
	}

	// Some special attacks are buffs only and therefore
	// you should still be able to use them when within a safety zone
	bool bCheckSafetyZone = true;
	switch (pSpecial->m_tEValue)
	{
	case SPECIAL_EFFECT_HPREGEN:
	case SPECIAL_EFFECT_INCREASEATTACK:
	case SPECIAL_EFFECT_DEFENSE:
	case SPECIAL_EFFECT_SPEED:
	case SPECIAL_EFFECT_DODGE:
	case SPECIAL_EFFECT_MPREGEN:
	case SPECIAL_EFFECT_RAPIDCAST:
	case SPECIAL_EFFECT_MPSAVE:
	case SPECIAL_EFFECT_TRACE:
	case SPECIAL_EFFECT_MASTERTRACE:
		bCheckSafetyZone = false;
		break;
	}

	// GetInc functions to get the inc values for
	// str, dex, int, wis and con.
	int iSTRInc = GetInc(INC_STR);
	int iDEXInc = GetInc(INC_DEX);
	int iINTInc = GetInc(INC_INT);
	int iWISInc = GetInc(INC_WIS);
	int iCONInc = GetInc(INC_CON);

	// Alter Stat Gain Inc values.
	// Weapons give different stat %
	// Some stats cannot be gained
	if (!CheckDemon(m_sClass))
	{
		if (!m_InvItem[ARM_RHAND].IsEmpty())
		{
			// Calculate and change the str inc and dex inc
			// different weapons give different % on stat gain.
			CalcPlusValue(INC_STR, iSTRInc);
			CalcPlusValue(INC_DEX, iDEXInc);

			// Cannot gain these stats if wearing a weapon.
			iCONInc = 0;
			iINTInc = 0;
			iWISInc = 0;
		}
		else
		{
			// Cannot gain these stats.
			iINTInc = 0;
			iWISInc = 0;
		}
	}
	else
	{
		CalcPlusValue(INC_STR, iSTRInc);
		CalcPlusValue(INC_DEX, iDEXInc);
		CalcPlusValue(INC_INT, iINTInc);
		CalcPlusValue(INC_WIS, iWISInc);
		CalcPlusValue(INC_CON, iCONInc);
	}

	// Increase of stats is based on the sRate value of the special attack
	iSTRInc = (iSTRInc * pSpecial->m_sRate) / 100;
	iDEXInc = (iDEXInc * pSpecial->m_sRate) / 100;
	iINTInc = (iINTInc * pSpecial->m_sRate) / 100;
	iWISInc = (iWISInc * pSpecial->m_sRate) / 100;
	iCONInc = (iCONInc * pSpecial->m_sRate) / 100;

	if (pSpecial->m_sEDist <= 0)
	{
		SendSpecialAttackRail(FAIL);
		return;
	}

	short sXAdd = 0;
	short sYAdd = 0;
	switch (byDir)
	{
	case DIR_DOWN:
		sYAdd = 1;
		break;
	case DIR_DOWN_LEFT:
		sXAdd = -1;
		sYAdd = 1;
		break;
	case DIR_LEFT:
		sXAdd = -1;
		break;
	case DIR_UP_LEFT:
		sXAdd = -1;
		sYAdd = -1;
		break;
	case DIR_UP:
		sYAdd = -1;
		break;
	case DIR_UP_RIGHT:
		sXAdd = 1;
		sYAdd = -1;
		break;
	case DIR_RIGHT:
		sXAdd = 1;
		break;
	case DIR_DOWN_RIGHT:
		sXAdd = 1;
		sYAdd = 1;
		break;
	}

	// Handles the specifics of the special attack depending on the target
	// USER or NPC
	int iTargetCount = 0;

	int iTargetsId[50] = {};
	short sTargetsHP[50] = {};

	for (int i = 1; i < pSpecial->m_sEDist; i++)
	{
		short sX = m_sX + sXAdd * i;
		short sY = m_sY + sYAdd * i;

		ASSERT(m_iZoneIndex >= 0 && m_iZoneIndex < g_Zones.GetSize());
		ASSERT(g_Zones[m_iZoneIndex]);

		if (sX > g_Zones[m_iZoneIndex]->m_sizeMap.cx || sX < 0) continue;
		if (sY > g_Zones[m_iZoneIndex]->m_sizeMap.cy || sY < 0) continue;

		if (abs(m_sX - sX) > pSpecial->m_sEDist) continue;
		if (abs(m_sY - sY) > pSpecial->m_sEDist) continue;

		int iTargetId = GetUid(sX, sY);
		if (iTargetId < USER_BAND || iTargetId == m_Uid + USER_BAND) continue;

		if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
		{
			USER* pTarget = GetUser(iTargetId - USER_BAND);
			if (!pTarget) continue;

			// Must check that the player can be PKed
			if (!CheckCanPK(pTarget))
			{
				// Ignore any special attacks that are buffs
				// Abuse of the safety zone check variable (maybe rename?)
				if (bCheckSafetyZone)
				{
					CString strMessage;
					strMessage.Format(IDS_USER_CANNOT_PK_LEVEL_BELOW, LEVEL_CAN_PK);
					SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
					continue;
				}
			}

			// Check for safety zone
			if (CheckSafetyZone() || pTarget->CheckSafetyZone())
			{
				continue;
			}

			// TODO : Maybe move this Live check on the target closer to the first check to be made.
			if (!pTarget->m_bLive ||
				pTarget->m_State != STATE_GAMESTARTED ||
				pTarget->m_bHidden ||
				pTarget->m_bIsWarping)
			{
				continue;
			}

			// TODO: Comment me
			/*if (CheckDemon(m_sClass) == CheckDemon(pTarget->m_sClass) &&
				m_BattleMode == BATTLEMODE_ATTACK &&
				pTarget->m_iGrayCount == 0 &&
				!CheckGuildWarArmy(pTarget))
			{
				continue;
			}*/
			// Check can cast SpecialAttack on the target
			if (!CheckSuccessSpecialAttack(pSpecial, pTarget))
			{
				continue;
			}

			// Remove the target from the location on map if for some reason they should no longer be there
			if (sX != pTarget->m_sX || sY != pTarget->m_sY)
			{
				g_Zones[m_iZoneIndex]->m_pMap[sX][sY].m_lUser = 0;
				continue;
			}

			iTargetsId[iTargetCount] = iTargetId;

			TRACE("SpecialAttackRail - Found USER Target: %d\n", iTargetsId[iTargetCount]);
			++iTargetCount;
		}
		else if (iTargetId >= NPC_BAND)						// NPC
		{
			CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
			if (!pNpc) continue;

			// Check if the npc is able to die.
			// This allows for preventing attack of a peaceful npc.
			if (pNpc->m_tNpcType == NPCTYPE_NPC) continue;

			if (pNpc->m_NpcState == NPC_DEAD || pNpc->m_NpcState == NPC_LIVE) continue;

			// Guards cannot be attacked by players when they are in battle mode (PK mode allows attack).
			if (pNpc->IsDetecter() && m_BattleMode == BATTLEMODE_ATTACK) continue;

			// Clear any existing special attacks being used
			InitRemainSpecial();

			// If AI Type of npc is NPCAITYPE_NORMAL or NPCAITYPE_STONE_GUARD then do something with the AttackList of the npc oO
			// Need to look at this in more detail before making an implementation.
			if (pNpc->m_tNpcAttType == NPC_AT_PASSIVE && pSpecial->m_sDamage > 0)
			{
				if (pNpc->CheckAIType(NPCAITYPE_NORMAL) || pNpc->CheckAIType(NPCAITYPE_STONE_GUARD))
				{
					pNpc->AttackListAdd(this);
				}
			}

			// Set the NPC target
			// Stones and Guard NPC types cannot have a target set
			// Target is only set if NPC state is standing, moving or attacking.
			if ((pNpc->m_NpcState == NPC_STANDING ||
				pNpc->m_NpcState == NPC_MOVING ||
				pNpc->m_NpcState == NPC_FIGHTING) &&
				!pNpc->IsDetecter() &&
				!pNpc->IsStone())
			{
				pNpc->m_iAttackedUid = m_Uid + USER_BAND;
				pNpc->m_NpcState = NPC_ATTACKING;
			}

			// Remove the target from the location on map if for some reason they should no longer be there
			if (sX != pNpc->m_sCurX || sY != pNpc->m_sCurY)
			{
				g_Zones[m_iZoneIndex]->m_pMap[sX][sY].m_lUser = 0;
				continue;
			}

			iTargetsId[iTargetCount] = iTargetId;

			TRACE("SpecialAttackRail - Found NPC Target: %d\n", iTargetsId[iTargetCount]);
			++iTargetCount;
		}

		if (iTargetCount >= 50) break;
	}

	// Make sure there are targets before continuing
	if (iTargetCount <= 0)
	{
		SendSpecialAttackRail(FAIL);
		return;
	}

	int iContinueTime = pSpecial->m_iTime / 1000;

	for (int i = 0; i < iTargetCount; i++)
	{
		int iTargetId = iTargetsId[i];

		if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
		{
			USER* pTarget = GetUser(iTargetId - USER_BAND);
			if (!pTarget)
			{
				SendSpecialAttackRail(FAIL);
				return;
			}

			switch (pSpecial->m_tEValue)
			{
			case SPECIAL_EFFECT_DOUBLEATTACK:
			case SPECIAL_EFFECT_PIERCE:
				{
				// Calculate the damage to be dealt
				int iDefense = pTarget->GetDefense();
				int iAttack = GetAttack();

				int iDamage = iAttack - iDefense;

				int iDefenseHuman = pTarget->GetDefenseHuman(this);
				iDamage -= iDefenseHuman;

				int iAttackHuman = GetAttackHuman(pTarget);
				iDamage += iAttackHuman;

				if (iDamage <= 0)
				{
					iDamage = myrand(0, 2);
				}
				iDamage +=  static_cast<int>(((static_cast<double>(myrand(90, 110)) * pSpecial->m_sDamage) / 100.0) + 0.5);

				// Check if the target can be pked
				bool bCanPKTarget = CheckPK(pTarget);

				// Do the damage!
				pTarget->SetDamage(iDamage, m_sClass);

				// If the target died check for any gained pos
				if (pTarget->m_lDeadUsed == 1)
				{
					CheckClassPoint(pTarget);
				}

				// Do extra things if the user pked another user
				if (bCanPKTarget)
				{
					if (pTarget->m_lDeadUsed == 1)
					{
						if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
							(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
							(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
						{
							if (SetMoralByPK(pTarget->m_iMoral))
							{
								SendCharData(INFO_BASICVALUE);
								SendMyInfo(TO_INSIGHT, INFO_MODIFY);
							}
						}
					}

					// Lets any guards know who are within area that a user has attacked another user
					IsDetecterRange();

					// Sets the user gray if they pass the checks
					if (CheckGray(pTarget))
					{
						SetGrayUser(GRAY_MODE_NORMAL);
					}
				}
				}
				break;
			case SPECIAL_EFFECT_HPREGEN:
			case SPECIAL_EFFECT_INCREASEATTACK:
			case SPECIAL_EFFECT_DEFENSE:
			case SPECIAL_EFFECT_SPEED:
			case SPECIAL_EFFECT_DODGE:
			case SPECIAL_EFFECT_MPREGEN:
			case SPECIAL_EFFECT_RAPIDCAST:
			case SPECIAL_EFFECT_MPSAVE:
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].sMid = pSpecial->m_sMid;
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].sDamage = pSpecial->m_sDamage;
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].dwTime = GetTickCount();
				break;
			case SPECIAL_EFFECT_HPDRAIN:
				{
				// Calculate the damage to be dealt
				int iDefense = pTarget->GetDefense();
				int iAttack = GetAttack();

				int iDamage = iAttack - iDefense;

				int iDefenseHuman = pTarget->GetDefenseHuman(this);
				iDamage -= iDefenseHuman;

				int iAttackHuman = GetAttackHuman(pTarget);
				iDamage += iAttackHuman;

				if (iDamage <= 0)
				{
					iDamage = myrand(0, 2);
				}

				int iHpDrain = (pSpecial->m_sDamage * iDamage) / 100;
				if (iHpDrain > pTarget->m_sHP)
				{
					iHpDrain = pTarget->m_sHP;
				}
				pTarget->m_sHP -= iHpDrain;

				m_sHP += iHpDrain;
				if (m_sHP > GetMaxHP())
				{
					m_sHP = GetMaxHP();
				}

				// Check if the target can be pked
				bool bCanPKTarget = CheckPK(pTarget);

				// Do the damage!
				pTarget->SetDamage(iDamage, m_sClass);

				// If the target died check for any gained pos
				if (pTarget->m_lDeadUsed == 1)
				{
					CheckClassPoint(pTarget);
				}

				// Do extra things if the user pked another user
				if (bCanPKTarget)
				{
					if (pTarget->m_lDeadUsed == 1)
					{
						if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
							(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
							(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
						{
							if (SetMoralByPK(pTarget->m_iMoral))
							{
								SendCharData(INFO_BASICVALUE);
								SendMyInfo(TO_INSIGHT, INFO_MODIFY);
							}
						}
					}

					// Lets any guards know who are within area that a user has attacked another user
					IsDetecterRange();

					// Sets the user gray if they pass the checks
					if (CheckGray(pTarget))
					{
						SetGrayUser(GRAY_MODE_NORMAL);
					}
				}
				}
				break;
			case SPECIAL_EFFECT_PARALYZE:
			case SPECIAL_EFFECT_CONCUSSION:
				{
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].sMid = pSpecial->m_sMid;
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].sDamage = CalcStopTime(m_sLevel, pTarget->m_sLevel);
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].dwTime = GetTickCount();
				iContinueTime = pTarget->m_RemainSpecial[pSpecial->m_tEValue].sDamage;

				if (CheckPK(pTarget))
				{
					// Lets any guards know who are within area that a user has attacked another user
					IsDetecterRange();

					// Sets the user gray if they pass the checks
					if (CheckGray(pTarget))
					{
						SetGrayUser(GRAY_MODE_NORMAL);
					}
				}
				}
				break;
			}

			// If either of users are not within the arena
			// stats and skill can be gained! (Needs checking!!!!!)
			if (!IsMapRange(MAP_AREA_DUEL) || !pTarget->IsMapRange(MAP_AREA_DUEL))
			{
				if ((m_Uid == pTarget->m_Uid && CheckPlusType(pSpecial, 4)) ||
					(m_Uid != pTarget->m_Uid && CheckPlusType(pSpecial, 5)))
				{
					// Increase weapon skill exp
					int iWeaponExpPercent = pSpecial->m_sPlusExpRate;
					PlusWeaponExpByAttack(iWeaponExpPercent);

					// Increase stats
					if (!CheckDemon(m_sClass))
					{
						PlusStr(iSTRInc);
						PlusDex(iDEXInc);
						PlusInt(iINTInc);
						PlusWis(iWISInc);
						PlusCon(iCONInc);
					}

					m_iSTRInc += iSTRInc;
					m_iDEXInc += iDEXInc;
					m_iINTInc += iINTInc;
					m_iWISInc += iWISInc;
					m_iCONInc += iCONInc;
				}
			}

			sTargetsHP[i] = pTarget->m_sHP;
		}
		else if (iTargetId >= NPC_BAND)						// NPC
		{
			CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
			if (!pNpc)
			{
				SendSpecialAttackRail(FAIL);
				return;
			}

			int iDamage = 0;
			bool bNpcDied = false;
			switch (pSpecial->m_tEValue)
			{
			case SPECIAL_EFFECT_DOUBLEATTACK:
			case SPECIAL_EFFECT_PIERCE:
				{
				int iDefense = pNpc->GetDefense();
				int iAttack = GetAttack();
				iDamage = iAttack - iDefense;
				if (iDamage <= 0)
				{
					iDamage = myrand(0, 2);
				}
				iDamage +=  static_cast<int>(((static_cast<double>(myrand(90, 110)) * pSpecial->m_sDamage) / 100.0) + 0.5);

				if (iDamage > pNpc->m_sHP)
				{
					iDamage = pNpc->m_sHP;
				}

				// Do the damage!
				pNpc->m_sHP -= iDamage;

				if (pNpc->CheckAIType(NPCAITYPE_DUMMY)) // Dummy
				{
					pNpc->m_sHP = pNpc->m_sMaxHp;
					SendDummyAttack(iTargetId, iDamage);
				}

				// Do stuff if the npc has died
				if (pNpc->m_sHP <= 0)
				{
					NpcDeadEvent(pNpc);
					pNpc->SetDead(m_pCom, m_Uid, true);
					bNpcDied = true;
					PlusMoralByNpc(pNpc->m_iMoral);
					if (CheckPartyLeader())
					{
						int iPlus = GetInc(INC_CHA);
						iPlus *= pNpc->m_sCharm;
						if (!CheckDemon(m_sClass))
						{
							PlusCha(iPlus);
						}
						m_iCHAInc += iPlus;
					}
					NpcThrowItem(pNpc);
					NpcThrowMoney(pNpc);
				}

				DWORD dwGainedExp = static_cast<DWORD>(((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) * 100.0);
				m_dwExp += dwGainedExp;

				CString str;
				str.Format(IDS_GM_MSG_ATTACK_INFO, iDamage, dwGainedExp);
				SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

				if (m_dwExp > m_dwMaxExp)
				{
					CheckLevelUp();
				}
				}
				break;
			case SPECIAL_EFFECT_HPDRAIN:
				{
				int iDefense = pNpc->GetDefense();
				int iAttack = GetAttack();
				iDamage = iAttack - iDefense;
				if (iDamage <= 0)
				{
					iDamage = myrand(0, 2);
				}

				if (pNpc->CheckAIType(NPCAITYPE_DUMMY)) // Dummy
				{
					pNpc->m_sHP = pNpc->m_sMaxHp;
					SendDummyAttack(iTargetId, iDamage);
				}

				int iHpDrain = (pSpecial->m_sDamage * iDamage) / 100;
				if (iHpDrain > pNpc->m_sHP)
				{
					iHpDrain = pNpc->m_sHP;
				}
				pNpc->m_sHP -= iHpDrain;

				m_sHP += iHpDrain;
				if (m_sHP > GetMaxHP())
				{
					m_sHP = GetMaxHP();
				}

				// Do stuff if the npc has died
				if (pNpc->m_sHP <= 0)
				{
					NpcDeadEvent(pNpc);
					pNpc->SetDead(m_pCom, m_Uid, true);
					bNpcDied = true;
					PlusMoralByNpc(pNpc->m_iMoral);
					if (CheckPartyLeader())
					{
						int iPlus = GetInc(INC_CHA);
						iPlus *= pNpc->m_sCharm;
						if (!CheckDemon(m_sClass))
						{
							PlusCha(iPlus);
						}
						m_iCHAInc += iPlus;
					}
					NpcThrowItem(pNpc);
					NpcThrowMoney(pNpc);
				}

				DWORD dwGainedExp = static_cast<DWORD>(((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) * 100.0);
				m_dwExp += dwGainedExp;

				CString str;
				str.Format(IDS_GM_MSG_ATTACK_INFO, iDamage, dwGainedExp);
				SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

				if (m_dwExp > m_dwMaxExp)
				{
					CheckLevelUp();
				}
				}
				break;
			case SPECIAL_EFFECT_PARALYZE:
			case SPECIAL_EFFECT_CONCUSSION:
				pNpc->m_sRemainSpecialNo[pSpecial->m_tEValue] = pSpecial->m_sMid;
				pNpc->m_dwRemainSpecialTime[pSpecial->m_tEValue] = GetTickCount();
				iContinueTime = pNpc->m_sCon;
				break;
			}

			if (pNpc->m_tNpcAttType == NPC_AT_PASSIVE && pSpecial->m_sDamage > 0)
			{
				if (pNpc->CheckAIType(NPCAITYPE_NORMAL) || pNpc->CheckAIType(NPCAITYPE_STONE_GUARD))
				{
					pNpc->AttackListAdd(this);
				}
			}

			// Set the NPC target
			// Stones and Guard NPC types cannot have a target set
			// Target is only set if NPC state is standing, moving or attacking.
			if ((pNpc->m_NpcState == NPC_STANDING ||
				pNpc->m_NpcState == NPC_MOVING ||
				pNpc->m_NpcState == NPC_FIGHTING) &&
				!pNpc->IsDetecter() &&
				!pNpc->IsStone())
			{
				pNpc->m_iAttackedUid = m_Uid + USER_BAND;
				pNpc->m_NpcState = NPC_ATTACKING;
			}
			// Checks for stone guard and normal guards decreasing moral
			// Moral is decreased by 100000.
			if (pNpc->IsDetecter() || pNpc->IsStone())
			{
				bool bSkip = false;
				for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
				{
					if (g_bGuildTownWar[i] && pNpc->m_sGuild == g_arGuildTownData[i]->sTownNum)
					{
						bSkip = true;
						break;
					}
				}

				if (!bSkip && (CheckDemon(m_sClass) == CheckDemon(pNpc->m_sClass)))
				{
					bool bRet = true;
					if (!CheckDemon(m_sClass))
					{
						bRet = SetMoral(-100000);
					}
					else
					{
						bRet = SetMoral(100000);
					}

					if (bRet)
					{
						SendCharData(INFO_BASICVALUE);
						SendMyInfo(TO_INSIGHT, INFO_MODIFY);
					}
				}
			}
			else if (pNpc->CheckAIType(NPCAITYPE_NORMAL) && CheckPlusType(pSpecial, 6)) // Normal AI
			{
				PlusWeaponExpByAttack(pSpecial->m_sPlusExpRate);

				// Con for hsoma is increased seperately
				if (!CheckDemon(m_sClass))
				{
					PlusCon(iCONInc);
					m_iCONInc += iCONInc;
				}

				// Increase following stats by their inc value
				// STR, DEX, INT and WIS (and CON if dsoma)
				// GetPlusValueCount calculates how many times we add the inc value.
				int PlusValueCount = GetPlusValueCount(pNpc, iDamage, bNpcDied, INC_STR);
				for (int i = 0; i < PlusValueCount; i++)
				{
					if (!CheckDemon(m_sClass))
					{
						PlusStr(iSTRInc);
						PlusDex(iDEXInc);
						PlusInt(iINTInc);
						PlusWis(iWISInc);
					}

					m_iSTRInc += iSTRInc;
					m_iDEXInc += iDEXInc;
					m_iINTInc += iINTInc;
					m_iWISInc += iWISInc;

					if (CheckDemon(m_sClass))
					{
						m_iCONInc += iCONInc;
					}
				}
			}

			sTargetsHP[i] = pNpc->m_sHP;
		}
	}

	m_sHP -= pSpecial->m_sHpdec;
	if (m_sHP <= 0) m_sHP = 1;

	m_sMP -= pSpecial->m_sMpdec;
	if (m_sMP < 0) m_sMP = 0;

	m_sStm -= pSpecial->m_sStmdec;
	if (m_sStm < 0) m_sStm = 0;

	SendSpecialAttackRail(SUCCESS, iTargetCount, iTargetsId, sSpecialNo, sTargetsHP, iContinueTime);
}

void USER::SpecialAttackCircle(TCHAR* pBuf)
{
	CString str;
	str.Format(IDS_GM_MSG_SPECIAL_ATTACK_INFO, m_sHP, m_sMP);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

	int index = 0;
	int iTargetId = GetDWORD(pBuf, index);
	short sDir = GetShort(pBuf, index);
	short sSpecialNo = GetShort(pBuf, index);

	// Set direction the user is now facing
	m_byDir = static_cast<BYTE>(sDir);

	// Validate the special attack being used
	if (sSpecialNo <= 0 || sSpecialNo > g_arSpecialAttackTable.GetSize())
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}

	CSpecialTable *pSpecial = g_arSpecialAttackTable.GetAt(sSpecialNo-1);
	if (!pSpecial)
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}

	if (!IsHaveMagic(sSpecialNo, MAGIC_TYPE_SPECIAL))
	{
		TRACE("User does not have this special attack.\n");
		SendSpecialAttackCircle(FAIL);
		return;
	}

	// Concussion means you are unable to attack
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION))
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}

	// Cannot attack when in peace mode
	if (m_BattleMode == BATTLEMODE_NORMAL)
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}

	// Invalid target id check
	if (iTargetId < 0 || iTargetId >= INVALID_BAND)
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}

	// This handles circle attack only
	if (pSpecial->m_tType != SPECIAL_TYPE_CIRCLE)
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}

	// Check have requirements for using the special attack
	if (pSpecial->m_sHpdec >= m_sHP ||
		pSpecial->m_sMpdec > m_sMP ||
		pSpecial->m_sStmdec > m_sStm)
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}
	if (!CheckSuccessSpecialAttackByItem(pSpecial))
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}
	if (!CheckSpecialAttackDelay(pSpecial))
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}

	// Some special attacks are buffs only and therefore
	// you should still be able to use them when within a safety zone
	bool bCheckSafetyZone = true;
	switch (pSpecial->m_tEValue)
	{
	case SPECIAL_EFFECT_HPREGEN:
	case SPECIAL_EFFECT_INCREASEATTACK:
	case SPECIAL_EFFECT_DEFENSE:
	case SPECIAL_EFFECT_SPEED:
	case SPECIAL_EFFECT_DODGE:
	case SPECIAL_EFFECT_MPREGEN:
	case SPECIAL_EFFECT_RAPIDCAST:
	case SPECIAL_EFFECT_MPSAVE:
	case SPECIAL_EFFECT_TRACE:
	case SPECIAL_EFFECT_MASTERTRACE:
		bCheckSafetyZone = false;
		break;
	}

	// GetInc functions to get the inc values for
	// str, dex, int, wis and con.
	int iSTRInc = GetInc(INC_STR);
	int iDEXInc = GetInc(INC_DEX);
	int iINTInc = GetInc(INC_INT);
	int iWISInc = GetInc(INC_WIS);
	int iCONInc = GetInc(INC_CON);

	// Alter Stat Gain Inc values.
	// Weapons give different stat %
	// Some stats cannot be gained
	if (!CheckDemon(m_sClass))
	{
		if (!m_InvItem[ARM_RHAND].IsEmpty())
		{
			// Calculate and change the str inc and dex inc
			// different weapons give different % on stat gain.
			CalcPlusValue(INC_STR, iSTRInc);
			CalcPlusValue(INC_DEX, iDEXInc);

			// Cannot gain these stats if wearing a weapon.
			iCONInc = 0;
			iINTInc = 0;
			iWISInc = 0;
		}
		else
		{
			// Cannot gain these stats.
			iINTInc = 0;
			iWISInc = 0;
		}
	}
	else
	{
		CalcPlusValue(INC_STR, iSTRInc);
		CalcPlusValue(INC_DEX, iDEXInc);
		CalcPlusValue(INC_INT, iINTInc);
		CalcPlusValue(INC_WIS, iWISInc);
		CalcPlusValue(INC_CON, iCONInc);
	}

	// Increase of stats is based on the sRate value of the special attack
	iSTRInc = (iSTRInc * pSpecial->m_sRate) / 100;
	iDEXInc = (iDEXInc * pSpecial->m_sRate) / 100;
	iINTInc = (iINTInc * pSpecial->m_sRate) / 100;
	iWISInc = (iWISInc * pSpecial->m_sRate) / 100;
	iCONInc = (iCONInc * pSpecial->m_sRate) / 100;

	if (pSpecial->m_sEDist <= 0)
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}

	int iMaxTarget = 50;
	bool bCheckAttackSuccess = false;
	bool bFacingThreeOnly = false;
	switch (pSpecial->m_tEValue)
	{
	case SPECIAL_EFFECT_AREAMULTIPLEATTACK:
		iMaxTarget = pSpecial->m_sDamage;
		bCheckAttackSuccess = true;
		break;
	case SPECIAL_EFFECT_MULTIPLEATTACK: // Fall through
	case SPECIAL_EFFECT_MASTERMULTIPLEATTACK:
		bFacingThreeOnly = true;
		break;
	}


	// Handles the specifics of the special attack depending on the target
	// USER or NPC
	int iTargetCount = 0;

	int iTargetsId[50] = {};
	short sTargetsHP[50] = {};

	ASSERT(m_iZoneIndex >= 0 && m_iZoneIndex < g_Zones.GetSize());
	ASSERT(g_Zones[m_iZoneIndex]);

	short sMinX = m_sX - pSpecial->m_sEDist;
	if (sMinX < 0) sMinX = 0;

	short sMaxX = m_sX + pSpecial->m_sEDist;
	if (sMaxX > g_Zones[m_iZoneIndex]->m_sizeMap.cx) sMaxX = static_cast<short>(g_Zones[m_iZoneIndex]->m_sizeMap.cx);

	short sMinY = m_sY - pSpecial->m_sEDist;
	if (sMinY < 0) sMinY = 0;

	short sMaxY = m_sY + pSpecial->m_sEDist;
	if (sMaxY > g_Zones[m_iZoneIndex]->m_sizeMap.cy) sMaxY = static_cast<short>(g_Zones[m_iZoneIndex]->m_sizeMap.cy);

	short sTargetX = 0;
	short sTargetY = 0;
	if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
	{
		USER* pTarget = GetUser(iTargetId - USER_BAND);
		if (!pTarget)
		{
			SendSpecialAttackCircle(FAIL);
			return;
		}
		sTargetX = pTarget->m_sX;
		sTargetY = pTarget->m_sY;
	}
	else if (iTargetId >= NPC_BAND)						// NPC
	{
		CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
		if (!pNpc)
		{
			SendSpecialAttackCircle(FAIL);
			return;
		}
		sTargetX = pNpc->m_sCurX;
		sTargetY = pNpc->m_sCurY;
	}

	short sDiffX = m_sX - sMinX;
	for (short sX = sMinX; sX <= sMaxX; sX++, sDiffX--)
	{
		short sDiffY = m_sY - sMinY;
		for (short sY = sMinY; sY <= sMaxY; sY++, sDiffY--)
		{
			if (bFacingThreeOnly && (abs((sTargetX - m_sX) + sDiffX) +
				abs((sTargetY - m_sY) + sDiffY)) > 1)
			{
				continue;
			}

			int iTargetId = GetUid(sX, sY);
			if (iTargetId < USER_BAND) continue;

			// Validate the target for the special attack
			if (pSpecial->m_tTarget == MAGIC_TARGET_SELF)
			{
				if (m_Uid + USER_BAND != iTargetId)
				{
					continue;
				}
			}
			else if (pSpecial->m_tTarget == MAGIC_TARGET_OTHER)
			{
				if (m_Uid + USER_BAND == iTargetId)
				{
					continue;
				}
			}

			if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
			{
				USER* pTarget = GetUser(iTargetId - USER_BAND);
				if (!pTarget) continue;

				// Must check that the player can be PKed
				if (!CheckCanPK(pTarget))
				{
					// Ignore any special attacks that are buffs
					// Abuse of the safety zone check variable (maybe rename?)
					if (bCheckSafetyZone)
					{
						CString strMessage;
						strMessage.Format(IDS_USER_CANNOT_PK_LEVEL_BELOW, LEVEL_CAN_PK);
						SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
						continue;
					}
				}

				// Check for safety zone
				if (bCheckSafetyZone &&
					(CheckSafetyZone() || pTarget->CheckSafetyZone()))
				{
					continue;
				}

				// TODO : Maybe move this Live check on the target closer to the first check to be made.
				if (!pTarget->m_bLive ||
					pTarget->m_State != STATE_GAMESTARTED ||
					pTarget->m_bHidden ||
					pTarget->m_bIsWarping)
				{
					continue;
				}

				// Check can cast SpecialAttack on the target
				if (!CheckSuccessSpecialAttack(pSpecial, pTarget))
				{
					continue;
				}

				if (bCheckAttackSuccess && !CheckAttackSuccess(pTarget))
				{
					continue;
				}

				// Remove the target from the location on map if for some reason they should no longer be there
				if (sX != pTarget->m_sX || sY != pTarget->m_sY)
				{
					g_Zones[m_iZoneIndex]->m_pMap[sX][sY].m_lUser = 0;
					continue;
				}

				iTargetsId[iTargetCount] = iTargetId;

				TRACE("SpecialAttackCircle - Found USER Target: %d\n", iTargetsId[iTargetCount]);
				++iTargetCount;
			}
			else if (iTargetId >= NPC_BAND)						// NPC
			{
				CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
				if (!pNpc) continue;

				// Check if the npc is able to die.
				// This allows for preventing attack of a peaceful npc.
				if (pNpc->m_tNpcType == NPCTYPE_NPC) continue;

				if (pNpc->m_NpcState == NPC_DEAD || pNpc->m_NpcState == NPC_LIVE) continue;

				// Guards cannot be attacked by players when they are in battle mode (PK mode allows attack).
				if (pNpc->IsDetecter() && m_BattleMode == BATTLEMODE_ATTACK) continue;

				// Clear any existing special attacks being used
				InitRemainSpecial();

				// If AI Type of npc is NPCAITYPE_NORMAL or NPCAITYPE_STONE_GUARD then do something with the AttackList of the npc oO
				// Need to look at this in more detail before making an implementation.
				if (pNpc->m_tNpcAttType == NPC_AT_PASSIVE && pSpecial->m_sDamage > 0)
				{
					if (pNpc->CheckAIType(NPCAITYPE_NORMAL) || pNpc->CheckAIType(NPCAITYPE_STONE_GUARD))
					{
						pNpc->AttackListAdd(this);
					}
				}

				// Set the NPC target
				// Stones and Guard NPC types cannot have a target set
				// Target is only set if NPC state is standing, moving or attacking.
				if ((pNpc->m_NpcState == NPC_STANDING ||
					pNpc->m_NpcState == NPC_MOVING ||
					pNpc->m_NpcState == NPC_FIGHTING) &&
					!pNpc->IsDetecter() &&
					!pNpc->IsStone())
				{
					pNpc->m_iAttackedUid = m_Uid + USER_BAND;
					pNpc->m_NpcState = NPC_ATTACKING;
				}

				// Remove the target from the location on map if for some reason they should no longer be there
				if (sX != pNpc->m_sCurX || sY != pNpc->m_sCurY)
				{
					g_Zones[m_iZoneIndex]->m_pMap[sX][sY].m_lUser = 0;
					continue;
				}

				if (bCheckAttackSuccess && !CheckAttackSuccess(pNpc))
				{
					continue;
				}

				iTargetsId[iTargetCount] = iTargetId;

				TRACE("SpecialAttackCircle - Found NPC Target: %d\n", iTargetsId[iTargetCount]);
				++iTargetCount;
			}

			if (iTargetCount >= iMaxTarget)
			{
				goto MaximumTargetReached;
			}
		}
	}

MaximumTargetReached:
	// Make sure there are targets before continuing
	if (iTargetCount <= 0)
	{
		SendSpecialAttackCircle(FAIL);
		return;
	}

	int iContinueTime = pSpecial->m_iTime / 1000;

	for (int i = 0; i < iTargetCount; i++)
	{
		m_dwLastSpecialTime = GetTickCount();

		int iTargetId = iTargetsId[i];

		if (iTargetId >= USER_BAND && iTargetId < NPC_BAND)	// USER
		{
			USER* pTarget = GetUser(iTargetId - USER_BAND);
			if (!pTarget)
			{
				SendSpecialAttackCircle(FAIL);
				return;
			}

			switch (pSpecial->m_tEValue)
			{
			case SPECIAL_EFFECT_DOUBLEATTACK: // Fall through
			case SPECIAL_EFFECT_PIERCE:
				{
				// Calculate the damage to be dealt
				int iDefense = pTarget->GetDefense();
				int iAttack = GetAttack();

				int iDamage = iAttack - iDefense;

				int iDefenseHuman = pTarget->GetDefenseHuman(this);
				iDamage -= iDefenseHuman;

				int iAttackHuman = GetAttackHuman(pTarget);
				iDamage += iAttackHuman;

				if (iDamage <= 0)
				{
					iDamage = myrand(0, 2);
				}
				iDamage +=  static_cast<int>(((static_cast<double>(myrand(90, 110)) * pSpecial->m_sDamage) / 100.0) + 0.5);

				// Check if the target can be pked
				bool bCanPKTarget = CheckPK(pTarget);

				// Do the damage!
				pTarget->SetDamage(iDamage, m_sClass);

				// If the target died check for any gained pos
				if (pTarget->m_lDeadUsed == 1)
				{
					CheckClassPoint(pTarget);
				}

				// Do extra things if the user pked another user
				if (bCanPKTarget)
				{
					if (pTarget->m_lDeadUsed == 1)
					{
						if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
							(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
							(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
						{
							if (SetMoralByPK(pTarget->m_iMoral))
							{
								SendCharData(INFO_BASICVALUE);
								SendMyInfo(TO_INSIGHT, INFO_MODIFY);
							}
						}
					}

					// Lets any guards know who are within area that a user has attacked another user
					IsDetecterRange();

					// Sets the user gray if they pass the checks
					if (CheckGray(pTarget))
					{
						SetGrayUser(GRAY_MODE_NORMAL);
					}
				}
				}
				break;
			case SPECIAL_EFFECT_HPREGEN: // Fall through
			case SPECIAL_EFFECT_INCREASEATTACK:
			case SPECIAL_EFFECT_DEFENSE:
			case SPECIAL_EFFECT_SPEED:
			case SPECIAL_EFFECT_DODGE:
			case SPECIAL_EFFECT_MPREGEN:
			case SPECIAL_EFFECT_RAPIDCAST:
			case SPECIAL_EFFECT_MPSAVE:
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].sMid = pSpecial->m_sMid;
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].sDamage = pSpecial->m_sDamage;
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].dwTime = GetTickCount();
				break;
			case SPECIAL_EFFECT_HPDRAIN:
				{
				// Calculate the damage to be dealt
				int iDefense = pTarget->GetDefense();
				int iAttack = GetAttack();

				int iDamage = iAttack - iDefense;

				int iDefenseHuman = pTarget->GetDefenseHuman(this);
				iDamage -= iDefenseHuman;

				int iAttackHuman = GetAttackHuman(pTarget);
				iDamage += iAttackHuman;

				if (iDamage <= 0)
				{
					iDamage = myrand(0, 2);
				}

				int iHpDrain = (pSpecial->m_sDamage * iDamage) / 100;
				if (iHpDrain > pTarget->m_sHP)
				{
					iHpDrain = pTarget->m_sHP;
				}
				pTarget->m_sHP -= iHpDrain;

				m_sHP += iHpDrain;
				if (m_sHP > GetMaxHP())
				{
					m_sHP = GetMaxHP();
				}

				// Check if the target can be pked
				bool bCanPKTarget = CheckPK(pTarget);

				// Do the damage!
				pTarget->SetDamage(iDamage, m_sClass);

				// If the target died check for any gained pos
				if (pTarget->m_lDeadUsed == 1)
				{
					CheckClassPoint(pTarget);
				}

				// Do extra things if the user pked another user
				if (bCanPKTarget)
				{
					if (pTarget->m_lDeadUsed == 1)
					{
						if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
							(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
							(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
						{
							if (SetMoralByPK(pTarget->m_iMoral))
							{
								SendCharData(INFO_BASICVALUE);
								SendMyInfo(TO_INSIGHT, INFO_MODIFY);
							}
						}
					}

					// Lets any guards know who are within area that a user has attacked another user
					IsDetecterRange();

					// Sets the user gray if they pass the checks
					if (CheckGray(pTarget))
					{
						SetGrayUser(GRAY_MODE_NORMAL);
					}
				}
				}
				break;
			case SPECIAL_EFFECT_PARALYZE: // Fall through
			case SPECIAL_EFFECT_CONCUSSION:
				{
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].sMid = pSpecial->m_sMid;
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].sDamage = CalcStopTime(m_sLevel, pTarget->m_sLevel);
				pTarget->m_RemainSpecial[pSpecial->m_tEValue].dwTime = GetTickCount();
				iContinueTime = pTarget->m_RemainSpecial[pSpecial->m_tEValue].sDamage;

				if (CheckPK(pTarget))
				{
					// Lets any guards know who are within area that a user has attacked another user
					IsDetecterRange();

					// Sets the user gray if they pass the checks
					if (CheckGray(pTarget))
					{
						SetGrayUser(GRAY_MODE_NORMAL);
					}
				}
				}
				break;
			case SPECIAL_EFFECT_AREAMULTIPLEATTACK: // Fall through
			case SPECIAL_EFFECT_MULTIPLEATTACK:
			case SPECIAL_EFFECT_MASTERMULTIPLEATTACK:
				{
				// Calculate the damage to be dealt
				int iDefense = pTarget->GetDefense();
				int iAttack = GetAttack();

				int iDamage = iAttack - iDefense;

				int iDefenseHuman = pTarget->GetDefenseHuman(this);
				iDamage -= iDefenseHuman;

				int iAttackHuman = GetAttackHuman(pTarget);
				iDamage += iAttackHuman;

				if (iDamage <= 0)
				{
					iDamage = myrand(0, 2);
				}

				// Check if the target can be pked
				bool bCanPKTarget = CheckPK(pTarget);

				// Do the damage!
				pTarget->SetDamage(iDamage, m_sClass);

				// If the target died check for any gained pos
				if (pTarget->m_lDeadUsed == 1)
				{
					CheckClassPoint(pTarget);
				}

				// Do extra things if the user pked another user
				if (bCanPKTarget)
				{
					if (pTarget->m_lDeadUsed == 1)
					{
						if ((!pTarget->IsMapRange(MAP_AREA_HSOMA_GV) && !IsMapRange(MAP_AREA_HSOMA_GV)) &&
							(!pTarget->IsMapRange(MAP_AREA_DSOMA_GV) && !IsMapRange(MAP_AREA_DSOMA_GV)) &&
							(!pTarget->IsMapRange(MAP_AREA_DUEL) && !IsMapRange(MAP_AREA_DUEL)))
						{
							if (SetMoralByPK(pTarget->m_iMoral))
							{
								SendCharData(INFO_BASICVALUE);
								SendMyInfo(TO_INSIGHT, INFO_MODIFY);
							}
						}
					}

					// Lets any guards know who are within area that a user has attacked another user
					IsDetecterRange();

					// Sets the user gray if they pass the checks
					if (CheckGray(pTarget))
					{
						SetGrayUser(GRAY_MODE_NORMAL);
					}
				}
				}
				break;
			}

			// If either of users are not within the arena
			// stats and skill can be gained! (Needs checking!!!!!)
			if (!IsMapRange(MAP_AREA_DUEL) || !pTarget->IsMapRange(MAP_AREA_DUEL))
			{
				if ((m_Uid == pTarget->m_Uid && CheckPlusType(pSpecial, 4)) ||
					(m_Uid != pTarget->m_Uid && CheckPlusType(pSpecial, 5)))
				{
					// Increase weapon skill exp
					int iWeaponExpPercent = pSpecial->m_sPlusExpRate;
					PlusWeaponExpByAttack(iWeaponExpPercent);

					// Increase stats
					if (!CheckDemon(m_sClass))
					{
						PlusStr(iSTRInc);
						PlusDex(iDEXInc);
						PlusInt(iINTInc);
						PlusWis(iWISInc);
						PlusCon(iCONInc);
					}

					m_iSTRInc += iSTRInc;
					m_iDEXInc += iDEXInc;
					m_iINTInc += iINTInc;
					m_iWISInc += iWISInc;
					m_iCONInc += iCONInc;
				}
			}

			sTargetsHP[i] = pTarget->m_sHP;
		}
		else if (iTargetId >= NPC_BAND)						// NPC
		{
			CNpc* pNpc = GetNpc(iTargetId - NPC_BAND);
			if (!pNpc)
			{
				SendSpecialAttackCircle(FAIL);
				return;
			}

			int iDamage = 0;
			bool bNpcDied = false;
			switch (pSpecial->m_tEValue)
			{
			case SPECIAL_EFFECT_DOUBLEATTACK: // Fall through
			case SPECIAL_EFFECT_PIERCE:
				{
				int iDefense = pNpc->GetDefense();
				int iAttack = GetAttack();
				iDamage = iAttack - iDefense;
				if (iDamage <= 0)
				{
					iDamage = myrand(0, 2);
				}
				iDamage +=  static_cast<int>(((static_cast<double>(myrand(90, 110)) * pSpecial->m_sDamage) / 100.0) + 0.5);

				if (iDamage > pNpc->m_sHP)
				{
					iDamage = pNpc->m_sHP;
				}

				// Do the damage!
				pNpc->m_sHP -= iDamage;

				if (pNpc->CheckAIType(NPCAITYPE_DUMMY)) // Dummy
				{
					pNpc->m_sHP = pNpc->m_sMaxHp;
					SendDummyAttack(iTargetId, iDamage);
				}

				// Do stuff if the npc has died
				if (pNpc->m_sHP <= 0)
				{
					NpcDeadEvent(pNpc);
					pNpc->SetDead(m_pCom, m_Uid, true);
					bNpcDied = true;
					PlusMoralByNpc(pNpc->m_iMoral);
					if (CheckPartyLeader())
					{
						int iPlus = GetInc(INC_CHA);
						iPlus *= pNpc->m_sCharm;
						if (!CheckDemon(m_sClass))
						{
							PlusCha(iPlus);
						}
						m_iCHAInc += iPlus;
					}
					NpcThrowItem(pNpc);
					NpcThrowMoney(pNpc);
				}

				DWORD dwGainedExp = static_cast<DWORD>(((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) * 100.0);
				m_dwExp += dwGainedExp;

				CString str;
				str.Format(IDS_GM_MSG_ATTACK_INFO, iDamage, dwGainedExp);
				SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

				if (m_dwExp > m_dwMaxExp)
				{
					CheckLevelUp();
				}
				}
				break;
			case SPECIAL_EFFECT_HPDRAIN:
				{
				int iDefense = pNpc->GetDefense();
				int iAttack = GetAttack();
				iDamage = iAttack - iDefense;
				if (iDamage <= 0)
				{
					iDamage = myrand(0, 2);
				}

				if (pNpc->CheckAIType(NPCAITYPE_DUMMY)) // Dummy
				{
					pNpc->m_sHP = pNpc->m_sMaxHp;
					SendDummyAttack(iTargetId, iDamage);
				}

				int iHpDrain = (pSpecial->m_sDamage * iDamage) / 100;
				if (iHpDrain > pNpc->m_sHP)
				{
					iHpDrain = pNpc->m_sHP;
				}
				pNpc->m_sHP -= iHpDrain;

				m_sHP += iHpDrain;
				if (m_sHP > GetMaxHP())
				{
					m_sHP = GetMaxHP();
				}

				// Do stuff if the npc has died
				if (pNpc->m_sHP <= 0)
				{
					NpcDeadEvent(pNpc);
					pNpc->SetDead(m_pCom, m_Uid, true);
					bNpcDied = true;
					PlusMoralByNpc(pNpc->m_iMoral);
					if (CheckPartyLeader())
					{
						int iPlus = GetInc(INC_CHA);
						iPlus *= pNpc->m_sCharm;
						if (!CheckDemon(m_sClass))
						{
							PlusCha(iPlus);
						}
						m_iCHAInc += iPlus;
					}
					NpcThrowItem(pNpc);
					NpcThrowMoney(pNpc);
				}

				DWORD dwGainedExp = static_cast<DWORD>(((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) * 100.0);
				m_dwExp += dwGainedExp;

				CString str;
				str.Format(IDS_GM_MSG_ATTACK_INFO, iDamage, dwGainedExp);
				SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

				if (m_dwExp > m_dwMaxExp)
				{
					CheckLevelUp();
				}
				}
				break;
			case SPECIAL_EFFECT_PARALYZE:
			case SPECIAL_EFFECT_CONCUSSION:
				pNpc->m_sRemainSpecialNo[pSpecial->m_tEValue] = pSpecial->m_sMid;
				pNpc->m_dwRemainSpecialTime[pSpecial->m_tEValue] = GetTickCount();
				iContinueTime = pNpc->m_sCon;
				break;
			case SPECIAL_EFFECT_AREAMULTIPLEATTACK: // Fall through
			case SPECIAL_EFFECT_MULTIPLEATTACK: // Fall through
			case SPECIAL_EFFECT_MASTERMULTIPLEATTACK:
				{
				int iDefense = pNpc->GetDefense();
				int iAttack = GetAttack();
				iDamage = iAttack - iDefense;
				if (iDamage <= 0)
				{
					iDamage = myrand(0, 2);
				}

				if (pSpecial->m_tEValue == SPECIAL_EFFECT_MASTERMULTIPLEATTACK)
				{
					iDamage = static_cast<int>(iDamage * 1.5);
				}

				if (iDamage > pNpc->m_sHP)
				{
					iDamage = pNpc->m_sHP;
				}

				// Do the damage!
				pNpc->m_sHP -= iDamage;

				if (pNpc->CheckAIType(NPCAITYPE_DUMMY)) // Dummy
				{
					pNpc->m_sHP = pNpc->m_sMaxHp;
					SendDummyAttack(iTargetId, iDamage);
				}

				// Do stuff if the npc has died
				if (pNpc->m_sHP <= 0)
				{
					NpcDeadEvent(pNpc);
					pNpc->SetDead(m_pCom, m_Uid, true);
					bNpcDied = true;
					PlusMoralByNpc(pNpc->m_iMoral);
					if (CheckPartyLeader())
					{
						int iPlus = GetInc(INC_CHA);
						iPlus *= pNpc->m_sCharm;
						if (!CheckDemon(m_sClass))
						{
							PlusCha(iPlus);
						}
						m_iCHAInc += iPlus;
					}
					NpcThrowItem(pNpc);
					NpcThrowMoney(pNpc);
				}

				DWORD dwGainedExp = static_cast<DWORD>(((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) * 100.0);
				m_dwExp += dwGainedExp;

				CString str;
				str.Format(IDS_GM_MSG_ATTACK_INFO, iDamage, dwGainedExp);
				SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

				if (m_dwExp > m_dwMaxExp)
				{
					CheckLevelUp();
				}
				}
				break;
			}

			if (pNpc->m_tNpcAttType == NPC_AT_PASSIVE && pSpecial->m_sDamage > 0)
			{
				if (pNpc->CheckAIType(NPCAITYPE_NORMAL) || pNpc->CheckAIType(NPCAITYPE_STONE_GUARD))
				{
					pNpc->AttackListAdd(this);
				}
			}

			// Set the NPC target
			// Stones and Guard NPC types cannot have a target set
			// Target is only set if NPC state is standing, moving or attacking.
			if ((pNpc->m_NpcState == NPC_STANDING ||
				pNpc->m_NpcState == NPC_MOVING ||
				pNpc->m_NpcState == NPC_FIGHTING) &&
				!pNpc->IsDetecter() &&
				!pNpc->IsStone())
			{
				pNpc->m_iAttackedUid = m_Uid + USER_BAND;
				pNpc->m_NpcState = NPC_ATTACKING;
			}

			// Checks for stone guard and normal guards decreasing moral
			// Moral is decreased by 100000.
			if (pNpc->IsDetecter() || pNpc->IsStone())
			{
				bool bSkip = false;
				for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
				{
					if (g_bGuildTownWar[i] && pNpc->m_sGuild == g_arGuildTownData[i]->sTownNum)
					{
						bSkip = true;
						break;
					}
				}

				if (!bSkip && (CheckDemon(m_sClass) == CheckDemon(pNpc->m_sClass)))
				{
					bool bRet = true;
					if (!CheckDemon(m_sClass))
					{
						bRet = SetMoral(-100000);
					}
					else
					{
						bRet = SetMoral(100000);
					}

					if (bRet)
					{
						SendCharData(INFO_BASICVALUE);
						SendMyInfo(TO_INSIGHT, INFO_MODIFY);
					}
				}
			}
			else if (pNpc->CheckAIType(NPCAITYPE_NORMAL) && CheckPlusType(pSpecial, 6)) // Normal AI
			{
				PlusWeaponExpByAttack(pSpecial->m_sPlusExpRate);

				// Con for hsoma is increased seperately
				if (!CheckDemon(m_sClass))
				{
					PlusCon(iCONInc);
					m_iCONInc += iCONInc;
				}

				// Increase following stats by their inc value
				// STR, DEX, INT and WIS (and CON if dsoma)
				// GetPlusValueCount calculates how many times we add the inc value.
				int PlusValueCount = GetPlusValueCount(pNpc, iDamage, bNpcDied, INC_STR);
				for (int i = 0; i < PlusValueCount; i++)
				{
					if (!CheckDemon(m_sClass))
					{
						PlusStr(iSTRInc);
						PlusDex(iDEXInc);
						PlusInt(iINTInc);
						PlusWis(iWISInc);
					}

					m_iSTRInc += iSTRInc;
					m_iDEXInc += iDEXInc;
					m_iINTInc += iINTInc;
					m_iWISInc += iWISInc;

					if (CheckDemon(m_sClass))
					{
						m_iCONInc += iCONInc;
					}
				}
			}

			sTargetsHP[i] = pNpc->m_sHP;
		}
	}

	m_sHP -= pSpecial->m_sHpdec;
	if (m_sHP <= 0) m_sHP = 1;

	m_sMP -= pSpecial->m_sMpdec;
	if (m_sMP < 0) m_sMP = 0;

	m_sStm -= pSpecial->m_sStmdec;
	if (m_sStm < 0) m_sStm = 0;

	SendSpecialAttackCircle(SUCCESS, iTargetCount, iTargetsId, sSpecialNo, sTargetsHP, iContinueTime);
}

void USER::HairShopOK(TCHAR* pBuf)
{
	int index = 0;
	short sHair = GetShort(pBuf, index);
	short sHairMode = GetShort(pBuf, index);
	BYTE byStyle = GetByte(pBuf, index);
	BYTE byColor = GetByte(pBuf, index);

	if (byStyle < 0 || byStyle > 2) return;
	if (byColor < 0 || byColor > 2) return;

	int iStylePrice = 0;
	if (byStyle > 0) iStylePrice = m_iHairShopPrices[byStyle-1];

	int iColorPrice = 0;
	if (byColor > 0) iColorPrice = m_iHairShopPrices[byColor+1];

	DWORD dwTotalPrice =  static_cast<DWORD>(iStylePrice) + iColorPrice;
	if (m_dwBarr < dwTotalPrice)
	{
		SendServerChatMessage(IDS_BUY_NOT_ENOUGH_BARR, TO_ME);
	}
	else
	{
		m_dwBarr -= dwTotalPrice;
		m_sHair = sHair;
		m_sHairMode = sHairMode;
		SendMoneyChanged();
		SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	}
}

void USER::GuildWarReqResult(TCHAR* pBuf)
{
	int index = 0;
	BYTE byResult = GetByte(pBuf, index);
	TCHAR strOtherGuildName[GUILD_NAME_LENGTH+1] = {0};

	// Retrieve the name of other guild involved in the war
	int iLen = GetVarString(sizeof(strOtherGuildName), strOtherGuildName, pBuf, index);
	if (iLen <= 0 || iLen > GUILD_NAME_LENGTH)
	{
		return;
	}

	Guild* pOtherGuild = GetGuildByName(strOtherGuildName);
	if (pOtherGuild == NULL)
	{
		ReleaseGuild();
		return;
	}

	int iUserIndex = pOtherGuild->GetUserByRank(Guild::GUILD_CHIEF);
	if (iUserIndex == -1)
	{
		ReleaseGuild();
		return;
	}

	USER* pOtherGuildLeader = GetUserId(pOtherGuild->m_arMembers[iUserIndex].m_strUserId);
	if (pOtherGuildLeader == NULL || pOtherGuildLeader->m_State != STATE_GAMESTARTED)
	{
		ReleaseGuild();
		return;
	}

	ReleaseGuild();

	if (byResult == 0x01)
	{
		m_bGuildWar = true;
		m_bGuildWarOk = false;
		m_iGuildWarUid = pOtherGuildLeader->m_Uid;
		strcpy(m_strGuildWarUser, pOtherGuildLeader->m_strUserId);
		pOtherGuildLeader->m_bGuildWar = true;
		pOtherGuildLeader->m_bGuildWarOk = false;
		pOtherGuildLeader->m_iGuildWarUid = m_Uid;
		strcpy(pOtherGuildLeader->m_strGuildWarUser, m_strUserId);

		// TODO: Guild Storage stuff
		// TODO: Update guild state in database (although i think it aint needed, unless want to restore a war? oO)
		Guild* pGuild = GetGuild(m_sGroup);
		if (pGuild == NULL)
		{
			ReleaseGuild();
			return;
		}
		pGuild->m_sState = pOtherGuildLeader->m_sGroup;
		ReleaseGuild();

		pGuild = GetGuild(pOtherGuildLeader->m_sGroup);
		if (pGuild == NULL)
		{
			ReleaseGuild();
			return;
		}
		pGuild->m_sState = m_sGroup;
		ReleaseGuild();

		index = 0;
		SetByte(m_TempBuf, PKT_GUILDWAR_REQ_RESULT, index);
		SetByte(m_TempBuf, SUCCESS, index);
		SetInt(m_TempBuf, 0, index);
		SetShort(m_TempBuf, 0, index);
		Send(m_TempBuf, index);

		index = 0;
		SetByte(m_TempBuf, PKT_GUILDWAR_REQ_RESULT, index);
		SetByte(m_TempBuf, SUCCESS, index);
		SetInt(m_TempBuf, 0, index);
		SetShort(m_TempBuf, 0, index);
		pOtherGuildLeader->Send(m_TempBuf, index);
	}
	else if (byResult == 0x02)
	{
		m_bGuildWar = false;
		pOtherGuildLeader->m_bGuildWar = false;

		index = 0;
		SetByte(m_TempBuf, PKT_GUILDWAR_REQ_RESULT, index);
		SetByte(m_TempBuf, FAIL, index);
		SetVarString(m_TempBuf, m_strGuildName, strlen(m_strGuildName), index);
		pOtherGuildLeader->Send(m_TempBuf, index);
	}
	else
	{
		index = 0;
		SetByte(m_TempBuf, PKT_GUILDWAR_REQ_RESULT, index);
		SetByte(m_TempBuf, byResult, index);
		SetVarString(m_TempBuf, m_strGuildName, strlen(m_strGuildName), index);
		pOtherGuildLeader->Send(m_TempBuf, index);
	}
}

void USER::GuildWarReqCancel(TCHAR* pBuf)
{
	int index = 0;
	TCHAR strOtherGuildName[GUILD_NAME_LENGTH+1] = {0};

	// Retrieve the name of other guild involved in the war
	int iLen = GetVarString(sizeof(strOtherGuildName), strOtherGuildName, pBuf, index);
	if (iLen <= 0 || iLen > GUILD_NAME_LENGTH)
	{
		return;
	}

	Guild* pOtherGuild = GetGuildByName(strOtherGuildName);
	if (pOtherGuild == NULL)
	{
		ReleaseGuild();
		return;
	}

	int iUserIndex = pOtherGuild->GetUserByRank(Guild::GUILD_CHIEF);
	if (iUserIndex == -1)
	{
		ReleaseGuild();
		return;
	}

	USER* pOtherGuildLeader = GetUserId(pOtherGuild->m_arMembers[iUserIndex].m_strUserId);
	if (pOtherGuildLeader == NULL || pOtherGuildLeader->m_State != STATE_GAMESTARTED)
	{
		ReleaseGuild();
		return;
	}

	// TODO: UpdateGuildState and ReleaseGuildStorage

	pOtherGuild->m_sState = -1;
	ReleaseGuild();

	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		return;
	}
	pGuild->m_sState = -1;
	ReleaseGuild();

	m_bGuildWar = false;
	pOtherGuildLeader->m_bGuildWar = false;

	index = 0;
	SetByte(m_TempBuf, PKT_GUILDWAR_REQ_CANCEL, index);
	pOtherGuildLeader->Send(m_TempBuf, index);
}

void USER::GuildWarOK()
{
	USER* pOtherGuildLeader = GetUser(m_iGuildWarUid);
	if (pOtherGuildLeader == NULL) return;
	if (pOtherGuildLeader->m_Uid == m_Uid) return;
	if (strcmp(pOtherGuildLeader->m_strUserId, m_strGuildWarUser) != 0) return;

	m_bGuildWarOk = true;

	int index = 0;
	SetByte(m_TempBuf, PKT_GUILDWAR_OK, index);

	if (!pOtherGuildLeader->m_bGuildWarOk)
	{
		SetByte(m_TempBuf, 0x01, index);
		pOtherGuildLeader->Send(m_TempBuf, index);
		return;
	}
	else
	{
		SetByte(m_TempBuf, 0x02, index);
		Send(m_TempBuf, index);
		pOtherGuildLeader->Send(m_TempBuf, index);
	}

	// Reach here when both guilds are ready to start the war
	pOtherGuildLeader->m_sGuildWar = m_sGroup;
	pOtherGuildLeader->m_bGuildWar = false;
	m_sGuildWar = pOtherGuildLeader->m_sGroup;
	m_bGuildWar = false;
	// TODO: SetGuildWarData
	// TODO: ReleaseGuildStorage

	// FIXME This could be improved!!!!!
	for (int i = 0; i< MAX_USER; i++)
	{
		USER *pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) continue;
		if (pUser->m_sGroup == m_sGroup)
		{
			pUser->m_bInGuildWar = true;
			pUser->m_bGuildWarDead = false;
			pUser->m_sGuildWar = m_sGuildWar;
			pUser->SendMyInfo(TO_INSIGHT, INFO_MODIFY);

			// Guild leaders cannot run during the war
			if (pUser->m_sGuildRank == Guild::GUILD_CHIEF)
			{
				TCHAR bRunMode[1];
				bRunMode[0] = 0;
				pUser->SetRunModeReq(bRunMode);
			}
		}
		else if (pUser->m_sGroup == pOtherGuildLeader->m_sGroup)
		{
			pUser->m_bInGuildWar = true;
			pUser->m_bGuildWarDead = false;
			pUser->m_sGuildWar = m_sGroup;
			pUser->SendMyInfo(TO_INSIGHT, INFO_MODIFY);

			// Guild leaders cannot run during the war
			if (pUser->m_sGuildRank == Guild::GUILD_CHIEF)
			{
				TCHAR bRunMode[1];
				bRunMode[0] = 0;
				pUser->SetRunModeReq(bRunMode);
			}
		}

		CString str;
		str.Format(IDS_GUILDWAR_START, m_strGuildName, pOtherGuildLeader->m_strGuildName);
		pUser->SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}
}

void USER::GuildStorageOpen()
{
	// TODO: When guild war implemented block usage of the storage while in a war

	if (m_bTrading) return;

	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		return;
	}

	if (pGuild->m_lUsed == 1)
	{
		int nLen = strlen(pGuild->m_strUsedUser);
		if (nLen <= 0)
		{
			pGuild->m_lUsed = 0;
		}

		if (nLen > 0)
		{
			USER* pUser = GetUserId(pGuild->m_strUsedUser);
			if (pUser == NULL || strcmp(pUser->m_strUserId, pGuild->m_strUsedUser) != 0)
			{
				pGuild->m_iUsedUid = -1;
				ZeroMemory(pGuild->m_strUsedUser, sizeof(pGuild->m_strUsedUser));
				pGuild->m_lUsed = 0;
			}
		}

		CString str;
		str.Format(IDS_GUILD_STORAGE_IN_USE, pGuild->m_strUsedUser);
		SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
		ReleaseGuild();
		return;
	}

	int nLen = strlen(m_strUserId);
	if (nLen <= 0 || nLen > NAME_LENGTH)
	{
		ReleaseGuild();
		return;
	}

	for (int i = 0; i < GUILD_STORAGE_ITEM_NUM; i++)
	{
		m_GuildItem[i].Init();
	}

	if (!LoadGuildStorageData(pGuild->m_strGuildName))
	{
		ReleaseGuild();
		return;
	}

	pGuild->m_lUsed = 1;
	pGuild->m_iUsedUid = m_Uid;
	strncpy(pGuild->m_strUsedUser, m_strUserId, nLen);

	CByteArray arItemSlotList;
	for (int i = 0; i < GUILD_STORAGE_ITEM_NUM; i++)
	{
		if (m_GuildItem[i].sUsage > 0 && m_GuildItem[i].sNum > 0)
		{
			arItemSlotList.Add(i);
		}
	}

	CBufferEx TempBuf;
	TempBuf.Add(PKT_GUILD_STORAGEOPEN);
	TempBuf.Add((BYTE)pGuild->m_sStorageOpenType);
	TempBuf.Add((short)arItemSlotList.GetSize());
	for (int i = 0; i < arItemSlotList.GetSize(); i++)
	{
		TCHAR pData[150];
		short tempSlot = 0;
		tempSlot = arItemSlotList[i];
		TempBuf.Add(tempSlot);
		TempBuf.AddData(pData, GetSendItemData(m_GuildItem[tempSlot], pData, ITEM_TYPE_STORAGE));
	}
	Send(TempBuf, TempBuf.GetLength());

	ReleaseGuild();
}

void USER::GuildStorageClose()
{
	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild)
	{
		if (strcmp(pGuild->m_strUsedUser, m_strUserId) == 0)
		{
			pGuild->m_iUsedUid = -1;
			ZeroMemory(pGuild->m_strUsedUser, sizeof(pGuild->m_strUsedUser));
			pGuild->m_lUsed = 0;
		}
	}
	ReleaseGuild();
}

void USER::GuildSaveItem(TCHAR* pBuf)
{
	int index = 0, i;
	short sSlot = GetShort(pBuf, index);
	short sItemCount = GetShort(pBuf, index);
	short sMaxStorageCount = 0, sBankSlot = -1, sFoundSameSlot = -1, sFoundEmptySlot = -1, sItemCountRemain = 0;
	CItemTable* pItem = NULL;
	CBufferEx TempBuf;
	ItemList InventoryItem, StorageItem;
	ItemList BackupSourceItem, BackupDestItem;


	// Verify that the user can put items into the guild storage
	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	int nLen = strlen(pGuild->m_strUsedUser);
	if (nLen <= 0 || nLen > NAME_LENGTH)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	if (strcmp(m_strUserId, pGuild->m_strUsedUser) != 0)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	if (strcmp(m_strGuildName, pGuild->m_strGuildName) != 0)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	if (pGuild->m_lUsed == 0)
	{
		pGuild->m_iUsedUid = -1;
		ZeroMemory(pGuild->m_strUsedUser, sizeof(pGuild->m_strUsedUser));
		ReleaseGuild();
		goto send_fail_;
	}
	ReleaseGuild();

	if (m_bTrading) goto send_fail_;

	if (sSlot < 0 || sSlot >= INV_ITEM_NUM) goto send_fail_;
	if (sItemCount <= 0 || sItemCount > MAX_ITEM_DURA) goto send_fail_;
	if (!g_mapItemTable.Lookup(m_InvItem[sSlot].sNum, pItem)) goto send_fail_;
	if (m_InvItem[sSlot].IsEmpty()) goto send_fail_;
	if (!CanTradeItem(m_InvItem[sSlot]))
	{
		SendServerChatMessage(IDS_CANNOT_PLACE_GUILD_STORAGE, TO_ME);
		goto send_fail_;
	}

	BackupSourceItem = m_InvItem[sSlot];

	// NOTE: it is correct for these to be pointing to same slot in inventory
	// because they change later on in below code and both need same starting point.
	InventoryItem = m_InvItem[sSlot];
	StorageItem = m_InvItem[sSlot];

	if (InventoryItem.bType > TYPE_ACC) // Non equipment
	{
		if (InventoryItem.sUsage > sItemCount)
		{
			sItemCountRemain = InventoryItem.sUsage - sItemCount;
			PlusItemDur(&InventoryItem, sItemCount, true);
		}
		else if (InventoryItem.sUsage == sItemCount)
		{
			InventoryItem.Init();
		}
		else
		{
			goto send_fail_;
		}

		for (i = 0; i < GUILD_STORAGE_ITEM_NUM; i++)
		{
			if (m_GuildItem[i].sNum == StorageItem.sNum && m_GuildItem[i].sUsage < MAX_ITEM_DURA)
			{
				sFoundSameSlot = i;
				break;
			}
		}
		// Add feature to store left over items in a new slot..
		if (sFoundSameSlot >= 0)
		{
			// Take a backup of the guild item so it can be restored (if needed) to before any changes are made.
			BackupDestItem = m_GuildItem[sFoundSameSlot];

			if (m_GuildItem[sFoundSameSlot].sUsage + sItemCount > MAX_ITEM_DURA)
			{
				short sItemCountLeft = m_GuildItem[sFoundSameSlot].sUsage + sItemCount - MAX_ITEM_DURA;
				InventoryItem.sUsage += sItemCountLeft;
				sItemCountRemain += sItemCountLeft;
				m_GuildItem[sFoundSameSlot].sUsage = MAX_ITEM_DURA;
				sBankSlot = sFoundSameSlot;
			}
			else
			{
				PlusItemDur(&m_GuildItem[sFoundSameSlot], sItemCount);
				sBankSlot = sFoundSameSlot;
			}
		}
		else
		{
			StorageItem.sUsage = sItemCount;
		}
	}
	else
	{
		InventoryItem.Init();
	}

	if (sFoundSameSlot == -1) // Item either equipment or didn't find same item in storage
	{
		for (i = 0; i < GUILD_STORAGE_ITEM_NUM; i++)
		{
			if (m_GuildItem[i].IsEmpty())
			{
				sFoundEmptySlot = i;
				break;
			}
		}

		if (sFoundEmptySlot == -1)
		{
			SendServerChatMessage(IDS_GUILD_STORAGE_FULL, TO_ME);
			goto send_fail_;
		}
		else
		{
			sBankSlot = sFoundEmptySlot;
			m_GuildItem[sBankSlot] = StorageItem;
		}
	}

	m_InvItem[sSlot] = InventoryItem;

	if (!UpdateGuildStorage(m_strGuildName, sBankSlot, sSlot))
	{
		m_GuildItem[sBankSlot] = BackupDestItem;
		m_InvItem[sSlot] = BackupSourceItem;
		goto send_fail_;
	}

	TempBuf.Add(PKT_GUILD_SAVEITEM);
	TempBuf.Add(sSlot);
	TempBuf.Add(sItemCountRemain);
	TempBuf.Add(sBankSlot);
	TCHAR pData[150];
	TempBuf.AddData(pData, GetSendItemData(m_GuildItem[sBankSlot], pData, ITEM_TYPE_STORAGE));

	Send(TempBuf, TempBuf.GetLength());

	SendChangeWgt();

	return;

send_fail_:
	TempBuf.Add(PKT_GUILD_SAVEITEM);
	TempBuf.Add((short)-1);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::GuildTakeBackItem(TCHAR* pBuf)
{
	int index = 0;
	short sSlot = GetShort(pBuf, index);
	short sItemCount = GetShort(pBuf, index);
	short sMaxStorageCount = 0, sFoundSlot = -1, sItemCountRemain = 0, sWeight = 0;
	CItemTable* pItem = NULL;
	CBufferEx TempBuf;
	ItemList InventoryItem, StorageItem;
	ItemList BackupMyItem[INV_ITEM_NUM], BackupStorageItem;
	int iUserIndex = -1;

	if (m_bTrading) goto send_fail_;

	if (sSlot < 0 || sSlot >= STORAGE_ITEM_NUM) goto send_fail_;
	if (sItemCount <= 0 || sItemCount > MAX_ITEM_DURA) goto send_fail_;
	if (!g_mapItemTable.Lookup(m_GuildItem[sSlot].sNum, pItem)) goto send_fail_;
	if (m_GuildItem[sSlot].IsEmpty()) goto send_fail_;

	// Verify that the user can take items from the guild storage
	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	int nLen = strlen(pGuild->m_strUsedUser);
	if (nLen <= 0 || nLen > NAME_LENGTH)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	if (strcmp(m_strUserId, pGuild->m_strUsedUser) != 0)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	if (strcmp(m_strGuildName, pGuild->m_strGuildName) != 0)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	if (pGuild->m_lUsed == 0)
	{
		pGuild->m_iUsedUid = -1;
		ZeroMemory(pGuild->m_strUsedUser, sizeof(pGuild->m_strUsedUser));
		ReleaseGuild();
		goto send_fail_;
	}

	// Must have a guild rank or storage type must be open for all (No Limit)
	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	if (pGuild->m_sStorageOpenType == 1 && pGuild->m_arMembers[iUserIndex].m_sGuildRank >= Guild::GUILD_MEMBER)
	{
		ReleaseGuild();
		SendServerChatMessage(IDS_USER_NO_PERMISSION_USE, TO_ME);
		goto send_fail_;
	}
	ReleaseGuild();

	for (int i = 0; i < INV_ITEM_NUM; i++)
	{
		BackupMyItem[i] = m_InvItem[i];
	}

	BackupStorageItem = m_GuildItem[sSlot];
	StorageItem = m_GuildItem[sSlot];
	InventoryItem = m_GuildItem[sSlot];

	if (StorageItem.bType > TYPE_ACC) // Non equipment
	{
		sWeight = m_sWgt + (StorageItem.sWgt * sItemCount);
		if (sWeight > GetMaxWgt())
		{
			SendServerChatMessage(IDS_TAKESTORAGE_WEIGHT_FULL, TO_ME);
			goto send_fail_;
		}
		if (StorageItem.sUsage > sItemCount)
		{
			sItemCountRemain = StorageItem.sUsage - sItemCount;
			PlusItemDur(&StorageItem, sItemCount, true);
			InventoryItem.sUsage = sItemCount;
		}
		else if (StorageItem.sUsage == sItemCount)
		{
			StorageItem.Init();
		}
		else
		{
			goto send_fail_;
		}
	}
	else
	{
		sWeight = m_sWgt + StorageItem.sWgt;
		if (sWeight > GetMaxWgt())
		{
			SendServerChatMessage(IDS_TAKESTORAGE_WEIGHT_FULL, TO_ME);
			goto send_fail_;
		}

		StorageItem.Init();
	}

	sFoundSlot = PushItemInventory(&InventoryItem);
	if (sFoundSlot == -1)
	{
		goto send_fail_;
	}

	if (InventoryItem.sUsage > 0)
	{
		StorageItem = InventoryItem;
		sItemCountRemain = StorageItem.sUsage;
	}

	m_GuildItem[sSlot] = StorageItem;

	if (!UpdateGuildStorage(m_strGuildName, sSlot, sFoundSlot))
	{
		for (int i = 0; i < INV_ITEM_NUM; i++)
		{
			m_InvItem[i] = BackupMyItem[i];
		}

		m_GuildItem[sSlot] = BackupStorageItem;

		goto send_fail_;
	}

	TempBuf.Add(PKT_GUILD_TAKEBACKITEM);
	TempBuf.Add(sSlot);
	TempBuf.Add(sItemCountRemain);
	TempBuf.Add(sFoundSlot);

	TCHAR pData[150];
	TempBuf.AddData(pData, GetSendItemData(m_InvItem[sFoundSlot], pData, ITEM_TYPE_ME));

	Send(TempBuf, TempBuf.GetLength());

	SendChangeWgt();

	return;

send_fail_:
	TempBuf.Add(PKT_GUILD_TAKEBACKITEM);
	TempBuf.Add((short)-1);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::GuildSymbolReq()
{
	int index = 0;
	Guild* pGuild = NULL;
	int iUserIndex = -1;

	// Cannot do this if in a guild war.
	if (m_bInGuildWar) return;

	// Must be in a guild
	if (m_sGroup < 0 || m_sGroup >= MAX_GUILD)
	{
		goto send_fail_;
	}

	// Fetch the guild details
	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		m_sGroup = -1;
		goto send_fail_;
	}

	// Must be Guild::GUILD_CHIEF of the guild to do this
	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		goto send_fail_;
	}

	if (pGuild->m_arMembers[iUserIndex].m_sGuildRank != Guild::GUILD_CHIEF)
	{
		goto send_fail_;
	}

	ReleaseGuild();

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_SYMBOL_CHANGE_REQ, index);
	SetByte(m_TempBuf, SUCCESS, index);
	SetInt(m_TempBuf, GUILD_SYMBOL_BARR, index);
	Send(m_TempBuf, index);
	return;

send_fail_:
	ReleaseGuild();
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_SYMBOL_CHANGE_REQ, index);
	SetByte(m_TempBuf, FAIL, index);
	Send(m_TempBuf, index);
}

void USER::GuildSymbol(TCHAR* pBuf)
{
	int index = 0;
	Guild* pGuild = NULL;
	int iUserIndex = -1;
	TCHAR strFileName[20+1] = {0};
	int iSymbolFileNameLen = 0;
	short sSymbolFileSize = 0;

	// Cannot do this if in a guild war.
	if (m_bInGuildWar)
	{
		goto send_fail_;
	}

	// Retrieve the filename for the guild symbol (this will not be used on here)
	iSymbolFileNameLen = GetVarString(sizeof(strFileName), strFileName, pBuf, index);
	if (iSymbolFileNameLen <= 0 || iSymbolFileNameLen > 20)
	{
		goto send_fail_;
	}

	// +4 because client includes symbol version
	sSymbolFileSize = GetShort(pBuf, index);
	if (sSymbolFileSize != GUILD_SYMBOL_SIZE+4)
	{
		goto send_fail_;
	}

	// Fetch the guild details
	pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		ReleaseGuild();
		m_sGroup = -1;
		goto send_fail_;
	}

	// Must be Guild::GUILD_CHIEF of the guild to do this
	iUserIndex = pGuild->GetUser(m_strUserId);
	if (iUserIndex < 0)
	{
		ReleaseGuild();
		goto send_fail_;
	}
	if (pGuild->m_arMembers[iUserIndex].m_sGuildRank != Guild::GUILD_CHIEF)
	{
		ReleaseGuild();
		goto send_fail_;
	}

	DWORD dwGuildBarr = pGuild->m_dwBarr;
	if (dwGuildBarr < GUILD_SYMBOL_BARR)
	{
		ReleaseGuild();
		goto send_fail_;
	}
	dwGuildBarr -= GUILD_SYMBOL_BARR;
	if (!UpdateGuildMoney(pGuild->m_sNum, dwGuildBarr))
	{
		ReleaseGuild();
		goto send_fail_;
	}

	pGuild->m_dwBarr = dwGuildBarr;

	if (pGuild->m_sSymbolVersion > 0) CheckMaxValue((short &)pGuild->m_sSymbolVersion, (short)1);
	else pGuild->m_sSymbolVersion = 1;

	m_sGuildSymbolVersion = pGuild->m_sSymbolVersion;
	memcpy(pGuild->m_strSymbol, pBuf+index, GUILD_SYMBOL_SIZE);

	UpdateGuildSymbol(pGuild->m_sNum, pGuild->m_sSymbolVersion, pGuild->m_strSymbol);

	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_SYMBOL_CHANGE, index);
	SetByte(m_TempBuf, SUCCESS, index);
	SetInt(m_TempBuf, pGuild->m_dwBarr, index);
	Send(m_TempBuf, index);

	ReleaseGuild();

	for (int i = 0; i < MAX_USER; i++)
	{
		USER *pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) continue;
		if (m_sGroup == pUser->m_sGroup)
		{
			pUser->m_sGuildSymbolVersion = m_sGuildSymbolVersion;
			pUser->SendMyInfo(TO_ME, INFO_MODIFY);
		}
	}

	return;

send_fail_:
	index = 0;
	SetByte(m_TempBuf, PKT_GUILD_SYMBOL_CHANGE, index);
	SetByte(m_TempBuf, FAIL, index);
	Send(m_TempBuf, index);
	return;
}

void USER::GuildSymbolData(TCHAR* pBuf)
{
	int index = 0;
	short sGuildNum = GetShort(pBuf, index);
	// don't need sSymbolVersion = GetShort(pBuf, index);

	// Fetch the guild details
	Guild* pGuild = GetGuild(sGuildNum);
	if (pGuild && pGuild->m_sSymbolVersion > 0)
	{
		CString strFileName;
		strFileName.Format("g%04d%03d.obm", sGuildNum, 1);
		index = 0;
		SetByte(m_TempBuf, PKT_GUILD_SYMBOL_DATA, index);
		SetShort(m_TempBuf, pGuild->m_sNum, index);
		SetVarString(m_TempBuf, (LPTSTR)(LPCTSTR)strFileName, strFileName.GetLength(), index);
		SetShort(m_TempBuf, GUILD_SYMBOL_SIZE+4, index);
		memcpy(m_TempBuf+index, pGuild->m_strSymbol, GUILD_SYMBOL_SIZE);
		index+= GUILD_SYMBOL_SIZE;
		SetInt(m_TempBuf, pGuild->m_sSymbolVersion, index);
		Send(m_TempBuf, index);
	}
	ReleaseGuild();
}

void USER::ExchangeResult(TCHAR* pBuf)
{
	int index = 0;

	int iTradeUid = GetInt(pBuf, index);
	if (iTradeUid < 0 || iTradeUid >= NPC_BAND) return;
	if (iTradeUid - USER_BAND == m_Uid) return;

	USER* pTradeUser = GetUser(iTradeUid - USER_BAND);
	if (pTradeUser == NULL) return;
	if (!CheckDistance(pTradeUser, 2)) return;

	// Get and validate the name of character that going to trade with
	TCHAR strTradeCharName[NAME_LENGTH+1];
	int	nNameLen = GetVarString(sizeof(strTradeCharName), strTradeCharName, pBuf, index);
	if (nNameLen <= 0 || nNameLen > NAME_LENGTH || !strTradeCharName) return;

	// Name of player trading with must match that sent in the request
	if (_stricmp(pTradeUser->m_strUserId, strTradeCharName) != 0)
		return;

	// Must not be already trading but must be waiting for result
	if ((m_bTrading || pTradeUser->m_bTrading) || (!m_bTradeWaiting && !pTradeUser->m_bTradeWaiting))
		return;

	m_bTradeWaiting = false;
	pTradeUser->m_bTradeWaiting = false;

	BYTE byResult = GetByte(pBuf, index);
	if (byResult != 1)
	{
		index = 0;
		SetByte(m_TempBuf, PKT_TRADE_ACK, index);
		SetByte(m_TempBuf, byResult, index);
		pTradeUser->Send(m_TempBuf, index);
		return;
	}

	m_bTrading = true;
	m_iTradeUid = pTradeUser->m_Uid;
	pTradeUser->m_bTrading = true;
	pTradeUser->m_iTradeUid = m_Uid;
	strcpy(m_strTradeUser, pTradeUser->m_strUserId);
	strcpy(pTradeUser->m_strTradeUser, m_strUserId);
	m_bExchangeOk = false;
	pTradeUser->m_bExchangeOk = false;
	m_dwTradeMoney = 0;
	pTradeUser->m_dwTradeMoney = 0;

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

	EnterCriticalSection(&pTradeUser->m_CS_ExchangeItem);
	for (int i = 0; i < pTradeUser->m_arExchangeItem.GetSize(); i++)
	{
		if (pTradeUser->m_arExchangeItem[i] != NULL)
		{
			delete pTradeUser->m_arExchangeItem[i];
			pTradeUser->m_arExchangeItem[i] = NULL;
		}
	}
	pTradeUser->m_arExchangeItem.RemoveAll();
	LeaveCriticalSection(&pTradeUser->m_CS_ExchangeItem);

	index = 0;
	SetByte(m_TempBuf, PKT_TRADE_ACK, index);
	SetByte(m_TempBuf, SUCCESS, index);
	Send(m_TempBuf, index);
	pTradeUser->Send(m_TempBuf, index);
}

void USER::ExchangeItem(TCHAR* pBuf)
{
	// Adds Item to the current trade between two players
	if (!m_bTrading)
	{
		SendExchangeItemFail();
		return;
	}

	USER* pTradeUser = GetUser(m_iTradeUid);
	if (!pTradeUser)
	{
		SendExchangeItemFail();
		return;
	}

	if (strcmp(m_strTradeUser, pTradeUser->m_strUserId) != 0)
	{
		SendExchangeItemFail();
		return;
	}

	if (m_bExchangeOk || pTradeUser->m_bExchangeOk)
	{
		SendExchangeItemFail();
		return;
	}

	int index = 0;
	short sSlot = GetShort(pBuf, index);
	short sItemCount = GetShort(pBuf, index);
	if (sSlot < 0 || sSlot >= INV_ITEM_NUM)
	{
		SendExchangeItemFail();
		return;
	}

	if (sItemCount <= 0 || sItemCount > MAX_ITEM_DURA)
	{
		SendExchangeItemFail();
		return;
	}

	ItemList tradeItem = m_InvItem[sSlot];
	if (tradeItem.IsEmpty())
	{
		SendExchangeItemFail();
		return;
	}

	// Make sure the item is tradeable!
	if (!CanTradeItem(tradeItem))
	{
		SendExchangeItemFail();
		SendServerChatMessage(IDS_CANNOT_TRADE_ITEM, TO_ME);
		return;
	}

	// Non Equipment, Item Count Check
	if (tradeItem.bType > TYPE_ACC && sItemCount > tradeItem.sUsage)
	{
		SendExchangeItemFail();
		return;
	}

	// Lock the item count to 1 if its an equipment item
	if (tradeItem.bType <= TYPE_ACC)
	{
		sItemCount = 1;
	}

	// Go through user's trade items that
	// will be traded to other user
	// Calculate the total weight and space to be taken up in the inventory
	int iTotalTradeSpace = 1;
	int iTotalTradeWeight = 0;
	EnterCriticalSection(&m_CS_ExchangeItem);
	for (int i = 0; i < m_arExchangeItem.GetSize(); i++)
	{
		if (m_arExchangeItem[i] == NULL) continue;
		if (m_InvItem[m_arExchangeItem[i]->sSlot].IsEmpty()) continue;
		++iTotalTradeSpace;
		if (m_InvItem[m_arExchangeItem[i]->sSlot].bType > TYPE_ACC) // Non Equipment
		{
			iTotalTradeWeight += m_InvItem[m_arExchangeItem[i]->sSlot].sWgt * m_arExchangeItem[i]->sCount;
		}
		else
		{
			iTotalTradeWeight += m_InvItem[m_arExchangeItem[i]->sSlot].sWgt;
		}
	}
	LeaveCriticalSection(&m_CS_ExchangeItem);

	// This is the amount of weight that the other user is taking
	// off of their current weight because its being moved to
	// this user in the trade.
	// So the result will be negative.
	int iTotalTradeWeightOther = 0;
	EnterCriticalSection(&pTradeUser->m_CS_ExchangeItem);
	for (int i = 0; i < pTradeUser->m_arExchangeItem.GetSize(); i++)
	{
		if (pTradeUser->m_arExchangeItem[i] == NULL) continue;
		if (pTradeUser->m_InvItem[pTradeUser->m_arExchangeItem[i]->sSlot].IsEmpty()) continue;
		if (pTradeUser->m_InvItem[pTradeUser->m_arExchangeItem[i]->sSlot].bType > TYPE_ACC) // Non Equipment
		{
			iTotalTradeWeightOther -= pTradeUser->m_InvItem[pTradeUser->m_arExchangeItem[i]->sSlot].sWgt * pTradeUser->m_arExchangeItem[i]->sCount;
		}
		else
		{
			iTotalTradeWeightOther -= pTradeUser->m_InvItem[pTradeUser->m_arExchangeItem[i]->sSlot].sWgt;
		}
	}
	LeaveCriticalSection(&pTradeUser->m_CS_ExchangeItem);

	// Check that the other user has space in inventory for the items
	// being traded
	int iEmptyInventorySpaceOther = 0;
	for (int i = EQUIP_ITEM_NUM; i < INV_ITEM_NUM; i++)
	{
		if (pTradeUser->m_InvItem[i].IsEmpty())
		{
			++iEmptyInventorySpaceOther;
		}
	}

	if (iEmptyInventorySpaceOther < iTotalTradeSpace)
	{
		SendServerChatMessage(IDS_TRADE_NO_INV_SPACE, TO_ME);
		SendExchangeItemFail();
		return;
	}

	SendChangeWgt(); // WHY?? Euro1p1 does this for some reason

	int iWeight = tradeItem.sWgt;
	if (tradeItem.bType > TYPE_ACC) // Non Equipment
	{
		iWeight = tradeItem.sWgt * sItemCount;
	}

	if ((pTradeUser->m_sWgt + iTotalTradeWeight + iWeight + iTotalTradeWeightOther) > pTradeUser->GetMaxWgt())
	{
		CString str;
		str.Format(IDS_TRADE_WEIGHT_FULL, pTradeUser->m_strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
		SendExchangeItemFail();
		return;
	}

	if (tradeItem.bType > TYPE_ACC) // Non Equipment
	{
		int iNum = GetSameItem(tradeItem, TRADE_SLOT);
		if (iNum != -1)
		{
			EnterCriticalSection(&m_CS_ExchangeItem);
			if (m_arExchangeItem[iNum]->sCount + sItemCount >= MAX_ITEM_DURA)
			{
				LeaveCriticalSection(&m_CS_ExchangeItem);
				SendExchangeItemFail();
				return;
			}

			m_arExchangeItem[iNum]->sCount += sItemCount;
			LeaveCriticalSection(&m_CS_ExchangeItem);

			SendExchangeItem(pTradeUser, sSlot, sItemCount);
			return;
		}

	}

	EnterCriticalSection(&m_CS_ExchangeItem);
	if (m_arExchangeItem.GetSize() >= INV_ITEM_NUM - EQUIP_ITEM_NUM)
	{
		LeaveCriticalSection(&m_CS_ExchangeItem);
		SendExchangeItemFail();
		return;
	}
	LeaveCriticalSection(&m_CS_ExchangeItem);

	ExchangeItemList* pNewItem = new ExchangeItemList;
	pNewItem->sSlot = sSlot;
	pNewItem->sCount = sItemCount;
	EnterCriticalSection(&m_CS_ExchangeItem);
	m_arExchangeItem.Add(pNewItem);
	LeaveCriticalSection(&m_CS_ExchangeItem);

	SendExchangeItem(pTradeUser, sSlot, sItemCount);
}

void USER::ExchangeMoney(TCHAR* pBuf)
{
	// Adds money to the current trade between two players
	if (!m_bTrading)
		return;

	USER* pTradeUser = GetUser(m_iTradeUid);
	if (!pTradeUser)
		return;

	if (strcmp(m_strTradeUser, pTradeUser->m_strUserId) != 0)
		return;

	if (m_bExchangeOk || pTradeUser->m_bExchangeOk)
		return;

	int index = 0;
	DWORD dwTradeMoney = GetDWORD(pBuf, index);
	if (dwTradeMoney <= 0 || dwTradeMoney > m_dwBarr)
		return;

	if (!CheckMaxValueReturn((DWORD &)pTradeUser->m_dwBarr, (DWORD)(dwTradeMoney)))
		return;

	m_dwTradeMoney = dwTradeMoney;

	index = 0;
	SetByte(m_TempBuf, PKT_TRADE_SETMONEY, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	SetDWORD(m_TempBuf, dwTradeMoney, index);
	Send(m_TempBuf, index);
	pTradeUser->Send(m_TempBuf, index);
}

void USER::ExchangeOk()
{
	if (!m_bTrading)
		return;

	USER* pTradeUser = GetUser(m_iTradeUid);
	if (!pTradeUser || pTradeUser->m_State != STATE_GAMESTARTED)
		return;

	if (strcmp(m_strTradeUser, pTradeUser->m_strUserId) != 0)
		return;

	m_bExchangeOk = true;
	if (!pTradeUser->m_bExchangeOk)
	{
		int index = 0;
		SetByte(m_TempBuf, PKT_TRADE_OK, index);
		pTradeUser->Send(m_TempBuf, index);
		return;
	}

	// Below code is only executed if both players have accepted the trade (both m_bExchangeOk are true)
	if (!CheckDistance(pTradeUser, 2))
	{
		int index = 0;
		SetByte(m_TempBuf, PKT_TRADE_RESULT, index);
		SetByte(m_TempBuf, 4, index); // Transaction failed, too far away from other player
		Send(m_TempBuf, index);
		pTradeUser->Send(m_TempBuf, index);
		return;
	}

	int myItemNumber = m_arExchangeItem.GetSize();
	int otherItemNumber = pTradeUser->m_arExchangeItem.GetSize();

	if (myItemNumber == 0 && otherItemNumber == 0 && m_dwTradeMoney <= 0 && pTradeUser->m_dwTradeMoney <= 0)
	{
		int index = 0;
		SetByte(m_TempBuf, PKT_TRADE_RESULT, index);
		SetByte(m_TempBuf, 1, index); // Transaction failed
		Send(m_TempBuf, index);
		pTradeUser->Send(m_TempBuf, index);
		return;
	}

	// START Trade Items
	//
	ItemList myInvItem[INV_ITEM_NUM];
	ItemList otherInvItem[INV_ITEM_NUM];

	for (int i = 0; i < INV_ITEM_NUM; i++)
	{
		myInvItem[i] = m_InvItem[i];
		otherInvItem[i] = pTradeUser->m_InvItem[i];
	}

	bool bTransactionOk = true;

	// Give items in trade to user from other user
	EnterCriticalSection(&pTradeUser->m_CS_ExchangeItem);
	for (int i = 0; i < pTradeUser->m_arExchangeItem.GetSize(); i++)
	{
		if (pTradeUser->m_arExchangeItem[i] == NULL)
			continue;

		short sSlot = pTradeUser->m_arExchangeItem[i]->sSlot;
		if (sSlot < EQUIP_ITEM_NUM || sSlot >= INV_ITEM_NUM)
		{
			bTransactionOk = false;
			break;
		}

		if (pTradeUser->m_InvItem[sSlot].IsEmpty())
		{
			bTransactionOk = false;
			break;
		}

		if (pTradeUser->m_InvItem[sSlot].bType > TYPE_ACC) // Non Equipment
		{
			short sCount = pTradeUser->m_arExchangeItem[i]->sCount;
			if (pTradeUser->m_InvItem[sSlot].sUsage < sCount)
			{
				bTransactionOk = false;
				break;
			}

			ItemList tempItem = pTradeUser->m_InvItem[sSlot];
			tempItem.sUsage = sCount;
			if (PushItemInventory(&tempItem) != -1)
			{
				PlusItemDur(&pTradeUser->m_InvItem[sSlot], sCount, true);
				if (pTradeUser->m_InvItem[sSlot].sUsage <= 0)
				{
					pTradeUser->m_InvItem[sSlot].Init();
				}
			}
		}
		else // Equipment (Not Stackable)
		{
			if (PushItemInventory(&pTradeUser->m_InvItem[sSlot]) != -1)
			{
				pTradeUser->m_InvItem[sSlot].Init();
			}
			else
			{
				bTransactionOk = false;
				break;
			}
		}
	}
	LeaveCriticalSection(&pTradeUser->m_CS_ExchangeItem);


	// Give items in trade from other user to user
	EnterCriticalSection(&m_CS_ExchangeItem);
	for (int i = 0; i < m_arExchangeItem.GetSize(); i++)
	{
		if (m_arExchangeItem[i] == NULL)
			continue;

		short sSlot = m_arExchangeItem[i]->sSlot;
		if (sSlot < EQUIP_ITEM_NUM || sSlot >= INV_ITEM_NUM)
		{
			bTransactionOk = false;
			break;
		}

		if (m_InvItem[sSlot].IsEmpty())
		{
			bTransactionOk = false;
			break;
		}

		if (m_InvItem[sSlot].bType > TYPE_ACC) // Non Equipment
		{
			short sCount = m_arExchangeItem[i]->sCount;
			if (m_InvItem[sSlot].sUsage < sCount)
			{
				bTransactionOk = false;
				break;
			}

			ItemList tempItem = m_InvItem[sSlot];
			tempItem.sUsage = sCount;
			if (pTradeUser->PushItemInventory(&tempItem) != -1)
			{
				pTradeUser->PlusItemDur(&m_InvItem[sSlot], sCount, true);
				if (m_InvItem[sSlot].sUsage <= 0)
				{
					m_InvItem[sSlot].Init();
				}
			}
		}
		else // Equipment (Not Stackable)
		{
			if (pTradeUser->PushItemInventory(&m_InvItem[sSlot]) != -1)
			{
				m_InvItem[sSlot].Init();
			}
			else
			{
				bTransactionOk = false;
				break;
			}
		}
	}
	LeaveCriticalSection(&m_CS_ExchangeItem);

	if (!bTransactionOk)
	{
		for (int i = 0; i < INV_ITEM_NUM; i++)
		{
			m_InvItem[i] = myInvItem[i];
			pTradeUser->m_InvItem[i] = otherInvItem[i];
		}

		int index = 0;
		SetByte(m_TempBuf, PKT_TRADE_RESULT, index);
		SetByte(m_TempBuf, 1, index); // Transaction failed
		Send(m_TempBuf, index);
		pTradeUser->Send(m_TempBuf, index);
		return;
	}
	//
	// END Trade Items

	// Take a backup of the money
	// Only changes the main money variables when transaction is fully complete
	DWORD dwMyMoney = m_dwBarr;
	DWORD dwOtherMoney = pTradeUser->m_dwBarr;

	// START Trade Money
	//
	// Give user money from the trade
	if (pTradeUser->m_dwTradeMoney > 0)
	{
		if (!CheckMaxValueReturn((DWORD &)dwMyMoney, (DWORD)(pTradeUser->m_dwTradeMoney)))
		{
			int index = 0;
			SetByte(m_TempBuf, PKT_TRADE_RESULT, index);
			SetByte(m_TempBuf, 3, index); // Transaction failed, too much money involved
			Send(m_TempBuf, index);
			pTradeUser->Send(m_TempBuf, index);
			return;
		}
		CheckMaxValue((DWORD &)dwMyMoney, (DWORD)pTradeUser->m_dwTradeMoney);
		dwOtherMoney -= pTradeUser->m_dwTradeMoney;
	}

	// Give trade user money from the trade
	if (m_dwTradeMoney > 0)
	{
		if (!CheckMaxValueReturn((DWORD &)dwOtherMoney, (DWORD)(m_dwTradeMoney)))
		{
			int index = 0;
			SetByte(m_TempBuf, PKT_TRADE_RESULT, index);
			SetByte(m_TempBuf, 3, index);
			Send(m_TempBuf, index);
			pTradeUser->Send(m_TempBuf, index);
			return;
		}
		CheckMaxValue((DWORD &)dwOtherMoney, (DWORD)m_dwTradeMoney);
		dwMyMoney -= m_dwTradeMoney;
	}
	//
	// END Trade Money

	// TODO: Update User Data  Database / Shared memory

	m_bTrading = false;
	m_bTradeWaiting = false;
	pTradeUser->m_bTrading = false;
	pTradeUser->m_bTradeWaiting = false;

	SendCharItemData();
	SendChangeWgt();
	pTradeUser->SendCharItemData();
	pTradeUser->SendChangeWgt();

	m_dwBarr = dwMyMoney;
	pTradeUser->m_dwBarr = dwOtherMoney;
	SendMoneyChanged();
	pTradeUser->SendMoneyChanged();

	int index = 0;
	SetByte(m_TempBuf, PKT_TRADE_RESULT, index);
	SetByte(m_TempBuf, 0, index);
	Send(m_TempBuf, index);
	pTradeUser->Send(m_TempBuf, index);
}

void USER::ExchangeCancel(TCHAR* pBuf)
{
	m_bTrading = false;
	m_bTradeWaiting = false;

	int index = 0;
	int iTradeUid = GetInt(pBuf, index);
	if (iTradeUid < 0 || iTradeUid >= NPC_BAND) return;
	if (iTradeUid - USER_BAND == m_Uid) return;
	USER* pTradeUser = GetUser(iTradeUid - USER_BAND);
	if (pTradeUser)
	{
		pTradeUser->m_bTrading = false;
		pTradeUser->m_bTradeWaiting = false;

		int index = 0;
		SetByte(m_TempBuf, PKT_TRADE_CANCEL, index);
		pTradeUser->Send(m_TempBuf, index);
		pTradeUser->SendServerChatMessage(IDS_TRADE_CANCEL, TO_ME);
	}
}

void USER::SpecialMove(TCHAR* pBuf)
{
	int index = 0;

	BYTE byType = GetByte(pBuf, index);

	TCHAR strName[NAME_LENGTH+1] = {0};
	int iLen = GetVarString(sizeof(strName), strName, pBuf, index);
	if (iLen <= 0 || iLen > NAME_LENGTH)
		return;

	if (byType == 1) // Recall
	{
		if (m_sGuildRank != Guild::GUILD_CHIEF)
		{
			CString str;
			str.Format(IDS_CANNOT_FIND_USER, strName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			return;
		}

		short sTownNum;
		if (!CheckDemon(m_sClass))
			sTownNum = 1;
		else
			sTownNum = 2;

		if (!CheckGuildTown(sTownNum))
		{
			SendServerChatMessage(IDS_GUILD_TOWN_OWNER_ONLY, TO_ME);
			return;
		}

		USER* pUser = GetUserId(strName);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED)
		{
			CString str;
			str.Format(IDS_CANNOT_FIND_USER, strName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			return;
		}

		if (!IsServerRank(SERVER_RANK_NORMAL))
		{
			CString str;
			str.Format(IDS_CANNOT_FIND_USER, strName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			return;
		}

		if (m_sGroup != pUser->m_sGroup)
		{
			SendServerChatMessage(IDS_GUILD_MEMBERS_ONLY, TO_ME);
			return;
		}

		CPoint pt = ConvertToClient(m_sX, m_sY, m_sZ);
		if (pt.x == -1 || pt.y == -1)
		{
			CString str;
			str.Format(IDS_CANNOT_FIND_USER, strName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			return;
		}

		if (m_sZ != pUser->m_sZ)
		{
			pUser->LinkToOtherZone(m_sZ, static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
		else
		{
			pUser->LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
	}
	else if (byType == 2) // Trace
	{
		if (m_sGuildRank == Guild::GUILD_MEMBER)
			return;

		USER* pUser = GetUserId(strName);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED)
		{
			CString str;
			str.Format(IDS_CANNOT_FIND_USER, strName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			return;
		}

		if (!IsServerRank(SERVER_RANK_NORMAL))
		{
			CString str;
			str.Format(IDS_CANNOT_FIND_USER, strName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			return;
		}

		if (CheckDemon(m_sClass) != CheckDemon(pUser->m_sClass))
		{
			CString str;
			str.Format(IDS_CANNOT_FIND_USER, strName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			return;
		}

		if (pUser->m_sZ == 7) // Dungeon Level 4 Boss
		{
			CString str;
			str.Format(IDS_CANNOT_FIND_USER, strName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			return;
		}

		if (m_sGroup != pUser->m_sGroup)
		{
			SendServerChatMessage(IDS_GUILD_MEMBERS_ONLY, TO_ME);
			return;
		}

		CPoint pt = ConvertToClient(pUser->m_sX, pUser->m_sY, pUser->m_sZ);
		if (pt.x == -1 || pt.y == -1)
		{
			CString str;
			str.Format(IDS_CANNOT_FIND_USER, strName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
			return;
		}

		if (pUser->m_sZ != m_sZ)
		{
			LinkToOtherZone(pUser->m_sZ, static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
		else
		{
			LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
	}
}
