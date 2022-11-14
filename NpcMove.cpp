#include "stdafx.h"
#include "USER.h"
#include "Npc.h"

#include "Extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

bool CNpc::RandomMove(COM *pCom)
{
	if (pCom == NULL) return false;

	BOOL bIsIn = IsInRange();
	if (bIsIn)
	{
		// NPC will only continue to move if there is a player insight
		if (!FindUserInSight(pCom))
		{
			return false;
		}
	}

	if (m_search_range == 0 || m_movable_range == 0)
	{
		// Do something else
		return false;
	}

	// Guards are teleported to the player
	if (IsDetecter() && !bIsIn)
	{
		CPoint pt = FindNearAvailablePoint_S(m_sMinX, m_sMinY, 5);
		if (pt.x != -1 && pt.y != -1)
		{
			MoveNpc(static_cast<short>(pt.x), static_cast<short>(pt.y), pCom);
		}
		return false;
	}

	MAP* pMap = g_Zones[m_ZoneIndex];
	if (!pMap)return false;
	if (!pMap->m_pMap)return false;

	CPoint pt;
	int nLoop = 0;
	int nDestX = -1, nDestY = -1;
	int min_x, min_y, max_x, max_y;
	int temp_minx = 0, temp_miny = 0, temp_maxx = 0, temp_maxy = 0;

	CRect rectIn;

	if (bIsIn && (m_sMinX == m_sMaxX || m_sMinY == m_sMaxY))
	{
		// Remove NPC from current position
		if (g_Zones[m_ZoneIndex]->m_pMap[m_sCurX][m_sCurY].m_lUser == m_sNid + NPC_BAND)
		{
			InterlockedExchange(&g_Zones[m_ZoneIndex]->m_pMap[m_sCurX][m_sCurY].m_lUser, 0);
		}

		m_sCurX = m_sMinX;
		m_sCurY = m_sMinY;

		// Add NPC in new position
		InterlockedExchange(&g_Zones[m_ZoneIndex]->m_pMap[m_sCurX][m_sCurY].m_lUser, m_sNid + NPC_BAND);

		return false;
	}

	if (bIsIn) // NPC inside min/max database co-ordinates
	{
		m_bRandMove = true;

		m_arRandMove.RemoveAll();

		int axis_x[3];	axis_x[0] = -1;	axis_x[1] = 0;	axis_x[2] = 1;
		int axis_y[3];	axis_y[0] = -1;	axis_y[1] = 0;	axis_y[2] = 1;
		int rand_x, rand_y, rand_d;

		rand_x = myrand(0, 2);
		rand_y = myrand(0, 2);
		rand_d = myrand(1, 1);

		for (int i = 1; i <= rand_d; i++)
		{
			m_arRandMove.Add(CPoint(axis_x[rand_x] * i, axis_y[rand_y] * i));
		}

		m_min_x = m_sCurX;
		m_min_y = m_sCurY;

		return true;
	}
	else // NPC Outside of min / max co-ordinates (Need to put this in at some point...)
	{
		int x = 0, y = 0;

		min_x = m_sCurX;
		min_y = m_sCurY;
		max_x = m_sCurX;
		max_y = m_sCurY;

		if (m_sMinX < m_sCurX)	{min_x -= m_sMoveDistance;	x += 1;} if (min_x < 0) min_x = 0;
		if (m_sMinY < m_sCurY)	{min_y -= m_sMoveDistance;	y += 1;} if (min_y < 0) min_y = 0;
		if (m_sMaxX > m_sCurX)	{max_x += m_sMoveDistance;	x += 1;} if (max_x >= g_Zones[m_ZoneIndex]->m_sizeMap.cx) max_x = g_Zones[m_ZoneIndex]->m_sizeMap.cx - 1;
		if (m_sMaxY > m_sCurY)	{max_y += m_sMoveDistance;	y += 1;} if (max_y >= g_Zones[m_ZoneIndex]->m_sizeMap.cy) max_y = g_Zones[m_ZoneIndex]->m_sizeMap.cy - 1;

		nLoop = 0;
		while (1)
		{
			nDestX = min_x + (rand() % (m_sMoveDistance * x + 1));
			if (nDestX > max_x) nDestX = max_x;

			nDestY = min_y + (rand() % (m_sMoveDistance * y + 1));
			if (nDestY > max_y) nDestY = max_y;

			if (pMap->m_pMap[nDestX][nDestY].m_bMove == MAP_NON_MOVEABLE || pMap->m_pMap[nDestX][nDestY].m_lUser != 0)
			{
				if (nLoop++ >= 10) return false;
				continue;
			}

			break;
		}
	}

	if (nDestX < 0 || nDestY < 0)
	{
		return false;
	}

	// Run Path Find ---------------------------------------------//
	CPoint start, end;
	start.x = m_sCurX - min_x;
	start.y = m_sCurY - min_y;
	end.x = nDestX - min_x;
	end.y = nDestY - min_y;

	if (start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
	{
		return false;
	}

	m_ptDest.x = nDestX;
	m_ptDest.y = nDestY;

	m_min_x = min_x;
	m_min_y = min_y;
	m_max_x = max_x;
	m_max_y = max_y;

	return PathFind(start, end);
}

// Finds a path from start to end
bool CNpc::PathFind(CPoint start, CPoint end)
{
	m_bRandMove = false;

	if (start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
	{
		return false;
	}

	int i, j;

	int min_x, max_x;
	int min_y, max_y;

	min_x = m_min_x;
	min_y = m_min_y;
	max_x = m_max_x;
	max_y = m_max_y;

	if (InterlockedCompareExchangePointer((PVOID*)&m_lMapUsed, (PVOID)1, (PVOID)0) == (PVOID*)0)
	{
//		TRACE("Attempting path finding\n");
		ClearPathFindData();

		m_vMapSize.cx = max_x - min_x + 1;
		m_vMapSize.cy = max_y - min_y + 1;

		// Creates a mini map of all the non-movable and movable tiles so it doesn't need to use the main map.
		for (i = 0; i < m_vMapSize.cy; i++)
		{
			for (j = 0; j < m_vMapSize.cx; j++)
			{
				if ((j*m_vMapSize.cy + i) >= MAX_MAP_SIZE)
				{
					break;
				}

				if (min_x+j == m_sCurX && min_y+i == m_sCurY)
				{
					m_pMap[j*m_vMapSize.cy + i] = MAP_MOVEABLE;
				}
				else
				{
					if (m_pOrgMap[min_x + j][min_y + i].m_bMove == MAP_NON_MOVEABLE || m_pOrgMap[min_x + j][min_y + i].m_lUser != 0)
					{
						m_pMap[j*m_vMapSize.cy + i] = MAP_NON_MOVEABLE;
					}
					else
					{
						m_pMap[j*m_vMapSize.cy + i] = MAP_MOVEABLE;
					}
				}
			}
		}

		m_vStartPoint  = start;		m_vEndPoint = end;
		m_pPath = NULL;
		m_vPathFind.SetMap(m_vMapSize.cx, m_vMapSize.cy, m_pMap);

		// Start / end swapped around so that we can use m_pPath->Parent to get the route from our location
		// To end location
		m_pPath = m_vPathFind.FindPath(end.x, end.y, start.x, start.y);

		::InterlockedExchange(&m_lMapUsed, 0);

		if (m_pPath)
		{
			//NpcTrace(_T("NPC Succeeded in finding path to target!"));
			return true;
		}
		else
		{
			return false;
		}
	}
	else return false;
}

// Used in path finding to check if there are any more places the monster is set to move to.
bool CNpc::IsMovingEnd()
{
	if (m_bRandMove)
	{
		if (m_arRandMove.GetSize()) return false;

		return true;
	}

	if (!m_pPath) return true;

	int min_x = m_min_x;
	int min_y = m_min_y;

	if ((m_sCurX - min_x) == m_vEndPoint.x && (m_sCurY - min_y) == m_vEndPoint.y) return true;

	return false;
}

// Moves nStep using either a path or random movement.
bool CNpc::StepMove(COM* pCom, int nStep)
{
	if (!m_pPath && !m_bRandMove) return false; // No movement calculated..
	if (m_NpcState != NPC_MOVING && m_NpcState != NPC_TRACING && m_NpcState != NPC_BACK) return false;

	int min_x;
	int min_y;
	int will_x;
	int will_y;

	CPoint ptPre;

	MAP* pMap = g_Zones[m_ZoneIndex];
	if (!pMap)return false;
	if (!pMap->m_pMap)return false;

	for (int i = 0; i < nStep; i++)
	{
		if (m_bRandMove)
		{
			if (!m_arRandMove.GetSize()) return false;

			min_x = m_min_x;
			min_y = m_min_y;

			will_x = min_x + m_arRandMove[0].x;
			will_y = min_y + m_arRandMove[0].y;

			m_arRandMove.RemoveAt(0);

			if (will_x >= pMap->m_sizeMap.cx || will_x < 0 || will_y >= pMap->m_sizeMap.cy || will_y < 0)
			{
				m_vEndPoint.x = m_sCurX - min_x;
				m_vEndPoint.y = m_sCurY - min_y;
				return false;
			}

			if (pMap->m_pMap[will_x][will_y].m_bMove == MAP_NON_MOVEABLE || pMap->m_pMap[will_x][will_y].m_lUser != 0)
			{
				m_vEndPoint.x = m_sCurX - min_x;
				m_vEndPoint.y = m_sCurY - min_y;
				return false;
			}

			ptPre.x = m_sCurX;
			ptPre.y = m_sCurY;

			m_sCurX = will_x;
			m_sCurY = will_y;

			SightRecalc(pCom);

			break;
		}
		else if (m_pPath->Parent)
		{
			m_pPath = m_pPath->Parent;

			min_x = m_min_x;
			min_y = m_min_y;

			will_x = min_x + m_pPath->x;
			will_y = min_y + m_pPath->y;

			if (will_x >= pMap->m_sizeMap.cx || will_x < 0 || will_y >= pMap->m_sizeMap.cy || will_y < 0)
			{
				m_vEndPoint.x = m_sCurX - min_x;
				m_vEndPoint.y = m_sCurY - min_y;
				return false;
			}

			if (pMap->m_pMap[will_x][will_y].m_bMove == MAP_NON_MOVEABLE || pMap->m_pMap[will_x][will_y].m_lUser != 0)
			{
				m_vEndPoint.x = m_sCurX - min_x;
				m_vEndPoint.y = m_sCurY - min_y;
				return false;
			}

			ptPre.x = m_sCurX;
			ptPre.y = m_sCurY;

			m_sCurX = will_x;
			m_sCurY = will_y;

			SightRecalc(pCom);
			break;
		}

		return false;
	}

	if (SetUid(m_sCurX, m_sCurY, m_sNid + NPC_BAND))
	{
		pMap->m_pMap[ptPre.x][ptPre.y].m_lUser = 0;
		return true;
	}

	return false;
}

void CNpc::ClearPathFindData()
{
	::ZeroMemory(m_pMap, sizeof(m_pMap));
}

bool CNpc::GetTargetPath(COM* pCom)
{
	if (m_tNpcType == NPCTYPE_NPC)
		return false;

	if (CheckAIType(NPCAITYPE_DUMMY) || CheckAIType(NPCAITYPE_GUILD) || CheckAIType(NPCAITYPE_STONE_GUARD))
		return false;

	BYTE bySearchRange = m_search_range;
	if (!IsInRange())
		bySearchRange = static_cast<BYTE>((m_sRangeRate / 100.0) * m_search_range);

	if (m_iAttackedUid != 0)
	{
		bySearchRange = 10;
		m_Target.id = m_iAttackedUid;
		m_iAttackedUid = 0;
	}

	USER* pUser = GetUser(pCom, m_Target.id - USER_BAND);
	TRACE("Test 1 : target uid: %d\n", m_Target.id);
	if (!CheckUser(pUser))
	{
		InitTarget();
		return false;
	}

	// Guards are teleported to the player
	if (IsDetecter())
	{
		CPoint pt = FindNearAvailablePoint_S(pUser->m_sX, pUser->m_sY, m_sDistance);
		if (pt.x != -1 && pt.y != -1)
		{
			MoveNpc(static_cast<short>(pt.x), static_cast<short>(pt.y), pCom);
			if (IsCloseTarget(pCom, m_sDistance))
			{
				m_NpcState = NPC_FIGHTING;
				m_Delay = 0;
			}
			return true;
		}
		return false;
	}

	int min_x = m_sCurX - bySearchRange;	if (min_x < 0) min_x = 0;
	int min_y = m_sCurY - bySearchRange;	if (min_y < 0) min_y = 0;
	int max_x = m_sCurX + bySearchRange;	if (max_x >= g_Zones[m_ZoneIndex]->m_sizeMap.cx) max_x = g_Zones[m_ZoneIndex]->m_sizeMap.cx - 1;
	int max_y = m_sCurY + bySearchRange;	if (max_y >= g_Zones[m_ZoneIndex]->m_sizeMap.cy) max_y = g_Zones[m_ZoneIndex]->m_sizeMap.cy - 1;

	CRect r = CRect(min_x, min_y, max_x+1, max_y+1);
	if (r.PtInRect(CPoint(pUser->m_sX, pUser->m_sY)) == FALSE) return false; // User not in range

	NpcTrace(_T("NPC Running path find to target"));
	// Run Path Find ---------------------------------------------//
	CPoint start, end;
	start.x = m_sCurX - min_x;
	start.y = m_sCurY - min_y;
	end.x = pUser->m_sX - min_x;
	end.y = pUser->m_sY - min_y;

	m_ptDest.x = m_Target.x = pUser->m_sX;
	m_ptDest.y = m_Target.y = pUser->m_sY;

	m_min_x = min_x;
	m_min_y = min_y;
	m_max_x = max_x;
	m_max_y = max_y;

	return PathFind(start, end);
}

bool CNpc::GetTargetPos(COM *pCom, CPoint &pt)
{
	USER* pUser = GetUser(pCom, m_Target.id - USER_BAND);

	if (!pUser) return false;

	pt.x = pUser->m_sX;
	pt.y = pUser->m_sY;

	return true;
}

bool CNpc::IsChangePath(COM* pCom, int nStep)
{
	if (!m_pPath) return true;

	CPoint pt;
	GetTargetPos(pCom, pt);
	NODE* pTemp = m_pPath;

	CPoint ptPath[2];
	while (true)
	{
		if (pTemp == NULL) break;

		if (pTemp->Parent)
		{
			ptPath[0].x = m_min_x + pTemp->x;
			ptPath[0].y = m_min_y + pTemp->y;
			pTemp = pTemp->Parent;
		}
		else
		{
			ptPath[1].x = m_min_x + pTemp->x;
			ptPath[1].y = m_min_y + pTemp->y;
			break;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		if (abs(ptPath[i].x - pt.x) <= m_sDistance && abs(ptPath[i].y - pt.y) <= m_sDistance) return false;
	}

	return true;
}

bool CNpc::ResetPath(COM* pCom)
{
	CPoint pt;
	GetTargetPos(pCom, pt);

	m_Target.x = pt.x;
	m_Target.y = pt.y;

	return GetTargetPath(pCom);
}

bool CNpc::IsStepEnd()
{
	if (!m_pPath)	return false;

	if (m_NpcState != NPC_MOVING && m_NpcState != NPC_TRACING) return false;

	if (!m_pPath->Parent) return true;

	return false;
}

// Checks if NPC is within the defined co-ordinates of the database
BOOL CNpc::IsInRange()
{
	// Set minX and minY below if NPC is part of a group! it makes sure the NPC stays near the group leader
	if (m_sRecallNid != -1)
	{
		CNpc *pMaster = GetNpc(m_sRecallNid);
		if (pMaster)
		{
			m_sMinX = pMaster->m_sCurX - 3;
			m_sMinY = pMaster->m_sCurY - 3;
			m_sMaxX = pMaster->m_sCurX + 3;
			m_sMaxY = pMaster->m_sCurY + 3;
		}
	}

	CRect rect(m_sMinX, m_sMinY, m_sMaxX + 1, m_sMaxY + 1);
	return rect.PtInRect(CPoint(m_sCurX, m_sCurY));
}

bool CNpc::IsInMovableRange(short sX, short sY)
{
	if (m_pOrgMap[sX][sY].m_bMove == MAP_NON_MOVEABLE) return false;
	if (m_pOrgMap[sX][sY].m_lUser != 0 && m_pOrgMap[sX][sY].m_lUser != m_sNid + NPC_BAND) return false;
	return true;
}

CPoint CNpc::FindNearAvailablePoint_S(int x, int y, int iDistance)
{
	if (x <= -1 || y <= -1) return CPoint(-1,-1);
	if (m_ZoneIndex < 0 || m_ZoneIndex >= g_Zones.GetSize()) return CPoint(-1,-1);
	if (x >= g_Zones[m_ZoneIndex]->m_sizeMap.cx || y >= g_Zones[m_ZoneIndex]->m_sizeMap.cy) return CPoint(-1,-1);

	if (IsInMovableRange(x, y))
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
					if (IsInMovableRange(iCheckX, iCheckY))
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

void CNpc::MoveNpc(short x, short y, COM *pCom)
{
	// Remove NPC from current position
	if (g_Zones[m_ZoneIndex]->m_pMap[m_sCurX][m_sCurY].m_lUser == m_sNid + NPC_BAND)
	{
		InterlockedExchange(&g_Zones[m_ZoneIndex]->m_pMap[m_sCurX][m_sCurY].m_lUser, 0);
	}

	// Tell client the NPC is going!
	int delete_index = 0;
	char delete_send[2048];
	::ZeroMemory(delete_send, sizeof(delete_send));
	FillNpcInfo(delete_send, delete_index, INFO_DELETE, pCom);
	ASSERT(delete_index <= 2048);
	SendInsight(pCom, delete_send, delete_index);

	// Add NPC in new position
	m_sCurX = x;
	m_sCurY = y;
	InterlockedExchange(&g_Zones[m_ZoneIndex]->m_pMap[x][y].m_lUser, m_sNid + NPC_BAND);

	// Send NPC info to clients
	int modify_index = 0;
	char modify_send[2048];
	::ZeroMemory(modify_send, sizeof(modify_send));
	FillNpcInfo(modify_send, modify_index, INFO_MODIFY, pCom);
	ASSERT(modify_index <= 2048);
	SendInsight(pCom, modify_send, modify_index);

	// Prevent sight change because we already sent info to clients
	// Stops problem with NPC appearing to 'warp' to a new position instead of
	// doing move animation.
	m_presx = m_sCurX / SIGHT_SIZE_X;
	m_presy = m_sCurY / SIGHT_SIZE_Y;
}
