// UserMisc.cpp: implementation of the misc of USER class.
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
extern NpcThreadArray g_arNpcThread;

CPoint USER::ConvertToServer(int x, int y, int z /* = -1 */)
{
	if (z != -1) z = GetZoneIndex(z);
	else z = m_iZoneIndex;

	if (z < 0 || z >= g_Zones.GetSize()) return CPoint(-1,-1);

	int tempx, tempy;
	int temph = g_Zones[z]->m_vMoveCell.m_vDim.cy / 2 - 1;

	if (x <= -1 || y <= -1) return CPoint(-1,-1);

	if (y >= g_Zones[z]->m_vMoveCell.m_vDim.cy ||
		x >= g_Zones[z]->m_vMoveCell.m_vDim.cx)
	{
		return CPoint(-1,-1);
	}

	if ((x+y)%2 == 0)
	{
		tempx = temph - (y / 2) + (x / 2);

		if (x % 2) tempy = (y / 2) + ((x / 2) + 1);
		else        tempy = (y / 2) + (x / 2);

		return CPoint(tempx, tempy);
		return CPoint(x, y);
	}

	return CPoint(-1,-1);
}

CPoint USER::ConvertToClient(int x, int y, int z /* = -1 */)
{
	if (z != -1) z = GetZoneIndex(z);
	else z = m_iZoneIndex;

	if (z < 0 || z >= g_Zones.GetSize()) return CPoint(-1,-1);
	if (!g_Zones[z]) return CPoint(-1, -1);

	int tempx, tempy;
	int temph = g_Zones[z]->m_vMoveCell.m_vDim.cy / 2 - 1;

	if (x <= -1 || y <= -1) return CPoint(-1,-1);
	if (y >= g_Zones[z]->m_sizeMap.cy || x >= g_Zones[z]->m_sizeMap.cx) return CPoint(-1,-1);

	tempx = x - temph + y;
	tempy = y - x + temph;

	return CPoint(tempx, tempy);
}

CPoint USER::FindRandPointInRect_C(int z, int x, int y, int x2, int y2)
{
	// Looping 10 times should be enough to find a free spot!!
	// If not...then eeeeeeeek lots of users!
	for (int i = 0; i < 10; i++)
	{
		int iRandX = myrand(x, x2);
		int iRandY = myrand(y, y2);

		if (IsMovable_C(iRandX, iRandY, z))
		{
			return CPoint(iRandX, iRandY);
		}
	}

	return CPoint(-1, -1);
}


bool USER::IsMovable_C(int x, int y, int z)
{
	int iZoneIndex = m_iZoneIndex;
	if (z != -1) iZoneIndex = GetZoneIndex(z);

	CPoint pt = ConvertToServer(x, y, z);
	if (pt.x == -1 || pt.y == -1)
	{
		return false;
	}

	x = pt.x;
	y = pt.y;

	if (iZoneIndex < 0 || iZoneIndex >= g_Zones.GetSize()) return false;
	if (!g_Zones[iZoneIndex]) return false;
	if (x >= g_Zones[iZoneIndex]->m_sizeMap.cx || x < 0 ||
		y >= g_Zones[iZoneIndex]->m_sizeMap.cy || y < 0)
	{
		return false;
	}

	if (g_Zones[iZoneIndex]->m_pMap[x][y].m_bMove == MAP_NON_MOVEABLE) return false;
	int nUid = g_Zones[iZoneIndex]->m_pMap[x][y].m_lUser;
	if (nUid != 0 && nUid != m_Uid + USER_BAND)
	{
		if (nUid >= NPC_BAND && nUid < INVALID_BAND)
		{
			CNpc *pNpc = GetNpc(nUid - NPC_BAND);
			if (!pNpc)
			{
				g_Zones[iZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return true;
			}
			if (pNpc->m_sCurX != x || pNpc->m_sCurY != y)
			{
				g_Zones[iZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return true;
			}
		}
		else if (nUid >= USER_BAND && nUid < NPC_BAND)
		{
			if (nUid - USER_BAND < 0 || nUid - USER_BAND >= MAX_USER)
			{
				g_Zones[iZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return true;
			}

			USER *pUser = g_pUserList->GetUserUid(nUid - USER_BAND);
			if (!pUser || pUser->m_State != STATE_GAMESTARTED)
			{
				g_Zones[iZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return true;
			}

			if (pUser->m_sX != x || pUser->m_sY != y || pUser->m_State != STATE_GAMESTARTED)
			{
				g_Zones[iZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return true;
			}
		}

		return false;
	}

	return true;
}

USER* USER::GetUserId(TCHAR* id)
{
	if (!strlen(id)) return NULL;

	CString szSource = id;
	USER *pUser;

	for (int i = 0; i < MAX_USER; i++)
	{
		pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) continue;

		if (pUser->m_State == STATE_DISCONNECTED || pUser->m_State == STATE_LOGOUT) continue;

		if (!szSource.CompareNoCase(pUser->m_strUserId)) return pUser;
	}

	return NULL;
}

void USER::SetZoneIndex(int zone)
{
	for (int i = 0; i < g_Zones.GetSize(); i++)
	{
		if (g_Zones[i]->m_Zone == zone)
		{
			m_iZoneIndex = i;
			break;
		}
	}
}

int USER::GetZoneIndex(int iZone)
{
	int iZoneIndex = -1;
	for (int i = 0; i < g_Zones.GetSize(); i++)
	{
		MAP* pMap = g_Zones[i];
		if (pMap->m_Zone == iZone)
		{
			iZoneIndex = i;
			break;
		}
	}

	return iZoneIndex;
}

CPoint USER::FindNearAvailablePoint_S(int x, int y, int iDistance)
{
	if (x <= -1 || y <= -1) return CPoint(-1,-1);
	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize()) return CPoint(-1,-1);
	if (x >= g_Zones[m_iZoneIndex]->m_sizeMap.cx || y >= g_Zones[m_iZoneIndex]->m_sizeMap.cy) return CPoint(-1,-1);

	if (IsMovable_S(x, y))
	{
		return CPoint(x,y);
	}

	int iAdd = 0;
	for (int i = 0; i <= iDistance; i++)
	{
		int iStepAdd = iAdd;
		int iCheckX = iAdd + x;
		for (int j = iStepAdd; j <= i; j++)
		{
			int iCheckY = iAdd + y;
			for (int k = iAdd; k <= i; k++)
			{
				if (abs(iStepAdd) >= i || abs(k) >= i)
				{
					if (IsMovable_S(iCheckX, iCheckY))
					{
						return CPoint(iCheckX, iCheckY);
					}
				}
				iCheckY++;
			}
			iStepAdd++;
			iCheckX++;
		}
		iAdd--;
	}
	return CPoint(-1,-1);
}

CPoint USER::FindNearAvailablePoint_C(int x, int y, int iDistance)
{
	if (IsMovable_C(x, y))
	{
		return CPoint(x,y);
	}

	CPoint pt = ConvertToServer(x, y);
	x = pt.x, y = pt.y;

	if (x <= -1 || y <= -1) return CPoint(-1,-1);
	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize()) return CPoint(-1,-1);
	if (x >= g_Zones[m_iZoneIndex]->m_sizeMap.cx || y >= g_Zones[m_iZoneIndex]->m_sizeMap.cy) return CPoint(-1,-1);

	int iAdd = 0;
	for (int i = 0; i <= iDistance; i++)
	{
		int iStepAdd = iAdd;
		int iCheckX = iAdd + x;
		for (int j = iStepAdd; j <= i; j++)
		{
			int iCheckY = iAdd + y;
			for (int k = iAdd; k <= i; k++)
			{
				if (abs(iStepAdd) >= i || abs(k) >= i)
				{
					if (IsMovable_S(iCheckX, iCheckY))
					{
						return ConvertToClient(iCheckX, iCheckY);
					}
				}
				iCheckY++;
			}
			iStepAdd++;
			iCheckX++;
		}
		iAdd--;
	}
	return CPoint(-1,-1);
}

bool USER::IsMovable_S(int x, int y)
{
	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize()) return false;

	if (!g_Zones[m_iZoneIndex]) return false;
	if (!g_Zones[m_iZoneIndex]->m_pMap) return false;
	if (x >= g_Zones[m_iZoneIndex]->m_sizeMap.cx || x < 0 || y >= g_Zones[m_iZoneIndex]->m_sizeMap.cy || y < 0) return false;

	CPoint t = ConvertToClient(x, y);
	if (t.x == -1 || t.y == -1) return false;

	if (g_Zones[m_iZoneIndex]->m_pMap[x][y].m_bMove == MAP_NON_MOVEABLE) return false;

	int nUid = g_Zones[m_iZoneIndex]->m_pMap[x][y].m_lUser;
	if (nUid != 0 && nUid != m_Uid + USER_BAND)
	{
		if (nUid >= NPC_BAND && nUid < INVALID_BAND)
		{
			CNpc *pNpc = GetNpc(nUid - NPC_BAND);
			if (!pNpc)
			{
				g_Zones[m_iZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return true;
			}
			if (pNpc->m_sCurX != x || pNpc->m_sCurY != y)
			{
				g_Zones[m_iZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return true;
			}
		}
		else if (nUid >= USER_BAND && nUid < NPC_BAND)
		{
			if (nUid - USER_BAND < 0 || nUid - USER_BAND >= MAX_USER)
			{
				g_Zones[m_iZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return true;
			}

			USER *pUser = g_pUserList->GetUserUid(nUid - USER_BAND);
			if (!pUser || pUser->m_State != STATE_GAMESTARTED)
			{
				g_Zones[m_iZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return true;
			}

			if (pUser->m_sX != x || pUser->m_sY != y || pUser->m_State != STATE_GAMESTARTED)
			{
				g_Zones[m_iZoneIndex]->m_pMap[x][y].m_lUser = 0;
				return true;
			}
		}

		return false;
	}

	return true;
}

bool USER::Move_C(int x, int y, int nDist /* = 1 */)
{
	CPoint t = ConvertToServer(x, y);
	if (t.x == -1 || t.y == -1) return false;

	if (abs(m_sX - t.x) > nDist || abs(m_sY - t.y) > nDist) return false;

	if (!IsMovable_S(t.x, t.y)) return false;
	if (t.x == m_sX && t.y == m_sY) return false;

	if (g_Zones[m_iZoneIndex]->m_pMap[m_sX][m_sY].m_lUser == 0 ||
		g_Zones[m_iZoneIndex]->m_pMap[m_sX][m_sY].m_lUser == m_Uid + USER_BAND)
	{
		//::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[m_curx][m_cury].m_lUser, 0);
		g_Zones[m_iZoneIndex]->m_pMap[m_sX][m_sY].m_lUser = 0;
	}
	//::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[t.x][t.y].m_lUser, m_uid + USER_BAND);
	g_Zones[m_iZoneIndex]->m_pMap[t.x][t.y].m_lUser = m_Uid + USER_BAND;

	m_sX = static_cast<short>(t.x);
	m_sY = static_cast<short>(t.y);

	// Guild Town War, Change user guild town war type
	// if the guild town war is currently in progress
	// it changes the name color on the client.
	short sTownNum = -1;
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_bGuildTownWar[i] && IsMapRange(g_iGuildTownRange[i]))
		{
			sTownNum = i + 1;
			break;
		}
	}

	short sGuildTownWarType = -1;
	if (sTownNum != -1)
	{
		if (CheckGuildTown(sTownNum) || CheckGuildTownAlly(sTownNum))
			sGuildTownWarType = 1;
		else
			sGuildTownWarType = 2;
	}

	if (m_sGuildTownWarType != sGuildTownWarType)
	{
		m_sGuildTownWarType = sGuildTownWarType;
		SendMyInfo(TO_ME, INFO_MODIFY);
	}

	return true;
}

short USER::GetStm()
{
	// Recovery of stamina when standing still[5 sp every second]. [Recovery when walking is done within the 'Move' function.]
	DWORD dwTickCount = GetTickCount();
	int nTimeGap;
	if (m_bCanRecoverStm)
	{
		nTimeGap = dwTickCount - m_dwLastRecoverStmTime;
	}
	else
	{
		nTimeGap = dwTickCount - m_dwLastRecoverStmTime - 1600; // 1600 is what was in the client
	}

	if (nTimeGap >= 0)
	{
		if (!m_bCanRecoverStm)
		{
			m_dwLastRecoverStmTime = dwTickCount;
			m_bCanRecoverStm = true;
		}
		if (m_sStm < GetMaxStm() && !m_bRunMode)
		{
			if (nTimeGap >= 1000)
			{
				IncStm((nTimeGap/1000)*5);
				m_dwLastRecoverStmTime = dwTickCount;
			}
		}
		else
		{
			m_dwLastRecoverStmTime = dwTickCount;
		}
	}

	return m_sStm;
}

void USER::IncStm(short sInc)
{
	m_sStm += sInc;
	if (m_sStm > GetMaxStm()) m_sStm = GetMaxStm();
}

short USER::DecStm(short sDec)
{
	m_sStm -= sDec;
	m_bCanRecoverStm = false;
	if (m_sStm < 0) m_sStm = 0;

	return m_sStm;
}

bool USER::GetDistance(int xpos, int ypos, int dist, int* ret)
{
	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize()) return false;
	if (xpos >= g_Zones[m_iZoneIndex]->m_sizeMap.cx || xpos < 0 ||
		ypos >= g_Zones[m_iZoneIndex]->m_sizeMap.cy || ypos < 0) return false;

	// With 1 co-ord per sqaure, the highest difference between the 2 co-ords (either x or y) is
	// the distance.
	double dx = abs(m_sX - xpos);
	double dy = abs(m_sY - ypos);

    if (dx > dist || dy > dist) return false;

	if (ret != NULL)
	{
		if (dx > dy)
			*ret = (int)dx;
		else
			*ret = (int)dy;
	}

	// Works for both client and server co-ords returning the distance as 1 per sqaure.. rather than
	// Clients distance = 2 per sqaure and servers 1 for some and 2 for others lol ;p
/*	double dx = (xpos - m_sX)/2;
	double dy = (ypos - m_sY)/2;
	int distance = (int)ceil(abs(dx) + abs(dy));
	if (distance > dist) return false;
	if (ret != NULL) *ret = distance;*/

	return true;
}

void USER::SendTime()
{
	int index = 0;
	SetByte(m_TempBuf, PKT_SET_TIME, index);
	SetShort(m_TempBuf, g_pMainDlg->m_nYear, index);
	SetShort(m_TempBuf, g_pMainDlg->m_nMonth, index);
	SetShort(m_TempBuf, g_pMainDlg->m_nDay, index);
	SetShort(m_TempBuf, g_pMainDlg->m_nHour, index);
	SetShort(m_TempBuf, g_pMainDlg->m_nMin, index);
	SetByte(m_TempBuf, (BYTE)g_pMainDlg->m_nWeather, index);
	SetByte(m_TempBuf, 0, index);

	BYTE byNightState = 4;
	if (g_pMainDlg->m_nHour >= 3 && g_pMainDlg->m_nHour < 21)
	{
		byNightState = 4;
	}
	else if (g_pMainDlg->m_nHour == 21)
	{
		byNightState = 1;
	}
	else if (g_pMainDlg->m_nHour == 2)
	{
		byNightState = 3;
	}
	else
	{
		byNightState = 2;
	}
	SetByte(m_TempBuf, byNightState, index);

	SetInt(m_TempBuf, 0, index);  // TODO: Something to do with event count o_O no idea atm!

	Send(m_TempBuf, index);
}

void USER::LinkToSameZone(short nX, short nY)
{
	CPoint pt = FindNearAvailablePoint_C(nX, nY, 10);
	if (pt.x == -1 || pt.y == -1)
	{
		pt.x = nX;
		pt.y = nY;
	}
	if (!Move_C(pt.x, pt.y, 10000))
	{
		CString entry;
		entry.Format("Could not move %s to %d, %d (client) on same zone.", m_strUserId, pt.x, pt.y);
		g_pMainDlg->LogToFile(entry);
		return;
	}
	if (m_bHidden)
	{
		SendMyInfo(TO_ME, INFO_MODIFY);
	}
	else
	{
		SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	}
	SightRecalc();
}

void USER::SendHPMP()
{
	CBufferEx TempBuf;
	TempBuf.Add(PKT_HPMP_RECOVERY);
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add(m_sHP);
	TempBuf.Add(m_sMP);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::ShowCurrentUser()
{
	if (m_sRank == 0) return;

	int nCount = 0;
	USER *pUser;
	nCount = 0;
	for (int i = 0; i < MAX_USER; i++)
	{
		pUser = g_pUserList->GetUserUid(i);
		if (pUser && pUser->m_State == STATE_GAMESTARTED) nCount++;
	}

	if (nCount >= 500) nCount = (int)((double)nCount * 1.1 + 0.5);

	TCHAR strTitle[256];
	::ZeroMemory(strTitle, sizeof(strTitle));
	wsprintf(strTitle, _ID(IDS_USER_CURRENT_USER_COUNT), nCount);
	SendServerChatMessage(strTitle, TO_ME);
}

bool USER::LinkToOtherZone(int moveZ, int moveX, int moveY)
{
	// Incase we need to restore the original zone index
	int iOldZoneIndex = m_iZoneIndex;

	// Checks the new zone is valid!
	int iNewZoneIndex = GetZoneIndex(moveZ);
	if (iNewZoneIndex == -1) return false;

	// The following checks m_lUser is same as m_Uid and then changes the m_lUser of map at user position to 0
	// this tells the map that the user is no longer there anymore.
	InterlockedCompareExchangePointer((PVOID*)&g_Zones[m_iZoneIndex]->m_pMap[m_sX][m_sY].m_lUser,
		(PVOID)0, (PVOID)(m_Uid + USER_BAND)); // == (PVOID*)(m_Uid + USER_BAND);

	// Tell other users that we are no longer there (at old location).
	SendMyInfo(TO_INSIGHT, INFO_DELETE);

	// Convert new co-ordinates to server and check they are valid!
	m_iZoneIndex = iNewZoneIndex;
	CPoint pt = ConvertToServer(moveX, moveY);
	if (pt.x == -1 || pt.y == -1)
	{
		CString entry;
		entry.Format("Could not move %s to other zone %d. Invalid co-ordinates %d, %d (client)", m_strUserId, moveZ, moveX, moveY);
		g_pMainDlg->LogToFile(entry);
		m_iZoneIndex = iOldZoneIndex;
		return false;
	}

	// Set our new location and inform the map that we are there!
	CPoint ptNear = FindNearAvailablePoint_S(pt.x, pt.y, 10);
	if (ptNear.x == -1 || ptNear.y == -1)
	{
		CString entry;
		entry.Format("Could not move %s to other zone %d. No available location on map near %d, %d (client)", m_strUserId, moveZ, moveX, moveY);
		g_pMainDlg->LogToFile(entry);
		m_iZoneIndex = iOldZoneIndex;
		return false;
	}
	m_sX = static_cast<short>(ptNear.x);
	m_sY = static_cast<short>(ptNear.y);
	m_sZ = moveZ;
	SetUid(m_sX, m_sY, m_Uid + USER_BAND);
	SendZoneChange(true);

	SendTime();

	// Let other users know we are around!
	m_sPrevX = -1;
	m_sPrevY = -1;
	SightRecalc();
	SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	return true;
}

void USER::SendZoneChange(bool bResult)
{
	if (bResult == false) return;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_ZONECHANGE);

	CPoint pos = ConvertToClient(m_sX, m_sY);
	if (pos.x == -1 || pos.y == -1) { pos.x = 1; pos.y = 1; }

	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add((short)pos.x);
	TempBuf.Add((short)pos.y);
	TempBuf.Add((short)m_sZ);
	TempBuf.Add((BYTE)g_mapZoneDetails[m_sZ].unknown); // Unknown
	TempBuf.Add((BYTE)g_mapZoneDetails[m_sZ].nightType); // Unknown
	TempBuf.Add((short)g_mapZoneDetails[m_sZ].hasMusic); // Unknown

	// IsMapRange 0x6E - checks if its a wotw map for sending 'Human's World' and 'Devil's World'
	// Check wotw is on
	// Send class who owns the map (class is read from soma.ini)
	if (IsMapRange(MAP_AREA_WOTW_MAP))
	{
		TempBuf.Add((short)g_iClassWar); // wotw map
	}
	else
	{
		TempBuf.Add((short)-1); // Not wotw map
	}

	TempBuf.Add((BYTE)0);

	Send(TempBuf, TempBuf.GetLength());

	m_dwNoDamageTime = NO_DAMAGE_TIME;	// TODO: Stops users getting killed when there map is loading...
	m_dwLastNoDamageTime = GetTickCount();
}

bool USER::ChangeGender()
{
	for (int i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		if (!m_InvItem[i].IsEmpty())
		{
			SendServerChatMessage(IDS_USER_ITEM_STILL_EQUIPPED, TO_ME);
			return false;
		}
	}

	m_sGender = !m_sGender;
	SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	ShowMagic(60, 0);
	return true;
}

bool USER::ChangeSkin()
{
	if (CheckDemon(m_sClass))
	{
		m_sSkin = !m_sSkin;
		SendMyInfo(TO_INSIGHT, INFO_MODIFY);
		ShowMagic(60, 0);
	}
	else
	{
		SendServerChatMessage(IDS_USER_ITEM_CANNOT_USE, TO_ME);
		return false;
	}

	return true;
}

short USER::GetMaxHP()
{
	short sMaxHP = m_sMaxHP;
	sMaxHP += static_cast<short>(PlusFromItem(SPECIAL_MAX_HP));
	if (m_sHP > sMaxHP)
	{
		m_sHP = sMaxHP;
	}
	return sMaxHP;
}

short USER::GetMaxMP()
{
	short sMaxMP = m_sMaxMP;
	sMaxMP += static_cast<short>(PlusFromItem(SPECIAL_MAX_MP));
	if (m_sMP > sMaxMP)
	{
		m_sMP = sMaxMP;
	}
	return sMaxMP;
}

short USER::GetMaxWgt()
{
	short sMaxWgt = m_sMaxWgt;
	sMaxWgt += static_cast<short>(PlusFromItem(SPECIAL_MAX_WGT));
	if (m_sWgt > sMaxWgt)
	{
		m_sWgt = sMaxWgt;
	}
	return sMaxWgt;
}

short USER::GetMaxStm()
{
	short sMaxStm = m_sMaxStm;
	sMaxStm += static_cast<short>(PlusFromItem(SPECIAL_MAX_STM));
	if (m_sStm > sMaxStm)
	{
		m_sStm = sMaxStm;
	}
	return sMaxStm;
}

bool USER::CheckAttackDelay()
{
	DWORD dwDelay = DEFAULT_ATTACK_DELAY;

	if (!m_InvItem[ARM_RHAND].IsEmpty())
	{
		dwDelay = m_InvItem[ARM_RHAND].sTime;
	}

	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_SPEED))
	{
		dwDelay = m_RemainSpecial[SPECIAL_EFFECT_SPEED].sDamage;
	}

	if (CheckRemainMagic(MAGIC_EFFECT_SLOW))
	{
		dwDelay *= 2;
	}

	CString str;
	str.Format(IDS_GM_MSG_ATTACK_DELAY, dwDelay);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

	DWORD dwCurTick = GetTickCount();
	if (dwCurTick - m_dwLastAttackTime < dwDelay)
	{
		return false;
	}

	m_dwLastAttackTime = dwCurTick;
	return true;
}

void USER::SendAttackFail(int tuid)
{
	CBufferEx TempBuf;

	TempBuf.Add(PKT_ATTACK);
	TempBuf.Add(ATTACK_FAIL);
	TempBuf.Add((int)(m_Uid + USER_BAND));
	TempBuf.Add(tuid);
	TempBuf.Add((BYTE)0); // Arrow Attack animation
	TempBuf.Add((short)m_byDir);

	for (int i = 0; i < 4; i++)
	{
		TempBuf.Add((short)-1); // User Item and dura change...
		TempBuf.Add((short)0);
	}
	TempBuf.Add((short)0);

	SendInsight(TempBuf, TempBuf.GetLength());
}


void USER::SendAttackMiss(int tuid)
{
	CBufferEx TempBuf;

	TempBuf.Add(PKT_ATTACK);
	TempBuf.Add(ATTACK_MISS);
	TempBuf.Add((int)(m_Uid + USER_BAND));
	TempBuf.Add(tuid);
	TempBuf.Add((BYTE)0);  // Arrow Attack animation
	TempBuf.Add((short)m_byDir);

	for (int i = 0; i < 4; i++)
	{
		TempBuf.Add((short)-1); // User Item and dura change...
		TempBuf.Add((short)0);
	}
	TempBuf.Add((short)0);

	SendInsight(TempBuf, TempBuf.GetLength());
}

void USER::SendAttackSuccess(int tuid, short sHP, short sMaxHP)
{
	CBufferEx TempBuf;

	TempBuf.Add(PKT_ATTACK);
	TempBuf.Add(ATTACK_SUCCESS);
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add(tuid);
	TempBuf.Add((BYTE)0); // Arrow Attack animation
	TempBuf.Add((short)m_byDir);

	// Not bothering to send item dura this way as i am sending it when it changes...
	// As far as i can tell... the normal 1p1 does it this way too... so this could be something
	// that could be got rid of in the future...
	for (int i = 0; i < 4; i++)
	{
		TempBuf.Add((short)-1); // User Item and dura change...
		TempBuf.Add((short)0);
	}

	TempBuf.Add((short)sHP);
	TempBuf.Add((short)sMaxHP);
	TempBuf.Add((DWORD)m_dwExp / 100); // Exp

	SendInsight(TempBuf, TempBuf.GetLength());
}

bool USER::CheckAttackSuccess(CNpc* pNpc)
{
	if (!pNpc) return false;

	// Get the amount of attacking dex
	double dDexAttack = static_cast<double>(m_iDEX) / CLIENT_EXT_STATS;
	int iPlusDex = PlusFromItem(SPECIAL_DEX);
	dDexAttack += iPlusDex;

	// Get the defense dex
	double dDexDefense = static_cast<double>(pNpc->m_sDex_df);

	// Defense Dex - Attack Dex
	int iDexAttack = static_cast<int>((dDexAttack - dDexDefense) + 0.5);

	// Lookup inside attack success tables to get a value for the hit %
	if (iDexAttack >= -50)
	{
		if (iDexAttack < 0)
		{
			iDexAttack = g_DexHitRateLow[(abs(iDexAttack)-1)];
		}
		else if (iDexAttack == 0)
		{
			iDexAttack = 55; // TODO: Should this be 50 not 55?
		}
		else if (iDexAttack > 0 && iDexAttack < 50)
		{
			iDexAttack = g_DexHitRateHigh[(iDexAttack-1)];
		}
		else
		{
			iDexAttack = 99;
		}
	}
	else
	{
		iDexAttack = 20;
	}

	// Add on any accuracy that is gained from wearing items
	iDexAttack += PlusFromItem(SPECIAL_ACCURACY);

	// Minimum is 1
	if (iDexAttack <= 0)
	{
		iDexAttack = 1;
	}

	// Maximum is 99
	if (iDexAttack > 99)
	{
		iDexAttack = 99;
	}

	// Calculate success / failure
	int iRandom = myrand(1, 100);
	return iDexAttack > iRandom ? true : false;
}

int USER::GetAttack()
{
	double dAttack = 0;
	dAttack += PlusFromItem(SPECIAL_PHY_ATT);

	double dSTR = (static_cast<double>(m_iSTR) / CLIENT_BASE_STATS) + PlusFromItem(SPECIAL_STR);
	dAttack += dSTR;

	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_INCREASEATTACK))
	{
		dAttack += m_RemainSpecial[SPECIAL_EFFECT_INCREASEATTACK].sDamage;
	}

	double dDEX = static_cast<double>(m_iDEX) / CLIENT_EXT_STATS;

	if (!m_InvItem[ARM_RHAND].IsEmpty() &&
		CheckDemon(m_sClass) == m_InvItem[ARM_RHAND].IsDemonItem())
	{
		double dWeaponAttack = m_InvItem[ARM_RHAND].GetAttack();
		double dWeaponExp = 0;

		switch (m_InvItem[ARM_RHAND].bType)
		{
		case TYPE_SWORD:
			dWeaponExp = static_cast<double>(m_iSwordExp) / CLIENT_SKILL;
			dAttack += ((dWeaponExp / 200) + 1.0) * dWeaponAttack;
			dAttack += myrand(1, static_cast<int>(dSTR / 8));
			break;
		case TYPE_AXE:
			dWeaponExp = static_cast<double>(m_iAxeExp) / CLIENT_SKILL;
			dAttack += ((dWeaponExp / 200) + 1.0) * dWeaponAttack;
			dAttack += myrand(1, static_cast<int>(dSTR / 8));
			break;
		case TYPE_BOW:
			dWeaponExp = static_cast<double>(m_iBowExp) / CLIENT_SKILL;
			dAttack += ((dWeaponExp / 200) + 1.0) * dWeaponAttack;
			dAttack += myrand(1, static_cast<int>(dDEX / 9));
			break;
		case TYPE_SPEAR:
			dWeaponExp = static_cast<double>(m_iSpearExp) / CLIENT_SKILL;
			dAttack += ((dWeaponExp / 200) + 1.0) * dWeaponAttack;
			dAttack += myrand(1, static_cast<int>(dSTR / 8));
			break;
		case TYPE_KNUCKLE:
			dWeaponExp = static_cast<double>(m_iKnuckleExp) / CLIENT_SKILL;
			dAttack += ((dWeaponExp / 200) + 1.0) * dWeaponAttack;
			dAttack += myrand(1, static_cast<int>(dDEX / 8));
			break;
		case TYPE_STAFF:
			dWeaponExp = static_cast<double>(m_iStaffExp) / CLIENT_SKILL;
			dAttack += ((dWeaponExp / 200) + 1.0) * dWeaponAttack;
			dAttack += myrand(1, static_cast<int>(dSTR / 9));
			break;
		default:
			break;
		}
	}

	DecreaseWeaponItemDura();

	// Need to check for GVW Map Area and CheckGuildTown / CheckGuildTownAlly
	short sTownNum = -1;
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_bGuildTownWar[i] && IsMapRange(g_iGuildTownRange[i]))
		{
			sTownNum = i + 1;
			break;
		}
	}

	if (sTownNum != -1)
	{
		if (CheckGuildTown(sTownNum))
		{
			dAttack += 3;
		}

		if (CheckGuildTownAlly(sTownNum))
		{
			dAttack += 2;
		}
	}

	// CheckMinus (Checks to see if attack should be taken away because of
	// human being in a devil map or devil being in a human map etc...)
	if (CheckMinus())
	{
		dAttack = (dAttack * g_iClassWarMinus) / 100.0;
	}

	// Reduce damage dealt to other class in WotW by the WotW owners calculated by how many wins and limit
	if (IsMapRange(MAP_AREA_ON_WOTW_MAP) &&
		(CheckDemon(g_iClassWar) == CheckDemon(m_sClass)))
	{
		int iReduce = (20 - g_iClassWarCount) * 5;
		if (iReduce < g_iClassWarLimit)
		{
			iReduce = g_iClassWarLimit;
		}
		dAttack = (dAttack * iReduce) / 100.0;
	}

	int iAttack  = static_cast<int>(dAttack + 0.5);

	CString str;
	str.Format(IDS_GM_MSG_ATTACK_DAMAGE, iAttack);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

	return iAttack;
}

void USER::DecreaseWeaponItemDura()
{
	if (!m_InvItem[ARM_RHAND].IsEmpty() &&
		m_InvItem[ARM_RHAND].bType < TYPE_ARMOR)
	{
		PlusItemDur(&m_InvItem[ARM_RHAND], 1, true);
		SendItemInfoChange(BASIC_INV, ARM_RHAND, INFO_DUR);
		SendItemStatusMessage(&m_InvItem[ARM_RHAND]);
		if (m_InvItem[ARM_RHAND].sUsage <= 0)
		{
			m_InvItem[ARM_RHAND].Init();
			InitRemainSpecial();
			SendDeleteItem(BASIC_INV, ARM_RHAND);
			SendItemInfo(ARM_RHAND);
			SendChangeWgt();
		}
	}
}

void USER::CalcPlusValue(int iType, int& iPlus, bool bCheckJob)
{
	if (m_sLevel >= LEVEL_CAP)
	{
		iPlus = 0;
		return;
	}

	if (!m_InvItem[ARM_RHAND].IsEmpty())
	{
		if (!CheckDemon(m_sClass))
		{
			switch (m_InvItem[ARM_RHAND].bType)
			{
			case TYPE_SWORD:
				if (iType == INC_STR)
				{
					iPlus *= H_SWORD_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= H_SWORD_PERCENT_DEX;
				}
				break;
			case TYPE_AXE:
				if (iType == INC_STR)
				{
					iPlus *= H_AXE_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= H_AXE_PERCENT_DEX;
				}
				break;
			case TYPE_BOW:
				if (iType == INC_STR)
				{
					iPlus *= H_BOW_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= H_BOW_PERCENT_DEX;
				}
				break;
			case TYPE_SPEAR:
				if (iType == INC_STR)
				{
					iPlus *= H_SPEAR_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= H_SPEAR_PERCENT_DEX;
				}
				break;
			case TYPE_KNUCKLE:
				if (iType == INC_STR)
				{
					iPlus *= H_KNUCKLE_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= H_KNUCKLE_PERCENT_DEX;
				}
				break;
			case TYPE_STAFF:
				if (iType == INC_STR)
				{
					iPlus *= H_STAFF_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= H_STAFF_PERCENT_DEX;
				}
				break;
			default:
				break;
			}
		}
		else
		{
			int iMyJob = -1;
			switch (m_InvItem[ARM_RHAND].bType)
			{
			case TYPE_SWORD:
				if (iType == INC_STR)
				{
					iPlus *= D_SWORD_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= D_SWORD_PERCENT_DEX;
				}
				else if (iType == INC_INT)
				{
					iPlus *= D_SWORD_PERCENT_INT;
				}
				else if (iType == INC_WIS)
				{
					iPlus *= D_SWORD_PERCENT_WIS;
				}
				else if (iType == INC_CON)
				{
					iPlus *= D_SWORD_PERCENT_CON;
				}
				iMyJob = DEVIL_CLASS_SWORDSMAN;
				break;
			case TYPE_AXE:
				if (iType == INC_STR)
				{
					iPlus *= D_AXE_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= D_AXE_PERCENT_DEX;
				}
				else if (iType == INC_INT)
				{
					iPlus *= D_AXE_PERCENT_INT;
				}
				else if (iType == INC_WIS)
				{
					iPlus *= D_AXE_PERCENT_WIS;
				}
				else if (iType == INC_CON)
				{
					iPlus *= D_AXE_PERCENT_CON;
				}
				iMyJob = DEVIL_CLASS_WARRIOR;
				break;
			case TYPE_BOW:
				if (iType == INC_STR)
				{
					iPlus *= D_BOW_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= D_BOW_PERCENT_DEX;
				}
				else if (iType == INC_INT)
				{
					iPlus *= D_BOW_PERCENT_INT;
				}
				else if (iType == INC_WIS)
				{
					iPlus *= D_BOW_PERCENT_WIS;
				}
				else if (iType == INC_CON)
				{
					iPlus *= D_BOW_PERCENT_CON;
				}
				iMyJob = DEVIL_CLASS_ARCHER;
				break;
			case TYPE_SPEAR:
				if (iType == INC_STR)
				{
					iPlus *= D_SPEAR_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= D_SPEAR_PERCENT_DEX;
				}
				else if (iType == INC_INT)
				{
					iPlus *= D_SPEAR_PERCENT_INT;
				}
				else if (iType == INC_WIS)
				{
					iPlus *= D_SPEAR_PERCENT_WIS;
				}
				else if (iType == INC_CON)
				{
					iPlus *= D_SPEAR_PERCENT_CON;
				}
				iMyJob = DEVIL_CLASS_KNIGHT;
				break;
			case TYPE_KNUCKLE:
				if (iType == INC_STR)
				{
					iPlus *= D_KNUCKLE_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= D_KNUCKLE_PERCENT_DEX;
				}
				else if (iType == INC_INT)
				{
					iPlus *= D_KNUCKLE_PERCENT_INT;
				}
				else if (iType == INC_WIS)
				{
					iPlus *= D_KNUCKLE_PERCENT_WIS;
				}
				else if (iType == INC_CON)
				{
					iPlus *= D_KNUCKLE_PERCENT_CON;
				}
				iMyJob = DEVIL_CLASS_FIGHTER;
				break;
			case TYPE_STAFF:
				if (iType == INC_STR)
				{
					iPlus *= D_STAFF_PERCENT_STR;
				}
				else if (iType == INC_DEX)
				{
					iPlus *= D_STAFF_PERCENT_DEX;
				}
				else if (iType == INC_INT)
				{
					iPlus *= D_STAFF_PERCENT_INT;
				}
				else if (iType == INC_WIS)
				{
					iPlus *= D_STAFF_PERCENT_WIS;
				}
				else if (iType == INC_CON)
				{
					iPlus *= D_STAFF_PERCENT_CON;
				}
				iMyJob = DEVIL_CLASS_WIZARD;
				break;
			default:
				break;
			}

			if (bCheckJob && iMyJob != -1 && !CheckMyJob(iMyJob))
			{
				if (CheckOtherJobLimit(iMyJob))
				{
					iPlus = static_cast<int>(iPlus * 0.05);
				}
				else
				{
					iPlus = static_cast<int>(iPlus * 0.2);
				}
			}
		}

		if (iPlus >= 100 || iPlus < 0)
		{
			iPlus /= 100;
		}
		else
		{
			iPlus = 1;
		}
	}
}

void USER::PlusWeaponExpByAttack(int nWeaponExpPercent)
{
	if (!m_InvItem[ARM_RHAND].IsEmpty())
	{
		int iInc = 0;
		switch (m_InvItem[ARM_RHAND].bType)
		{
		case TYPE_SWORD:
			iInc = GetInc(INC_SWORD);
			PlusWeaponExp(1, (nWeaponExpPercent * iInc) / 100);
			break;
		case TYPE_AXE:
			iInc = GetInc(INC_AXE);
			PlusWeaponExp(3, (nWeaponExpPercent * iInc) / 100);
			break;
		case TYPE_BOW:
			iInc = GetInc(INC_BOW);
			PlusWeaponExp(4, (nWeaponExpPercent * iInc) / 100);
			break;
		case TYPE_SPEAR:
			iInc = GetInc(INC_SPEAR);
			PlusWeaponExp(2, (nWeaponExpPercent * iInc) / 100);
			break;
		case TYPE_KNUCKLE:
			iInc = GetInc(INC_KNUCKLE);
			PlusWeaponExp(5, (nWeaponExpPercent * iInc) / 100);
			break;
		case TYPE_STAFF:
			iInc = GetInc(INC_STAFF);
			PlusWeaponExp(6, (nWeaponExpPercent * iInc) / 100);
			break;
		default:
			break;
		}
	}
}

int USER::GetInc(int nIncType)
{
	// Level Cap
	if (m_sLevel >= LEVEL_CAP && nIncType <= INC_CON) return 0;

	// This will hold the value exp will increase by
	int iInc = 0;

	// Lookup the inc value for hsoma
	if (!CheckDemon(m_sClass))
	{
		int iCurStr = m_iSTR / CLIENT_BASE_STATS;
		int iCurDex = m_iDEX / CLIENT_EXT_STATS;
		int iCurInt = m_iINT / CLIENT_BASE_STATS;
		int iCurWis = m_iWIS / CLIENT_EXT_STATS;
		int iCurCon = m_iCON / CLIENT_BASE_STATS;
		int iCurCha = m_iCHA / CLIENT_EXT_STATS;

		int nTableIndex = -1;

		switch (nIncType)
		{
		case INC_STR:
			nTableIndex = GetStatExpTableIndex(m_iSTR / CLIENT_BASE_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_str;
			break;
		case INC_RDSTR:
			nTableIndex = GetStatExpTableIndex(m_iSTR / CLIENT_BASE_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_rdstr;
			break;
		case INC_WDSTR:
			nTableIndex = GetStatExpTableIndex(m_iSTR / CLIENT_BASE_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_wdstr;
			break;
		case INC_DEX:
			nTableIndex = GetStatExpTableIndex(m_iDEX / CLIENT_EXT_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_dex;
			break;
		case INC_RDDEX:
			nTableIndex = GetStatExpTableIndex(m_iDEX / CLIENT_EXT_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_rddex;
			break;
		case INC_WDDEX:
			nTableIndex = GetStatExpTableIndex(m_iDEX / CLIENT_EXT_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_wddex;
			break;
		case INC_INT:
			nTableIndex = GetStatExpTableIndex(m_iINT / CLIENT_BASE_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_int;
			break;
		case INC_RDINT:
			nTableIndex = GetStatExpTableIndex(m_iINT / CLIENT_BASE_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_rdint;
			break;
		case INC_WDINT:
			nTableIndex = GetStatExpTableIndex(m_iINT / CLIENT_BASE_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_wdint;
			break;
		case INC_WIS:
			nTableIndex = GetStatExpTableIndex(m_iWIS / CLIENT_EXT_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_wis;
			break;
		case INC_RDWIS:
			nTableIndex = GetStatExpTableIndex(m_iWIS / CLIENT_EXT_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_rdwis;
			break;
		case INC_WDWIS:
			nTableIndex = GetStatExpTableIndex(m_iWIS / CLIENT_EXT_STATS);
			iInc = g_arStatExpTable[nTableIndex]->m_wdwis;
			break;
		case INC_CON:
			nTableIndex = GetConExpTableIndex(m_iCON / CLIENT_BASE_STATS);
			iInc = g_arConExpTable[nTableIndex]->m_con;
			break;
		case INC_RDCON:
			nTableIndex = GetConExpTableIndex(m_iCON / CLIENT_BASE_STATS);
			iInc = g_arConExpTable[nTableIndex]->m_rdcon;
			break;
		case INC_WDCON:
			nTableIndex = GetConExpTableIndex(m_iCON / CLIENT_BASE_STATS);
			iInc = g_arConExpTable[nTableIndex]->m_wdcon;
			break;
		case INC_CHA:
			nTableIndex = GetChaExpTableIndex(m_iCHA / CLIENT_EXT_STATS);
			iInc = g_arChaExpTable[nTableIndex]->m_cha;
			break;
		case INC_RDCHA:
			nTableIndex = GetChaExpTableIndex(m_iCHA / CLIENT_EXT_STATS);
			iInc = g_arChaExpTable[nTableIndex]->m_rdcha;
			break;
		case INC_WDCHA:
			nTableIndex = GetChaExpTableIndex(m_iCHA / CLIENT_EXT_STATS);
			iInc = g_arChaExpTable[nTableIndex]->m_wdcha;
			break;
		case INC_LEVEL_MAX_EXP:
			iInc = g_arExpTable[m_sLevel]->m_maxexp;
			break;
		case INC_LEVEL_RDEXP:
			iInc = g_arExpTable[m_sLevel]->m_rdexp;
			iInc *= 100;
			break;
		case INC_LEVEL_WDEXP:
			iInc = g_arExpTable[m_sLevel]->m_wdexp;
			iInc *= 100;
			break;
		case INC_AXE:
			nTableIndex = GetSkillExpTableIndex(m_iAxeExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_axe;
			break;
		case INC_RDAXE:
			nTableIndex = GetSkillExpTableIndex(m_iAxeExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_rdaxe;
			break;
		case INC_WDAXE:
			nTableIndex = GetSkillExpTableIndex(m_iAxeExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_wdaxe;
			break;
		case INC_BOW:
			nTableIndex = GetSkillExpTableIndex(m_iBowExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_bow;
			break;
		case INC_RDBOW:
			nTableIndex = GetSkillExpTableIndex(m_iBowExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_rdbow;
			break;
		case INC_WDBOW:
			nTableIndex = GetSkillExpTableIndex(m_iBowExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_wdbow;
			break;
		case INC_KNUCKLE:
			nTableIndex = GetSkillExpTableIndex(m_iKnuckleExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_knuckle;
			break;
		case INC_RDKNUCKLE:
			nTableIndex = GetSkillExpTableIndex(m_iKnuckleExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_rdknuckle;
			break;
		case INC_WDKNUCKLE:
			nTableIndex = GetSkillExpTableIndex(m_iKnuckleExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_wdknuckle;
			break;
		case INC_SPEAR:
			nTableIndex = GetSkillExpTableIndex(m_iSpearExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_spear;
			break;
		case INC_RDSPEAR:
			nTableIndex = GetSkillExpTableIndex(m_iSpearExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_rdspear;
			break;
		case INC_WDSPEAR:
			nTableIndex = GetSkillExpTableIndex(m_iSpearExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_wdspear;
			break;
		case INC_STAFF:
			nTableIndex = GetSkillExpTableIndex(m_iStaffExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_staff;
			break;
		case INC_RDSTAFF:
			nTableIndex = GetSkillExpTableIndex(m_iStaffExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_rdstaff;
			break;
		case INC_WDSTAFF:
			nTableIndex = GetSkillExpTableIndex(m_iStaffExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_wdstaff;
			break;
		case INC_SWORD:
			nTableIndex = GetSkillExpTableIndex(m_iSwordExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_sword;
			break;
		case INC_RDSWORD:
			nTableIndex = GetSkillExpTableIndex(m_iSwordExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_rdsword;
			break;
		case INC_WDSWORD:
			nTableIndex = GetSkillExpTableIndex(m_iSwordExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_wdsword;
			break;
		case INC_BLACK_MAGIC:
			nTableIndex = GetSkillExpTableIndex(m_iDMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_darkmg;
			break;
		case INC_RDBLACK_MAGIC:
			nTableIndex = GetSkillExpTableIndex(m_iDMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_rddarkmg;
			break;
		case INC_WDBLACK_MAGIC:
			nTableIndex = GetSkillExpTableIndex(m_iDMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_wddarkmg;
			break;
		case INC_WHITE_MAGIC:
			nTableIndex = GetSkillExpTableIndex(m_iWMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_whitemg;
			break;
		case INC_RDWHITE_MAGIC:
			nTableIndex = GetSkillExpTableIndex(m_iWMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_rdwhitemg;
			break;
		case INC_WDWHITE_MAGIC:
			nTableIndex = GetSkillExpTableIndex(m_iWMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_wdwhitemg;
			break;
		case INC_BLUE_MAGIC:
			nTableIndex = GetSkillExpTableIndex(m_iBMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_bluemg;
			break;
		case INC_RDBLUE_MAGIC:
			nTableIndex = GetSkillExpTableIndex(m_iBMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_rdbluemg;
			break;
		case INC_WDBLUE_MAGIC:
			nTableIndex = GetSkillExpTableIndex(m_iBMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTable[nTableIndex]->m_wdbluemg;
			break;
		}
	}
	// Lookup the inc value for dsoma
	else
	{
		int iCurStr = (m_iSTR / CLIENT_BASE_STATS) + (m_iSTRInc / CLIENT_BASE_STATS);
		int iCurDex = (m_iDEX / CLIENT_EXT_STATS) + (m_iDEXInc / CLIENT_EXT_STATS);
		int iCurInt = (m_iINT / CLIENT_BASE_STATS) + (m_iINTInc / CLIENT_BASE_STATS);
		int iCurWis = (m_iWIS / CLIENT_EXT_STATS) + (m_iWISInc / CLIENT_EXT_STATS);
		int iCurCon = (m_iCON / CLIENT_BASE_STATS) + (m_iCONInc / CLIENT_BASE_STATS);
		int iCurCha = (m_iCHA / CLIENT_EXT_STATS) + (m_iCHAInc / CLIENT_EXT_STATS);

		int nTableIndex = -1;

		switch (nIncType)
		{
		case INC_STR:
			nTableIndex = GetStatExpTableDIndex(m_iSTR / CLIENT_BASE_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_str;
			break;
		case INC_RDSTR:
			nTableIndex = GetStatExpTableDIndex(m_iSTR / CLIENT_BASE_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_rdstr;
			break;
		case INC_WDSTR:
			nTableIndex = GetStatExpTableDIndex(m_iSTR / CLIENT_BASE_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_wdstr;
			break;
		case INC_DEX:
			nTableIndex = GetStatExpTableDIndex(m_iDEX / CLIENT_EXT_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_dex;
			break;
		case INC_RDDEX:
			nTableIndex = GetStatExpTableDIndex(m_iDEX / CLIENT_EXT_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_rddex;
			break;
		case INC_WDDEX:
			nTableIndex = GetStatExpTableDIndex(m_iDEX / CLIENT_EXT_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_wddex;
			break;
		case INC_INT:
			nTableIndex = GetStatExpTableDIndex(m_iINT / CLIENT_BASE_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_int;
			break;
		case INC_RDINT:
			nTableIndex = GetStatExpTableDIndex(m_iINT / CLIENT_BASE_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_rdint;
			break;
		case INC_WDINT:
			nTableIndex = GetStatExpTableDIndex(m_iINT / CLIENT_BASE_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_wdint;
			break;
		case INC_WIS:
			nTableIndex = GetStatExpTableDIndex(m_iWIS / CLIENT_EXT_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_wis;
			break;
		case INC_RDWIS:
			nTableIndex = GetStatExpTableDIndex(m_iWIS / CLIENT_EXT_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_rdwis;
			break;
		case INC_WDWIS:
			nTableIndex = GetStatExpTableDIndex(m_iWIS / CLIENT_EXT_STATS);
			iInc = g_arStatExpTableD[nTableIndex]->m_wdwis;
			break;
		case INC_CON:
			nTableIndex = GetConExpTableDIndex(m_iCON / CLIENT_BASE_STATS);
			iInc = g_arConExpTableD[nTableIndex]->m_con;
			break;
		case INC_RDCON:
			nTableIndex = GetConExpTableDIndex(m_iCON / CLIENT_BASE_STATS);
			iInc = g_arConExpTableD[nTableIndex]->m_rdcon;
			break;
		case INC_WDCON:
			nTableIndex = GetConExpTableDIndex(m_iCON / CLIENT_BASE_STATS);
			iInc = g_arConExpTableD[nTableIndex]->m_wdcon;
			break;
		case INC_CHA:
			nTableIndex = GetChaExpTableDIndex(m_iCHA / CLIENT_EXT_STATS);
			iInc = g_arChaExpTableD[nTableIndex]->m_cha;
			break;
		case INC_RDCHA:
			nTableIndex = GetChaExpTableDIndex(m_iCHA / CLIENT_EXT_STATS);
			iInc = g_arChaExpTableD[nTableIndex]->m_rdcha;
			break;
		case INC_WDCHA:
			nTableIndex = GetChaExpTableDIndex(m_iCHA / CLIENT_EXT_STATS);
			iInc = g_arChaExpTableD[nTableIndex]->m_wdcha;
			break;
		case INC_LEVEL_MAX_EXP:
			iInc = g_arExpTableD[m_sLevel]->m_maxexp;
			break;
		case INC_LEVEL_RDEXP:
			iInc = g_arExpTableD[m_sLevel]->m_rdexp;
			iInc *= 100;
			break;
		case INC_LEVEL_WDEXP:
			iInc = g_arExpTableD[m_sLevel]->m_wdexp;
			iInc *= 100;
			break;
		case INC_AXE:
			nTableIndex = GetSkillExpTableDIndex(m_iAxeExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_axe;
			break;
		case INC_RDAXE:
			nTableIndex = GetSkillExpTableDIndex(m_iAxeExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_rdaxe;
			break;
		case INC_WDAXE:
			nTableIndex = GetSkillExpTableDIndex(m_iAxeExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_wdaxe;
			break;
		case INC_BOW:
			nTableIndex = GetSkillExpTableDIndex(m_iBowExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_bow;
			break;
		case INC_RDBOW:
			nTableIndex = GetSkillExpTableDIndex(m_iBowExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_rdbow;
			break;
		case INC_WDBOW:
			nTableIndex = GetSkillExpTableDIndex(m_iBowExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_wdbow;
			break;
		case INC_KNUCKLE:
			nTableIndex = GetSkillExpTableDIndex(m_iKnuckleExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_knuckle;
			break;
		case INC_RDKNUCKLE:
			nTableIndex = GetSkillExpTableDIndex(m_iKnuckleExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_rdknuckle;
			break;
		case INC_WDKNUCKLE:
			nTableIndex = GetSkillExpTableDIndex(m_iKnuckleExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_wdknuckle;
			break;
		case INC_SPEAR:
			nTableIndex = GetSkillExpTableDIndex(m_iSpearExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_spear;
			break;
		case INC_RDSPEAR:
			nTableIndex = GetSkillExpTableDIndex(m_iSpearExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_rdspear;
			break;
		case INC_WDSPEAR:
			nTableIndex = GetSkillExpTableDIndex(m_iSpearExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_wdspear;
			break;
		case INC_STAFF:
			nTableIndex = GetSkillExpTableDIndex(m_iStaffExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_staff;
			break;
		case INC_RDSTAFF:
			nTableIndex = GetSkillExpTableDIndex(m_iStaffExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_rdstaff;
			break;
		case INC_WDSTAFF:
			nTableIndex = GetSkillExpTableDIndex(m_iStaffExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_wdstaff;
			break;
		case INC_SWORD:
			nTableIndex = GetSkillExpTableDIndex(m_iSwordExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_sword;
			break;
		case INC_RDSWORD:
			nTableIndex = GetSkillExpTableDIndex(m_iSwordExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_rdsword;
			break;
		case INC_WDSWORD:
			nTableIndex = GetSkillExpTableDIndex(m_iSwordExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_wdsword;
			break;
		case INC_BLACK_MAGIC:
			nTableIndex = GetSkillExpTableDIndex(m_iDMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_darkmg;
			break;
		case INC_RDBLACK_MAGIC:
			nTableIndex = GetSkillExpTableDIndex(m_iDMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_rddarkmg;
			break;
		case INC_WDBLACK_MAGIC:
			nTableIndex = GetSkillExpTableDIndex(m_iDMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_wddarkmg;
			break;
		case INC_WHITE_MAGIC:
			nTableIndex = GetSkillExpTableDIndex(m_iWMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_whitemg;
			break;
		case INC_RDWHITE_MAGIC:
			nTableIndex = GetSkillExpTableDIndex(m_iWMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_rdwhitemg;
			break;
		case INC_WDWHITE_MAGIC:
			nTableIndex = GetSkillExpTableDIndex(m_iWMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_wdwhitemg;
			break;
		case INC_BLUE_MAGIC:
			nTableIndex = GetSkillExpTableDIndex(m_iBMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_bluemg;
			break;
		case INC_RDBLUE_MAGIC:
			nTableIndex = GetSkillExpTableDIndex(m_iBMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_rdbluemg;
			break;
		case INC_WDBLUE_MAGIC:
			nTableIndex = GetSkillExpTableDIndex(m_iBMagicExp / CLIENT_SKILL);
			iInc = g_arSkillExpTableD[nTableIndex]->m_wdbluemg;
			break;
		}
	}

	// Changes inc to 0 for all warp death inc if the class war is on
	if (g_bClassWar)
	{
		switch (nIncType)
		{
		case INC_LEVEL_WDEXP:
		case INC_WDSTR:
		case INC_WDDEX:
		case INC_WDINT:
		case INC_WDWIS:
		case INC_WDCON:
		case INC_WDCHA:
		case INC_WDAXE:
		case INC_WDBOW:
		case INC_WDKNUCKLE:
		case INC_WDSPEAR:
		case INC_WDSTAFF:
		case INC_WDSWORD:
		case INC_WDBLACK_MAGIC:
		case INC_WDWHITE_MAGIC:
		case INC_WDBLUE_MAGIC:
			iInc = 0;
			break;
		}
	}

	if (CheckDemon(m_sClass))
	{
		switch (nIncType)
		{
		case INC_RDSTR:
		case INC_WDSTR:
			CalcPlusValue(INC_STR, iInc, false);
			break;
		case INC_RDDEX:
		case INC_WDDEX:
			CalcPlusValue(INC_DEX, iInc, false);
			break;
		case INC_RDINT:
		case INC_WDINT:
			CalcPlusValue(INC_INT, iInc, false);
			break;
		case INC_RDWIS:
		case INC_WDWIS:
			CalcPlusValue(INC_WIS, iInc, false);
			break;
		case INC_RDCON:
		case INC_WDCON:
			CalcPlusValue(INC_CON, iInc, false);
			break;
		}
	}

	// Increase the stat exp taken upon revival from death
	// depending on the type of bad moral player has.
	if (nIncType >= INC_RDSTR && nIncType <= INC_WDCON)
	{
		int iMoral = m_iMoral / CLIENT_MORAL;
		if (!CheckDemon(m_sClass))
		{
			if (iMoral <= -50) // Devil
			{
				iInc += (iInc * 20) / 100;
			}
			else if (iMoral >= -49 && iMoral <= -31) // Wicked
			{
				iInc += (iInc * 10) / 100;
			}
			else if (iMoral >= -30 && iMoral <= -11) // Evil
			{
				iInc += (iInc * 5) / 100;
			}
		}
		else
		{
			if (iMoral >= 50) // Angel
			{
				iInc += (iInc * 20) / 100;
			}
			else if (iMoral >= 31 && iMoral <= 49) // Virtuous
			{
				iInc += (iInc * 10) / 100;
			}
			else if (iMoral >= 11 && iMoral <= 30) // Moral
			{
				iInc += (iInc * 5) / 100;
			}
		}
	}

	if (!m_bLiveGray)
	{
		switch (nIncType)
		{
		case INC_LEVEL_RDEXP:
		case INC_RDSTR:
		case INC_RDDEX:
		case INC_RDINT:
		case INC_RDWIS:
		case INC_RDCON:
		case INC_RDAXE:
		case INC_RDBOW:
		case INC_RDKNUCKLE:
		case INC_RDSPEAR:
		case INC_RDSTAFF:
		case INC_RDSWORD:
		case INC_RDBLACK_MAGIC:
		case INC_RDWHITE_MAGIC:
		case INC_RDBLUE_MAGIC:
			iInc /= 2;
			if (iInc <= 0) iInc = 0;
			break;
		case INC_LEVEL_WDEXP:
		case INC_WDSTR:
		case INC_WDDEX:
		case INC_WDINT:
		case INC_WDWIS:
		case INC_WDCON:
		case INC_WDAXE:
		case INC_WDBOW:
		case INC_WDKNUCKLE:
		case INC_WDSPEAR:
		case INC_WDSTAFF:
		case INC_WDSWORD:
		case INC_WDBLACK_MAGIC:
		case INC_WDWHITE_MAGIC:
		case INC_WDBLUE_MAGIC:
			iInc = 0;
			break;
		}
	}

	//If you are party leader and are within 9 tile distance of another member
	//you gain 10% extra exp (Stats + Weapon Skills).
	//Else, get the party leader and are within within 9 tile distance of leader
	//you gain  10% extra stat exp  (Stats + Weapon Skills).
	if (m_bInParty)
	{
		bool bIsInDistance = false;
		if (strcmp(m_strUserId, m_PartyMembers[0].m_strUserId) == 0 &&
			m_PartyMembers[0].uid == m_PartyMembers[0].uid)
		{
			for (int i = 1; i < MAX_PARTY_USER_NUM; i++)
			{
				if (m_PartyMembers[i].uid != -1)
				{
					USER *pMember = g_pUserList->GetUserUid(m_PartyMembers[i].uid);
					if (!pMember) continue;
					if (pMember->m_State != STATE_GAMESTARTED) continue;

					// Below check is done incase the uid has been reused for another user
					// and for some reason user is still in party list.
					if (strcmp(pMember->m_strUserId, m_PartyMembers[i].m_strUserId) != 0) continue;

					if (CheckDistance(pMember, 9))
					{
						bIsInDistance = true;
						break;
					}
				}
			}
		}
		else
		{
			USER *pLeader = g_pUserList->GetUserUid(m_PartyMembers[0].uid);
			if (pLeader != NULL)
			{
				// Validate that it is the leader of this user's party
				if (strcmp(pLeader->m_strUserId, m_PartyMembers[0].m_strUserId) == 0 &&
					pLeader->m_Uid == m_PartyMembers[0].uid)
				{
					bIsInDistance = CheckDistance(pLeader, 9);
				}
			}
		}

		// Give 10% extra for stats and skills.
		if (bIsInDistance)
		{
			switch (nIncType)
			{
			case INC_STR:
			case INC_DEX:
			case INC_INT:
			case INC_WIS:
			case INC_CON:
			case INC_CHA:
			case INC_AXE:
			case INC_BOW:
			case INC_KNUCKLE:
			case INC_SPEAR:
			case INC_STAFF:
			case INC_SWORD:
			case INC_BLACK_MAGIC:
			case INC_WHITE_MAGIC:
			case INC_BLUE_MAGIC:
				iInc = static_cast<int>(iInc * 1.1);
				break;
			}
		}
		else
		{
			// Cannot gain charisma if not within party distance
			if (nIncType == INC_CHA)
			{
				iInc = 0;
			}
		}
	}

	// Weapon and Magic Skill Caps
	switch (nIncType)
	{
	case INC_AXE:	// Fall-through!
	case INC_BOW:
	case INC_KNUCKLE:
	case INC_SPEAR:
	case INC_STAFF:
	case INC_SWORD:
		{
		// Add all skills together
		int iAllWeaponExp = (m_iAxeExp + m_iBowExp + m_iKnuckleExp +
			m_iSpearExp + m_iStaffExp + m_iSwordExp) / CLIENT_SKILL;
		if (iAllWeaponExp >= WEAPON_SKILL_CAP)
		{
			MinusWeaponExp(nIncType, iInc);
		}
		}
		break;
	case INC_BLACK_MAGIC: // Fall-through!
	case INC_WHITE_MAGIC:
	case INC_BLUE_MAGIC:
		{
		int iAllMagicExp = (m_iDMagicExp + m_iBMagicExp + m_iWMagicExp) / CLIENT_SKILL;
		if (iAllMagicExp >= MAGIC_SKILL_CAP)
		{
			MinusMagicExp(nIncType, iInc);
		}
		}
		break;
	}

	return iInc;
}

void USER::PlusWeaponExp(int nWeaponType, int nPlusWeaponExp)
{
	if (nPlusWeaponExp == 0) return;

	int iCurExp = 0;
	int iNewExp = 0;
	CString strWeaponSkill = "";

	switch (nWeaponType)
	{
	case 1:		// Sword
		iCurExp = m_iSwordExp / SERVER_SKILL;
		if (!CheckMyJob(DEVIL_CLASS_SWORDSMAN))
		{
			if (iCurExp >= 50 && nPlusWeaponExp > 0)
			{
				return;
			}
		}
		m_iSwordExp += nPlusWeaponExp;
		if (m_iSwordExp < 0)
		{
			m_iSwordExp = 0;
		}
		iNewExp = m_iSwordExp / SERVER_SKILL;
		strWeaponSkill.Format(IDS_USER_SWORD);
		break;
	case 2:		// Spear
		iCurExp = m_iSpearExp / SERVER_SKILL;
		if (!CheckMyJob(DEVIL_CLASS_KNIGHT))
		{
			if (iCurExp >= 50 && nPlusWeaponExp > 0)
			{
				return;
			}
		}
		m_iSpearExp += nPlusWeaponExp;
		if (m_iSpearExp < 0)
		{
			m_iSpearExp = 0;
		}
		iNewExp = m_iSpearExp / SERVER_SKILL;
		strWeaponSkill.Format(IDS_USER_SPEAR);
		break;
	case 3:		// Axe
		iCurExp = m_iAxeExp / SERVER_SKILL;
		if (!CheckMyJob(DEVIL_CLASS_WARRIOR))
		{
			if (iCurExp >= 50 && nPlusWeaponExp > 0)
			{
				return;
			}
		}
		m_iAxeExp += nPlusWeaponExp;
		if (m_iAxeExp < 0)
		{
			m_iAxeExp = 0;
		}
		iNewExp = m_iAxeExp / SERVER_SKILL;
		strWeaponSkill.Format(IDS_USER_AXE);
		break;
	case 4:		// Bow
		iCurExp = m_iBowExp / SERVER_SKILL;
		if (!CheckMyJob(DEVIL_CLASS_ARCHER))
		{
			if (iCurExp >= 50 && nPlusWeaponExp > 0)
			{
				return;
			}
		}
		m_iBowExp += nPlusWeaponExp;
		if (m_iBowExp < 0)
		{
			m_iBowExp = 0;
		}
		iNewExp = m_iBowExp / SERVER_SKILL;
		strWeaponSkill.Format(IDS_USER_BOW);
		break;
	case 5:		// Knuckle
		iCurExp = m_iKnuckleExp / SERVER_SKILL;
		if (!CheckMyJob(DEVIL_CLASS_FIGHTER))
		{
			if (iCurExp >= 50 && nPlusWeaponExp > 0)
			{
				return;
			}
		}
		m_iKnuckleExp += nPlusWeaponExp;
		if (m_iKnuckleExp < 0)
		{
			m_iKnuckleExp = 0;
		}
		iNewExp = m_iKnuckleExp / SERVER_SKILL;
		strWeaponSkill.Format(IDS_USER_KNUCKLE);
		break;
	case 6:		// Staff
		iCurExp = m_iStaffExp / SERVER_SKILL;
		if (!CheckMyJob(DEVIL_CLASS_WIZARD))
		{
			if (iCurExp >= 50 && nPlusWeaponExp > 0)
			{
				return;
			}
		}
		m_iStaffExp += nPlusWeaponExp;
		if (m_iStaffExp < 0)
		{
			m_iStaffExp = 0;
		}
		iNewExp = m_iStaffExp / SERVER_SKILL;
		strWeaponSkill.Format(IDS_USER_STAFF);
		break;
	}

	int iDiff = abs(iNewExp - iCurExp);
	if (iDiff != 0)
	{
		int iCur = iCurExp % 10;
		if (iDiff + iCur >= 10) // Yellow skill Gain
		{
			CString str;
			if (nPlusWeaponExp > 0)
			{
				str.Format(IDS_USER_INC, strWeaponSkill);
				SendSpecialMsg((LPTSTR)(LPCTSTR)str, YELLOW_STAT, TO_ME); // Skill increase
			}
			else
			{
				str.Format(IDS_USER_DEC, strWeaponSkill);
				SendSpecialMsg((LPTSTR)(LPCTSTR)str, YELLOW_STAT, TO_ME); // Skill Decrease
			}
		}
		else if (iDiff + iCur >= 0  && iDiff + iCur < 10) // Blue skill gain
		{
			CString str;
			if (nPlusWeaponExp > 0)
			{
				str.Format(IDS_USER_INC, strWeaponSkill);
				SendSpecialMsg((LPTSTR)(LPCTSTR)str, BLUE_STAT, TO_ME); // Skill increase
			}
			else
			{
				str.Format(IDS_USER_DEC, strWeaponSkill);
				SendSpecialMsg((LPTSTR)(LPCTSTR)str, BLUE_STAT, TO_ME); // Skill Decrease
			}
		}

		SendCharData(INFO_WEAPONEXP);
	}

	CString str;
	str.Format(IDS_GM_MSG_PLUS_WEAPON_EXP, strWeaponSkill, nPlusWeaponExp);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);
}

bool USER::CheckMyJob(int nClass)
{
	if (m_sClass == 0 || m_sClass == nClass)
	{
		return true;
	}
	return false;
}

int USER::GetSkillExpTableIndex(int nLevel)
{
	int iIndex = g_arSkillExpTable.GetSize() - 1;
	for (int i = 0; i < g_arSkillExpTable.GetSize(); i++)
	{
		if (g_arSkillExpTable[i]->m_slevel > nLevel)
		{
			iIndex = i;
			break;
		}
	}
	return iIndex;
}

int USER::GetStatExpTableIndex(int nLevel)
{
	int iIndex = g_arStatExpTable.GetSize() - 1;
	for (int i = 0; i < g_arStatExpTable.GetSize(); i++)
	{
		if (g_arStatExpTable[i]->m_slevel > nLevel)
		{
			iIndex = i;
			break;
		}
	}
	return iIndex;
}

int USER::GetConExpTableIndex(int nLevel)
{
	int iIndex = g_arConExpTable.GetSize() - 1;
	for (int i = 0; i < g_arConExpTable.GetSize(); i++)
	{
		if (g_arConExpTable[i]->m_slevel > nLevel)
		{
			iIndex = i;
			break;
		}
	}
	return iIndex;
}

int USER::GetChaExpTableIndex(int nLevel)
{
	int iIndex = g_arChaExpTable.GetSize() - 1;
	for (int i = 0; i < g_arChaExpTable.GetSize(); i++)
	{
		if (g_arChaExpTable[i]->m_slevel > nLevel)
		{
			iIndex = i;
			break;
		}
	}
	return iIndex;
}

int USER::GetSkillExpTableDIndex(int nLevel)
{
	int iIndex = g_arSkillExpTableD.GetSize() - 1;
	for (int i = 0; i < g_arSkillExpTableD.GetSize(); i++)
	{
		if (g_arSkillExpTableD[i]->m_slevel > nLevel)
		{
			iIndex = i;
			break;
		}
	}
	return iIndex;
}

int USER::GetStatExpTableDIndex(int nLevel)
{
	int iIndex = g_arStatExpTableD.GetSize() - 1;
	for (int i = 0; i < g_arStatExpTableD.GetSize(); i++)
	{
		if (g_arStatExpTableD[i]->m_slevel > nLevel)
		{
			iIndex = i;
			break;
		}
	}
	return iIndex;
}

int USER::GetConExpTableDIndex(int nLevel)
{
	int iIndex = g_arConExpTableD.GetSize() - 1;
	for (int i = 0; i < g_arConExpTableD.GetSize(); i++)
	{
		if (g_arConExpTableD[i]->m_slevel > nLevel)
		{
			iIndex = i;
			break;
		}
	}
	return iIndex;
}

int USER::GetChaExpTableDIndex(int nLevel)
{
	int iIndex = g_arChaExpTableD.GetSize() - 1;
	for (int i = 0; i < g_arChaExpTableD.GetSize(); i++)
	{
		if (g_arChaExpTableD[i]->m_slevel > nLevel)
		{
			iIndex = i;
			break;
		}
	}
	return iIndex;
}

void USER::PlusCon(int iPlus)
{
	if (iPlus == 0) return;

	int iCurExp = m_iCON / SERVER_BASE_STATS;
	m_iCON += iPlus;

	// Minimum shouldn't be able to go below starting stat.
	if ((m_iCON / CLIENT_BASE_STATS) < m_sStartCon)
	{
		m_iCON = m_sStartCon * CLIENT_BASE_STATS;
	}

	int iNewExp = m_iCON / SERVER_BASE_STATS;

	int iDiff = abs(iNewExp - iCurExp);
	if (iDiff != 0)
	{
		int iCur = iCurExp % 10;
		if (iDiff + iCur >= 10) // Yellow skill Gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_CON_INC, YELLOW_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_CON_DEC, YELLOW_STAT, TO_ME); // Skill Decrease
			}
		}
		else if (iDiff + iCur >= 0  && iDiff + iCur < 10) // Blue skill gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_CON_INC, BLUE_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_CON_DEC, BLUE_STAT, TO_ME); // Skill Decrease
			}
		}

		SendCharData(INFO_BASICVALUE);
	}
}

void USER::PlusStr(int iPlus)
{
	if (iPlus == 0) return;

	int iCurExp = m_iSTR / SERVER_BASE_STATS;
	m_iSTR += iPlus;

	// Minimum shouldn't be able to go below starting stat.
	if ((m_iSTR / CLIENT_BASE_STATS) < m_sStartStr)
	{
		m_iSTR = m_sStartStr * CLIENT_BASE_STATS;
	}

	int iNewExp = m_iSTR / SERVER_BASE_STATS;

	int iDiff = abs(iNewExp - iCurExp);
	if (iDiff != 0)
	{
		int iCur = iCurExp % 10;
		if (iDiff + iCur >= 10) // Yellow skill Gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_STR_INC, YELLOW_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_STR_DEC, YELLOW_STAT, TO_ME); // Skill Decrease
			}
		}
		else if (iDiff + iCur >= 0  && iDiff + iCur < 10) // Blue skill gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_STR_INC, BLUE_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_STR_DEC, BLUE_STAT, TO_ME); // Skill Decrease
			}
		}

		SendCharData(INFO_BASICVALUE);
	}
}

void USER::PlusDex(int iPlus)
{
	if (iPlus == 0) return;

	int iCurExp = m_iDEX / SERVER_EXT_STATS;
	m_iDEX += iPlus;

	// Minimum shouldn't be able to go below starting stat.
	if ((m_iDEX / CLIENT_EXT_STATS) < m_sStartDex)
	{
		m_iDEX = m_sStartDex * CLIENT_EXT_STATS;
	}

	int iNewExp = m_iDEX / SERVER_EXT_STATS;

	int iDiff = abs(iNewExp - iCurExp);
	if (iDiff != 0)
	{
		int iCur = iCurExp % 10;
		if (iDiff + iCur >= 10) // Yellow skill Gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_DEX_INC, YELLOW_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_DEX_DEC, YELLOW_STAT, TO_ME); // Skill Decrease
			}
		}
		else if (iDiff + iCur >= 0  && iDiff + iCur < 10) // Blue skill gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_DEX_INC, BLUE_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_DEX_DEC, BLUE_STAT, TO_ME); // Skill Decrease
			}
		}

		SendCharData(INFO_BASICVALUE);
	}
}

void USER::PlusInt(int iPlus)
{
	if (iPlus == 0) return;

	int iCurExp = m_iINT / SERVER_BASE_STATS;
	m_iINT += iPlus;

	// Minimum shouldn't be able to go below starting stat.
	if ((m_iINT / CLIENT_BASE_STATS) < m_sStartInt)
	{
		m_iINT = m_sStartInt * CLIENT_BASE_STATS;
	}

	int iNewExp = m_iINT / SERVER_BASE_STATS;

	int iDiff = abs(iNewExp - iCurExp);
	if (iDiff != 0)
	{
		int iCur = iCurExp % 10;
		if (iDiff + iCur >= 10) // Yellow skill Gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_INT_INC, YELLOW_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_INT_DEC, YELLOW_STAT, TO_ME); // Skill Decrease
			}
		}
		else if (iDiff + iCur >= 0  && iDiff + iCur < 10) // Blue skill gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_INT_INC, BLUE_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_INT_DEC, BLUE_STAT, TO_ME); // Skill Decrease
			}
		}

		SendCharData(INFO_BASICVALUE);
	}
}

void USER::PlusWis(int iPlus)
{
	if (iPlus == 0) return;

	int iCurExp = m_iWIS / SERVER_EXT_STATS;
	m_iWIS += iPlus;

	// Minimum shouldn't be able to go below starting stat.
	if ((m_iWIS / CLIENT_EXT_STATS) < m_sStartWis)
	{
		m_iWIS = m_sStartWis * CLIENT_EXT_STATS;
	}

	int iNewExp = m_iWIS / SERVER_EXT_STATS;

	int iDiff = abs(iNewExp - iCurExp);
	if (iDiff != 0)
	{
		int iCur = iCurExp % 10;
		if (iDiff + iCur >= 10) // Yellow skill Gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_WIS_INC, YELLOW_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_WIS_DEC, YELLOW_STAT, TO_ME); // Skill Decrease
			}
		}
		else if (iDiff + iCur >= 0  && iDiff + iCur < 10) // Blue skill gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_WIS_INC, BLUE_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_WIS_DEC, BLUE_STAT, TO_ME); // Skill Decrease
			}
		}

		SendCharData(INFO_BASICVALUE);
	}
}

void USER::PlusCha(int iPlus)
{
	if (iPlus == 0) return;

	int iCurExp = m_iCHA / SERVER_EXT_STATS;
	m_iCHA += iPlus;

	// Minimum shouldn't be able to go below starting stat.
	if ((m_iCHA / CLIENT_EXT_STATS) < m_sStartCha)
	{
		m_iCHA = m_sStartCha * CLIENT_EXT_STATS;
	}

	int iNewExp = m_iCHA / SERVER_EXT_STATS;

	int iDiff = abs(iNewExp - iCurExp);
	if (iDiff != 0)
	{
		int iCur = iCurExp % 10;
		if (iDiff + iCur >= 10) // Yellow skill Gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_CHA_INC, YELLOW_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_CHA_DEC, YELLOW_STAT, TO_ME); // Skill Decrease
			}
		}
		else if (iDiff + iCur >= 0  && iDiff + iCur < 10) // Blue skill gain
		{
			if (iPlus > 0)
			{
				SendSpecialMsg(IDS_USER_CHA_INC, BLUE_STAT, TO_ME); // Skill increase
			}
			else
			{
				SendSpecialMsg(IDS_USER_CHA_DEC, BLUE_STAT, TO_ME); // Skill Decrease
			}
		}

		SendCharData(INFO_BASICVALUE);
	}
}


int USER::GetPlusValueCount(CNpc* pNpc, int iDamage, bool bNpcDead, int nIncType)
{
	// Calculates and returns how many times inc will be added to stats.
	if (!pNpc) return 0;

	double dPlusValueCount = ((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) / 10.0;

	if (nIncType == INC_STR)
	{
		int iSTRNpc = ((pNpc->m_sStr * 2) + pNpc->m_sAStr + pNpc->m_sWStr) / 2;
		int iSTR = m_iSTR / CLIENT_BASE_STATS;
		int iDiff = iSTRNpc - iSTR;

		if (iDiff > 50)
		{
			if (bNpcDead)
			{
				dPlusValueCount *= 2.0;
			}
			else
			{
				dPlusValueCount *= 1.7;
			}
		}
		else if (iDiff >= 30)
		{
			if (bNpcDead)
			{
				dPlusValueCount *= 1.7;
			}
			else
			{
				dPlusValueCount *= 1.4;
			}
		}
		else if (iDiff >= 10)
		{
			if (bNpcDead)
			{
				dPlusValueCount *= 1.5;
			}
			else
			{
				dPlusValueCount *= 1.2;
			}
		}
		else if (iDiff >= -10)
		{
			// don't need to do anything here
			// dPlusValueCount *= 1.0;
		}
		else if (iDiff >= -15)
		{
			dPlusValueCount *= 0.8;
		}
		else if (iDiff >= -30)
		{
			dPlusValueCount *= 0.5;
		}
		else
		{
			dPlusValueCount = 0.0;
		}
	}
	else if (nIncType == INC_INT)
	{
		int iINTNpc = pNpc->m_sStr + pNpc->m_sIntel;
		int iINT = m_iINT / CLIENT_BASE_STATS;
		int iDiff = iINTNpc - iINT;
		if (iDiff > 30)
		{
			if (bNpcDead)
			{
				dPlusValueCount *= 2.0;
			}
			else
			{
				dPlusValueCount *= 1.7;
			}
		}
		else if (iDiff >= 15)
		{
			if (bNpcDead)
			{
				dPlusValueCount *= 1.7;
			}
			else
			{
				dPlusValueCount *= 1.4;
			}
		}
		else if (iDiff >= 5)
		{
			if (bNpcDead)
			{
				dPlusValueCount *= 1.5;
			}
			else
			{
				dPlusValueCount *= 1.2;
			}
		}
		else if (iDiff >= -10)
		{
			// don't need to do anything here
			// dPlusValueCount *= 1.0;
		}
		else if (iDiff >= -20)
		{
			dPlusValueCount *= 0.8;
		}
		else if (iDiff >= -30)
		{
			dPlusValueCount *= 0.5;
		}
		else
		{
			dPlusValueCount = 0.0;
		}
	}

	// Set how many times (adds the remainder from previous call)
	int iPlusValueCount = static_cast<int>(dPlusValueCount + m_dPlusValueCountRemainder);

	// Set the remainder value that will be added next time this function is called
	m_dPlusValueCountRemainder = (dPlusValueCount + m_dPlusValueCountRemainder) - static_cast<double>(iPlusValueCount);

	return iPlusValueCount;
}

void USER::SendDummyAttack(int iTargetId, int iDamage)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_DUMMY_ATTACK, index);
	SetInt(m_TempBuf, iTargetId, index);
	SetShort(m_TempBuf, static_cast<short>(iDamage), index);
	SendInsight(m_TempBuf, index);
}

void USER::DecreaseArmorItemDura()
{
	// Get the armor that player is wearing.
	int iArmorItemCount = 0;
	short sArmorItemIndex[EQUIP_ITEM_NUM];
	for (int i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		if (!m_InvItem[i].IsEmpty())
		{
			if (m_InvItem[i].bType == TYPE_ARMOR)
			{
				sArmorItemIndex[iArmorItemCount] = i;
				++iArmorItemCount;
			}
		}
	}

	// Decide which of the armor player is wearing will have its dura decreased.
	if (iArmorItemCount > 0)
	{
		int iIndex = myrand(0, iArmorItemCount-1);

		// Decrease the armor's durability.
		short sSlot = sArmorItemIndex[iIndex];
		ItemList* pArmor = &m_InvItem[sSlot];
		if (pArmor != NULL && !pArmor->IsEmpty())
		{
			PlusItemDur(pArmor, 1, true);
			SendItemInfoChange(BASIC_INV, sSlot, INFO_DUR);
			SendItemStatusMessage(pArmor);
			if (pArmor->sUsage <= 0)
			{
				pArmor->Init();
				SendDeleteItem(BASIC_INV, sSlot);
				SendItemInfo(sSlot);
				SendChangeWgt();
			}
		}
	}

	// Decide if either the earring or necklace accessory player is wearing will have its dura decreased.
	int iRand = myrand(0, 100);
	if (iRand >= 50)
	{
		BYTE byArm = -1;
		if (!m_InvItem[ARM_EAR].IsEmpty() && !m_InvItem[ARM_EAR].IsEmpty())
		{
			byArm = myrand(0, 1) ? ARM_EAR : ARM_NECK;
		}
		else if (!m_InvItem[ARM_EAR].IsEmpty())
		{
			byArm = ARM_EAR;
		}
		else
		{
			byArm = ARM_NECK;
		}

		// Decrease the accessory's durability.
		short sSlot = byArm;
		if (sSlot != -1) // Should never ever be -1 but meh :P
		{
			ItemList* pAccessory = &m_InvItem[sSlot];
			if (pAccessory != NULL && !pAccessory->IsEmpty())
			{
				PlusItemDur(pAccessory, 1, true);
				SendItemInfoChange(BASIC_INV, sSlot, INFO_DUR);
				SendItemStatusMessage(pAccessory);
				if (pAccessory->sUsage <= 0)
				{
					pAccessory->Init();
					SendDeleteItem(BASIC_INV, sSlot);
					SendItemInfo(sSlot);
					SendChangeWgt();
				}
			}
		}
	}

	// Decide if ring accessorys player is wearing will have its dura decreased.
	iRand = myrand(0, 100);
	if (iRand >= 50)
	{
		BYTE byArm = -1;
		if (!m_InvItem[ARM_HANDS].IsEmpty()	&& !m_InvItem[ARM_WRIST].IsEmpty())
		{
			byArm = myrand(0, 1) ? ARM_HANDS : ARM_WRIST;
		}
		else if (!m_InvItem[ARM_HANDS].IsEmpty())
		{
			byArm = ARM_HANDS;
		}
		else
		{
			byArm = ARM_WRIST;
		}

		// Decrease the accessory's durability.
		short sSlot = byArm;
		if (sSlot != -1) // Should never ever be -1 but meh :P
		{
			ItemList* pAccessory = &m_InvItem[sSlot];
			if (pAccessory != NULL && !pAccessory->IsEmpty())
			{
				PlusItemDur(pAccessory, 1, true);
				SendItemInfoChange(BASIC_INV, sSlot, INFO_DUR);
				SendItemStatusMessage(pAccessory);
				if (pAccessory->sUsage <= 0)
				{
					pAccessory->Init();
					SendDeleteItem(BASIC_INV, sSlot);
					SendItemInfo(sSlot);
					SendChangeWgt();
				}
			}
		}
	}
}

bool USER::CheckHaveSpecialNumInBody(short sSpecialNo)
{
	for (int i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		if (m_InvItem[i].sSpecial == sSpecialNo)
		{
			return true;
		}
	}
	return false;
}

void USER::CheckLevelUp()
{
	DWORD dwMaxExp = GetInc(INC_LEVEL_MAX_EXP) * 100;
	m_dwMaxExp = dwMaxExp;

	if (dwMaxExp == 0) return;
	if (m_dwExp < m_dwMaxExp) return;

	if (m_sLevel >= LEVEL_CAP)
	{
		m_dwExp = m_dwMaxExp;
		return;
	}

	LevelUp();

	m_dwExp = m_dwExp - m_dwMaxExp;

	DWORD dwNextMaxExp = GetInc(INC_LEVEL_MAX_EXP) * 100;
	m_dwMaxExp = dwNextMaxExp;

	SendCharData(INFO_ALL);
}

void USER::SetDamage(int iDamage, short sClass, bool bStopMagicCast)
{
	if (iDamage <= 0) return;
	if (!m_bLive) return;

	if (bStopMagicCast)
	{
		if (!m_bPreMagicCasted)
		{
			m_bMagicCasted = false;
		}
	}

	if (m_sHP <= (short)iDamage)
	{
		if (InterlockedCompareExchangePointer((PVOID*)&m_lDeadUsed, (PVOID)1, (PVOID)0) == (PVOID*)0)
		{
			m_sHP = 0;
			Dead(sClass);
		}
		else return;
	}
	else
	{
		m_sHP -= (short)iDamage;
	}
}

void USER::PlusMoralByNpc(int iNpcMoral)
{
	static int MoralTable[5][6] = {
		0,		100,	300,	500,	700,	10000,
		0,		50,		200,	300,	500,	8000,
		0,		10,		50,		200,	400,	8000,
		0,		1,		10,		50,		300,	5000,
		0,		0,		1,		10,		200,	5000};

	int iMoral = m_iMoral / CLIENT_MORAL;
	if (CheckDemon(m_sClass)) iMoral *= -1;

	int iMoralLevel = 0;
	if (iMoral < 4)
	{
		iMoralLevel = 0;
	}
	else if (iMoral < 10)
	{
		iMoralLevel = 1;
	}
	else if (iMoral < 30)
	{
		iMoralLevel = 2;
	}
	else if (iMoral < 45)
	{
		iMoralLevel = 3;
	}
	else if (iMoral < 49)
	{
		iMoralLevel = 4;
	}

	if (iNpcMoral > 5) iNpcMoral = 5;

	int iMoralExp = MoralTable[iMoralLevel][iNpcMoral];
	if (CheckDemon(m_sClass)) iMoralExp = -iMoralExp;

	if (SetMoral(iMoralExp))
	{
		SendCharData(INFO_BASICVALUE);
		SendMyInfo(TO_INSIGHT, INFO_MODIFY);
	}

	CString str;
	str.Format(IDS_GM_MSG_MORAL_EXP, iMoralExp);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);
}

void USER::NpcDeadEvent(CNpc* pNpc)
{
	// Run an event in evt script when npc is killed.
	if (!pNpc) return;
	if (pNpc->m_sEventNum == -1) return;

	int index = 0;
	SetByte(m_TempBuf, 2, index);
	SetInt(m_TempBuf, pNpc->m_sNid + NPC_BAND, index);
	ClientEvent(m_TempBuf);
}

void USER::LevelUp()
{
	int iSTR = m_iSTR / CLIENT_BASE_STATS;
	int iWIS = m_iWIS / CLIENT_EXT_STATS;
	int iCON = m_iCON / CLIENT_BASE_STATS;

	int iSTRInc = m_iSTRInc / CLIENT_BASE_STATS;
	int iDEXInc = m_iDEXInc / CLIENT_EXT_STATS;
	int iINTInc = m_iINTInc / CLIENT_BASE_STATS;
	int iWISInc = m_iWISInc / CLIENT_EXT_STATS;
	int iCONInc = m_iCONInc / CLIENT_BASE_STATS;

	int iAddHP = 0;
	int iAddMP = 0;

	// CHECKME : Check calculations for AddHP and AddMP - PASSED
	if (!CheckDemon(m_sClass))
	{
		double dAddHP = (static_cast<double>(iDEXInc) + iSTRInc + iCON) / 4.0;
		double dAddMP = (static_cast<double>(iWISInc) + iINTInc + iWIS) / 3.0;

		double dGainedSTR = static_cast<double>(m_iSTRInc) / GetInc(INC_STR);
		double dGainedINT = (static_cast<double>(m_iINTInc) / GetInc(INC_INT)) * 3.0;

		if (dGainedSTR > dGainedINT)
		{
			double dDiff = dGainedINT / dGainedSTR;
			if (dDiff < 0.8)
			{
				dAddHP *= 1.3;
				dAddMP *= 0.7;
			}
		}
		else if (dGainedSTR < dGainedINT)
		{
			double dDiff = dGainedSTR / dGainedINT;
			if (dDiff < 0.8)
			{
				dAddHP *= 0.7;
				dAddMP *= 1.3;
			}
		}

		iAddHP = static_cast<int>(dAddHP + 0.5);
		iAddMP = static_cast<int>(dAddMP + 0.5);

		// iAddHP = static_cast<int>((static_cast<double>(iDEXInc) + iSTRInc + iCON) / 4.0 + 0.5);
		// iAddMP = static_cast<int>((static_cast<double>(iWISInc) + iINTInc + iWIS) / 3.0 + 0.5);
	}
	else
	{
		iAddHP = static_cast<int>((((static_cast<double>(iCONInc) * 9.0) * 2.0) + ((iCON / 12.0) + (m_sLevel / 4.0)) + 30.0) / 15.0);
		iAddMP = static_cast<int>((((static_cast<double>(iWISInc) * 9.0) * 2.0) + ((iWIS / 12.0) + (m_sLevel / 4.0)) + 30.0) / 15.0);
	}

	m_sMaxHP += static_cast<short>(iAddHP);
	m_sHP = GetMaxHP();

	m_sMaxMP += static_cast<short>(iAddMP);
	m_sMP = GetMaxMP();

	m_sMaxStm += static_cast<short>((static_cast<double>(iSTR) + iCON) / 6.0);
	m_sStm = GetMaxStm();

	m_sMaxWgt += iCON / 2;

	if (CheckDemon(m_sClass))
	{
		PlusStr(m_iSTRInc);
		PlusDex(m_iDEXInc);
		PlusInt(m_iINTInc);
		PlusWis(m_iWISInc);
		PlusCon(m_iCONInc);
		PlusCha(m_iCHAInc);
	}

	m_iSTRInc = 0;
	m_iDEXInc = 0;
	m_iINTInc = 0;
	m_iWISInc = 0;
	m_iCONInc = 0;
	m_iCHAInc = 0;

	int iCurStorage = GetMaxStorageCount();

	++m_sLevel;
	ShowMagic(903, 0);

	if (m_sLevel >= LEVEL_CAP) SendServerChatMessage("Max Level Reached.", TO_ME);

	SendMyInfo(TO_INSIGHT, INFO_MODIFY);

	int iNowStorage = GetMaxStorageCount();
	if (iNowStorage > iCurStorage)
	{
		SendServerChatMessage(IDS_USER_STORAGE_SPACE_INC, TO_ME);
	}

	CheckNewEvent(); // (Looks to see if player has any new quests)
}

void USER::SendItemStatusMessage(ItemList* pItem)
{
	ASSERT(pItem != NULL);

	if (pItem->sUsage == 20 || pItem->sUsage == 15)
	{
		CString str;
		str.Format(IDS_USER_ITEM_BADLY_DMG, pItem->strNewName);
		SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}
	else if (pItem->sUsage == 10 || pItem->sUsage == 5)
	{
		CString str;
		str.Format(IDS_USER_ITEM_ABOUT_TO_BREAK, pItem->strNewName);
		SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}
	else if (pItem->sUsage > 0 && pItem->sUsage < 5)
	{
		CString str;
		str.Format(IDS_USER_ITEM_ABOUT_TO_BREAK, pItem->strNewName);
		SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}
	else if (pItem->sUsage <= 0)
	{
		CString str;
		str.Format(IDS_USER_ITEM_DESTROYED, pItem->strNewName);
		SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}
}

void USER::SendUserStatus(int iStatusType)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_STATUS_INFO, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);

	for (int i = 0; i < g_arMagicTable.GetSize(); i++)
	{
		CMagicTable* pMagic = g_arMagicTable[i];
		if (pMagic)
		{
			switch (iStatusType)
			{
			case MAGIC_CLASS_BLUE:	// Blue Magic
			case MAGIC_CLASS_WHITE: // White Magic
			case MAGIC_CLASS_BLACK: // Black Magic
				if (pMagic->m_tClass == iStatusType)
				{
					SetByte(m_TempBuf, 1, index);
					SetShort(m_TempBuf, pMagic->m_sMid, index);
				}
				break;
			case MAGIC_CLASS_ALL: // All
				SetByte(m_TempBuf, 1, index);
				SetShort(m_TempBuf, pMagic->m_sMid, index);
				break;
			case 5: // Poison
				if (pMagic->m_tEValue == MAGIC_EFFECT_POISON)
				{
					if (m_sRemainMagicNo[MAGIC_EFFECT_POISON] == -1)
					{
						SetByte(m_TempBuf, 1, index);
						SetShort(m_TempBuf, pMagic->m_sMid, index);
					}
				}
				break;
			}
		}
	}

	SetByte(m_TempBuf, 0, index);
	SendInsight(m_TempBuf, index);
}

bool USER::IsMapRange(int iAreaNumber)
{
	return IsMapRange(iAreaNumber, m_sX, m_sY);
}

bool USER::IsMapRange(int iAreaNumber, short sX, short sY)
{
	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize()) return false;
	if (sX >= g_Zones[m_iZoneIndex]->m_sizeMap.cx || sX < 0 ||
		sY >= g_Zones[m_iZoneIndex]->m_sizeMap.cy || sY < 0) return false;

	MAP* pMap = g_Zones[m_iZoneIndex];
	if (!pMap) return false;

	if (pMap->m_pMap[sX][sY].m_nAreaNumber == iAreaNumber)
	{
		if (iAreaNumber == MAP_AREA_HSOMA_GV)
		{
			// Extra check to see if the GVW for hsoma has started
			return g_bGuildTownWar[0];
		}
		else if (iAreaNumber == MAP_AREA_DSOMA_GV)
		{
			// Extra check to see if the GVW for dsoma has started
			return g_bGuildTownWar[1];
		}
		return true;
	}

	if (iAreaNumber == MAP_AREA_ON_WOTW_MAP)
	{
		if (m_sZ >= 14 && m_sZ <= 17 && g_bClassWar)
		{
			return true;
		}
	}
	else if (iAreaNumber == MAP_AREA_WOTW_MAP)
	{
		if (m_sZ >= 14 && m_sZ <= 17)
		{
			return true;
		}
	}

	return false;
}

void USER::CheckRangeAbility()
{
	// TODO: Implement CheckRangeAbility
}

bool USER::CheckDistance(USER *pTarget, int iDistance)
{
	if (!pTarget) return false;
	if (pTarget->m_sZ != m_sZ) return false;

	if (abs(m_sX - pTarget->m_sX) > iDistance || abs(m_sY - pTarget->m_sY) > iDistance)
	{
		return false;
	}

	return true;
}

bool USER::CheckAttackSuccess(USER* pTarget)
{
	if (!pTarget) return false;

	// Get the amount of attacking dex
	double dDexAttack = static_cast<double>(m_iDEX) / CLIENT_EXT_STATS;
	dDexAttack += PlusFromItem(SPECIAL_DEX);

	// Get the defense dex
	double dDexDefense = static_cast<double>(pTarget->m_iDEX) / CLIENT_EXT_STATS;
	dDexDefense += pTarget->PlusFromItem(SPECIAL_DEX);

	// Defense Dex - Attack Dex
	int iDexAttack = static_cast<int>((dDexAttack - dDexDefense) + 0.5);

	// Lookup inside attack success tables to get a value for the hit %
	if (iDexAttack >= -50)
	{
		if (iDexAttack < 0)
		{
			iDexAttack = g_DexHitRateLow[(abs(iDexAttack)-1)];
		}
		else if (iDexAttack == 0)
		{
			iDexAttack = 50;
		}
		else if (iDexAttack > 0 && iDexAttack < 50)
		{
			iDexAttack = g_DexHitRateHigh[(iDexAttack-1)];
		}
		else
		{
			iDexAttack = 99;
		}
	}
	else
	{
		iDexAttack = 20;
	}

	// Subtract any evasion gained from special attack dodging aura
	if (pTarget->CheckRemainSpecialAttack(SPECIAL_EFFECT_DODGE))
	{
		iDexAttack -= pTarget->m_RemainSpecial[SPECIAL_EFFECT_DODGE].sDamage;
	}

	// Add on any accuracy that is gained from wearing items
	iDexAttack += PlusFromItem(SPECIAL_ACCURACY);

	// Subtract any evasion that is gained from wearing items
	iDexAttack -= pTarget->PlusFromItem(SPECIAL_EVASION);

	// Minimum is 1
	if (iDexAttack <= 0)
	{
		iDexAttack = 1;
	}

	// Maximum is 99
	if (iDexAttack > 99)
	{
		iDexAttack = 99;
	}

	// Dev Message Showing Attack Rate of User
	CString strMessage;
	strMessage.Format(IDS_USER_ATTACK_RATE, iDexAttack);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)strMessage, TO_ME);

	// Calculate success / failure
	int iRandom = myrand(1, 100);
	return iDexAttack > iRandom ? true : false;
	return true;
}

int USER::GetDefense()
{
	double dDefense = 0;

	// Get the total amount of defense given by armor items being worn
	for (int i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		if (m_InvItem[i].bType == TYPE_ARMOR &&
			CheckDemon(m_sClass) == m_InvItem[i].IsDemonItem())
		{
			dDefense += m_InvItem[i].GetDefense();
		}
	}

	// TODO: Missing defense magic check and code to add the defense it gives.

	// Check for defense magic on user and apply the defense bonus
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_DEFENSE))
	{
		dDefense += m_RemainSpecial[SPECIAL_EFFECT_DEFENSE].sDamage;
	}

	if (!CheckDemon(m_sClass))
	{
		dDefense += (static_cast<double>(m_iSTR) / CLIENT_BASE_STATS) + PlusFromItem(SPECIAL_STR);
	}
	else
	{
		dDefense += (static_cast<double>(m_iCON) / CLIENT_BASE_STATS) + PlusFromItem(SPECIAL_CON);
	}

	dDefense += PlusFromItem(SPECIAL_PHY_DEF);

	DecreaseArmorItemDura();

	// Need to check for GVW Map Area and CheckGuildTown / CheckGuildTownAlly
	short sTownNum = -1;
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_bGuildTownWar[i] && IsMapRange(g_iGuildTownRange[i]))
		{
			sTownNum = i + 1;
			break;
		}
	}

	if (sTownNum != -1)
	{
		if (CheckGuildTown(sTownNum))
		{
			dDefense += 3;
		}

		if (CheckGuildTownAlly(sTownNum))
		{
			dDefense += 2;
		}
	}

	// CheckMinus (Checks to see if defense should be taken away because of
	// human being in a devil map or devil being in a human map etc...)
	if (CheckMinus())
	{
		dDefense = (dDefense * g_iClassWarMinus) / 100.0;
	}

	// Increase damage dealt to other class in WotW by the WotW owners calculated by how many wins and limit
	if (IsMapRange(MAP_AREA_ON_WOTW_MAP) &&
		(CheckDemon(g_iClassWar) == CheckDemon(m_sClass)))
	{
		int iReduce = (20 - g_iClassWarCount) * 5;
		if (iReduce < g_iClassWarLimit)
		{
			iReduce = g_iClassWarLimit;
		}
		dDefense = (dDefense * iReduce) / 100.0;
	}

	int iDefense  = static_cast<int>(dDefense + 0.5);

	CString str;
	str.Format(IDS_GM_MSG_DEFENSE, iDefense);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

	return iDefense;
}

bool USER::CheckCanPK(USER* pTarget)
{
	if (!pTarget) return false;

	if (CheckDemon(m_sClass) != CheckDemon(pTarget->m_sClass))
	{
		return true;
	}

	if (m_sLevel >= LEVEL_CAN_PK &&
		pTarget->m_iGrayCount > 0)
	{
		return true;
	}

	int iMoral = pTarget->m_iMoral / CLIENT_MORAL;
	if (!CheckDemon(pTarget->m_sClass))
	{
		if (iMoral <= -31) // Wicked
		{
			return true;
		}
	}
	else
	{
		if (iMoral >= 31) // Virtuous
		{
			return true;
		}
	}

	if (m_sGroup == pTarget->m_sGroup &&
		m_sGroup != -1)
	{
		return true;
	}

	// Can PK if at war with target's guild
	if (pTarget->m_sGroup == m_sGuildWar &&
		m_bInGuildWar)
	{
		return true;
	}

	if (pTarget->IsMapRange(MAP_AREA_DUEL) &&
		IsMapRange(MAP_AREA_DUEL))
	{
		return true;
	}

	if (pTarget->IsMapRange(MAP_AREA_HSOMA_GV) &&
		IsMapRange(MAP_AREA_HSOMA_GV))
	{
		return true;
	}

	if (pTarget->IsMapRange(MAP_AREA_DSOMA_GV) &&
		IsMapRange(MAP_AREA_DSOMA_GV))
	{
		return true;
	}

	if (pTarget->m_sLevel >= LEVEL_CAN_PK)
	{
		return true;
	}

	return false;
}

bool USER::CheckSafetyZone()
{
	// See if the player is within a safe zone
	// while class war is in progress.
	if (g_bClassWar)
	{
		return IsMapRange(MAP_AREA_WOTW_SAFE);
	}
	return false;
}

bool USER::CheckGuildWarArmy(USER* pTarget)
{
	if (!pTarget) return false;

	if (pTarget->m_sGroup == m_sGuildWar &&
		m_bInGuildWar)
	{
		return true;
	}

	if (IsMapRange(MAP_AREA_DUEL) &&
		pTarget->IsMapRange(MAP_AREA_DUEL))
	{
		return true;
	}

	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_bGuildTownWar[i])
		{
			if (IsMapRange(g_iGuildTownRange[i]) && pTarget->IsMapRange(g_iGuildTownRange[i]))
			{
				if (m_sGroup != -1)
				{
					return true;
				}
			}
		}
	}

	return false;
}

int USER::GetDefenseHuman(USER* pTarget)
{
	// TODO: Implement GetDefenseHuman
	return 0;
}

int USER::GetAttackHuman(USER* pTarget)
{
	// TODO: Implement GetAttackHuman
	return 0;
}

void USER::CheckClassPoint(USER* pTarget)
{
	// Pos doesn't change when same class (Race).
	if (CheckDemon(m_sClass) == CheckDemon(pTarget->m_sClass)) return;

	// Pos doesn't change when inside the arena area.
	if (IsMapRange(MAP_AREA_DUEL) || pTarget->IsMapRange(MAP_AREA_DUEL)) return;

	// Pos doesn't change if level less than can PK level.
	if (m_sLevel < LEVEL_CAN_PK || pTarget->m_sLevel < LEVEL_CAN_PK) return;

	// Pos doesn't change if target has <= POS_LOWEST pos.
	if (pTarget->m_iClassPoint <= CLASS_POINT_LOWEST) return;

	// Get the amount of pos by level difference
	int iLevelDiff = pTarget->m_sLevel - m_sLevel;
	int iPos = 500 + (iLevelDiff * 20);
	if (iPos < 10) iPos = 10;

	// Update the target's pos.
	pTarget->m_iClassPoint -= iPos;
	if (pTarget->m_iClassPoint < CLASS_POINT_LOWEST)
	{
		pTarget->m_iClassPoint = CLASS_POINT_LOWEST;
	}
	pTarget->SendCharData(INFO_NAMES);
	pTarget->SendMyInfo(TO_INSIGHT, INFO_MODIFY);

	// Update the source's pos.
	CheckMaxValue(m_iClassPoint, iPos);
	SendCharData(INFO_NAMES);
	SendMyInfo(TO_INSIGHT, INFO_MODIFY);
}

void USER::IsDetecterRange()
{
	for (int i = 0; i < g_arNpcTypeDetecter.GetSize(); i++)
	{
		CNpc* pNpc = GetNpc(g_arNpcTypeDetecter[i]);
		if (!pNpc) continue;

		if (m_sZ != pNpc->m_sZone) continue;
		if (m_sX < pNpc->m_sMinDetecterX) continue;
		if (m_sX > pNpc->m_sMaxDetecterX) continue;
		if (m_sY < pNpc->m_sMinDetecterY) continue;
		if (m_sY > pNpc->m_sMaxDetecterY) continue;

		pNpc->AttackListAdd(this);

		CPoint pt = pNpc->FindNearAvailablePoint_S(m_sX, m_sY, pNpc->m_sDistance);
		if (pt.x != -1 && pt.y != -1)
		{
			pNpc->MoveNpc(static_cast<short>(pt.x), static_cast<short>(pt.y), m_pCom);
			if (pNpc->IsCloseTarget(m_pCom, pNpc->m_sDistance))
			{
				pNpc->m_NpcState = NPC_FIGHTING;
				pNpc->m_Delay = 0;
			}
		}
		return;
	}
}

bool USER::CheckGray(USER* pTarget)
{
	if (!pTarget) return false;

	if (CheckDemon(m_sClass) != CheckDemon(pTarget->m_sClass)) return false;

	int iMoral = pTarget->m_iMoral / CLIENT_MORAL;
	if (!CheckDemon(pTarget->m_sClass))
	{
		if (iMoral <= -31) // Wicked
		{
			return false;
		}
	}
	else
	{
		if (iMoral >= 31) // Virtuous
		{
			return false;
		}
	}

	if (pTarget->IsMapRange(MAP_AREA_DUEL) &&
		IsMapRange(MAP_AREA_DUEL))
	{
		return false;
	}

	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_bGuildTownWar[i] && IsMapRange(g_iGuildTownRange[i]))
		{
			if (m_sGroup != -1)
			{
				return false;
			}
		}
	}

	return true;
}

void USER::SetGrayUser(BYTE byGrayMode)
{
	if (byGrayMode == GRAY_MODE_NORMAL)
	{
		m_iGrayCount = 120;
		m_dwGrayTime = GetTickCount();
	}

	SendGrayUser(byGrayMode);
}

BYTE USER::GetGrayUser()
{
	BYTE byGrayMode = GRAY_MODE_NONE;
	CheckGrayCount();

	if (m_iGrayCount > 0)
	{
		if (m_iGrayCount > 60)
		{
			byGrayMode = GRAY_MODE_NORMAL;
		}
		else
		{
			byGrayMode = GRAY_MODE_FLASH;
		}
	}
	return byGrayMode;
}

void USER::CheckGrayCount()
{
	if (m_iGrayCount <= 0) return;

	DWORD dwCurTime = GetTickCount();
	DWORD dwTimeDif = dwCurTime - m_dwGrayTime;
	if (dwTimeDif < 5000)
	{
		return;
	}

	m_dwGrayTime = dwCurTime;
	m_iGrayCount -= dwTimeDif / 1000;

	if (m_iGrayCount <= 0)
	{
		m_iGrayCount = 0;
		SetGrayUser(GRAY_MODE_NONE);
	}
	else if (m_iGrayCount > 0 && m_iGrayCount <= 60)
	{
		SetGrayUser(GRAY_MODE_FLASH);
	}
}

void USER::SendGrayUser(BYTE byGrayMode)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_SETGRAY, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	SetByte(m_TempBuf, byGrayMode, index);
	SendInsight(m_TempBuf, index);
}

void USER::Dead(short sClass)
{
	// Release guild storage
	GuildStorageClose();

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

	if (!m_bLive)
	{
		return;
	}

	if (IsMapRange(MAP_AREA_DUEL) &&
		CheckDemon(m_sClass) == CheckDemon(g_mapZoneClass[m_sZ]))
	{
		m_iDeathType |= DEATH_TYPE_DUEL;
	}
	else if (IsMapRange(MAP_AREA_HSOMA_GV))
	{
		m_iDeathType |= DEATH_TYPE_HSOMA_GV;
	}
	else if (IsMapRange(MAP_AREA_DSOMA_GV))
	{
		m_iDeathType |= DEATH_TYPE_DSOMA_GV;
	}
	else if (IsMapRange(MAP_AREA_ON_WOTW_MAP))
	{
		m_iDeathType |= DEATH_TYPE_WOTW;
	}
	else if (sClass != -1 && (CheckDemon(sClass) != CheckDemon(m_sClass)))
	{
		m_iDeathType |= DEATH_TYPE_CLASS;
	}

	SetUid(m_sX, m_sY, 0);

	m_bLive = false;

	// Tell other users insight that user is now dead
	int index = 0;
	SetByte(m_TempBuf, PKT_DEAD, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	CPoint pt = ConvertToClient(m_sX, m_sY);
	SetShort(m_TempBuf, (short)pt.x, index);
	SetShort(m_TempBuf, (short)pt.y, index);
	SendInsight(m_TempBuf, index);

	InitRemainMagic(MAGIC_CLASS_ALL);
	InitRemainSpecial();

	m_bGuildWarDead = true; // sets name to black when died when in a guild war

	if (m_iGrayCount > 0)
	{
		m_bLiveGray = true;
	}

	m_iGrayCount = 0;

	SendUserStatus(MAGIC_CLASS_ALL);

	if (m_bInGuildWar && m_sGuildRank == Guild::GUILD_CHIEF)
	{
		CheckGuildBossDead();
	}

	// TODO: UpdateUserData 1 - 0 (Updates the shared memory not database afaik)
}

void USER::SendSetLive()
{
	int index = 0;
	SetByte(m_TempBuf, PKT_LIFE, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	SendInsight(m_TempBuf, index);
}

bool USER::CheckPK(USER* pTarget)
{
	if (!pTarget) return false;

	if (CheckDemon(m_sClass) != CheckDemon(pTarget->m_sClass))
	{
		return false;
	}

	if (pTarget->m_iGrayCount > 0)
	{
		return false;
	}

	int iMoral = pTarget->m_iMoral / CLIENT_MORAL;
	if (!CheckDemon(pTarget->m_sClass))
	{
		if (iMoral <= -31) // Wicked
		{
			return false;
		}
	}
	else
	{
		if (iMoral >= 31) // Virtuous
		{
			return false;
		}
	}

	if (m_sGroup == pTarget->m_sGroup &&
		m_sGroup != -1)
	{
		return false;
	}

	if (pTarget->m_iDeathType & DEATH_TYPE_GUILDWAR)
	{
		pTarget->m_iDeathType ^= DEATH_TYPE_GUILDWAR;
	}

	// Can PK if at war with target's guild
	if (pTarget->m_sGroup == m_sGuildWar &&
		m_bInGuildWar)
	{
		if (pTarget->m_bInGuildWar &&
			pTarget->m_sGuildWar == m_sGroup)
		{
			pTarget->m_iDeathType |= DEATH_TYPE_GUILDWAR;
		}
		return false;
	}

	if (IsMapRange(MAP_AREA_DUEL) &&
		pTarget->IsMapRange(MAP_AREA_DUEL))
	{
		return false;
	}

	return true;
}

int USER::CompareOtherValue(int iIncType)
{
	int iCount = 1;

	switch (iIncType)
	{
	case INC_STR:
		if (m_iSTR / static_cast<double>(CLIENT_BASE_STATS) >
			m_iINT / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}

		if (m_iSTR / static_cast<double>(CLIENT_BASE_STATS) >
			m_iDEX / static_cast<double>(CLIENT_EXT_STATS))
		{
			++iCount;
		}

		if (m_iSTR / static_cast<double>(CLIENT_BASE_STATS) >
			m_iWIS / static_cast<double>(CLIENT_EXT_STATS))
		{
			++iCount;
		}

		if (m_iSTR / static_cast<double>(CLIENT_BASE_STATS) >
			m_iCON / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}
		break;
	case INC_DEX:
		if (m_iDEX / static_cast<double>(CLIENT_EXT_STATS) >
			m_iSTR / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}

		if (m_iDEX / static_cast<double>(CLIENT_EXT_STATS) >
			m_iINT / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}

		if (m_iDEX / static_cast<double>(CLIENT_EXT_STATS) >
			m_iWIS / static_cast<double>(CLIENT_EXT_STATS))
		{
			++iCount;
		}

		if (m_iDEX / static_cast<double>(CLIENT_EXT_STATS) >
			m_iCON / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}
		break;
	case INC_INT:
		if (m_iINT / static_cast<double>(CLIENT_BASE_STATS) >
			m_iSTR / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}

		if (m_iINT / static_cast<double>(CLIENT_BASE_STATS) <
			m_iDEX / static_cast<double>(CLIENT_EXT_STATS))
		{
			++iCount;
		}

		if (m_iINT / static_cast<double>(CLIENT_BASE_STATS) >
			m_iWIS / static_cast<double>(CLIENT_EXT_STATS))
		{
			++iCount;
		}

		if (m_iINT / static_cast<double>(CLIENT_BASE_STATS) >
			m_iCON / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}
		break;
	case INC_WIS:
		if (m_iWIS / static_cast<double>(CLIENT_EXT_STATS) >
			m_iSTR / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}

		if (m_iWIS / static_cast<double>(CLIENT_EXT_STATS) >
			m_iINT / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}

		if (m_iWIS / static_cast<double>(CLIENT_EXT_STATS) >
			m_iDEX / static_cast<double>(CLIENT_EXT_STATS))
		{
			++iCount;
		}

		if (m_iWIS / static_cast<double>(CLIENT_EXT_STATS) >
			m_iCON / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}
		break;
	case INC_CON:
		if (m_iCON / static_cast<double>(CLIENT_BASE_STATS) >
			m_iSTR / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}

		if (m_iCON / static_cast<double>(CLIENT_BASE_STATS) >
			m_iINT / static_cast<double>(CLIENT_BASE_STATS))
		{
			++iCount;
		}

		if (m_iCON / static_cast<double>(CLIENT_BASE_STATS) >
			m_iDEX / static_cast<double>(CLIENT_EXT_STATS))
		{
			++iCount;
		}

		if (m_iCON / static_cast<double>(CLIENT_BASE_STATS) >
			m_iWIS / static_cast<double>(CLIENT_EXT_STATS))
		{
			++iCount;
		}
		break;
	default:
		break;
	}

	if (iCount >= 3 && iIncType == INC_CON)
	{
		return 100;
	}
	else if (iCount >= 4)
	{
		return 50;
	}
	else if (iCount == 3)
	{
		return 10;
	}

	return 0;
}

CPoint USER::RandNearPoint_S(int iDistance)
{
	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize()) return CPoint(-1,-1);

	int iMinX = m_sX - iDistance;
	if (iMinX < 0) iMinX = 0;

	int iMaxX = m_sX + iDistance;
	if (iMaxX > g_Zones[m_iZoneIndex]->m_sizeMap.cx) iMaxX = g_Zones[m_iZoneIndex]->m_sizeMap.cx;

	int iMinY = m_sY - iDistance;
	if (iMinY < 0) iMinY = 0;

	int iMaxY = m_sY + iDistance;
	if (iMaxY > g_Zones[m_iZoneIndex]->m_sizeMap.cy) iMaxY = g_Zones[m_iZoneIndex]->m_sizeMap.cy;

	for (int i = 0; i < 5; i++)
	{
		int iX = myrand(iMinX, iMaxX);
		int iY = myrand(iMinY, iMaxY);

		if (IsMovable_S(iX, iY))
		{
			return CPoint(iX, iY);
		}
	}

	return CPoint(-1,-1);
}

int USER::GetInsurancePolicy()
{
	int iSlot = -1;
	for (int i = 0; i < INV_ITEM_NUM; i++)
	{
		if (m_InvItem[i].sSpecial == SPECIAL_REVIVE)
		{
			iSlot = i;
			break;
		}
	}
	return iSlot;
}

void USER::SetBeLiveInDuel()
{
	m_sHP = 1;
	m_sStm = GetMaxStm();
	m_bLive = true;
	m_bLiveGray = false;
	InterlockedExchange(&m_lDeadUsed, (LONG)0);
	SendCharData(INFO_ALL);

	// Move player to location where they will revive in arena
	CPoint pt;
	if (!CheckDemon(m_sClass))
	{
		pt = FindRandPointInRect_C(4, 193, 479, 211, 483);
		if (pt.x == -1 || pt.y == -1)
		{
			pt.x = 193;
			pt.y = 479;
		}
	}
	else
	{
		pt = FindRandPointInRect_C(9, 472, 966, 490, 966);
		if (pt.x == -1 || pt.y == -1)
		{
			pt.x = 472;
			pt.y = 966;
		}
	}

	LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
	SendSetLive();

	DWORD dwTick = GetTickCount();
	m_dwLastRecoverHPTime = dwTick;
	m_dwLastRecoverMPTime = dwTick;
}

void USER::SetBeLiveInPKZone()
{
	if (m_iDeathType & DEATH_TYPE_WOTW)
	{
		m_sHP = GetMaxHP() / 2;
	}
	else
	{
		m_sHP = 1;
	}
	m_sStm = GetMaxStm();
	m_bLive = true;
	m_bLiveGray = false;
	InterlockedExchange(&m_lDeadUsed, (LONG)0);
	SendCharData(INFO_ALL);

	switch (m_iDeathType)
	{
	case DEATH_TYPE_HSOMA_GV:
	case DEATH_TYPE_DSOMA_GV:
		if (m_sGroup == -1)
		{
			if (m_bLogout)
			{
				DeadThrowMoney(false);
			}
			else
			{
				DeadThrowMoney(true);
			}
		}
		break;
	case 200:
		if (m_bLogout)
		{
			DeadThrowItem(false, false);
			DeadThrowMoney(false);
		}
		else
		{
			DeadThrowItem(true, true);
			DeadThrowMoney(true);
		}
		break;
	default:
		break;
	}

	if (m_iDeathType & DEATH_TYPE_HSOMA_GV)
	{
		if (CheckGuildTown(1) || CheckGuildTownAlly(1))
		{
			if (m_sZ == 8)
			{
				CPoint pt = FindRandPointInRect_C(1, 168, 793, 265, 794);
				if (pt.x == -1 || pt.y == -1)
				{
					pt.x = 168;
					pt.y = 794;
				}
				LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
			}
			else
			{
				CPoint pt = FindRandPointInRect_C(8, 1, 41, 58, 44);
				if (pt.x == -1 || pt.y == -1)
				{
					pt.x = 1;
					pt.y = 41;
				}
				LinkToOtherZone(8, static_cast<short>(pt.x), static_cast<short>(pt.y));
			}
		}
		else
		{
			if (m_sGroup == -1)
			{
				if (m_bLogout)
				{
					DeadThrowItem(false, false);
				}
				else
				{
					DeadThrowItem(false, true);
				}
			}

			CPoint pt = FindRandPointInRect_C(1, 250, 812, 278, 812);
			if (pt.x == -1 || pt.y == -1)
			{
				pt.x = 242;
				pt.y = 812;
			}
			LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
	}
	else if (m_iDeathType & DEATH_TYPE_DSOMA_GV)
	{
		if (CheckGuildTown(2) || CheckGuildTownAlly(2))
		{
			if (m_sZ == 13)
			{
				CPoint pt = FindRandPointInRect_C(11, 266, 302, 278, 302);
				if (pt.x == -1 || pt.y == -1)
				{
					pt.x = 266;
					pt.y = 302;
				}
				LinkToOtherZone(11, static_cast<short>(pt.x), static_cast<short>(pt.y));
			}
			else
			{
				CPoint pt = FindRandPointInRect_C(13, 10, 42, 56, 42);
				if (pt.x == -1 || pt.y == -1)
				{
					pt.x = 10;
					pt.y = 42;
				}
				LinkToOtherZone(13, static_cast<short>(pt.x), static_cast<short>(pt.y));
			}
		}
		else
		{
			if (m_sGroup == -1)
			{
				if (m_bLogout)
				{
					DeadThrowItem(false, false);
				}
				else
				{
					DeadThrowItem(false, true);
				}
			}

			CPoint pt = FindRandPointInRect_C(11, 276, 324, 302, 324);
			if (pt.x == -1 || pt.y == -1)
			{
				pt.x = 276;
				pt.y = 324;
			}
			LinkToOtherZone(11, static_cast<short>(pt.x), static_cast<short>(pt.y));
		}
	}
	else if (m_iDeathType & DEATH_TYPE_WOTW || m_iDeathType & DEATH_TYPE_CLASS)
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

	SendSetLive();

	DWORD dwTick = GetTickCount();
	m_dwLastRecoverHPTime = dwTick;
	m_dwLastRecoverMPTime = dwTick;
}

bool USER::CheckRevive()
{
	// TODO : Check Calculations on CheckRevive (Including Mage type)
	if (m_sLevel > 3)
	{
		return true;
	}

	if (GetInsurancePolicy() != -1)
	{
		return true;
	}

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

	if (m_dwExp < dwRDExp) return false;
	if (m_iSTR < iRDSTR) return false;
	if (m_iDEX < iRDDEX) return false;
	if (m_iINT < iRDINT) return false;
	if (m_iWIS < iRDWIS) return false;
	if (m_iCON < iRDCON) return false;
	if (m_iCHA < iRDCHA) return false;

	if (!m_InvItem[ARM_RHAND].IsEmpty())
	{
		switch (m_InvItem[ARM_RHAND].bType)
		{
		case TYPE_SWORD:
			if (m_iSwordExp < GetInc(INC_RDSWORD)) return false;
			break;
		case TYPE_AXE:
			if (m_iAxeExp < GetInc(INC_RDAXE)) return false;
			break;
		case TYPE_BOW:
			if (m_iBowExp < GetInc(INC_RDBOW)) return false;
			break;
		case TYPE_SPEAR:
			if (m_iSpearExp < GetInc(INC_RDKNUCKLE)) return false;
			break;
		case TYPE_KNUCKLE:
			if (m_iKnuckleExp < GetInc(INC_RDKNUCKLE)) return false;
			break;
		case TYPE_STAFF:
			if (m_iStaffExp < GetInc(INC_RDSTAFF)) return false;
			break;
		}
	}

	switch (m_sMageType)
	{
	case MAGIC_CLASS_BLUE:
		if (m_iBMagicExp < GetInc(INC_RDBLUE_MAGIC)) return false;
		break;
	case MAGIC_CLASS_WHITE:
		if (m_iWMagicExp < GetInc(INC_RDWHITE_MAGIC)) return false;
		break;
	case MAGIC_CLASS_BLACK:
		if (m_iDMagicExp < GetInc(INC_RDBLACK_MAGIC)) return false;
		break;
	}

	return true;
}

void USER::SetBeLiveByDetecter()
{
	m_bLiveGray = true;
	InterlockedExchange(&m_lDeadUsed, (LONG)0);

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

	m_sHP = 1;
	m_sStm = GetMaxStm();
	m_bLive = true;
	m_bLiveGray = false;
	SendCharData(INFO_ALL);

	// Move player to location where they will revive
	CPoint pt;
	if (CheckDemon(m_sClass) != CheckDemon(g_mapZoneClass[m_sZ]))
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
	else
	{
		switch (m_sZ)
		{
		case 1:
			pt = FindRandPointInRect_C(1, 32, 126, 40, 126);
			if (pt.x == -1 || pt.y == -1)
			{
				pt.x = 32;
				pt.y = 126;
			}
			LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
			break;
		case 4:
			pt = FindRandPointInRect_C(4, 149, 495, 160, 496);
			if (pt.x == -1 || pt.y == -1)
			{
				pt.x = 149;
				pt.y = 495;
			}
			LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
			break;
		case 5:
		case 12:
			pt = FindRandPointInRect_C(5, 369, 303, 384, 304);
			if (pt.x == -1 || pt.y == -1)
			{
				pt.x = 369;
				pt.y = 303;
			}
			LinkToOtherZone(5, static_cast<short>(pt.x), static_cast<short>(pt.y));
			break;
		case 9:
		case 10:
		case 11:
		case 13:
			pt = FindRandPointInRect_C(9, 307, 959, 333, 959);
			if (pt.x == -1 || pt.y == -1)
			{
				pt.x = 307;
				pt.y = 959;
			}
			LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
			break;
		case 14:
		case 15:
		case 16:
		case 17:
			pt = FindRandPointInRect_C(14, 4, 232, 12, 232);
			if (pt.x == -1 || pt.y == -1)
			{
				pt.x = 4;
				pt.y = 232;
			}
			LinkToOtherZone(14, static_cast<short>(pt.x), static_cast<short>(pt.y));
			break;
		default:
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
			break;
		}
	}

	SendSetLive();

	DWORD dwTick = GetTickCount();
	m_dwLastRecoverHPTime = dwTick;
	m_dwLastRecoverMPTime = dwTick;
}

bool USER::IsZoneInThisServer(int zone)
{
	for (int i = 0; i < g_Zones.GetSize(); i++)
	{
		if (g_Zones[i]->m_Zone == zone)
		{
			return true;
		}
	}
	return false;
}

bool USER::OpenChangeOtherItem(short sSpecialNum)
{
	if (sSpecialNum < 0) return false;

	m_sChangeOtherItemSpecialNum = sSpecialNum;

	int index = 0;
	short sItemCount = 0;
	for (int i = 0; i < g_arChangeOtherItemTable.GetSize(); i++)
	{
		CChangeOtherItemTable* pOtherItem = g_arChangeOtherItemTable[i];
		if (pOtherItem)
		{
			if (pOtherItem->m_sSpecialNum == m_sChangeOtherItemSpecialNum)
			{
				SetShort(m_TempBuf, pOtherItem->m_sId, index);
				SetVarString(m_TempBuf, (LPTSTR)(LPCTSTR)pOtherItem->m_strItemName, pOtherItem->m_strItemName.GetLength(),index);
				++sItemCount;
			}
		}
	}

	CBufferEx TempBuf;
	TempBuf.Add(PKT_CHANGE_OTHER_ITEM);
	TempBuf.Add((BYTE)0);
	TempBuf.Add(sItemCount);
	TempBuf.AddData(m_TempBuf, index);
	Send(TempBuf, TempBuf.GetLength());

	return true;
}

bool USER::CheckMinus()
{
	if (CheckDemon(m_sClass) != CheckDemon(g_mapZoneClass[m_sZ]))
	{
		if (!IsMapRange(MAP_AREA_ON_WOTW_MAP))
		{
			return true;
		}
	}
	return false;
}

bool USER::SetMoral(int iPlusMoral)
{
	int iOldMoral = m_iMoral;
	m_iMoral += iPlusMoral;

	if (m_iMoral < -7400000) m_iMoral = -7400000;
	if (m_iMoral > 7400000) m_iMoral = 7400000;

	CString strOldMoralName = GetMoralName(iOldMoral / CLIENT_MORAL);
	CString strMoralName = GetMoralName(m_iMoral / CLIENT_MORAL);
	if (strOldMoralName.Compare(strMoralName) != 0)
	{
		return true;
	}
	return false;
}

CString USER::GetMoralName(int iMoral)
{
	CString strMoralName = "";

	if (iMoral <= -50)
	{
		strMoralName = "Devil";
	}
	else if (iMoral >= -49 && iMoral <= -31)
	{
		strMoralName = "Wicked";
	}
	else if (iMoral >= -30 && iMoral <= -11)
	{
		strMoralName = "Evil";
	}
	else if (iMoral >= -10 && iMoral <= -4)
	{
		strMoralName = "Bad";
	}
	else if (iMoral >= -3 && iMoral <= 3)
	{
		strMoralName = "Neutral";
	}
	else if (iMoral >= 4 && iMoral <= 10)
	{
		strMoralName = "Good";
	}
	else if (iMoral >= 11 && iMoral <= 30)
	{
		strMoralName = "Moral";
	}
	else if (iMoral >= 31 && iMoral <= 49)
	{
		strMoralName = "Virtuous";
	}
	else if (iMoral >= 50 && iMoral <= 74)
	{
		strMoralName = "Angel";
	}
	else if (iMoral >= 75)
	{
		strMoralName = "Unknown";
	}

	return strMoralName;
}

bool USER::IsServerRank(int iRank)
{
	if (iRank == m_sRank || m_sRank == SERVER_RANK_ALL) return true;

	if (iRank != SERVER_RANK_NORMAL)
	{
		if (iRank == SERVER_RANK_BBS &&
			m_sRank == SERVER_RANK_LEVEL2 ||
			m_sRank == SERVER_RANK_LEVEL3 ||
			m_sRank == SERVER_RANK_LEVEL4)
		{
			return true;
		}
	}

	return false;
}

int USER::GetMinWeaponExp(int iIncType)
{
	int iWeaponExps[6] = {0,};

	if (iIncType != INC_AXE)
		iWeaponExps[0] = m_iAxeExp;
	if (iIncType != INC_BOW)
		iWeaponExps[1] = m_iBowExp;
	if (iIncType != INC_KNUCKLE)
		iWeaponExps[2] = m_iKnuckleExp;
	if (iIncType != INC_SPEAR)
		iWeaponExps[3] = m_iSpearExp;
	if (iIncType != INC_STAFF)
		iWeaponExps[4] = m_iStaffExp;
	if (iIncType != INC_SWORD)
		iWeaponExps[5] = m_iSwordExp;

	int iMinWeaponExp = 0;
	int iIncTypeMin = -1;
	for (int i = 0; i < 6; i++)
	{
		int iWeaponExp = iWeaponExps[i];
		if (iMinWeaponExp == 0)
		{
			iMinWeaponExp = iWeaponExp;
			iIncTypeMin = i;
		}
		else if (iWeaponExp != 0 &&
			iWeaponExp < iMinWeaponExp)
		{
			iMinWeaponExp = iWeaponExp;
			iIncTypeMin = i;
		}
	}

	return iIncTypeMin + INC_AXE;
}

void USER::MinusWeaponExp(int iIncType, int iInc)
{
	while (iInc > 0)
	{
		int iIncTypeMin = GetMinWeaponExp(iIncType);
		switch (iIncTypeMin)
		{
		case INC_AXE:
			m_iAxeExp -= iInc;
			if (m_iAxeExp < 0)
			{
				iInc = abs(m_iAxeExp);
				m_iAxeExp = 0;
			}
			else
			{
				iInc = 0;
			}
			break;
		case INC_BOW:
			m_iBowExp -= iInc;
			if (m_iBowExp < 0)
			{
				iInc = abs(m_iBowExp);
				m_iBowExp = 0;
			}
			else
			{
				iInc = 0;
			}
			break;
		case INC_KNUCKLE:
			m_iKnuckleExp -= iInc;
			if (m_iKnuckleExp < 0)
			{
				iInc = abs(m_iKnuckleExp);
				m_iKnuckleExp = 0;
			}
			else
			{
				iInc = 0;
			}
			break;
		case INC_SPEAR:
			m_iSpearExp -= iInc;
			if (m_iSpearExp < 0)
			{
				iInc = abs(m_iSpearExp);
				m_iSpearExp = 0;
			}
			else
			{
				iInc = 0;
			}
			break;
		case INC_STAFF:
			m_iStaffExp -= iInc;
			if (m_iStaffExp < 0)
			{
				iInc = abs(m_iStaffExp);
				m_iStaffExp = 0;
			}
			else
			{
				iInc = 0;
			}
			break;
		case INC_SWORD:
			m_iSwordExp -= iInc;
			if (m_iSwordExp < 0)
			{
				iInc = abs(m_iSwordExp);
				m_iSwordExp = 0;
			}
			else
			{
				iInc = 0;
			}
			break;
		}
	}
}

int USER::GetMinMagicExp(int iIncType)
{
	int iMagicExps[3] = {0,};

	if (iIncType != INC_BLACK_MAGIC)
		iMagicExps[0] = m_iDMagicExp;
	if (iIncType != INC_WHITE_MAGIC)
		iMagicExps[1] = m_iWMagicExp;
	if (iIncType != INC_BLUE_MAGIC)
		iMagicExps[2] = m_iBMagicExp;

	int iMinMagicExp = 0;
	int iIncTypeMin = -1;
	for (int i = 0; i < 3; i++)
	{
		int iMagicExp = iMagicExps[i];
		if (iMinMagicExp == 0 || (iMagicExp != 0 && iMagicExp < iMinMagicExp))
		{
			iMinMagicExp = iMagicExp;
			iIncTypeMin = i;
		}
	}

	return iIncTypeMin + INC_BLACK_MAGIC;
}

void USER::MinusMagicExp(int iIncType, int iInc)
{
	while (iInc > 0)
	{
		int iIncTypeMin = GetMinMagicExp(iIncType);
		switch (iIncTypeMin)
		{
		case INC_BLUE_MAGIC:
			m_iBMagicExp -= iInc;
			if (m_iBMagicExp < 0)
			{
				iInc = abs(m_iBMagicExp);
				m_iBMagicExp = 0;
			}
			else
			{
				iInc = 0;
			}
			break;
		case INC_WHITE_MAGIC:
			m_iWMagicExp -= iInc;
			if (m_iWMagicExp < 0)
			{
				iInc = abs(m_iWMagicExp);
				m_iWMagicExp = 0;
			}
			else
			{
				iInc = 0;
			}
			break;
		case INC_BLACK_MAGIC:
			m_iDMagicExp -= iInc;
			if (m_iDMagicExp < 0)
			{
				iInc = abs(m_iDMagicExp);
				m_iDMagicExp = 0;
			}
			else
			{
				iInc = 0;
			}
			break;
		}
	}
}

bool USER::CheckOtherJobLimit(int iClass)
{
	// Check isn't hsoma class.
	if (m_sClass <= 0) return true;

	// Nothing to check if not wearing a weapon
	if (m_InvItem[ARM_RHAND].IsEmpty()) return true;

	// Weapon skill is checked based on type of weapon being worn.
	// Dsoma characters cannot gain more than 50 skill in another class weapon type.
	int iWeaponExp = 0;
	int iMyJob = -1;
	switch (m_InvItem[ARM_RHAND].bType)
	{
	case TYPE_SWORD:
		iMyJob = DEVIL_CLASS_SWORDSMAN;
		iWeaponExp = m_iSwordExp / CLIENT_SKILL;
		break;
	case TYPE_AXE:
		iMyJob = DEVIL_CLASS_WARRIOR;
		iWeaponExp = m_iAxeExp / CLIENT_SKILL;
		break;
	case TYPE_BOW:
		iMyJob = DEVIL_CLASS_ARCHER;
		iWeaponExp = m_iBowExp / CLIENT_SKILL;
		break;
	case TYPE_SPEAR:
		iMyJob = DEVIL_CLASS_KNIGHT;
		iWeaponExp = m_iSpearExp / CLIENT_SKILL;
		break;
	case TYPE_KNUCKLE:
		iMyJob = DEVIL_CLASS_FIGHTER;
		iWeaponExp = m_iKnuckleExp / CLIENT_SKILL;
		break;
	case TYPE_STAFF:
		iMyJob = DEVIL_CLASS_WIZARD;
		iWeaponExp = m_iStaffExp / CLIENT_SKILL;
		break;
	}
	if (iWeaponExp > 50 && !CheckMyJob(iMyJob))
	{
		return true;
	}

	return false;
}

void USER::ClassWarStart()
{
	// Is it started?
	if (!g_bClassWar) return;

	// Is it supposed to be starting?
	if (!g_bClassWarStart) return;

	g_bClassWarStart = false;

	// Inform all that the class war is has begun!
	SendServerChatMessage(IDS_CLASS_WAR_BEGUN, TO_ALL);

	// MyInfo is sent because it contains information for clients
	// about class war. Maybe need to replace this with another packet?
	// because sending all the data for just a few things is about stupid.
	for (int i = 0; i < MAX_USER; i++)
	{
		USER *pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) continue;
		pUser->SendMyInfo(TO_ME, INFO_MODIFY);
	}

	for (int i = 0; i < g_arNpcTypeGuild.GetSize(); i++)
	{
		NPC_TYPE_GUILD* Guild = g_arNpcTypeGuild[i];
		if (Guild->sGuild == 1000 && Guild->sGuildOpt == 2)
		{
			CNpc* pNpc = GetNpc(Guild->sNId);
			if (!pNpc)
				continue;

			pNpc->m_sHP = pNpc->m_sMaxHp;
			pNpc->m_sRegenType = NPCREGENTYPE_FORCE;
			pNpc->m_tNpcType = NPCTYPE_NPC;

			int index = 0;
			pNpc->FillNpcInfo(m_TempBuf, index, INFO_MODIFY, m_pCom);
			Send(m_TempBuf, index);
		}
	}
}

void USER::ClassWarEnd()
{
	// Is it supposed to be ending?
	if (!g_bClassWarEnd) return;

	// Class war is no longer in progress.
	g_bClassWarEnd = false;
	g_bClassWar = false;
	g_iClassWarCount++;

	// Compute winner of the class war.

	// Stone Locations
	CPoint ptStones[3];
	ptStones[0] = ConvertToServer(4, 54, 15);
	if (ptStones[0].x == -1 || ptStones[0].y == -1) return;
	ptStones[1] = ConvertToServer(18, 26, 15);
	if (ptStones[1].x == -1 || ptStones[1].y == -1) return;
	ptStones[2] = ConvertToServer(42, 30, 15);
	if (ptStones[2].x == -1 || ptStones[2].y == -1) return;

	// Get possible user standing on the stones.
	int iStoneUsers[3];
	iStoneUsers[0] = GetUid(static_cast<short>(ptStones[0].x), static_cast<short>(ptStones[0].y), 15);
	iStoneUsers[1] = GetUid(static_cast<short>(ptStones[1].x), static_cast<short>(ptStones[1].y), 15);
	iStoneUsers[2] = GetUid(static_cast<short>(ptStones[2].x), static_cast<short>(ptStones[2].y), 15);

	// Go through each stone checking class that is standing on it and
	// to see who won. Class needs to own 2 or more stones to win.
	int iOpponentStoneCount = 0;
	for (int i = 0; i < 3; i++)
	{
		// Not interested in invalid user or npcs on the stone.
		if (iStoneUsers[i] == 0 || iStoneUsers[i] >= NPC_BAND)
		{
			continue;
		}

		USER* pUser = GetUser(iStoneUsers[i] - USER_BAND);
		if (pUser == NULL) continue;

		if (pUser->m_sX != ptStones[i].x) continue;
		if (pUser->m_sY != ptStones[i].y) continue;

		if (CheckDemon(pUser->m_sClass) != CheckDemon(g_iClassWar))
			iOpponentStoneCount++;
	}

	if (iOpponentStoneCount >= 2)
	{
		g_iClassWar = g_iClassWar == 0 ? 10 : 0;
		g_iClassWarCount = 0;
		// Re-initialise the zones owned by class's because
		// owner of some maps has changed.
		g_mapZoneClass[14] = g_iClassWar;
		g_mapZoneClass[15] = g_iClassWar;
		g_mapZoneClass[16] = g_iClassWar;
		g_mapZoneClass[17] = g_iClassWar;

		// Update the soma ini with new classwar class
		g_pMainDlg->m_Ini.SetProfileInt("CLASSWAR", "CLASS", g_iClassWar);
	}

	// Update the soma ini with new classwar count
	g_pMainDlg->m_Ini.SetProfileInt("CLASSWAR", "COUNT", g_iClassWarCount);

	// Inform all that the class war is has ended and who won!
	UINT strID = -1;
	if (!CheckDemon(g_iClassWar))
	{
		strID = IDS_CLASS_WAR_END_HSOMA;
	}
	else
	{
		strID = IDS_CLASS_WAR_END_DSOMA;
	}
	SendServerChatMessage(strID, TO_ALL);

	// Remove the losing class npcs from the continental maps.
	DeleteOtherClass();

	// MyInfo is sent because it contains information for clients
	// about class war. Maybe need to replace this with another packet?
	// because sending all the data for just a few things is about stupid.
	for (int i = 0; i < MAX_USER; i++)
	{
		USER *pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) continue;
		pUser->SendMyInfo(TO_ME, INFO_MODIFY);

		// Warp users out of continental maps if they didn't win the class war.
		if (pUser->IsMapRange(MAP_AREA_WOTW_MAP))
		{
			bool bInTown = true;
			int iMoral = pUser->m_iMoral / CLIENT_MORAL;
			if (!CheckDemon(pUser->m_sClass))
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

			if (CheckDemon(g_iClassWar) != CheckDemon(pUser->m_sClass))
			{
				if (bInTown)
				{
					if (!CheckDemon(pUser->m_sClass))
					{
						CPoint pt = pUser->FindRandPointInRect_C(1, 277, 93, 300, 120);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 291;
							pt.y = 113;
						}
						pUser->LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					else
					{
						CPoint pt = pUser->FindRandPointInRect_C(9, 393, 903, 433, 903);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 393;
							pt.y = 903;
						}
						pUser->LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
				}
				else
				{
					if (!CheckDemon(pUser->m_sClass))
					{
						CPoint pt = pUser->FindRandPointInRect_C(1, 32, 126, 40, 126);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 32;
							pt.y = 126;
						}
						pUser->LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
					else
					{
						CPoint pt = pUser->FindRandPointInRect_C(9, 307, 959, 333, 959);
						if (pt.x == -1 || pt.y == -1)
						{
							pt.x = 307;
							pt.y = 959;
						}
						pUser->LinkToOtherZone(9, static_cast<short>(pt.x), static_cast<short>(pt.y));
					}
				}
			}
		}
	}
}

void USER::DeleteOtherClass()
{
	if (g_arNpcTypeClass.GetSize() <= 0) return;

	for (int i = 0; i < g_arNpcTypeClass.GetSize(); i++)
	{
		if (!g_arNpcTypeClass[i]) continue;
		if (g_arNpcTypeClass[i]->sClass == g_iClassWar) continue;
		CNpc* pNpc = GetNpc(g_arNpcTypeClass[i]->sNId);
		if (pNpc == NULL) continue;
		pNpc->SetDead(m_pCom, -1, false);
	}
}

void USER::UserTimer()
{
	short sMagicNo = m_sRemainMagicNo[MAGIC_EFFECT_POISON];
	if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
	{
		m_sRemainMagicNo[MAGIC_EFFECT_POISON] = -1;
		m_dwRemainMagicTime[MAGIC_EFFECT_POISON] = 0;
		return;
	}

	CMagicTable* pMagic = g_arMagicTable[sMagicNo-1];
	if (!pMagic)
	{
		m_sRemainMagicNo[MAGIC_EFFECT_POISON] = -1;
		m_dwRemainMagicTime[MAGIC_EFFECT_POISON] = 0;
		return;
	}

	if (pMagic->m_tEValue != MAGIC_EFFECT_POISON)
	{
		m_sRemainMagicNo[MAGIC_EFFECT_POISON] = -1;
		m_dwRemainMagicTime[MAGIC_EFFECT_POISON] = 0;
		return;
	}

	int iDamage = pMagic->m_sDamage;

	// Check for weaken and multiply the damage
	if (CheckRemainMagic(MAGIC_EFFECT_WEAKEN))
	{
		if (iDamage >= 0)
		{
			iDamage *= 2;
		}
		else
		{
			// TODO: implement weaken negative damage calc
		}
	}
}

void USER::ParseUserStatusData(TCHAR *pBuf)
{
	if (pBuf[0] == 'N' &&
		pBuf[1] == 'E' &&
		pBuf[2] == 'W')
	{
		return;
	}

	if (pBuf[0] == 0)
	{
		return;
	}

	DWORD dwCurrTick = GetTickCount();

	int index = 0;
	int iMagicEffectCount = GetShort(pBuf, index);
	for (int i = 0; i < iMagicEffectCount; i++)
	{
		BYTE bHasEffect = GetByte(pBuf, index);
		if (bHasEffect == 1)
		{
			m_sRemainMagicNo[i] = GetShort(pBuf, index);
			m_dwRemainMagicTime[i] = dwCurrTick - GetDWORD(pBuf, index);
			short unknown = GetShort(pBuf, index);
		}
	}

	int iSpecialEffectCount = GetShort(pBuf, index);
	for (int i = 0; i < iSpecialEffectCount; i++)
	{
		BYTE bHasEffect = GetByte(pBuf, index);
		if (bHasEffect == 1)
		{
			m_RemainSpecial[i].sMid = GetShort(pBuf, index);
			m_RemainSpecial[i].dwTime = dwCurrTick - GetDWORD(pBuf, index);
			m_RemainSpecial[i].sDamage = GetShort(pBuf, index);
		}
	}

	m_dwLastSpecialTime = dwCurrTick - GetDWORD(pBuf, index);
	m_sSpecialAttackNo = GetShort(pBuf, index);
	m_dwSpecialAttackDelay = GetDWORD(pBuf, index);

	//bool bHaveDefenseMagic = GetByte(pBuf, index);
	//if (bHaveDefenseMagic)
	//{
	//	m_sDefenseMagicNo = GetShort(pBuf, index);
	//	m_dwDefenseMagicTime = dwCurrTick - GetDWORD(pBuf, index);
	//}

	//bool bHavePoisonMagic = GetByte(pBuf, index);
	//if (bHavePoisonMagic)
	//{
	//	m_dwPoisonMagicCount = GetShort(pBuf, index);
	//	m_sPoisonMagicNo = GetShort(pBuf, index);
	//}

	//bool m_bPKEnabled = GetByte(pBuf, index);
}

void USER::FillUserStatusData(TCHAR *pBuf)
{
	DWORD dwCurrTick = GetTickCount();

	int index = 0;
	SetShort(pBuf, MAX_MAGIC_EFFECT, index);
	for (int i = 0; i < MAX_MAGIC_EFFECT; i++)
	{
		if (CheckRemainMagic(i))
		{
			SetByte(pBuf, 1, index);
			SetShort(pBuf, m_sRemainMagicNo[i], index);
			SetDWORD(pBuf, dwCurrTick - m_dwRemainMagicTime[i], index);
			SetShort(pBuf, 0, index); // unknown
		}
		else
		{
			SetByte(pBuf, 0, index);
		}
	}

	SetShort(pBuf, MAX_SPECIAL_EFFECT, index);
	for (int i = 0; i < MAX_SPECIAL_EFFECT; i++)
	{
		if (CheckRemainSpecialAttack(i))
		{
			SetByte(pBuf, 1, index);
			SetShort(pBuf, m_RemainSpecial[i].sMid, index);
			SetDWORD(pBuf, dwCurrTick - m_RemainSpecial[i].dwTime, index);
			SetShort(pBuf, m_RemainSpecial[i].sDamage, index);
		}
		else
		{
			SetByte(pBuf, 0, index);
		}
	}

	SetDWORD(pBuf, dwCurrTick - m_dwLastSpecialTime, index);
	SetShort(pBuf, m_sSpecialAttackNo, index);
	SetDWORD(pBuf, m_dwSpecialAttackDelay, index);

	//bool bHaveDefenseMagic = GetByte(pBuf, index);
	//if (bHaveDefenseMagic)
	//{
	//	m_sDefenseMagicNo = GetShort(pBuf, index);
	//	m_dwDefenseMagicTime = dwCurrTick - GetDWORD(pBuf, index);
	//}

	//bool bHavePoisonMagic = GetByte(pBuf, index);
	//if (bHavePoisonMagic)
	//{
	//	m_dwPoisonMagicCount = GetShort(pBuf, index);
	//	m_sPoisonMagicNo = GetShort(pBuf, index);
	//}

	//bool m_bPKEnabled = GetByte(pBuf, index);
}

void USER::PlusHpMp(DWORD dwTime)
{
	// TODO: Range ability stuff when range ability is implemented

	int iSpecialHPRegen = 0;
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_HPREGEN))
	{
		iSpecialHPRegen = m_RemainSpecial[SPECIAL_EFFECT_HPREGEN].sDamage;
	}

	int iSpecialMPRegen = 0;
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_MPREGEN))
	{
		iSpecialMPRegen = m_RemainSpecial[SPECIAL_EFFECT_MPREGEN].sDamage;
	}

	// Cannot regain Hp or MP if weight is full
	if (m_sWgt >= GetMaxWgt())
	{
		m_dwLastRecoverHPTime = dwTime;
		m_dwLastRecoverMPTime = dwTime;
		return;
	}

	// Work out HP and MP regen delays and
	// apply a reduction to HP and MP regen when weight is over 50%
	int iWeightReduction = static_cast<int>((static_cast<double>(m_sWgt) / GetMaxWgt()) * 100.0);
	if (iWeightReduction > 100)
	{
		iWeightReduction = 100;
	}

	if (iWeightReduction > 50)
	{
		if (m_BattleMode == BATTLEMODE_NORMAL)
		{
			iWeightReduction -= 50;
			iWeightReduction = static_cast<int>(((static_cast<double>(iWeightReduction)) / 50) * 13);
		}
		else
		{
			iWeightReduction -= 50;
			iWeightReduction = static_cast<int>(((static_cast<double>(iWeightReduction)) / 50) * 20);
		}
	}

	int iHPRegenDelay = 0;
	int iMPRegenDelay = 0;
	if (m_BattleMode == BATTLEMODE_NORMAL)
	{
		iHPRegenDelay = iMPRegenDelay = 7 + iWeightReduction;
	}
	else
	{
		iHPRegenDelay = iMPRegenDelay = 20 + iWeightReduction;
	}

	// Cannot allow the regen to be <= 0
	if (iHPRegenDelay <= 0 || iMPRegenDelay <= 0)
	{
		return;
	}

	// Apply the bonuses from having HP / MP regen special attacks
	if (iSpecialHPRegen > 0 && iSpecialHPRegen <= iHPRegenDelay)
	{
		iHPRegenDelay /= iSpecialHPRegen;
	}

	if (iSpecialMPRegen > 0 && iSpecialMPRegen <= iMPRegenDelay)
	{
		iMPRegenDelay /= iSpecialMPRegen;
	}

	// Work out amount of HP and MP to add and add it.
	DWORD dwTimeDiff = dwTime - m_dwLastRecoverHPTime;
	int iHPRegen = dwTimeDiff / (iHPRegenDelay * 1000);

	dwTimeDiff = dwTime - m_dwLastRecoverMPTime;
	int iMPRegen = dwTimeDiff / (iMPRegenDelay * 1000);

	if (iHPRegen > 0)
	{
		// TODO: CON BONUS
		int iCONBonus = 1;
		int iCON = (m_iCON / CLIENT_BASE_STATS) + PlusFromItem(SPECIAL_CON) - 10;
		if (!CheckDemon(m_sClass))
		{
			iCONBonus += iCON / 3;
		}
		else
		{
			 // TODO: DSoma CON BONUS for regen is more than likely incorrect
			iCONBonus += iCON / 12;
		}
		CheckMaxValue(m_sHP, iHPRegen * iCONBonus);
		if (m_sHP > GetMaxHP())
		{
			m_sHP = GetMaxHP();
		}
		m_dwLastRecoverHPTime = dwTime;
	}

	if (iMPRegen > 0)
	{
		// TODO: WIS BONUS
		int iWISBonus = 1;
		int iWIS = (m_iWIS / CLIENT_EXT_STATS) + PlusFromItem(SPECIAL_WIS) - 10;
		if (!CheckDemon(m_sClass))
		{
			iWISBonus += iWIS / 2;
		}
		else
		{
			// TODO: DSoma WIS BONUS for regen is more than likely incorrect
			if (CheckMyJob(DEVIL_CLASS_WIZARD))
			{
				iWISBonus += iWIS / 2;
			}
			else
			{
				iWISBonus += iWIS / 12;
			}
		}
		CheckMaxValue(m_sMP, iMPRegen * iWISBonus);
		if (m_sMP > GetMaxMP())
		{
			m_sMP = GetMaxMP();
		}
		m_dwLastRecoverMPTime = dwTime;
	}

	// Update user client with their new HP and MP
	SendHPMP();
}

void USER::CheckNewEvent()
{
	for (int i = 0; i < g_arEventViewData.GetSize(); i++)
	{
		EventViewData* e = g_arEventViewData[i];
		if (e == NULL) continue;

		if (e->sClass != m_sClass) continue;

		if (e->sLevel == m_sLevel)
		{
			int index = 0;
			SetByte(m_TempBuf, PKT_QUEST_VIEW, index);
			SetByte(m_TempBuf, 3, index);
			Send(m_TempBuf, index);
		}
	}
}

bool USER::ArrowPathFind(CPoint ptSource, CPoint ptTarget)
{
	if (m_sZ != 3 && m_sZ != 6 && m_sZ != 7 && m_sZ != 21) return true;

	int iSourceX = ptSource.x;
	int iSourceY = ptSource.y;
	if (iSourceX == -1 || iSourceY == -1) return false;

	int iTargetX = ptTarget.x;
	int iTargetY = ptTarget.y;
	if (iTargetX == -1 || iTargetY == -1) return false;

	// Directions
	while (true)
	{
		int iX = -1;
		int iY = -1;

		if (iTargetX == iSourceX && iTargetY < iSourceY) // Up
		{
			int a = (iTargetX - iSourceX) * (iTargetX - iSourceX) +
				((iTargetY - iSourceY) + 2) * ((iTargetY - iSourceY) + 2);

			int b = ((iTargetX - iSourceX) + 1) * ((iTargetX - iSourceX) + 1) +
				((iTargetY - iSourceY) + 1) * ((iTargetY - iSourceY) + 1);

			int c = ((iTargetX - iSourceX) - 1) * ((iTargetX - iSourceX) - 1) +
				((iTargetY - iSourceY) + 1) * ((iTargetY - iSourceY) + 1);

			int d = b;
			if (a < d) d = a;
			if (d >= c) d = c;

			int iAddX = 0;
			int iAddY = 0;
			if (d == a)
			{
				iAddX = 0;
				iAddY = -2;
			}
			else
			{
				if (d == b)
				{
					iAddX = -1;
					iAddY = -1;
				}
				else
				{
					iAddX = 1;
					iAddY = -1;
				}
			}

			iX = iSourceX + iAddX;
			iY = iSourceY + iAddY;
			if (iX == iTargetX && iY == iTargetY)
			{
				return true;
			}
		}
		else if (iTargetX > iSourceX && iTargetY < iSourceY) // Up Right
		{
			int a = (iTargetX - iSourceX) * (iTargetX - iSourceX) +
				((iTargetY - iSourceY) + 2) * ((iTargetY - iSourceY) + 2);

			int b = ((iTargetX - iSourceX) - 1) * ((iTargetX - iSourceX) - 1) +
				((iTargetY - iSourceY) + 1) * ((iTargetY - iSourceY) + 1);

			int c = ((iTargetX - iSourceX) - 2) * ((iTargetX - iSourceX) - 2) +
				(iTargetY - iSourceY) * (iTargetY - iSourceY);

			int d = b;
			if (a < d) d = a;
			if (d >= c) d = c;

			int iAddX = 0;
			int iAddY = 0;
			if (d == a)
			{
				iAddX = 0;
				iAddY = -2;
			}
			else
			{
				if (d == b)
				{
					iAddX = 1;
					iAddY = -1;
				}
				else
				{
					iAddX = 2;
					iAddY = 0;
				}
			}

			iX = iSourceX + iAddX;
			iY = iSourceY + iAddY;
			if (iX == iTargetX && iY == iTargetY)
			{
				return true;
			}
		}
		else if (iTargetX > iSourceX && iTargetY == iSourceY) // Right
		{
			int a = ((iTargetX - iSourceX) - 2) * ((iTargetX - iSourceX) - 2) +
				(iTargetY - iSourceY) * (iTargetY - iSourceY);

			int b = ((iTargetX - iSourceX) - 1) * ((iTargetX - iSourceX) - 1) +
				((iTargetY - iSourceY) + 1) * ((iTargetY - iSourceY) + 1);

			int c = ((iTargetX - iSourceX) - 1) * ((iTargetX - iSourceX) - 1) +
				((iTargetY - iSourceY) - 1) * ((iTargetY - iSourceY) - 1);

			int d = b;
			if (a < d) d = a;
			if (d >= c) d = c;

			int iAddX = 0;
			int iAddY = 0;
			if (d == a)
			{
				iAddX = 2;
				iAddY = 0;
			}
			else
			{
				if (d == b)
				{
					iAddX = 1;
					iAddY = -1;
				}
				else
				{
					iAddX = 1;
					iAddY = 1;
				}
			}

			iX = iSourceX + iAddX;
			iY = iSourceY + iAddY;
			if (iX == iTargetX && iY == iTargetY)
			{
				return true;
			}
		}
		else if (iTargetX > iSourceX && iTargetY > iSourceY) // Down Right
		{
			int a = ((iTargetX - iSourceX) - 2) * ((iTargetX - iSourceX) - 2) +
				(iTargetY - iSourceY) * (iTargetY - iSourceY);

			int b = ((iTargetX - iSourceX) - 1) * ((iTargetX - iSourceX) - 1) +
				((iTargetY - iSourceY) - 1) * ((iTargetY - iSourceY) - 1);

			int c = (iTargetX - iSourceX) * (iTargetX - iSourceX) +
				((iTargetY - iSourceY) - 2) * ((iTargetY - iSourceY) - 2);

			int d = b;
			if (a < d) d = a;
			if (d >= c) d = c;

			int iAddX = 0;
			int iAddY = 0;
			if (d == a)
			{
				iAddX = 2;
				iAddY = 0;
			}
			else
			{
				if (d == b)
				{
					iAddX = 1;
					iAddY = 1;
				}
				else
				{
					iAddX = 0;
					iAddY = 2;
				}
			}

			iX = iSourceX + iAddX;
			iY = iSourceY + iAddY;
			if (iX == iTargetX && iY == iTargetY)
			{
				return true;
			}
		}
		else if (iTargetX == iSourceX && iTargetY > iSourceY) // Down
		{
			int a = (iTargetX - iSourceX) * (iTargetX - iSourceX) +
				((iTargetY - iSourceY) - 2) * ((iTargetY - iSourceY) - 2);

			int b = ((iTargetX - iSourceX) - 1) * ((iTargetX - iSourceX) - 1) +
				((iTargetY - iSourceY) - 1) * ((iTargetY - iSourceY) - 1);

			int c = ((iTargetX - iSourceX) + 1) * ((iTargetX - iSourceX) + 1) +
				((iTargetY - iSourceY) - 1) * ((iTargetY - iSourceY) - 1);

			int d = b;
			if (a < d) d = a;
			if (d >= c) d = c;

			int iAddX = 0;
			int iAddY = 0;
			if (d == a)
			{
				iAddX = 0;
				iAddY = 2;
			}
			else
			{
				if (d == b)
				{
					iAddX = 1;
					iAddY = 1;
				}
				else
				{
					iAddX = -1;
					iAddY = 1;
				}
			}

			iX = iSourceX + iAddX;
			iY = iSourceY + iAddY;
			if (iX == iTargetX && iY == iTargetY)
			{
				return true;
			}
		}
		else if (iTargetX < iSourceX && iTargetY > iSourceY) // Down Left
		{
			int a = (iTargetX - iSourceX) * (iTargetX - iSourceX) +
				((iTargetY - iSourceY) - 2) * ((iTargetY - iSourceY) - 2);

			int b = ((iTargetX - iSourceX) + 1) * ((iTargetX - iSourceX) + 1) +
				((iTargetY - iSourceY) - 1) * ((iTargetY - iSourceY) - 1);

			int c = ((iTargetX - iSourceX) + 2) * ((iTargetX - iSourceX) + 2) +
				(iTargetY - iSourceY) * (iTargetY - iSourceY);

			int d = b;
			if (a < d) d = a;
			if (d >= c) d = c;

			int iAddX = 0;
			int iAddY = 0;
			if (d == a)
			{
				iAddX = 0;
				iAddY = 2;
			}
			else
			{
				if (d == b)
				{
					iAddX = -1;
					iAddY = 1;
				}
				else
				{
					iAddX = -2;
					iAddY = 0;
				}
			}

			iX = iSourceX + iAddX;
			iY = iSourceY + iAddY;
			if (iX == iTargetX && iY == iTargetY)
			{
				return true;
			}
		}
		else if (iTargetX < iSourceX && iTargetY == iSourceY) // Left
		{
			int a = ((iTargetX - iSourceX) + 2) * ((iTargetX - iSourceX) + 2) +
				(iTargetY - iSourceY) * (iTargetY - iSourceY);

			int b = ((iTargetX - iSourceX) + 1) * ((iTargetX - iSourceX) + 1) +
				((iTargetY - iSourceY) - 1) * ((iTargetY - iSourceY) - 1);

			int c = ((iTargetX - iSourceX) + 1) * ((iTargetX - iSourceX) + 1) +
				((iTargetY - iSourceY) + 1) * ((iTargetY - iSourceY) + 1);

			int d = b;
			if (a < d) d = a;
			if (d >= c) d = c;

			int iAddX = 0;
			int iAddY = 0;
			if (d == a)
			{
				iAddX = -2;
				iAddY = 0;
			}
			else
			{
				if (d == b)
				{
					iAddX = -1;
					iAddY = 1;
				}
				else
				{
					iAddX = -1;
					iAddY = -1;
				}
			}

			iX = iSourceX + iAddX;
			iY = iSourceY + iAddY;
			if (iX == iTargetX && iY == iTargetY)
			{
				return true;
			}
		}
		else if (iTargetX < iSourceX && iTargetY < iSourceY) // Up Left
		{
			int a = (iTargetX - iSourceX) * (iTargetX - iSourceX) +
				((iTargetY - iSourceY) + 2) * ((iTargetY - iSourceY) + 2);

			int b = ((iTargetX - iSourceX) + 1) * ((iTargetX - iSourceX) + 1) +
				((iTargetY - iSourceY) + 1) * ((iTargetY - iSourceY) + 1);

			int c = ((iTargetX - iSourceX) + 2) * ((iTargetX - iSourceX) + 2) +
				(iTargetY - iSourceY) * (iTargetY - iSourceY);

			int d = b;
			if (a < d) d = a;
			if (d >= c) d = c;

			int iAddX = 0;
			int iAddY = 0;
			if (d == a)
			{
				iAddX = 0;
				iAddY = -2;
			}
			else
			{
				if (d == b)
				{
					iAddX = -1;
					iAddY = -1;
				}
				else
				{
					iAddX = -2;
					iAddY = 0;
				}
			}

			iX = iSourceX + iAddX;
			iY = iSourceY + iAddY;
			if (iX == iTargetX && iY == iTargetY)
			{
				return true;
			}
		}
		else
		{
			break;
		}

		if (!IsMapMovable_C(iX, iY))
		{
			return false;
		}

		iSourceX = iX;
		iSourceY = iY;
	}

	return true;
}

bool USER::IsMapMovable_C(int x, int y)
{
	CPoint pt = ConvertToServer(x, y);
	if (pt.x == -1 || pt.y == -1)
	{
		return false;
	}

	x = pt.x;
	y = pt.y;

	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize()) return false;
	if (!g_Zones[m_iZoneIndex]) return false;
	if (x >= g_Zones[m_iZoneIndex]->m_sizeMap.cx || x < 0 ||
		y >= g_Zones[m_iZoneIndex]->m_sizeMap.cy || y < 0)
	{
		return false;
	}

	if (g_Zones[m_iZoneIndex]->m_pMap[x][y].m_bMove == MAP_NON_MOVEABLE) return false;

	return true;
}

bool USER::ChangeRandItem(short sGroup)
{
	if (sGroup < 0 || sGroup > 20) return false;
	if (g_arChangeRandItemData[sGroup].GetSize() <= 0) return false;

	int i = myrand(0, g_arChangeRandItemData[sGroup].GetSize()-1);
	if (!g_arChangeRandItemData[sGroup][i]) return false;

	if (g_arChangeRandItemData[sGroup][i]->sGroup != sGroup) return false;

	ItemList GiveItem;
	if (!GiveItem.InitFromItemTable(g_arChangeRandItemData[sGroup][i]->sItemNum)) return false;

	SendChangeWgt();
	if ((m_sWgt + GiveItem.sWgt) > GetMaxWgt())
	{
		SendServerChatMessage(IDS_NOT_ENOUGH_WEIGHT, TO_ME);
		return false;
	}

	short sInvSlot = PushItemInventory(&GiveItem);
	if (sInvSlot == -1)
	{
		SendServerChatMessage(IDS_INV_FULL, TO_ME);
		return false;
	}

	SendItemInfoChange(BASIC_INV, sInvSlot, INFO_ALL);
	return true;
}

void USER::GuildTownWarStart()
{
	// Is a guild town war started?
	// and is a guild town war supposed to be starting?
	short sTownNum = -1;
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_bGuildTownWar[i] && g_bGuildTownWarStart[i])
		{
			sTownNum = i + 1;
			break;
		}
	}
	if (sTownNum == -1)
		return;

	g_bGuildTownWarStart[sTownNum-1] = false;

	// Find the guild town data
	GuildTownData* GuildTown = NULL;
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_arGuildTownData[i] == NULL)
			continue;

		if (g_arGuildTownData[i]->sTownNum == sTownNum)
		{
			GuildTown = g_arGuildTownData[i];
			break;
		}
	}
	if (GuildTown == NULL)
		return;

	// Inform all that a guild town war has begun!
	CString str;
	switch (GuildTown->sTownNum)
	{
	case 1:
		str.Format(IDS_GUILD_TOWN_WAR_TYT_BEGUN, GuildTown->strGuildName);
		break;
	case 2:
		str.Format(IDS_GUILD_TOWN_WAR_IC_BEGUN, GuildTown->strGuildName);
		break;
	default:
		break;
	}
	SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ALL);

	for (int i = 0; i < g_arNpcTypeGuild.GetSize(); i++)
	{
		NPC_TYPE_GUILD* GuildNPC = g_arNpcTypeGuild[i];
		if (GuildNPC->sGuild == GuildTown->sTownNum)
		{
			if (GuildNPC->sGuildOpt == 2)
			{
				CNpc* pNpc = GetNpc(GuildNPC->sNId);
				if (!pNpc) continue;
				pNpc->m_sHP = pNpc->m_sMaxHp;
				pNpc->m_tNpcType = NPCTYPE_NORMAL;
				pNpc->m_sRegenType =  NPCREGENTYPE_FORCE;
			}
			else if (GuildNPC->sGuildOpt == 3)
			{
				CNpc* pNpc = GetNpc(GuildNPC->sNId);
				if (!pNpc) continue;
				pNpc->m_sHP = pNpc->m_sMaxHp;
				pNpc->m_tNpcAttType = NPC_AT_AGGRO;
				pNpc->m_sTownGuildNum = GuildTown->sGuildNum;
				if (pNpc->m_NpcState != NPC_LIVE && pNpc->m_NpcState != NPC_DEAD)
				{
					int index = 0;
					pNpc->FillNpcInfo(m_TempBuf, index, INFO_MODIFY, m_pCom);
					Send(m_TempBuf, index);
				}
				else
				{
					pNpc->m_NpcState = NPC_LIVE;
					pNpc->m_Delay = 750;
				}
			}
			else if (GuildNPC->sGuildOpt == 4)
			{
				CNpc* pNpc = GetNpc(GuildNPC->sNId);
				if (!pNpc) continue;
				pNpc->m_sHP = pNpc->m_sMaxHp;
				if (pNpc->m_NpcState == NPC_LIVE || pNpc->m_NpcState == NPC_DEAD)
				{
					pNpc->m_NpcState = NPC_LIVE;
					pNpc->m_Delay = 750;
				}
			}
		}
	}

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

	// MyInfo is sent because it contains information for clients
	// about town war. Maybe need to replace this with another packet?
	// because sending all the data for just a few things is about stupid.
	for (int i = 0; i < MAX_USER; i++)
	{
		USER *pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) continue;

		if (GuildTown->sTownNum == 1)
		{
			if (pUser->m_sZ != 1 && pUser->m_sZ != 8)
				continue;

			if (pUser->m_sZ == 1 && !GuildTownRect.PtInRect(CPoint(pUser->m_sX, pUser->m_sY)))
				continue;
		}
		else if (GuildTown->sTownNum == 2)
		{
			if (pUser->m_sZ != 11 && pUser->m_sZ != 13)
				continue;

			if (pUser->m_sZ == 11 && !GuildTownRect.PtInRect(CPoint(pUser->m_sX, pUser->m_sY)))
				continue;
		}

		if (pUser->CheckGuildTown(GuildTown->sTownNum) || pUser->CheckGuildTownAlly(GuildTown->sTownNum))
		{
			pUser->m_sGuildTownWarType = 1;
			pUser->SendMyInfo(TO_ME, INFO_MODIFY);
		}
		else
		{
			pUser->m_sGuildTownWarType = 2;
			pUser->SendMyInfo(TO_ME, INFO_MODIFY);

			if (GuildTown->sTownNum == 1)
			{
				CPoint pt = pUser->FindRandPointInRect_C(1, 250, 812, 278, 812);
				if (pt.x == -1 || pt.y == -1)
				{
					pt.x = 242;
					pt.y = 812;
				}
				if (pUser->m_sZ != 1)
					pUser->LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
				else
					pUser->LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
			}
			else if (GuildTown->sTownNum == 2)
			{
				if (CheckDemon(pUser->m_sClass))
				{
					CPoint pt = pUser->FindRandPointInRect_C(11, 276, 324, 302, 324);
					if (pt.x == -1 || pt.y == -1)
					{
						pt.x = 276;
						pt.y = 324;
					}
					if (pUser->m_sZ != 11)
						pUser->LinkToOtherZone(11, static_cast<short>(pt.x), static_cast<short>(pt.y));
					else
						pUser->LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
				}
				else
				{
					CPoint pt = pUser->FindRandPointInRect_C(1, 277, 93, 300, 120);
					if (pt.x == -1 || pt.y == -1)
					{
						pt.x = 291;
						pt.y = 113;
					}
					pUser->LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
				}
			}
		}
	}
}

void USER::GuildTownWarEnd()
{
	// Is a guild town war supposed to be ending?
	short sTownNum = -1;
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_bGuildTownWarEnd[i])
		{
			sTownNum = i + 1;
			break;
		}
	}
	if (sTownNum == -1)
		return;

	// Guild town war is no longer in progress.
	g_bGuildTownWarEnd[sTownNum-1] = false;
	g_bGuildTownWar[sTownNum-1] = false;

	// Compute winner of the guild town war.

	// Find the guild town data
	GuildTownData* GuildTown = NULL;
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (g_arGuildTownData[i] == NULL)
			continue;

		if (g_arGuildTownData[i]->sTownNum == sTownNum)
		{
			GuildTown = g_arGuildTownData[i];
			break;
		}
	}
	if (GuildTown == NULL)
		return;

	TCHAR strGuildTownGuildName[GUILD_NAME_LENGTH];
	strcpy(strGuildTownGuildName, GuildTown->strGuildName);

	// Stone Location
	CPoint ptStone;
	if (GuildTown->sTownNum == 1)
	{
		ptStone = ConvertToServer(24, 34, 8);
		if (ptStone.x == -1 || ptStone.y == -1) return;
	}
	else if (GuildTown->sTownNum == 2)
	{
		ptStone = ConvertToServer(30, 34, 13);
		if (ptStone.x == -1 || ptStone.y == -1) return;
	}

	// Get possible user standing on the stone.
	int iStoneUser;
	if (GuildTown->sTownNum == 1)
		iStoneUser = GetUid(static_cast<short>(ptStone.x), static_cast<short>(ptStone.y), 8);
	else if (GuildTown->sTownNum == 2)
		iStoneUser = GetUid(static_cast<short>(ptStone.x), static_cast<short>(ptStone.y), 13);

	bool bAnotherGuildWon = false;
	if (iStoneUser > 0 && iStoneUser < NPC_BAND)
	{
		USER* pUser = GetUser(iStoneUser - USER_BAND);
		if (pUser)
		{
			if (pUser->m_sX == ptStone.x && pUser->m_sY == ptStone.y)
			{
				if (pUser->m_sGroup != -1 && pUser->m_sGroup != GuildTown->sGuildNum)
				{
					bAnotherGuildWon = true;
					GuildTown->sGuildNum = pUser->m_sGroup;
					strcpy(GuildTown->strGuildName, pUser->m_strGuildName);
					GuildTown->sTaxRate = 10;

					UpdateGuildTownChange(GuildTown->sTownNum, GuildTown->sGuildNum, GuildTown->strGuildName);
					Guild* pGuild = GetGuild(GuildTown->sGuildNum);
					if (pGuild)
					{
						pGuild->m_dwBarr += GUILD_TOWN_WAR_BARR;
						UpdateGuildMoney(pGuild->m_sNum, pGuild->m_dwBarr);
					}
					ReleaseGuild();
				}
			}
		}
	}

	if (!bAnotherGuildWon)
	{
		++GuildTown->sLevel;
		UpdateGuildTownLevel(GuildTown->sTownNum, GuildTown->sLevel);
		Guild* pGuild = GetGuild(GuildTown->sGuildNum);
		if (pGuild)
		{
			pGuild->m_dwBarr += GUILD_TOWN_WAR_OWNER_BARR;
			UpdateGuildMoney(pGuild->m_sNum, pGuild->m_dwBarr);
		}
		ReleaseGuild();

		for (int i = 0; i < 3; i++)
		{
			Guild* pGuild = GetGuild(GuildTown->sAlly[i]);
			if (pGuild)
			{
				pGuild->m_dwBarr += GUILD_TOWN_WAR_ALLY_BARR;
				UpdateGuildMoney(pGuild->m_sNum, pGuild->m_dwBarr);
			}
			ReleaseGuild();
		}
	}

	CString str;
	str.Format(IDS_GUILD_TOWN_WAR_END, strGuildTownGuildName, GuildTown->strGuildName);
	SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ALL);

	for (int i = 0; i < g_arNpcTypeGuild.GetSize(); i++)
	{
		NPC_TYPE_GUILD* GuildNPC = g_arNpcTypeGuild[i];
		if (GuildNPC->sGuild == GuildTown->sTownNum)
		{
			if (GuildNPC->sGuildOpt == 1)
			{
				CNpc* pNpc = GetNpc(GuildNPC->sNId);
				if (!pNpc) continue;

				pNpc->m_sTownGuildNum = GuildTown->sGuildNum;
				Guild* pGuild = GetGuild(GuildTown->sGuildNum);
				if (pGuild)
					pNpc->m_sTownGuildPicId = pGuild->m_sSymbolVersion;
				else
					pNpc->m_sTownGuildPicId = 0;
				ReleaseGuild();
				strcpy(pNpc->m_strTownGuildName, GuildTown->strGuildName);
			}
			else if (GuildNPC->sGuildOpt == 2)
			{
				CNpc* pNpc = GetNpc(GuildNPC->sNId);
				if (!pNpc) continue;
				pNpc->m_sHP = pNpc->m_sMaxHp;
				pNpc->m_tNpcType = NPCTYPE_NPC;
				pNpc->m_sRegenType =  NPCREGENTYPE_NORMAL;
				if (pNpc->m_NpcState != NPC_LIVE && pNpc->m_NpcState != NPC_DEAD)
				{
					int index = 0;
					pNpc->FillNpcInfo(m_TempBuf, index, INFO_MODIFY, m_pCom);
					Send(m_TempBuf, index);
				}
				else
				{
					pNpc->m_NpcState = NPC_LIVE;
					pNpc->m_Delay = 5000;
				}
			}
			else if (GuildNPC->sGuildOpt == 3)
			{
				CNpc* pNpc = GetNpc(GuildNPC->sNId);
				if (!pNpc) continue;

				pNpc->m_tNpcAttType = NPC_AT_PASSIVE;
				pNpc->m_sTownGuildNum = -1;
				if (pNpc->m_NpcState != NPC_LIVE && pNpc->m_NpcState != NPC_DEAD)
				{
					int index = 0;
					pNpc->FillNpcInfo(m_TempBuf, index, INFO_MODIFY, m_pCom);
					Send(m_TempBuf, index);
				}
			}
			else if (GuildNPC->sGuildOpt == 4)
			{
				CNpc* pNpc = GetNpc(GuildNPC->sNId);
				if (!pNpc) continue;

				if (pNpc->m_NpcState != NPC_LIVE || pNpc->m_NpcState != NPC_DEAD)
				{
					// TODO: pNpc->KilledBy = -1
					pNpc->SetDead(m_pCom, -1, true);
				}
			}
		}
	}

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

	// MyInfo is sent because it contains information for clients
	// about town war. Maybe need to replace this with another packet?
	// because sending all the data for just a few things is about stupid.
	for (int i = 0; i < MAX_USER; i++)
	{
		USER *pUser = g_pUserList->GetUserUid(i);
		if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) continue;

		if (GuildTown->sTownNum == 1)
		{
			if (pUser->m_sZ != 1 && pUser->m_sZ != 8)
				continue;

			if (pUser->m_sZ == 1 && !GuildTownRect.PtInRect(CPoint(pUser->m_sX, pUser->m_sY)))
				continue;
		}
		else if (GuildTown->sTownNum == 2)
		{
			if (pUser->m_sZ != 11 && pUser->m_sZ != 13)
				continue;

			if (pUser->m_sZ == 11 && !GuildTownRect.PtInRect(CPoint(pUser->m_sX, pUser->m_sY)))
				continue;
		}

		pUser->m_sGuildTownWarType = -1;
		pUser->SendMyInfo(TO_ME, INFO_MODIFY);

		if (!pUser->CheckGuildTown(GuildTown->sTownNum) && !pUser->CheckGuildTownAlly(GuildTown->sTownNum))
		{
			if (GuildTown->sTownNum == 1)
			{
				CPoint pt = pUser->FindRandPointInRect_C(1, 250, 812, 278, 812);
				if (pt.x == -1 || pt.y == -1)
				{
					pt.x = 242;
					pt.y = 812;
				}
				if (pUser->m_sZ != 1)
					pUser->LinkToOtherZone(1, static_cast<short>(pt.x), static_cast<short>(pt.y));
				else
					pUser->LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
			}
			else if (GuildTown->sTownNum == 2)
			{
				CPoint pt = pUser->FindRandPointInRect_C(11, 276, 324, 302, 324);
				if (pt.x == -1 || pt.y == -1)
				{
					pt.x = 276;
					pt.y = 324;
				}
				if (pUser->m_sZ != 11)
					pUser->LinkToOtherZone(11, static_cast<short>(pt.x), static_cast<short>(pt.y));
				else
					pUser->LinkToSameZone(static_cast<short>(pt.x), static_cast<short>(pt.y));
			}
		}
	}
}

bool USER::SetMoralByPK(int iTargetMoral)
{
	int iMoralPK[9][9] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		-1, -1, -1, -1, -3, -1, -2, -3, -4,
		-1, -1, -1, -2, -6, -5, -5, -8, -11,
		-1, -1, -2, -3, -9, -7, -6, -10, -12,
		-2, -2, -2, -3, -10, -9, -8, -12, -16,
		-2, -3, -3, -4, -11, -10, -10, -13, -17,
		-3, -3, -4, -5, -12, -11, -12, -14, -18,
		-4, -4, -5, -6, -13, -12, -14, -15, -19,
		-5, -5, -6, -8, -14, -15,- 16, -17, -20,
	};

	int iOldMoral = m_iMoral;

	int iMoralGroup = 0;
	int iMoral = m_iMoral / CLIENT_MORAL;
	if (iMoral <= -50)
	{
		iMoralGroup = 0;
	}
	else if (iMoral <= -31)
	{
		iMoralGroup = 1;
	}
	else if (iMoral <= -11)
	{
		iMoralGroup = 2;
	}
	else if (iMoral <= -4)
	{
		iMoralGroup = 3;
	}
	else if (iMoral <= 3)
	{
		iMoralGroup = 4;
	}
	else if (iMoral <= 10)
	{
		iMoralGroup = 5;
	}
	else if (iMoral <= 30)
	{
		iMoralGroup = 6;
	}
	else if (iMoral <= 49)
	{
		iMoralGroup = 7;
	}
	else if (iMoral <= 74)
	{
		iMoralGroup = 8;
	}

	int iTargetMoralGroup = 0;
	if (iTargetMoral <= -50)
	{
		iTargetMoralGroup = 0;
	}
	else if (iTargetMoral <= -31)
	{
		iTargetMoralGroup = 1;
	}
	else if (iTargetMoral <= -11)
	{
		iTargetMoralGroup = 2;
	}
	else if (iTargetMoral <= -4)
	{
		iTargetMoralGroup = 3;
	}
	else if (iTargetMoral <= 3)
	{
		iTargetMoralGroup = 4;
	}
	else if (iTargetMoral <= 10)
	{
		iTargetMoralGroup = 5;
	}
	else if (iTargetMoral <= 30)
	{
		iTargetMoralGroup = 6;
	}
	else if (iTargetMoral <= 49)
	{
		iTargetMoralGroup = 7;
	}
	else if (iTargetMoral <= 74)
	{
		iTargetMoralGroup = 8;
	}

	if (!CheckDemon(m_sClass))
	{
		m_iMoral += iMoralPK[iMoralGroup][iTargetMoralGroup] * CLIENT_MORAL;
	}
	else
	{
		m_iMoral -= iMoralPK[iMoralGroup][iTargetMoralGroup] * CLIENT_MORAL;
	}

	if (m_iMoral < -7400000) m_iMoral = -7400000;
	if (m_iMoral > 7400000) m_iMoral = 7400000;

	CString strOldMoralName = GetMoralName(iOldMoral / CLIENT_MORAL);
	CString strMoralName = GetMoralName(m_iMoral / CLIENT_MORAL);
	if (strOldMoralName.Compare(strMoralName) != 0)
	{
		return true;
	}
	return false;
}
