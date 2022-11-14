// UserDatabase.cpp: implementation of the database data loading of USER class.
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

bool USER::IsDoubleAccount(TCHAR* strAccount)
{
	if (!strlen(strAccount))
		return true;

	CString strSource = strAccount;

	USER *pUser;
	for (int i = 0; i < MAX_USER; i++)
	{
		if (i == m_Uid)
			continue;

		pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL)
			continue;

		if (!strSource.CompareNoCase(pUser->m_strAccount))
		{
			if (pUser->m_State != STATE_DISCONNECTED && pUser->m_State != STATE_LOGOUT)
			{
				pUser->Close();
				return true;
			}
		}
	}

	return false;
}

bool USER::CheckSessionLogin(TCHAR* strAccount, TCHAR* strPassword)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];

	SQLINTEGER		iResult = -1;
	SQLINTEGER		iResult2 = -1;
	SQLINTEGER		cbParmRet = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call check_session_login (?, ?, ?, ?)}"));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iResult, 0, &cbParmRet);
		retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &iResult2, 0, &cbParmRet);

		SQLINTEGER accountLen = strlen(strAccount);
		SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, ACCOUNT_LENGTH,	0, (TCHAR*)strAccount,		0, &accountLen);

		SQLINTEGER passwordLen = strlen(strPassword);
		SQLBindParameter(hstmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, ACCOUNT_LENGTH,	0, (TCHAR*)strPassword,		0, &passwordLen);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
			{
				SQLFetch(hstmt);
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (iResult == -1) return false;

	return true;
}

bool USER::CheckAccountLogin(TCHAR* strAccount)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];

	SQLINTEGER		iResult = -1;
	SQLINTEGER		iInd = 0;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("select nAcctType from currentuser where strclientid = ?"));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		SQLINTEGER accountLen = strlen(strAccount);
		SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, ACCOUNT_LENGTH,	0, (TCHAR*)strAccount,		0, &accountLen);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLGetData(hstmt, 1, SQL_C_SLONG, &iResult,	sizeof(iResult),	&iInd);
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (iResult == -1) return false;

	return true;
}

bool USER::LoadCharData(TCHAR *id)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	bool bFind = false;

	CString strNameSafe = id;
	strNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LOAD_CHAR_DATA (\'%s\')}"), strNameSafe);

	SQLCHAR		strChar[3][NAME_LENGTH+1];
	SQLINTEGER	strCharInd[3];

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	for (int i = 0; i < 3; i++)
	{
		::ZeroMemory(strChar[i], NAME_LENGTH+1);
		strCharInd[i] = SQL_NTS;
	}

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("COULD NOT ALLOCATE HANDLE!\n");
		// g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		int charIndex = 0;

		while (charIndex < 3)
		{
			retcode = SQLFetch(hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLGetData(hstmt, 1, SQL_C_CHAR, strChar[charIndex], sizeof(strChar[charIndex]), &strCharInd[charIndex]);
			}
			else
			{
				break;
			}
			++charIndex;
		}
	}
	else
	{
		TRACE("Could not exec query!\n");
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	m_nCharNum = 0;
	int nSize = sizeof(m_strCharacters);
	::ZeroMemory(m_strCharacters, sizeof(m_strCharacters));

	_tcscpy(m_strCharacters[0], (LPCTSTR)strChar[0]);		if (strlen(m_strCharacters[0])) m_nCharNum++;
	_tcscpy(m_strCharacters[1], (LPCTSTR)strChar[1]);		if (strlen(m_strCharacters[1])) m_nCharNum++;
	_tcscpy(m_strCharacters[2], (LPCTSTR)strChar[2]);		if (strlen(m_strCharacters[2])) m_nCharNum++;

	return true;
}

bool USER::GetLastChar(TCHAR *id)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	bool bFind = false;

	CString strNameSafe = id;
	strNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call proc_get_lastchar (\'%s\')}"), strNameSafe);

	SQLCHAR		strChar[NAME_LENGTH+1];
	SQLINTEGER	strCharInd;

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	::ZeroMemory(strChar, NAME_LENGTH+1);
	strCharInd = SQL_NTS;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("COULD NOT ALLOCATE HANDLE!\n");
		// g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLFetch(hstmt);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, strChar, NAME_LENGTH, &strCharInd);
		}
	}
	else
	{
		TRACE("Could not exec query!\n");
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	::ZeroMemory(m_strLastCharacter, sizeof(m_strLastCharacter));
	_tcscpy(m_strLastCharacter, (char*)strChar);

	return true;
}

bool USER::SendCharInfo(TCHAR *strChar, TCHAR* pBuf, int& index)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	bool			bFind = false;
	TCHAR			szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	CString strNameSafe = strChar;
	strNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call SEND_CHAR_DATA (\'%s\')}"), strNameSafe);

	SQLINTEGER	sInd;

	SQLCHAR		strCharID[NAME_LENGTH+1];
	::ZeroMemory(strCharID, sizeof(strCharID));
	SQLINTEGER	strCharInd = SQL_NTS;
	SQLSMALLINT sClass = 0, sLevel = 0;
	SQLINTEGER	iSTR = 0, iDEX = 0, iINT = 0, iCHA = 0, iWIS = 0, iCON = 0;
	SQLSMALLINT sHP = 0, sMAXHP = 0, sMP = 0, sMAXMP = 0, sSTM = 0, sMAXSTM = 0, sAge = 0;
	SQLINTEGER iMoral = 0;
	SQLSMALLINT sGender = 0, sHair = 0, sHairMode = 0, sSkin = 0, sGroup = 0, sFame = 0;
	SQLINTEGER iSwordExp = 0, iSpearExp = 0, iBowExp = 0, iAxeExp = 0, iKnuckleExp = 0, iStaffExp = 0;
	SQLINTEGER iWeaponMakeExp = 0, iArmorMakeExp = 0, iAccMakeExp = 0, iPotionMakeExp = 0, iCookingExp = 0;
	SQLINTEGER iWMagicExp = 0, iBMagicExp = 0, iDMagicExp = 0;
	SQLCHAR	strEquipItems[EQUIP_ITEM_NUM][ITEM_DB_SIZE];
	::ZeroMemory(strEquipItems, sizeof(strEquipItems));

	int dbIndex = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(dbIndex); // There is currently only going to be 1 database
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
//		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLFetch(hstmt);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			int i = 1;
			SQLGetData(hstmt, i++,	SQL_C_CHAR,		strCharID,	sizeof(strCharID),	&strCharInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sClass, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sLevel, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iSTR,	sizeof(iSTR),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iDEX,	sizeof(iDEX),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iINT,	sizeof(iINT),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iCHA,	sizeof(iCHA),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iWIS,	sizeof(iWIS),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iCON,	sizeof(iCON),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sHP, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sMAXHP, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sMP, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sMAXMP, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sSTM, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sMAXSTM, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sAge, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iMoral,	sizeof(iMoral),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sGender, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sHair, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sHairMode, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sSkin, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sGroup, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sFame, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iSwordExp,	sizeof(iSwordExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iSpearExp,	sizeof(iSpearExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iBowExp,	sizeof(iBowExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iAxeExp,	sizeof(iAxeExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iKnuckleExp,	sizeof(iKnuckleExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iStaffExp,	sizeof(iStaffExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iWeaponMakeExp,	sizeof(iWeaponMakeExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iArmorMakeExp,	sizeof(iArmorMakeExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iAccMakeExp,	sizeof(iAccMakeExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iPotionMakeExp,	sizeof(iPotionMakeExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iCookingExp,	sizeof(iCookingExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iWMagicExp,	sizeof(iWMagicExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iBMagicExp,	sizeof(iBMagicExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iDMagicExp,	sizeof(iDMagicExp),	&sInd);

			for (int j = 0; j < EQUIP_ITEM_NUM; j++)
			{
				SQLGetData(hstmt, i++, SQL_C_BINARY, &strEquipItems[j],	ITEM_DB_SIZE,	&sInd);
			}
		}
		else
		{
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(dbIndex);
			return false;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DB[m_iModSid].ReleaseDB(dbIndex);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(dbIndex);

	CBufferEx TempBuf;
	TempBuf.AddString(strChar);
	TempBuf.Add(static_cast<short>(sClass));
	TempBuf.Add(sLevel);

	short sSTR = static_cast<short>(iSTR/100000);
	TempBuf.Add(sSTR);

	short sDEX = static_cast<short>(iDEX/1000000);
	TempBuf.Add(sDEX);

	short sINT = static_cast<short>(iINT/100000);
	TempBuf.Add(sINT);

	short sCHA = static_cast<short>(iCHA/1000000);
	TempBuf.Add(sCHA);

	short sWIS = static_cast<short>(iWIS/1000000);
	TempBuf.Add(sWIS);

	short sCON = static_cast<short>(iCON/100000);
	TempBuf.Add(sCON);

	TempBuf.Add(static_cast<short>(sHP));
	TempBuf.Add(static_cast<short>(sMAXHP));
	TempBuf.Add(static_cast<short>(sMP));
	TempBuf.Add(static_cast<short>(sMAXMP));
	TempBuf.Add(static_cast<short>(sSTM));
	TempBuf.Add(static_cast<short>(sMAXSTM));
	TempBuf.Add(static_cast<short>(sAge));

	short sMoral = static_cast<short>(iMoral/100000);
	TempBuf.Add(sMoral);
	TempBuf.Add(static_cast<short>(sGender));
	TempBuf.Add(static_cast<short>(sHair));
	TempBuf.Add(static_cast<short>(sHairMode));
	TempBuf.Add(static_cast<short>(sSkin));

	// Gets the guild for the user
	Guild* pGuild = GetGuild(sGroup);
	if (pGuild == NULL)
	{
		TempBuf.Add((BYTE)0);
	}
	else
	{
		int iUserIndex = pGuild->GetUser(strChar);
		if (iUserIndex < 0)
		{
			TempBuf.Add((BYTE)0);
		}
		else
		{
			TempBuf.AddString(pGuild->m_strGuildName);
		}
	}
	ReleaseGuild();

	TempBuf.Add(static_cast<short>(sFame));

	TempBuf.Add((iSwordExp/10000));
	TempBuf.Add((iSpearExp/10000));
	TempBuf.Add((iBowExp/10000));
	TempBuf.Add((iAxeExp/10000));
	TempBuf.Add((iKnuckleExp/10000));
	TempBuf.Add((iStaffExp/10000));

	TempBuf.Add((iWeaponMakeExp/10000));
	TempBuf.Add((iArmorMakeExp/10000));
	TempBuf.Add((iAccMakeExp/10000));
	TempBuf.Add((iPotionMakeExp/10000));
	TempBuf.Add((iCookingExp/10000));

	// Parse Item
	TCHAR szItemBuf[2 * EQUIP_ITEM_NUM];		// 2 = Item Pid, 10 = Total Equip Slot Count
	::ZeroMemory(szItemBuf, sizeof(szItemBuf));
	if (!GetEquipItemPid((LPTSTR)strEquipItems, szItemBuf)) return false;
	TempBuf.AddData(szItemBuf, 2* EQUIP_ITEM_NUM);

	TempBuf.Add((BYTE)0); // no idea what this is
						  // Only used by devil soma characters o_O!

	::CopyMemory(pBuf + index, TempBuf, TempBuf.GetLength());
	index += TempBuf.GetLength();

	return true;
}

bool USER::IsExistCharId(TCHAR* strChar)
{
	CString strNameSafe = strChar;
	strNameSafe.Replace("'", "''");

	TCHAR			szSQL[1024]; ::ZeroMemory(szSQL, sizeof(szSQL));
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call proc_Exist_Char (?, \'%s\')}"), strNameSafe);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index); // There is currently only going to be 1 database
	if (!pDB) return true;

	SQLHSTMT hstmt = NULL;
	SQLRETURN retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		return true;
	}

	SQLSMALLINT	sRet = 0;
	SQLINTEGER sRetInd = SQL_NTS;
	retcode = SQLBindParameter(hstmt, 1 ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sRet, 0, &sRetInd);
	if (retcode != SQL_SUCCESS)
	{
		SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		return true;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return true;
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

	if (sRet == 0) return false;

	return true;
}

bool USER::LoadUserData(TCHAR *szID)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024]; ZeroMemory(szSQL, sizeof(szSQL));

	CString strNameSafe = szID;
	strNameSafe.Replace("'", "''");

	CString strAccountSafe = m_strAccount;
	strAccountSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LOAD_USER_DATA (\'%s\', \'%s\')}"), strNameSafe, strAccountSafe);

	SQLINTEGER	sInd = 0;
	SQLSMALLINT sClass, sLevel;
	SQLBIGINT	iExp, iMaxExp;
	SQLINTEGER iSTR, iSTRInc, iDEX, iDEXInc, iINT, iINTInc, iCHA, iCHAInc, iWIS, iWISInc, iCON, iCONInc;
	SQLSMALLINT sStartStr, sStartDex, sStartInt, sStartCha, sStartWis, sStartCon;
	SQLSMALLINT sHP, sMaxHP, sMP, sMaxMP, sStm, sMaxStm, sWgt, sMaxWgt;
	SQLINTEGER iStatus;
	SQLSMALLINT sAge;
	SQLINTEGER iPlayCount, iMoral;
	SQLSMALLINT sGender, sHair, sHairMode, sSkin, sGroup, sGroupReq, sFame, sMageType;
	SQLINTEGER iMoney, iBank;
	SQLSMALLINT sRank, sZ, sX, sY;
	SQLINTEGER iClassWarTime, iClassPoint, iTotClassPoint;
	SQLINTEGER iSwordExp, iSpearExp, iBowExp, iAxeExp, iKnuckleExp, iStaffExp;
	SQLINTEGER iWeaponMakeExp, iArmorMakeExp, iAccMakeExp, iPotionMakeExp, iCookingExp;
	SQLINTEGER iWMagicExp, iBMagicExp, iDMagicExp;
	SQLINTEGER iMoralCount, iGrayCount, iEventCount;
	SQLSMALLINT sPartyDeny, sMsgRecv, sAllChatRecv;
	SQLCHAR strDenyMsgUserId[5][NAME_LENGTH+1]; ZeroMemory(strDenyMsgUserId, sizeof(strDenyMsgUserId));
	SQLCHAR HaveMagic[200];	ZeroMemory(HaveMagic, sizeof(HaveMagic));
	SQLCHAR HaveEvent[600]; ZeroMemory(HaveEvent, sizeof(HaveEvent));
	SQLCHAR UserStatus[100]; ZeroMemory(UserStatus, sizeof(UserStatus));

	sClass = 0;
	sLevel = 1;
	iExp = iMaxExp = 1;
	iSTR = iSTRInc = iDEX = iDEXInc = iINT = iINTInc = iCHA = iCHAInc = iWIS = iWISInc = iCON = iCONInc = 0;
	sStartStr = sStartDex = sStartInt = sStartCha = sStartWis = sStartCon = 0;
	sHP = sMaxHP = sMP = sMaxMP = sStm = sMaxStm = sWgt = sMaxWgt = sAge = 1;
	iStatus = iPlayCount = iMoral = 1;
	sGender = sHair = sHairMode = sSkin = sGroup = sGroupReq = sFame = sMageType = 1;
	iMoney = iBank = 1;
	sRank = sZ = sX = sY = 1;
	iClassWarTime = iClassPoint = iTotClassPoint = 0;
	iSwordExp = iSpearExp = iBowExp = iAxeExp = iKnuckleExp = iStaffExp = 0;
	iWeaponMakeExp = iArmorMakeExp = iAccMakeExp = iPotionMakeExp = iCookingExp = 0;
	iWMagicExp = iBMagicExp = iDMagicExp = 0;
	iMoralCount = iGrayCount = iEventCount = 0;
	sPartyDeny = sMsgRecv = sAllChatRecv = 0;

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Load User Data !!\n");
//		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLFetch(hstmt);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			int i = 1;
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sClass, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sLevel, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SBIGINT, &iExp, sizeof(SQLBIGINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SBIGINT, &iMaxExp, sizeof(SQLBIGINT), &sInd);

			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iSTR,	sizeof(iSTR),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iSTRInc,	sizeof(iSTRInc),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iDEX,	sizeof(iDEX),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iDEXInc,	sizeof(iDEXInc),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iINT,	sizeof(iINT),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iINTInc,	sizeof(iINTInc),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iCHA,	sizeof(iCHA),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iCHAInc,	sizeof(iCHAInc),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iWIS,	sizeof(iWIS),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iWISInc,	sizeof(iWISInc),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iCON,	sizeof(iCON),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iCONInc,	sizeof(iCONInc),	&sInd);

			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sStartStr, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sStartDex, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sStartInt, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sStartCha, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sStartWis, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sStartCon, sizeof(SQLSMALLINT), &sInd);

			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sHP, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sMaxHP, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sMP, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sMaxMP, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sStm, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sMaxStm, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sWgt, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sMaxWgt, sizeof(SQLSMALLINT), &sInd);

			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iStatus,	sizeof(iStatus),	&sInd);

			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sAge, sizeof(SQLSMALLINT), &sInd);

			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iPlayCount,	sizeof(iPlayCount),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iMoral,	sizeof(iMoral),	&sInd);

			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sGender, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sHair, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sHairMode, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sSkin, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sGroup, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sGroupReq, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sFame, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sMageType, sizeof(SQLSMALLINT), &sInd);

			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iMoney,	sizeof(iMoney),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iBank,	sizeof(iBank),	&sInd);

			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sRank, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sZ, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sX, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sY, sizeof(SQLSMALLINT), &sInd);

			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iClassWarTime,	sizeof(iClassWarTime),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iClassPoint,	sizeof(iClassPoint),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iTotClassPoint,	sizeof(iTotClassPoint),	&sInd);

			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iSwordExp,	sizeof(iSwordExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iSpearExp,	sizeof(iSpearExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iBowExp,	sizeof(iBowExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iAxeExp,	sizeof(iAxeExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iKnuckleExp,	sizeof(iKnuckleExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iStaffExp,	sizeof(iStaffExp),	&sInd);

			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iWeaponMakeExp,	sizeof(iWeaponMakeExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iArmorMakeExp,	sizeof(iArmorMakeExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iAccMakeExp,	sizeof(iAccMakeExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iPotionMakeExp,	sizeof(iPotionMakeExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iCookingExp,	sizeof(iCookingExp),	&sInd);

			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iWMagicExp,	sizeof(iWMagicExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iBMagicExp,	sizeof(iBMagicExp),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iDMagicExp,	sizeof(iDMagicExp),	&sInd);

			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iMoralCount,	sizeof(iMoralCount),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iGrayCount,	sizeof(iGrayCount),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_SLONG,	&iEventCount,	sizeof(iEventCount),	&sInd);

			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sPartyDeny, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sMsgRecv, sizeof(SQLSMALLINT), &sInd);
			SQLGetData(hstmt, i++, SQL_C_SSHORT, &sAllChatRecv, sizeof(SQLSMALLINT), &sInd);

			for (int j = 0; j < 5; j++)
			{
				SQLGetData(hstmt, i++, SQL_C_BINARY,	strDenyMsgUserId[j],	sizeof(strDenyMsgUserId[j]),	&sInd);
			}
			SQLGetData(hstmt, i++, SQL_C_BINARY,	HaveMagic,	sizeof(HaveMagic),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_BINARY,	HaveEvent,	sizeof(HaveEvent),	&sInd);
			SQLGetData(hstmt, i++, SQL_C_BINARY,	UserStatus,	sizeof(UserStatus),	&sInd);
		}
		else
		{
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(db_index);
			return false;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	// Copy User Data
	strcpy(m_strUserId, szID);	// User ID

	m_sClass = sClass;
	m_sLevel = sLevel;

	m_dwExp = (DWORD)iExp; // TODO: make these 64 bit integer?
	m_dwMaxExp = GetInc(INC_LEVEL_MAX_EXP) * 100; // TODO: (DWORD)iMaxExp; // make these 64 bit integer?

	m_iSTR = iSTR;
	m_iDEX = iDEX;
	m_iINT = iINT;
	m_iWIS = iWIS;
	m_iCON = iCON;
	m_iCHA = iCHA;

	m_iSTRInc = iSTRInc;
	m_iDEXInc = iDEXInc;
	m_iINTInc = iINTInc;
	m_iWISInc = iWISInc;
	m_iCONInc = iCONInc;
	m_iCHAInc = iCHAInc;

	m_sStartStr = sStartStr;
	m_sStartDex = sStartDex;
	m_sStartInt = sStartInt;
	m_sStartCha = sStartCha;
	m_sStartWis = sStartWis;
	m_sStartCon = sStartCon;

	m_sHP = sHP;
	m_sMaxHP = sMaxHP;
	m_sMP = sMP;
	m_sMaxMP = sMaxMP;
	m_sStm = sStm;
	m_sMaxStm = sMaxStm;
	m_sWgt = sWgt;
	m_sMaxWgt = sMaxWgt;

	m_sAge = sAge;

	m_iPlayCount = iPlayCount;
	m_iMoral = iMoral;

	m_sGender = sGender;
	m_sHair = sHair;
	m_sHairMode = sHairMode;
	m_sSkin = sSkin;
	m_sGroup = sGroup;
	m_sGroupReq = sGroupReq;
	m_sFame = sFame;
	m_sMageType = sMageType;

	m_dwBarr = iMoney;
	m_dwBank = iBank;

	m_sRank = sRank;
	m_sZ = sZ;
	m_sX = sX;
	m_sY = sY;

	m_iClassPoint = iClassPoint;
	m_iTotClassPoint = iTotClassPoint;

	m_iSwordExp = iSwordExp;
	m_iSpearExp = iSpearExp;
	m_iBowExp = iBowExp;
	m_iAxeExp = iAxeExp;
	m_iKnuckleExp = iKnuckleExp;
	m_iStaffExp = iStaffExp;
	m_iWeaponMakeExp = iWeaponMakeExp;

	m_iArmorMakeExp = iArmorMakeExp;
	m_iAccMakeExp = iAccMakeExp;
	m_iPotionMakeExp = iPotionMakeExp;
	m_iCookingExp = iCookingExp;

	m_iWMagicExp = iWMagicExp;
	m_iBMagicExp = iBMagicExp;
	m_iDMagicExp = iDMagicExp;

	m_iMoralCount = iMoralCount;
	m_iGrayCount = iGrayCount;
	m_iEventCount = iEventCount;

	m_bAllChatRecv = sAllChatRecv ? true : false;
	m_bPrivMsgRecv = sMsgRecv ? true : false;
	m_bPartyDeny = sPartyDeny ? true : false;

	strcpy((char*)m_strDenyMsgUserId, (char*)strDenyMsgUserId);

	// Gets the guild for the user
	m_sGroup = GetGuildNumByMember(m_strUserId);
	if (m_sGroup == -1)
	{
		m_sGroupReq = GetGuildReqNumByMember(m_strUserId);
	}
	Guild* pGuild = GetGuild(m_sGroup);
	if (pGuild == NULL)
	{
		m_sGroup = -1;
	}
	else
	{
		int iUserIndex = pGuild->GetUser(m_strUserId);
		if (iUserIndex < 0)
		{
			m_sGroup = -1;
			m_sGuildRank = -1;
		}
		else
		{
			m_sGroupReq = -1;
			m_sGuildRank = pGuild->m_arMembers[iUserIndex].m_sGuildRank;
			strcpy(m_strGuildName, pGuild->m_strGuildName);
			m_sGuildSymbolVersion = pGuild->m_sSymbolVersion;
			if (strcmp(pGuild->m_strUsedUser, m_strUserId) == 0)
			{
				pGuild->m_iUsedUid = -1;
				::ZeroMemory(pGuild->m_strUsedUser, sizeof(pGuild->m_strUsedUser));
				pGuild->m_lUsed = 0;
			}
		}
	}
	ReleaseGuild();

	// TODO: GetGuildName - DONT THINK THIS NEEDED NOW
	// TODO: GetGuildCallName - DONT THINK THIS NEEDED NOW
	// TODO: GetGuildPictureNum - DONT THINK THIS NEEDED NOW
	// TODO: GetGuildRank - DONT THINK THIS NEEDED NOW
	// TODO: GetGuildReqNum

	ParseHaveMagicData((LPTSTR)HaveMagic); // ParseHaveMagicData
	StrToHaveEventData((LPTSTR)HaveEvent); // ParseHaveEventData
	ParseUserStatusData((LPTSTR)UserStatus);

	// Validate the starting stats of human soma character are correct!
	if (!CheckDemon(m_sClass))
	{
		short sTotalStartStatPoints = m_sStartStr + m_sStartDex + m_sStartInt +
									  m_sStartCha + m_sStartWis + m_sStartCon;

		if (sTotalStartStatPoints < 60 ||
			sTotalStartStatPoints > 63 ||
			(m_sStartStr < 10 || m_sStartStr > 13) ||
			(m_sStartDex < 10 || m_sStartDex > 13) ||
			(m_sStartInt < 10 || m_sStartInt > 13) ||
			(m_sStartCha < 10 || m_sStartCha > 13) ||
			(m_sStartWis < 10 || m_sStartWis > 13) ||
			(m_sStartCon < 10 || m_sStartCon > 13))
		{
			return false;
		}
	}

	// TODO: LoadPoliceData_CHAT
	// TODO: LoadPoliceData_BBS

	return true;
}

bool USER::LoadItemData(TCHAR *strChar)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	BOOL			bFind = FALSE;
	TCHAR			szSQL[1024]; ZeroMemory(szSQL, sizeof(szSQL));

	CString strNameSafe = strChar;
	strNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LOAD_ITEM_DATA (\'%s\')}"), strNameSafe);

	SQLINTEGER	sInd = 0;
	SQLCHAR	Belt[BELT_ITEM_NUM][ITEM_DB_SIZE], Item[INV_ITEM_NUM][ITEM_DB_SIZE];

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Load User item Data !!\n");
//		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLFetch(hstmt);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			int i = 1;
			for (int j = 0; j < BELT_ITEM_NUM; j++)
			{
				ZeroMemory(Belt[j], sizeof(Belt[j]));
				SQLGetData(hstmt, i++, SQL_C_BINARY,	Belt[j],	sizeof(Belt[j]),	&sInd);
				m_BeltItem[j] = StrToUserItem((char*)Belt[j]);
			}
			for (int j = 0; j < INV_ITEM_NUM; j++)
			{
				ZeroMemory(Item[j], sizeof(Item[j]));
				SQLGetData(hstmt, i++, SQL_C_BINARY,	Item[j],	sizeof(Item[j]),	&sInd);
				m_InvItem[j] = StrToUserItem((char*)Item[j]);
			}
		}
		else
		{
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(db_index);
			return false;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	RecalcWgt();

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	return true;
}

bool USER::LoadStorageData(TCHAR *strChar)
{
	int j;
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	CString strNameSafe = strChar;
	strNameSafe.Replace("'", "''");

	::ZeroMemory(szSQL, sizeof(szSQL));
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LOAD_STORAGE_DATA (\'%s\')}"), strNameSafe);

	SQLSMALLINT sType;
	SQLCHAR		Item[STORAGE_ITEM_NUM][ITEM_DB_SIZE];

	SQLINTEGER	sInd;

	for (j = 0; j < STORAGE_ITEM_NUM; j++)
		::ZeroMemory(Item[j], sizeof(Item[j]));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);

	if (retcode != SQL_SUCCESS)
	{

		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLFetch(hstmt);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			int i = 2;
			SQLGetData(hstmt, i++, SQL_C_SSHORT,	&sType,		sizeof(sType),		&sInd);
			for (j = 0; j < STORAGE_ITEM_NUM; j++)
			{
				SQLGetData(hstmt, i++, SQL_C_BINARY,	Item[j],	sizeof(Item[j]),	&sInd);
			}
		}
		else if (retcode == SQL_NO_DATA)
		{
			g_DB[m_iModSid].ReleaseDB(db_index);
			return true;
		}
		else
		{
			g_DB[m_iModSid].ReleaseDB(db_index);
			return false;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	for (int i = 0; i < STORAGE_ITEM_NUM; i++)
	{
		m_StorageItem[i] = StrToUserItem((char*)Item[i]);
	}

	return true;
}

bool USER::UpdateUserData()
{
	if (m_State != STATE_GAMESTARTED) return false;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	TCHAR			szSQL[1024]; ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call UPDATE_USER_DATA (?,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d, \
		%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d, \
		%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d, \
		%d, %d,?,?,?,?,?,?,?,?)}"),
		m_sClass, m_sLevel, m_dwExp, m_dwMaxExp, m_iSTR, m_iSTRInc, m_iDEX, m_iDEXInc, // 9
		m_iINT, m_iINTInc, m_iCHA, m_iCHAInc, m_iWIS, m_iWISInc, m_iCON, m_iCONInc,					// 8
		m_sStartStr, m_sStartDex, m_sStartInt, m_sStartCha, m_sStartWis, m_sStartCon,				// 6
		m_sHP, m_sMaxHP, m_sMP, m_sMaxMP, m_sStm, m_sMaxStm, m_sWgt, m_sMaxWgt,						// 8
		m_iDeathType, m_sAge, m_iPlayCount, m_iMoral, m_sGender, m_sHair, m_sHairMode,				// 7
		m_sSkin, m_sGroup, m_sGroupReq, m_sFame, m_sMageType, m_dwBarr, m_dwBank, m_sRank,			// 8
		m_sZ, m_sX, m_sY, 0, m_iClassPoint, m_iTotClassPoint, m_iSwordExp, m_iSpearExp,				// 8
		m_iBowExp, m_iAxeExp, m_iKnuckleExp, m_iStaffExp, m_iWeaponMakeExp, m_iArmorMakeExp,		// 6
		m_iAccMakeExp, m_iPotionMakeExp, m_iCookingExp, m_iWMagicExp, m_iBMagicExp, m_iDMagicExp,	// 6
		m_iMoralCount, m_iGrayCount, m_iEventCount, m_bPartyDeny, m_bPrivMsgRecv, m_bAllChatRecv);	// 6
																									// 72
	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Update User Data !!\n");
		return false;
	}

	SQLINTEGER userIdLen = strlen(m_strUserId);

	TCHAR strDenyMsgUserId[5][NAME_LENGTH+1]; ZeroMemory(strDenyMsgUserId, sizeof(strDenyMsgUserId));
	TCHAR strHaveMagic[200];	ZeroMemory(strHaveMagic, sizeof(strHaveMagic));
	TCHAR strHaveEvent[600];	ZeroMemory(strHaveEvent, sizeof(strHaveEvent));
	TCHAR strUserStatus[100];	ZeroMemory(strUserStatus, sizeof(strUserStatus));

	strcpy((char*)strDenyMsgUserId, (char*)m_strDenyMsgUserId);
	FillMagicData(strHaveMagic);
	UserHaveEventDataToStr(strHaveEvent);
	FillUserStatusData(strUserStatus);

	SDWORD sDenyMsgUserIdLen[5];
	sDenyMsgUserIdLen[0] = sizeof(strDenyMsgUserId[0]);
	sDenyMsgUserIdLen[1] = sizeof(strDenyMsgUserId[1]);
	sDenyMsgUserIdLen[2] = sizeof(strDenyMsgUserId[2]);
	sDenyMsgUserIdLen[3] = sizeof(strDenyMsgUserId[3]);
	sDenyMsgUserIdLen[4] = sizeof(strDenyMsgUserId[4]);
	SDWORD sHaveMagicLen = sizeof(strHaveMagic);
	SDWORD sHaveEventLen = sizeof(strHaveEvent);
	SDWORD sUserStatusLen = sizeof(strUserStatus);

	// Bind params
	if (retcode == SQL_SUCCESS)
	{
		int i = 1;
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(m_strUserId),	0, (TCHAR*)m_strUserId,		0, &userIdLen);
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, sizeof(strDenyMsgUserId[0]),	0, (TCHAR*)strDenyMsgUserId[0],		0, &sDenyMsgUserIdLen[0]);
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, sizeof(strDenyMsgUserId[1]),	0, (TCHAR*)strDenyMsgUserId[1],		0, &sDenyMsgUserIdLen[1]);
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, sizeof(strDenyMsgUserId[2]),	0, (TCHAR*)strDenyMsgUserId[2],		0, &sDenyMsgUserIdLen[2]);
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, sizeof(strDenyMsgUserId[3]),	0, (TCHAR*)strDenyMsgUserId[3],		0, &sDenyMsgUserIdLen[3]);
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, sizeof(strDenyMsgUserId[4]),	0, (TCHAR*)strDenyMsgUserId[4],		0, &sDenyMsgUserIdLen[4]);
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, sizeof(strHaveMagic),	0, (TCHAR*)strHaveMagic,		0, &sHaveMagicLen);
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, sizeof(strHaveEvent),	0, (TCHAR*)strHaveEvent,		0, &sHaveEventLen);
		SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARCHAR, sizeof(strUserStatus),	0, (TCHAR*)strUserStatus,		0, &sUserStatusLen);

		retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else
		{
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(db_index);
			return false;
		}
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	return true;
}

bool USER::InsertGuild(Guild* pGuild, BYTE& bySubResult)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[512];
	SQLINTEGER		cbParmRet = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	// Make the guild strings safe for inserting into sql statement
	CString strGuildName = pGuild->m_strGuildName;
	strGuildName.Replace("'", "''");

	CString strGuildInfo = pGuild->m_strInfo;
	strGuildInfo.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_INSERT (?, %d, %d, \'%s\', \'%s\', \'%s\')}"),
		pGuild->m_sNum, pGuild->m_sClass, strGuildName, m_strUserId, strGuildInfo);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return FALSE;

	if (retcode == SQL_SUCCESS)
	{
		SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &bySubResult, 0, &cbParmRet);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
			{
				SQLFetch(hstmt);
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (bySubResult != 0) return false;

	return true;
}

bool USER::ChangeGuildCallName(short sRank, TCHAR* strGuildName, TCHAR* strCallName, BYTE& bySubResult)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	// Make the guild strings safe for inserting into sql statement
	CString strGuildNameSafe = strGuildName;
	strGuildNameSafe.Replace("'", "''");

	CString strCallNameSafe = strCallName;
	strCallNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_CHANGE_CALL_NAME (?, \'%s\', \'%s\', %d)}"),
		strCallNameSafe, strGuildNameSafe, sRank);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return FALSE;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &bySubResult, 0, &cbParmRet);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
			{
				SQLFetch(hstmt);
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (bySubResult != 0) return false;

	return true;
}

bool USER::EditGuildInfo(short sGuildNum, TCHAR* strGuildInfo)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	// Make the guild strings safe for inserting into sql statement
	CString strGuildInfoSafe = strGuildInfo;
	strGuildInfoSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_EDIT_INFO (%d, \'%s\')}"),
		sGuildNum, strGuildInfoSafe);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return FALSE;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;

	return true;
}

bool USER::GetGuildReqUserInfo(TCHAR* strGuildName, TCHAR* strApplicantName, GuildApplicantUserInfo& UserInfo, BYTE& bySubResult)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;
	SQLINTEGER	sInd = 0;
	SQLSMALLINT sLevel = 0, sMageType = 0;
	SQLINTEGER iMoral = 0;
	SQLINTEGER iSwordExp = 0, iSpearExp = 0, iBowExp = 0, iAxeExp = 0, iKnuckleExp = 0, iStaffExp = 0;
	SQLINTEGER iWeaponMakeExp = 0, iArmorMakeExp = 0, iAccMakeExp = 0, iPotionMakeExp = 0, iCookingExp = 0;

	// Make the guild strings safe for inserting into sql statement
	CString strGuildNameSafe = strGuildName;
	strGuildNameSafe.Replace("'", "''");

	CString strApplicantNameSafe = strApplicantName;
	strApplicantNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_REQUSER_INFO (?, \'%s\', \'%s\')}"),
		strApplicantNameSafe, strGuildNameSafe);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &bySubResult, 0, &cbParmRet);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
			{
				retcode = SQLFetch(hstmt);
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				{
					int i = 1;
					SQLGetData(hstmt, i++, SQL_C_SSHORT, &sLevel, sizeof(sLevel), &sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG, &iMoral, sizeof(iMoral), &sInd);
					SQLGetData(hstmt, i++, SQL_C_SSHORT, &sMageType, sizeof(sMageType), &sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iSwordExp,	sizeof(iSwordExp),	&sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iSpearExp,	sizeof(iSpearExp),	&sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iBowExp,	sizeof(iBowExp),	&sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iAxeExp,	sizeof(iAxeExp),	&sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iKnuckleExp,	sizeof(iKnuckleExp),	&sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iStaffExp,	sizeof(iStaffExp),	&sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iWeaponMakeExp,	sizeof(iWeaponMakeExp),	&sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iArmorMakeExp,	sizeof(iArmorMakeExp),	&sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iAccMakeExp,	sizeof(iAccMakeExp),	&sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iPotionMakeExp,	sizeof(iPotionMakeExp),	&sInd);
					SQLGetData(hstmt, i++, SQL_C_SLONG,	&iCookingExp,	sizeof(iCookingExp),	&sInd);
				}
				else
				{
					DisplayErrorMsg(hstmt);
					retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
					g_DB[m_iModSid].ReleaseDB(db_index);
					return false;
				}
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (bySubResult != 0) return false;

	UserInfo.sLevel = sLevel;
	UserInfo.iMoral = iMoral;
	UserInfo.sMageType = sMageType;
	UserInfo.iWeaponExp[TYPE_SWORD] = iSwordExp;
	UserInfo.iWeaponExp[TYPE_AXE] = iAxeExp;
	UserInfo.iWeaponExp[TYPE_BOW] = iBowExp;
	UserInfo.iWeaponExp[TYPE_SPEAR] = iSpearExp;
	UserInfo.iWeaponExp[TYPE_KNUCKLE] = iKnuckleExp;
	UserInfo.iWeaponExp[TYPE_STAFF] = iStaffExp;
	UserInfo.iMakeExp[0] = iWeaponMakeExp;
	UserInfo.iMakeExp[1] = iArmorMakeExp;
	UserInfo.iMakeExp[2] = iAccMakeExp;
	UserInfo.iMakeExp[3] = iPotionMakeExp;
	UserInfo.iMakeExp[4] = iCookingExp;

	return true;
}

bool USER::InsertGuildReq(TCHAR* strApplicantName, TCHAR* strGuildName, BYTE& bySubResult)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	// Make the guild strings safe for inserting into sql statement
	CString strGuildNameSafe = strGuildName;
	strGuildNameSafe.Replace("'", "''");

	CString strApplicantNameSafe = strApplicantName;
	strApplicantNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_INSERT_REQ (?, \'%s\', \'%s\')}"),
		strApplicantNameSafe, strGuildNameSafe);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &bySubResult, 0, &cbParmRet);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
			{
				SQLFetch(hstmt);
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (bySubResult != 0) return false;

	return true;
}

bool USER::RemoveGuildReq(TCHAR* strApplicantName, TCHAR* strGuildName, BYTE& bySubResult)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	// Make the guild strings safe for inserting into sql statement
	CString strGuildNameSafe = strGuildName;
	strGuildNameSafe.Replace("'", "''");

	CString strApplicantNameSafe = strApplicantName;
	strApplicantNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_REMOVE_REQ (?, \'%s\', \'%s\')}"),
		strApplicantNameSafe, strGuildNameSafe);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &bySubResult, 0, &cbParmRet);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
			{
				SQLFetch(hstmt);
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (bySubResult != 0) return false;

	return true;
}

bool USER::MoveGuildReq(TCHAR* strApplicantName, TCHAR* strGuildName, BYTE& bySubResult)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	// Make the guild strings safe for inserting into sql statement
	CString strGuildNameSafe = strGuildName;
	strGuildNameSafe.Replace("'", "''");

	CString strApplicantNameSafe = strApplicantName;
	strApplicantNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_MOVE_REQ (?, \'%s\', \'%s\')}"),
		strApplicantNameSafe, strGuildNameSafe);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &bySubResult, 0, &cbParmRet);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
			{
				SQLFetch(hstmt);
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (bySubResult != 0) return false;

	return true;
}

bool USER::ChangeGuildRank(TCHAR* strMemberName, TCHAR* strGuildName, short from, short to, BYTE& bySubResult)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	// Make the guild strings safe for inserting into sql statement
	CString strGuildNameSafe = strGuildName;
	strGuildNameSafe.Replace("'", "''");

	CString strMemberNameSafe = strMemberName;
	strMemberNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_CHANGE_RANK (?, \'%s\', \'%s\', %d, %d)}"),
		strMemberNameSafe, strGuildNameSafe, from, to);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &bySubResult, 0, &cbParmRet);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
			{
				SQLFetch(hstmt);
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (bySubResult != 0) return false;

	return true;
}

bool USER::UpdateGuildMoney(short sGuildNum, DWORD dwGuildBarr)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;
	bool			bQuerySuccess = true;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("UPDATE GUILD SET iGuildGold = %d WHERE GuildNum = %d"),
		dwGuildBarr, sGuildNum);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	return bQuerySuccess;
}

bool USER::RemoveGuildMember(TCHAR* strMemberName, TCHAR* strGuildName, BYTE& bySubResult)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	// Make the guild strings safe for inserting into sql statement
	CString strGuildNameSafe = strGuildName;
	strGuildNameSafe.Replace("'", "''");

	CString strMemberNameSafe = strMemberName;
	strMemberNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_REMOVE_USER (?, \'%s\', \'%s\')}"),
		strMemberNameSafe, strGuildNameSafe);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &bySubResult, 0, &cbParmRet);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
			{
				SQLFetch(hstmt);
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (bySubResult != 0) return false;

	return true;
}

bool USER::RemoveGuild(TCHAR* strChiefName, TCHAR* strGuildName, BYTE& bySubResult)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	bool			bQuerySuccess = true;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	// Make the guild strings safe for inserting into sql statement
	CString strGuildNameSafe = strGuildName;
	strGuildNameSafe.Replace("'", "''");

	CString strChiefNameSafe = strChiefName;
	strChiefNameSafe.Replace("'", "''");

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_REMOVE (?, \'%s\', \'%s\')}"),
		strChiefNameSafe, strGuildNameSafe);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_UTINYINT, SQL_TINYINT, 0, 0, &bySubResult, 0, &cbParmRet);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
			{
				SQLFetch(hstmt);
			}
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return false;
	if (bySubResult != 0) return false;

	return true;
}

bool USER::UpdateItemAll()
{
	if (m_State != STATE_GAMESTARTED) return false;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	TCHAR			szSQL[6000]; ZeroMemory(szSQL, sizeof(szSQL));
	TCHAR			Item[INV_ITEM_NUM][ITEM_DB_SIZE];

	for (int i = 0; i < INV_ITEM_NUM; i++)
	{
		ZeroMemory(Item[i], sizeof(Item[i]));
		UserItemToStr(&m_InvItem[i], Item[i]);
	}

	_sntprintf(szSQL, sizeof(szSQL), TEXT("UPDATE USERITEM SET Item00=?,Item01=?,Item02=?,Item03=?,Item04=?,Item05=?,Item06=?, \
		Item07=?,Item08=?,Item09=?,Item10=?,Item11=?,Item12=?,Item13=?,Item14=?,Item15=?,\
		Item16=?,Item17=?,Item18=?,Item19=?,Item20=?,Item21=?,Item22=?,Item23=?,Item24=?,\
		Item25=?,Item26=?,Item27=?,Item28=?,Item29=?,Item30=?,Item31=?,Item32=?,Item33=?,\
		Item34=?,Item35=?,Item36=?,Item37=?,Item38=?,Item39=?,Item40=?,Item41=?,Item42=?,\
		Item43=?,Item44=?,Item45=?,Item46=?,Item47=?,Item48=?,Item49=? WHERE strUserId=?"));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Update Item Data !!\n");
		return false;
	}

	// Bind params
	if (retcode == SQL_SUCCESS)
	{
		SQLINTEGER itemLen = ITEM_DB_SIZE;
		for (int i = 0; i < INV_ITEM_NUM; i++)
		{
			SQLBindParameter(hstmt, i+1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(Item[i]),	0, (TCHAR*)Item[i],		0, &itemLen);
		}

		SQLINTEGER userIdLen = strlen(m_strUserId);
		SQLBindParameter(hstmt, INV_ITEM_NUM+1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(m_strUserId),	0, (TCHAR*)m_strUserId,		0, &userIdLen);

		retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else
		{
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(db_index);
			return false;
		}
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	return true;
}

bool USER::UpdateBeltAll()
{
	if (m_State != STATE_GAMESTARTED) return false;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	TCHAR			szSQL[512]; ZeroMemory(szSQL, sizeof(szSQL));
	TCHAR			Item[BELT_ITEM_NUM][ITEM_DB_SIZE];

	for (int i = 0; i < BELT_ITEM_NUM; i++)
	{
		ZeroMemory(Item[i], sizeof(Item[i]));
		UserItemToStr(&m_BeltItem[i], Item[i]);
	}

	_sntprintf(szSQL, sizeof(szSQL), TEXT("UPDATE USERITEM SET Belt00=?, Belt01=?, \
		Belt02=?, Belt03=? WHERE strUserId=?"));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Update Belt Data !!\n");
		return false;
	}

	// Bind params
	if (retcode == SQL_SUCCESS)
	{
		SQLINTEGER itemLen = ITEM_DB_SIZE;
		for (int i = 0; i < BELT_ITEM_NUM; i++)
		{
			SQLBindParameter(hstmt, i+1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(Item[i]),	0, (TCHAR*)Item[i],		0, &itemLen);
		}

		SQLINTEGER userIdLen =  strlen(m_strUserId);;
		SQLBindParameter(hstmt, BELT_ITEM_NUM+1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(m_strUserId),	0, (TCHAR*)m_strUserId,		0, &userIdLen);

		retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else
		{
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(db_index);
			return false;
		}
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	return true;
}

bool USER::UpdateStorageAll()
{
	if (m_State != STATE_GAMESTARTED) return false;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	TCHAR			szSQL[9000]; ZeroMemory(szSQL, sizeof(szSQL));
	TCHAR			Item[STORAGE_ITEM_NUM][ITEM_DB_SIZE];

	for (int i = 0; i < STORAGE_ITEM_NUM; i++)
	{
		ZeroMemory(Item[i], sizeof(Item[i]));
		UserItemToStr(&m_StorageItem[i], Item[i]);
	}

	_sntprintf(szSQL, sizeof(szSQL), TEXT("UPDATE STORAGE_01 SET Item00=?,Item01=?,Item02=?,Item03=?,Item04=?,Item05=?,Item06=?, \
		Item07=?,Item08=?,Item09=?,Item10=?,Item11=?,Item12=?,Item13=?,Item14=?,Item15=?,\
		Item16=?,Item17=?,Item18=?,Item19=?,Item20=?,Item21=?,Item22=?,Item23=?,Item24=?, \
		Item25=?,Item26=?,Item27=?,Item28=?,Item29=?,Item30=?,Item31=?,Item32=?,Item33=?, \
		Item34=?,Item35=?,Item36=?,Item37=?,Item38=?,Item39=?,Item40=?,Item41=?,Item42=?, \
		Item43=?,Item44=?,Item45=?,Item46=?,Item47=?,Item48=?,Item49=?,Item50=?,Item51=?, \
		Item52=?,Item53=?,Item54=?,Item55=?,Item56=?,Item57=?,Item58=?,Item59=?,Item60=?, \
		Item61=?,Item62=?,Item63=?,Item64=?,Item65=?,Item66=?,Item67=?,Item68=?,Item69=?, \
		Item70=?,Item71=?,Item72=?,Item73=?,Item74=?,Item75=?,Item76=?,Item77=?,Item78=?, \
		Item79=? WHERE strUserId=?"));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Update Storage Data !!\n");
		return false;
	}

	// Bind params
	if (retcode == SQL_SUCCESS)
	{
		SQLINTEGER itemLen = ITEM_DB_SIZE;
		for (int i = 0; i < STORAGE_ITEM_NUM; i++)
		{
			SQLBindParameter(hstmt, i+1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(Item[i]),	0, (TCHAR*)Item[i],		0, &itemLen);
		}

		SQLINTEGER userIdLen = strlen(m_strUserId);
		SQLBindParameter(hstmt, STORAGE_ITEM_NUM+1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(m_strUserId),	0, (TCHAR*)m_strUserId,		0, &userIdLen);

		retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else
		{
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(db_index);
			return false;
		}
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	return true;
}

bool USER::GetStatus(CString &strStatus)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	bool bFind = false;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("SELECT strStatus FROM GAMEUSER WHERE strUserId=?"));

	SQLCHAR		strStatusTmp[STATUS_LENGTH+1];
	SQLINTEGER	strStatusTmpInd;

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	::ZeroMemory(strStatusTmp, STATUS_LENGTH+1);
	strStatusTmpInd = SQL_NTS;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("COULD NOT ALLOCATE HANDLE!\n");
		//		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	SQLINTEGER userIdLen = strlen(m_strUserId);
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(m_strUserId),	0, (TCHAR*)m_strUserId,		0, &userIdLen);

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLFetch(hstmt);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLGetData(hstmt, 1, SQL_C_CHAR, strStatusTmp, STATUS_LENGTH, &strStatusTmpInd);
		}
	}
	else
	{
		TRACE("Could not exec query!\n");
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		//BREAKPOINT();

		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	strStatus = strStatusTmp;

	return true;
}

bool USER::SetStatus(CString strStatus)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	bool bFind = false;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("UPDATE GAMEUSER SET strStatus=?, LastStatusUpdate=getdate() WHERE strUserId=?"));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("COULD NOT ALLOCATE HANDLE!\n");
		//		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	SQLINTEGER userIdLen = strlen(m_strUserId);
	SQLINTEGER strStatusLen = strStatus.GetLength();
	SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, STATUS_LENGTH,	0, (LPTSTR)(LPCTSTR)strStatus,		0, &strStatusLen);
	SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(m_strUserId),	0, (TCHAR*)m_strUserId,		0, &userIdLen);

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		TRACE("Could not exec query!\n");
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	return true;
}

bool USER::UpdateGuildSymbol(short sGuildNum, short sSymbolVersion, TCHAR* strSymbol)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;
	bool			bQuerySuccess = true;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("UPDATE GUILD SET sSymbolVersion = %d, strSymbol = ? WHERE GuildNum = %d"),
		sSymbolVersion, sGuildNum);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		SQLINTEGER symbolLen = GUILD_SYMBOL_SIZE;
		SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, GUILD_SYMBOL_SIZE,	0, strSymbol, 0, &symbolLen);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	return bQuerySuccess;
}

bool USER::LoadGuildStorageData(TCHAR *strGuildName)
{
	int j;
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[1024];

	CString strGuildNameSafe = strGuildName;
	strGuildNameSafe.Replace("'", "''");

	::ZeroMemory(szSQL, sizeof(szSQL));
	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call LOAD_GUILD_STORAGE_DATA (\'%s\')}"), strGuildNameSafe);

	SQLCHAR		Item[GUILD_STORAGE_ITEM_NUM][ITEM_DB_SIZE];

	SQLINTEGER	sInd;

	for (j = 0; j < GUILD_STORAGE_ITEM_NUM; j++)
		::ZeroMemory(Item[j], sizeof(Item[j]));

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);

	if (retcode != SQL_SUCCESS)
	{

		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLFetch(hstmt);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			int i = 2;
			for (j = 0; j < GUILD_STORAGE_ITEM_NUM; j++)
			{
				SQLGetData(hstmt, i++, SQL_C_BINARY,	Item[j],	sizeof(Item[j]),	&sInd);
			}
		}
		else if (retcode == SQL_NO_DATA)
		{
			g_DB[m_iModSid].ReleaseDB(db_index);
			return true;
		}
		else
		{
			g_DB[m_iModSid].ReleaseDB(db_index);
			return false;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	for (int i = 0; i < GUILD_STORAGE_ITEM_NUM; i++)
	{
		m_GuildItem[i] = StrToUserItem((char*)Item[i]);
	}

	return true;
}

bool USER::UpdateGuildStorage(TCHAR *strGuildName, short sGuildSlot, short sUserSlot)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	TCHAR			szSQL[512];
	TCHAR			Item[ITEM_DB_SIZE];

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Update Guild Item Data !!\n");
		return false;
	}

	// TODO: Transaction is not working.

	SQLSetConnectAttr(pDB->m_hdbc, SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_OFF, 0);

	// Bind params
	if (retcode == SQL_SUCCESS)
	{
		SQLINTEGER itemLen = ITEM_DB_SIZE;

		// Update user item inventory
		ZeroMemory(Item, sizeof(Item));
		UserItemToStr(&m_InvItem[sUserSlot], Item);

		ZeroMemory(szSQL, sizeof(szSQL));
		_sntprintf(szSQL, sizeof(szSQL), TEXT("UPDATE USERITEM SET Item%02d=? WHERE strUserId=?"), sUserSlot);

		SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(Item),	0, (TCHAR*)Item, 0, &itemLen);
		SQLINTEGER userIdLen = strlen(m_strUserId);
		SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, sizeof(m_strUserId), 0, (TCHAR*)m_strUserId, 0, &userIdLen);

		retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			SQLRETURN sqlRet = SQLEndTran(SQL_HANDLE_DBC, pDB->m_hdbc, SQL_ROLLBACK);
			SQLSetConnectAttr(pDB->m_hdbc, SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_ON, 0);
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(db_index);
			return false;
		}

		// Update guild item
		ZeroMemory(Item, sizeof(Item));
		UserItemToStr(&m_GuildItem[sGuildSlot], Item);

		ZeroMemory(szSQL, sizeof(szSQL));
		_sntprintf(szSQL, sizeof(szSQL), TEXT("UPDATE GUILD_STORAGE SET Item%02d=? WHERE strGuildName=?"), sGuildSlot);

		SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, sizeof(Item),	0, (TCHAR*)Item, 0, &itemLen);
		SQLINTEGER guildNameLen = strlen(strGuildName);
		SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, GUILD_NAME_LENGTH, 0, strGuildName, 0, &guildNameLen);

		retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
		{
			SQLRETURN sqlRet = SQLEndTran(SQL_HANDLE_DBC, pDB->m_hdbc, SQL_ROLLBACK);
			SQLSetConnectAttr(pDB->m_hdbc, SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_ON, 0);
			DisplayErrorMsg(hstmt);
			retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
			g_DB[m_iModSid].ReleaseDB(db_index);
			return false;
		}
	}

	SQLRETURN sqlRet = SQLEndTran(SQL_HANDLE_DBC, pDB->m_hdbc, SQL_COMMIT);
	SQLSetConnectAttr(pDB->m_hdbc, SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_ON, 0);

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);
	return true;
}

bool USER::UpdateGuildTownChange(short sTownNum, short sGuildNum, TCHAR* strGuildName)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;
	bool			bQuerySuccess = true;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call GUILD_TOWN_CHANGE(%d, %d, ?)}"),
		sGuildNum, sTownNum);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		SQLINTEGER symbolLen = GUILD_NAME_LENGTH;
		SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, GUILD_NAME_LENGTH,	0, strGuildName, 0, &symbolLen);

		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	return bQuerySuccess;
}

bool USER::UpdateGuildTownLevel(short sTownNum, short sLevel)
{
	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode = 0;
	TCHAR			szSQL[256];
	SQLINTEGER		cbParmRet = SQL_NTS;
	bool			bQuerySuccess = true;

	::ZeroMemory(szSQL, sizeof(szSQL));

	int index = 0;

	_sntprintf(szSQL, sizeof(szSQL), TEXT("UPDATE guild_town SET sLevel = %d WHERE sType = 1 and sTownNum = %d"),
		sLevel, sTownNum);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB) return false;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
		return false;

	if (retcode == SQL_SUCCESS)
	{
		retcode = SQLExecDirect(hstmt, (unsigned char *)szSQL, SQL_NTS);
		if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
		}
		else if (retcode==SQL_ERROR)
		{
			DisplayErrorMsg(hstmt);
			bQuerySuccess = false;
		}
	}

	if (hstmt!=NULL) SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	return bQuerySuccess;
}
