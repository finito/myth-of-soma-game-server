#pragma once

typedef CTypedPtrArray <CPtrArray, int*> ZoneNumArray;

typedef struct _TOWNPORTAL
{
	short nItemNum;
	short nMoveZone;
	short nX;
	short nY;
} TOWNPORTAL;

typedef struct _TOWNPORTALFIXED
{
	short nZone;
	short nMoveZone;
	short nX;
	short nY;
} TOWNPORTALFIXED;


class SERVERINFO
{
public:
	SERVERINFO();
	virtual ~SERVERINFO();

	int m_sid;
	int m_sDBIndex;
	CString	m_szAddr;
	int	m_nPort;
	CString	m_strGTime;
	ZoneNumArray m_zone;
};
