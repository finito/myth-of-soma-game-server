#pragma once

#include "Map.h"
#include "ThrowItem.h"

class USER;

typedef struct _SEND_DATA
{
	BOOL	used;
	BYTE	flag;
	int		len;
	int		uid;
	int		x;
	int		y;
	int		z;
	int		zone_index;
	CRect	rect;
	char	pBuf[MAX_PACKET_SIZE];
} SEND_DATA;

typedef CTypedPtrArray <CPtrArray, SEND_DATA*> SendDataArray;

class COM
{
public:
	COM();
	virtual ~COM();

	USER* GetUserUid(int uid);

	bool Init();
	void Send(USER *pUser, TCHAR* pBuf, int nLength);
	void Send(SEND_DATA *pData);
	void SendTimeToAll();
	void DelThrowItem();
	void DelThrowItem(int iThrowIndex, bool bCheckTime);
	void DebugSetThrowItem();
	BOOL SetThrowItem(ItemList *pItem, int x, int y,  int z);
	void InitThrowItemArray();
	void DeleteThrowItemArray();

public:
	bool m_bInit;
	CRITICAL_SECTION	m_critThrowItem;
	CRITICAL_SECTION	m_critGuild;
	CThrowItem*			m_ThrowItemArray[MAX_THROW_ITEM];
	LONG				m_ThrowAddIndex;
	DWORD				m_dwFullThrowCheckTime;
};
