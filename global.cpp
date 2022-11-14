#include "stdafx.h"
#include "global.h"

int myrand(int min, int max, BOOL bSame)
{
	if (min == max)return min;
	if (min > max)
	{
		int temp = min;
		min = max;
		max = temp;
	}
	double gap = max - min + 1;
	double rrr = (double)RAND_MAX / gap;

	double rand_result;

	rand_result = (double)rand() / rrr;

	if ((int)(min + (int)rand_result)< min)return min;
	if ((int)(min + (int)rand_result)> max)return max;

	return (int)(min + (int)rand_result);
	//return rand()%(max-min)+min;
}

//////////////////////////////////////////////////////////////////////
//	Display Error Message in Stored Procdure (SQL)
//
void _DisplayErrorMsg(SQLHANDLE hstmt, LPCTSTR strFile, int nLine)
{
	SQLCHAR			SqlState[256], Msg[1024];
	SQLINTEGER		NativeError;
	SQLSMALLINT		i, MsgLen;
	SQLRETURN		rc2;

	i = 1;
	while ((rc2 = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, i, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen)) != SQL_NO_DATA)
	{
		if (MsgLen > 256) Msg[256] = '\0';
		TRACE("***[ %s, %d ] %s, %d, %s, %d ***\n", strFile, nLine, SqlState,NativeError,Msg,MsgLen);

		i++;
	}
}

CString _ID(WORD string_id)
{
	CString returnString;
	returnString.LoadString(string_id);
	return returnString;
}

int _GetVarString(int iMax, TCHAR* tBuf, TCHAR* sBuf, int& index, char* strFn, int nLine)
{
	int nLen = GetByte(sBuf, index);
	if (nLen >= iMax || nLen < 0) return 0;

	_GetString(tBuf, sBuf, nLen, index, strFn, nLine);
	*(tBuf + nLen) = 0;

	return nLen;
}

int _GetVarLongString(int iMax, TCHAR* tBuf, TCHAR* sBuf, int& index, char* strFn, int nLine)
{
	int nLen = GetShort(sBuf, index);
	if (nLen >= iMax || nLen < 0) return 0;

	_GetString(tBuf, sBuf, nLen, index, strFn, nLine);
	*(tBuf + nLen) = 0;

	return nLen;
}

void _GetString(char* tBuf, char* sBuf, int len, int& index, char* strFn, int nLine)
{
	char tempStr[512];
	memset(tempStr, 0, sizeof(tempStr));
	wsprintf(tempStr, "File = %s, Line = %d", strFn, nLine);

	memcpy(tBuf, sBuf+index, len);
	index += len;
}

int ParseSpace(char* tBuf, char* sBuf)
{
	int i = 0, index = 0;
	BOOL flag = FALSE;

	while (sBuf[index] == ' ' || sBuf[index] == '\t')index++;
	while (sBuf[index] !=' ' && sBuf[index] !='\t' && sBuf[index] !=(BYTE) 0){
		tBuf[i++] = sBuf[index++];
		flag = TRUE;
	}
	tBuf[i] = 0;

	while (sBuf[index] == ' ' || sBuf[index] == '\t')index++;
	if (!flag) return 0;
	return index;
}

int ParseSpaceInUser(char* tBuf, char* sBuf, int iSize)
{
	int i = 0, index = 0;
	BOOL flag = FALSE;
	BOOL bNext = FALSE;

	if (iSize == 0)
	{
		tBuf[0] = 0;
		return 0;
	}

	while (sBuf[index] == ' ' || sBuf[index] == '\t')
	{
		if (iSize > index) index++;
		else break;
	}

	while (sBuf[index] !=' ' && sBuf[index] !='\t' && sBuf[index] !=(BYTE) 0)
	{
		if (iSize > index)
		{
			tBuf[i++] = sBuf[index++];
			flag = TRUE;
		}
		else break;
	}

	tBuf[i] = 0;

	if (!flag)
	{
		tBuf[0] = 0;
		return 0;
	}

	return index;
}

CString GetProgPath()
{
	char Buf[256], Path[256];
	char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

	::GetModuleFileName(AfxGetApp()->m_hInstance, Buf, 256);
	_splitpath(Buf,drive,dir,fname,ext);
	strcpy(Path, drive);
	strcat(Path, dir);
	CString _Path = Path;
	return _Path;
}

CPoint ConvertToClient(int x, int y, int cx, int cy)
{
	int tempx, tempy;
	int temph = cy / 2 - 1;

	tempx = x - temph + y;
	tempy = y - x + temph;

	if (tempy >= cy || tempx >= cx)return CPoint(-1,-1);

	return CPoint(tempx, tempy);
}

CPoint ConvertToServer(int x, int y, int cx, int cy)
{
	int tempx, tempy;
	int temph = cy / 2 - 1;

	if (y >= cy || x >= cx)return CPoint(-1,-1);

	if ((x+y)%2 == 0)
	{
		tempx = temph - (y / 2)+ (x / 2);

		if (x % 2)tempy = (y / 2)+ ((x / 2)+ 1);
		else        tempy = (y / 2)+ (x / 2);

		return CPoint(tempx, tempy);
	}
	else return CPoint(-1,-1);
}

void CheckMaxValue(DWORD& dest, DWORD add)
{
	DWORD Diff = _MAX_DWORD - dest;

	if (add <= Diff) dest += add;
	else dest = _MAX_DWORD;
}

void CheckMaxValue(int& dest, int add)
{
	int result = dest + add;
	int sign = dest >= 0;
	if (sign == (add >= 0) && sign != (result >= 0))
	{
		dest = sign ? _MAX_INT : -2147483647;
	}
	else
	{
		dest += add;
	}
}

void CheckMaxValue(short& dest, short add)
{
	int result = dest + add;
	int sign = dest >= 0;
	if (sign == (add >= 0) && sign != (result >= 0))
	{
		dest = sign ? _MAX_SHORT : -32767;
	}
	else
	{
		dest += add;
	}
}

BOOL CheckMaxValueReturn(DWORD& dest, DWORD add)
{
	DWORD Diff = _MAX_DWORD - dest;
	if (add <= Diff) return TRUE;
	else return FALSE;
}
