#include "stdafx.h"
#include "1p1emu.h"
#include "USER.h"
#include "COM.h"
#include "Extern.h"
#include "1p1EmuDlg.h"
#include "Search.h"

#include <process.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

volatile long g_bShutDown = 0;
extern CSearch* g_pUserList;

COM::COM()
{
	m_bInit = false;
}

COM::~COM()
{
	if (m_bInit)
	{
		DeleteCriticalSection(&m_critThrowItem);
		DeleteCriticalSection(&m_critGuild);
		DeleteThrowItemArray();
	}
}

bool COM::Init()
{
	ASSERT(!m_bInit);
	InitializeCriticalSection(&m_critThrowItem);
	InitializeCriticalSection(&m_critGuild);
	InitThrowItemArray();
	m_dwFullThrowCheckTime = 0;
	m_bInit = true;
	return true;
}

USER* COM::GetUserUid(int uid)
{
	if (uid >= 0 && uid < MAX_USER)
	{
		return g_pUserList->GetUserUid(uid);
	}

	return NULL;
}

void COM::Send(SEND_DATA* pData)
{
	if (!pData)
	{
		return;
	}

	if (pData->len <= 0 || pData->len >= MAX_PACKET_SIZE)
	{
		TRACE("Data Length OverFlow - Send(SEND_DATA *pData)\n");
		return;
	}

	switch (pData->flag)
	{
	case SEND_USER:
		{
			USER* pUser = GetUserUid(pData->uid)
			if (!pUser ||
			  pUser->m_State == STATE_DISCONNECTED || pUser->m_State == STATE_LOGOUT)
			{
				return;
			}

			Send(pUser, pData->pBuf, pData->len);
		}
		break;
	case SEND_INSIGHT:
		{
			MAP* pMap = g_Zones[pData->zone_index];
			if (!pMap)
			{
				return;
			}

			const int sx = pData->x / SIGHT_SIZE_X;
			const int sy = pData->y / SIGHT_SIZE_Y;

			const int min_x = std::max(0, (sx - 1) * SIGHT_SIZE_X);
			const int max_x = std::min(pMap->m_sizeMap.cx - 1, (sx + 2) * SIGHT_SIZE_X);
			const int min_y = std::max(0, (sy - 1) * SIGHT_SIZE_X);
			const int max_y = std::min(pMap->m_sizeMap.cy - 1, (sy + 2) * SIGHT_SIZE_Y);

			for (int i = min_x; i < max_x; i++)
			{
				for (int j = min_y; j < max_y; j++)
				{
					const int temp_uid = pMap->m_pMap[i][j].m_lUser;
					if (temp_uid < USER_BAND || temp_uid >= NPC_BAND)
					{
						continue;
					}

					USER* pUser = GetUserUid(temp_uid - USER_BAND)
					if (!pUser || pUser->m_State != STATE_GAMESTARTED ||
					  pUser->m_sX != i || pUser->m_sY != j || pUser->m_sZ != pData->z)
					{
						continue;
					}

					Send(pUser, pData->pBuf, pData->len);
				}
			}
		}
		break;
	case SEND_RANGE:
		{
			MAP* pMap = g_Zones[pData->zone_index];
			if (!pMap)
			{
				return;
			}

			const int min_x = std::max(0, pData->rect.left);
			const int max_x = std::min(pMap->m_sizeMap.cx - 1, pData->rect.right);
			const int min_y = std::max(0, pData->rect.top);
			const int max_y = std::min(pMap->m_sizeMap.cy - 1, pData->rect.bottom);

			for (int i = min_x; i < max_x; i++)
			{
				for (int j = min_y; j < max_y; j++)
				{
					const int temp_uid = pMap->m_pMap[i][j].m_lUser;
					if (temp_uid < USER_BAND || temp_uid >= NPC_BAND)
					{
						continue;
					}

					USER* pUser = GetUserUid(temp_uid - USER_BAND)
					if (!pUser || pUser->m_State != STATE_GAMESTARTED ||
					  pUser->m_sX != i || pUser->m_sY != j || pUser->m_sZ != pData->z)
					{
						continue;
					}

					Send(pUser, pData->pBuf, pData->len);
				}
			}
		}
		break;
	case SEND_ZONE:
		for (int i = 0; i < MAX_USER; i++)
		{
			USER* pUser = g_pUserList->GetUserUid(i);
			if (pUser && pUser->m_State == STATE_GAMESTARTED &&
			  pData->z == pUser->m_sZ)
			{
				Send(pUser, pData->pBuf, pData->len);
			}
		}
		break;

	case SEND_ALL:
		for (int i = 0; i < MAX_USER; i++)
		{
			USER* pUser = g_pUserList->GetUserUid(i);
			if (pUser && pUser->m_State == STATE_GAMESTARTED)
			{
				Send(pUser, pData->pBuf, pData->len);
			}
		}
		break;

	case SEND_SCREEN:
		{
			MAP* pMap = g_Zones[pData->zone_index];
			if (!pMap)
			{
				return;
			}

			const int min_x = std::max(0, pData->x - 16);
			const int max_x = std::min(pMap->m_sizeMap.cx - 1, pData->x + 16);
			const int min_y = std::max(0, pData->y - 16);
			const int max_y = std::min(pMap->m_sizeMap.cy - 1, pData->y + 16);;

			for (int i = min_x; i < max_x; i++)
			{
				for (int j = min_y; j < max_y; j++)
				{
					if ((abs(pData->x - i) + abs(pData->y - j)) > 16)
					{
						continue;
					}

					const int temp_uid = pMap->m_pMap[i][j].m_lUser;
					if (temp_uid < USER_BAND || temp_uid >= NPC_BAND)
					{
						continue;
					}

					USER* pUser = GetUserUid(temp_uid - USER_BAND)
					if (!pUser || pUser->m_State != STATE_GAMESTARTED)
					{
						continue;
					}

					Send(pUser, pData->pBuf, pData->len);
				}
			}
		}
		break;
	default:
		break;
	}
}

void COM::Send(USER* pUser, TCHAR* pBuf, int nLength)
{
	if (!pUser)
	{
		return;
	}

	pUser->Send(pBuf, nLength);
}

void COM::SendTimeToAll()
{
	for (int i = 0; i < MAX_USER; i++)
	{
		USER* pUser = g_pUserList->GetUserUid(i);
		if (!pUser || pUser->m_State != STATE_GAMESTARTED)
		{
			continue;
		}

		pUser->SendTime();
	}
}

void COM::DebugSetThrowItem()
{
	MAP* pMap = g_Zones[0];
	for (int x = 0; x < pMap->m_sizeMap.cx; x++)
	{
		for (int y = 0; y < pMap->m_sizeMap.cy; y++)
		{
			if (g_Zones[0]->m_pMap[x][y].iIndex != -1)
			{
				continue;
			}

			int itemno = myrand(1, 2000);
			CItemTable* pItemTable = NULL;
			while (!g_mapItemTable.Lookup(itemno, pItemTable))
			{
				itemno = myrand(1, 2000);
			}

			ItemList* pItem = new ItemList;
			pItem->InitFromItemTable(itemno);
			m_ThrowItemArray[m_ThrowAddIndex]->m_pItem = pItem;
			m_ThrowItemArray[m_ThrowAddIndex]->m_z = 0;
			m_ThrowItemArray[m_ThrowAddIndex]->m_x = x;
			m_ThrowItemArray[m_ThrowAddIndex]->m_y = y;
			m_ThrowItemArray[m_ThrowAddIndex]->dwTime = GetTickCount();
			pMap->m_pMap[x][y].iIndex = m_ThrowAddIndex;

			m_ThrowAddIndex++;
			if (m_ThrowAddIndex >= MAX_THROW_ITEM)
			{
				m_ThrowAddIndex = 0;
			}
		}
	}
}

BOOL COM::SetThrowItem(ItemList* pItem, int x, int y, int z)
{
	if (!pItem)
	{
		return FALSE;
	}

	MAP* pAddMap = g_Zones[z];
	if (!pAddMap)
	{
		return FALSE;
	}

	if (pItem->IsEmpty())
	{
		return FALSE;
	}

	CPoint t = ConvertToClient(x, y, pAddMap->m_vMoveCell.m_vDim.cx, pAddMap->m_vMoveCell.m_vDim.cy);
	if (t.x == -1 || t.y == -1)
	{
		return FALSE;
	}

	EnterCriticalSection(&m_critThrowItem);

	m_ThrowItemArray[m_ThrowAddIndex]->m_pItem = pItem;
	m_ThrowItemArray[m_ThrowAddIndex]->m_z = z;
	m_ThrowItemArray[m_ThrowAddIndex]->m_x = x;
	m_ThrowItemArray[m_ThrowAddIndex]->m_y = y;
	m_ThrowItemArray[m_ThrowAddIndex]->dwTime = GetTickCount();

	pAddMap->m_pMap[x][y].iIndex = m_ThrowAddIndex;

	m_ThrowAddIndex++;
	if (m_ThrowAddIndex >= MAX_THROW_ITEM)
	{
		m_ThrowAddIndex = 0;
	}

	LeaveCriticalSection(&m_critThrowItem);

	int index = 0;
	char temp_send[1024];

	SetByte(temp_send, PKT_ITEM_FIELD_INFO, index);
	SetShort(temp_send, 1, index);
	SetByte(temp_send, ITEM_INFO_MODIFY, index);

	SetShort(temp_send, t.x, index);
	SetShort(temp_send, t.y, index);

	if (pItem->bType == TYPE_MONEY)
	{
		if (pItem->dwMoney >= 100 && pItem->dwMoney < 1000)
		{
			pItem->sPicNum = 178;
		}
		else if (pItem->dwMoney >= 1000)
		{
			pItem->sPicNum = 179;
		}

		SetShort(temp_send, pItem->sPicNum, index);
		SetByte(temp_send, pItem->bType, index);
		SetDWORD(temp_send, pItem->dwMoney, index);
		SetVarString(temp_send, (LPTSTR)(LPCTSTR)pItem->strNewName, pItem->strNewName.GetLength(), index);
	}
	else
	{
		SetShort(temp_send, pItem->sPicNum, index);
		SetByte(temp_send, pItem->bType, index);
		SetDWORD(temp_send, pItem->sUsage, index);
		SetVarString(temp_send, (LPTSTR)(LPCTSTR)pItem->strNewName, pItem->strNewName.GetLength(), index);
	}

	SEND_DATA* pNewData = new SEND_DATA;
	if (!pNewData)
	{
		return TRUE;
	}

	pNewData->flag = SEND_INSIGHT;
	pNewData->len = index;

	memcpy(pNewData->pBuf, temp_send, index);

	pNewData->uid = 0;
	pNewData->x = x;
	pNewData->y = y;
	pNewData->z = pAddMap->m_Zone;
	pNewData->zone_index = z;

	Send(pNewData);
	delete pNewData;

	return TRUE;
}

void COM::DelThrowItem()
{
	if (GetTickCount() - m_dwFullThrowCheckTime > 5000)
	{
		for (int i = 0; i < MAX_THROW_ITEM; i++)
		{
			DelThrowItem(i, true);
		}

		m_dwFullThrowCheckTime = GetTickCount();
	}

	DelThrowItem(m_ThrowAddIndex, false);
}

void COM::DelThrowItem(int iThrowIndex, bool bCheckTime)
{
	EnterCriticalSection(&m_critThrowItem);
	ItemList* pThrowItem = m_ThrowItemArray[iThrowIndex]->m_pItem;
	if (!pThrowItem)
	{
		LeaveCriticalSection(&m_critThrowItem);
		return;
	}

	if (bCheckTime && GetTickCount() - m_ThrowItemArray[iThrowIndex]->dwTime <= 1200000)
	{
		LeaveCriticalSection(&m_critThrowItem);
		return;
	}

	const int z = m_ThrowItemArray[iThrowIndex]->m_z;
	if (z < 0 || z >= g_Zones.GetSize())
	{
		TRACE("DelThrowItem: Invalid Zone Index: %d\n", z);
		LeaveCriticalSection(&m_critThrowItem);
		return;
	}

	MAP* pDelMap = g_Zones[z];
	if (!pDelMap)
	{
		LeaveCriticalSection(&m_critThrowItem);
		return;
	}

	const int x = m_ThrowItemArray[iThrowIndex]->m_x;
	const int y = m_ThrowItemArray[iThrowIndex]->m_y;
	if (x <= -1 || x >= pDelMap->m_sizeMap.cx ||  y <= -1 || y >= pDelMap->m_sizeMap.cy)
	{
		LeaveCriticalSection(&m_critThrowItem);
		return;
	}

	CPoint t = ConvertToClient(x, y, pDelMap->m_vMoveCell.m_vDim.cx, pDelMap->m_vMoveCell.m_vDim.cy);
	if (t.x == -1 || t.y == -1)
	{
		LeaveCriticalSection(&m_critThrowItem);
		return;
	}

	pDelMap->m_pMap[x][y].iIndex = -1;
	m_ThrowItemArray[iThrowIndex]->m_pItem = NULL;

	int index = 0;
	char temp_send[9];

	SetByte(temp_send, PKT_ITEM_FIELD_INFO, index);
	SetShort(temp_send, 1, index);
	SetByte(temp_send, ITEM_INFO_DELETE, index);
	SetShort(temp_send, t.x, index);
	SetShort(temp_send, t.y, index);

	delete pThrowItem;
	LeaveCriticalSection(&m_critThrowItem);

	SEND_DATA* pNewData = new SEND_DATA;
	if (!pNewData)
	{
		return;
	}

	pNewData->flag = SEND_INSIGHT;
	pNewData->len = index;

	memcpy(pNewData->pBuf, temp_send, index);

	pNewData->uid = 0;
	pNewData->x = x;
	pNewData->y = y;
	pNewData->z = pDelMap->m_Zone;
	pNewData->zone_index = z;

	Send(pNewData);
	delete pNewData;
}

void COM::InitThrowItemArray()
{
	m_ThrowAddIndex = 0;

	for (int i = 0; i < MAX_THROW_ITEM; i++)
	{
		m_ThrowItemArray[i] = new CThrowItem;
	}
}

void COM::DeleteThrowItemArray()
{
	for (int i = 0; i < MAX_THROW_ITEM; i++)
	{
		delete m_ThrowItemArray[i];
		m_ThrowItemArray[i] = NULL;
	}
}
