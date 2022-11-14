#include "stdafx.h"
#include "1p1emu.h"
#include "USER.h"
#include "COM.h"
#include "BufferEx.h"
#include "Extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void USER::BBSOpen(int iBBSNum)
{
	CBufferEx TempBuf;
	int iBBSCount = 0;
	int i = 0;

	bool bQuerySuccess = false;

	SQLHSTMT		hstmt = NULL;
	SQLRETURN		retcode;
	TCHAR			szSQL[256];	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call BBS_READ_INDEX (%d, -1)}"), iBBSNum);

	SQLCHAR	strWriter[NAME_LENGTH+1];	::ZeroMemory(strWriter, sizeof(strWriter));
	SQLCHAR	strTitle[50]; ::ZeroMemory(strTitle, sizeof(strTitle));
	SQLCHAR	strTime[30]; ::ZeroMemory(strTime, sizeof(strTime));

	SQLINTEGER	iIndex;
	SQLINTEGER	iInd;

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB)return;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("COULD NOT ALLOCATE HANDLE!\n");
		// g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (1)
		{
			retcode = SQLFetch(hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				bQuerySuccess = true;

				i = 1;
				SQLGetData(hstmt, i++, SQL_C_ULONG,	&iIndex,	sizeof(SQLINTEGER), &iInd);
				SQLGetData(hstmt, i++, SQL_C_BINARY,	strWriter,	sizeof(strWriter),	&iInd);
				SQLGetData(hstmt, i++, SQL_C_CHAR,		strTime,	sizeof(strTime),	&iInd);
				SQLGetData(hstmt, i++, SQL_C_BINARY,	strTitle,	sizeof(strTitle),	&iInd);

				TempBuf.Add((int)(iIndex));
				TempBuf.AddString((LPTSTR)strWriter);
				TempBuf.AddString((LPTSTR)strTime);
				TempBuf.AddString((LPTSTR)strTitle);

				::ZeroMemory(strWriter, sizeof(strWriter));
				::ZeroMemory(strTitle, sizeof(strTitle));
				::ZeroMemory(strTime, sizeof(strTime));

				iBBSCount++;
			}
			else if (retcode == SQL_NO_DATA)
			{
				bQuerySuccess = true;
				break;
			}
			else break;
		}
	}
	else
	{
		TRACE("Could not exec query!\n");
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);

		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess) return;

	int index = 0;
	SetByte(m_TempBuf, PKT_BBS_OPEN, index);
	SetShort(m_TempBuf, iBBSNum, index);
	SetByte(m_TempBuf, 0, index); // BBS NORMAL
	SetShort(m_TempBuf, iBBSCount, index);
	SetString(m_TempBuf, TempBuf, TempBuf.GetLength(), index);
	Send(m_TempBuf, index);
}

void USER::BBSNext(TCHAR *pBuf)
{
	if (pBuf == NULL)return;

	CBufferEx TempBuf;
	int i = 1;

	int index = 0;
	int	iBBSNum = GetShort(pBuf, index);
	int	iStart = GetInt(pBuf, index) - 1;
	if (iStart <= 0)
	{
		index = 0;
		SetByte(m_TempBuf, PKT_BBS_NEXT, index);
		SetShort(m_TempBuf, iBBSNum, index);
		SetShort(m_TempBuf, -1, index);
		Send(m_TempBuf, index);
		return;
	}

	int iBBSCount = 0;
	bool bQuerySuccess = false;

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call BBS_READ_INDEX (%d, %d)}"), iBBSNum, iStart);

	SQLCHAR		strWriter[NAME_LENGTH+1]; ::ZeroMemory(strWriter, sizeof(strWriter));
	SQLCHAR		strTitle[50]; ::ZeroMemory(strTitle, sizeof(strTitle));
	SQLCHAR		strTime[30]; ::ZeroMemory(strTime, sizeof(strTime));

	SQLINTEGER	iIndex;
	SQLINTEGER	iInd;

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB)return;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
//		TRACE("Fail To BBS Next(BBS = %d, Start = %d) !!\n", nBBS, nStart);
//		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (1)
		{
			retcode = SQLFetch(hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				bQuerySuccess = true;

				i = 1;
				SQLGetData(hstmt, i++, SQL_C_ULONG, &iIndex,	sizeof(SQLINTEGER), &iInd);
				SQLGetData(hstmt, i++, SQL_C_BINARY, strWriter,	sizeof(strWriter),	&iInd);
				SQLGetData(hstmt, i++, SQL_C_CHAR, strTime,	sizeof(strTime),	&iInd);
				SQLGetData(hstmt, i++, SQL_C_BINARY, strTitle,	sizeof(strTitle),	&iInd);

				TempBuf.Add((int)(iIndex));
				TempBuf.AddString((LPTSTR)strWriter);
				TempBuf.AddString((LPTSTR)strTime);
				TempBuf.AddString((LPTSTR)strTitle);

				::ZeroMemory(strWriter, sizeof(strWriter));
				::ZeroMemory(strTitle, sizeof(strTitle));
				::ZeroMemory(strTime, sizeof(strTime));

				iBBSCount++;
			}
			else if (retcode == SQL_NO_DATA)
			{
				bQuerySuccess = true;
				break;
			}
			else
				break;
		}
	}
	else if (retcode == SQL_ERROR)
	{
		bQuerySuccess = false;
		DisplayErrorMsg(hstmt);
		SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
//		g_DB[m_iModSid].ReleaseDB(db_index);
//		return;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess)
	{
		index = 0;
		SetByte(m_TempBuf, PKT_BBS_NEXT, index);
		SetShort(m_TempBuf, iBBSNum, index);
		SetShort(m_TempBuf, -1, index);
		Send(m_TempBuf, index);
		return;
	}

	index = 0;
	SetByte(m_TempBuf, PKT_BBS_NEXT, index);
	SetShort(m_TempBuf, iBBSNum, index);
	SetShort(m_TempBuf, iBBSCount, index);
	SetString(m_TempBuf, TempBuf, TempBuf.GetLength(), index);
	Send(m_TempBuf, index);
}

void USER::BBSRead(TCHAR *pBuf)
{
	if (pBuf == NULL)return;

	CBufferEx TempBuf;
	int i = 1;

	int index = 0;
	int iBBSNum = GetShort(pBuf, index);
	int iReadIndex = GetInt(pBuf, index);

	bool bQuerySuccess = false;

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[1024];		memset(szSQL, 0x00, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call BBS_READ_CONTENT (%d, %d)}"), iBBSNum, iReadIndex);

	SQLCHAR		strContent[8001];	memset(strContent, NULL, sizeof(strContent));

	SQLINTEGER	iIndex;
	SQLINTEGER	iInd;

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB)return;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Read BBS (BBS:%d,Index:%d) !!\n", iBBSNum, iReadIndex);
		return;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (1)
		{
			retcode = SQLFetch(hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				bQuerySuccess = true;

				i = 1;
				SQLGetData(hstmt, i++, SQL_C_ULONG,	&iIndex,	sizeof(SQLINTEGER), &iInd);
				SQLGetData(hstmt, i++, SQL_C_BINARY,	strContent,	sizeof(strContent),	&iInd);

				TempBuf.Add((int)(iIndex));
				TempBuf.AddLongString((LPTSTR)strContent, _tcslen((LPCTSTR)strContent));

				memset(strContent, NULL, sizeof(strContent));

				break;
			}
			else if (retcode == SQL_NO_DATA)
			{
				bQuerySuccess = true;
				break;
			}
			else break;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (!bQuerySuccess)
	{
		//if (g_bDebug) SendSystemMsg(IDS_USER_BBS_DELETED_ALREADY, SYSTEM_NORMAL, TO_ME);
		//SendBBSError(BBS_CONTENTS_DELETE);
		return;
	}

	if (!bQuerySuccess)return;

	index = 0;
	SetByte(m_TempBuf, PKT_BBS_READ, index);
	SetShort(m_TempBuf, iBBSNum, index);
	SetByte(m_TempBuf, 0, index); // Ignore for now
	SetString(m_TempBuf, TempBuf, TempBuf.GetLength(), index);
	Send(m_TempBuf, index);
}

void USER::BBSWrite(TCHAR *pBuf)
{
	if (pBuf == NULL)return;

	char strTitle[51];		::ZeroMemory(strTitle,		sizeof(strTitle));
	char strContent[8001];	::ZeroMemory(strContent,	sizeof(strContent));

	int index = 0;
	int iBBSNum = GetShort(pBuf, index);

	//if (m_tIsOP != 1 && (bbsnum == 1 || bbsnum == 3))		// 공지게시판 or 로열럼블 게시판
	//{
	//	SendSystemMsg(IDS_USER_NEED_OPERATOR, SYSTEM_NORMAL, TO_ME);
	//	return;
	//}

	int iLen = GetVarString(sizeof(strTitle), strTitle, pBuf, index);
	if (iLen <= 0 || iLen > 50) return;

	iLen = GetVarLongString(sizeof(strContent), strContent, pBuf, index);
	if (iLen <= 0 || iLen > 8000) return;

	SDWORD sTitleLen	= _tcslen(strTitle);
	SDWORD sContentLen	= _tcslen(strContent);
	SDWORD sIDLen		= _tcslen(m_strUserId);

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[8000];	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call BBS_WRITE (%d, ?, ?, ?)}"), iBBSNum);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB)return;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Write BBS (BBS:%d,Writer:%s,Title:%d) !!\n", iBBSNum, m_strUserId, strTitle);
		return;
	}

	int i = 1;
	SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 20,		0, (TCHAR*)m_strUserId,	0, &sIDLen);
	SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 50,		0, (TCHAR*)strTitle,	0, &sTitleLen);
	SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 8000,	0, (TCHAR*)strContent,	0, &sContentLen);

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
	}
	else if (retcode == SQL_ERROR)
	{
		DisplayErrorMsg(hstmt);
		SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	BBSOpen(iBBSNum);
}

void USER::BBSEdit(TCHAR *pBuf)
{
	if (pBuf == NULL) return;

	char strTitle[51];		::ZeroMemory(strTitle,		sizeof(strTitle));
	char strContent[8001];	::ZeroMemory(strContent,	sizeof(strContent));

	int index = 0;
	int iBBSNum = GetShort(pBuf, index);
	int iEditIndex = GetInt(pBuf, index);

	//if (m_tIsOP != 1 && (bbsnum == 1 || bbsnum == 3))		// 공지게시판 or 로열럼블 게시판
	//{
	//	SendSystemMsg(IDS_USER_NEED_OPERATOR, SYSTEM_NORMAL, TO_ME);
	//	return;
	//}

	int iLen = GetVarString(sizeof(strTitle), strTitle, pBuf, index);
	if (iLen <= 0 || iLen > 50) return;

	iLen = GetVarLongString(sizeof(strContent), strContent, pBuf, index);
	if (iLen <= 0 || iLen > 8000) return;

	SDWORD sTitleLen	= _tcslen(strTitle);
	SDWORD sContentLen	= _tcslen(strContent);
	SDWORD sIDLen		= _tcslen(m_strUserId);

	SQLSMALLINT	sRet = -1;
	SQLINTEGER	iRetInd = SQL_NTS;

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[8000];	::ZeroMemory(szSQL, sizeof(szSQL));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call BBS_EDIT (%d, %d, ?, ?, ?, ?)}"), iBBSNum, iEditIndex);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB)return;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Edit BBS (BBS:%d,Writer:%s,Index:%d) !!\n", iBBSNum, m_strUserId, iEditIndex);

		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	int i = 1;
	SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 20,		0, (TCHAR*)m_strUserId,	0, &sIDLen);
	SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 50,		0, (TCHAR*)strTitle,	0, &sTitleLen);
	SQLBindParameter(hstmt, i++, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 8000,	0, (TCHAR*)strContent,	0, &sContentLen);
	SQLBindParameter(hstmt, i++, SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT,	0,		0, &sRet,				0, &iRetInd);

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
		{
			SQLFetch(hstmt);
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	if (sRet == 2)
	{
		//if (g_bDebug) SendSystemMsg(IDS_USER_BBS_NO_PERMISSION, SYSTEM_NORMAL, TO_ME);
		//SendBBSError(BBS_CONTENTS_DELETE);
		return;
	}
	if (sRet == 3)
	{
		//if (g_bDebug) SendSystemMsg(IDS_USER_BBS_DELETED_ALREADY, SYSTEM_NORMAL, TO_ME);
		//SendBBSError(BBS_CONTENTS_DELETE);
		return;
	}

	BBSOpen(iBBSNum);
}

void USER::BBSDelete(TCHAR *pBuf)
{
	if (pBuf == NULL)return;

	int index = 0;
	int iBBSNum = GetShort(pBuf, index);
	int iDeleteIndex = GetInt(pBuf, index);

	//if (m_tIsOP != 1 && (bbsnum == 1 || bbsnum == 3))		//공지게시판 or 로열럼블 게시판
	//{
	//	SendSystemMsg(IDS_USER_NEED_OPERATOR, SYSTEM_NORMAL, TO_ME);
	//	return;
	//}

	SQLHSTMT	hstmt = NULL;
	SQLRETURN	retcode;
	TCHAR		szSQL[1024];	::ZeroMemory(szSQL, sizeof(szSQL));

	SQLSMALLINT	sRet = -1;
	SQLINTEGER	iRetInd = SQL_NTS;

	SDWORD sIDLen		= _tcslen(m_strUserId);

	_sntprintf(szSQL, sizeof(szSQL), TEXT("{call BBS_DELETE (%d, %d, ?)}"), iBBSNum, iDeleteIndex);

	int db_index = 0;
	CDatabase* pDB = g_DB[m_iModSid].GetDB(db_index);
	if (!pDB)return;

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Delete BBS (BBS:%d, Index:%d) !!\n", iBBSNum, iDeleteIndex);
		return;
	}

	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, 20, 0, (TCHAR*)m_strUserId,	0, &sIDLen);
	//retcode = SQLBindParameter(hstmt, 2 ,SQL_PARAM_OUTPUT,SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &sRet, 0, &iRetInd);
	if (retcode!=SQL_SUCCESS)
	{
		TRACE("USER::BBSDelete(), Line = %d, File = %s\n", __LINE__, __FILE__);
		return;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (SQLMoreResults(hstmt) != SQL_NO_DATA_FOUND)
		{
			SQLFetch(hstmt);
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[m_iModSid].ReleaseDB(db_index);
		return;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[m_iModSid].ReleaseDB(db_index);

	//if (sRet != 1)
	//{
		//if (g_bDebug) SendSystemMsg(IDS_USER_BBS_CANT_DELETE, SYSTEM_NORMAL, TO_ME);
		//SendBBSError(BBS_CONTENTS_DELETE);
	//	return;
	//}
	BBSOpen(iBBSNum);
}
