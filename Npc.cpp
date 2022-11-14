#include "stdafx.h"
#include "USER.h"
#include "Npc.h"

#include "BufferEx.h"
#include "Extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int surround_x[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
int surround_y[8] = {0, -1, -1, -1, 0, 1, 1, 1};

CNpc::CNpc()
{
	m_NpcState = NPC_DEAD;

	InitTarget();
	m_iAttackedUid = 0;

	m_Delay = 0;
	m_dwLastThreadTime = GetTickCount();
	m_dwLastFind = GetTickCount();
	m_dwLastTimeCount = GetTickCount();

	m_sClientSpeed = 0;
	m_dwStepDelay = 0;

	m_sMoveDistance = DEFAULT_MOVE_DISTANCE;

	m_lMapUsed = 0;
	m_pPath = NULL;
	m_pOrgMap = NULL;

	m_bFirstLive = true;

	::ZeroMemory(m_pMap, sizeof(m_pMap));

	m_presx = -1;
	m_presy = -1;

	m_bFirstMove = false;

//	m_lKillUid = -1;

	InitUserList();

	m_sPoisonMagicNo = -1;
	m_dwPoisonMagicCount = 0;

	for (int i = 0; i < MAX_NPC_CHAT_NUM; i++)
	{
		m_sChat[i] = -1;
	}

	m_sMinDetecterX = -1;
	m_sMaxDetecterX = -1;
	m_sMinDetecterY = -1;
	m_sMaxDetecterY = -1;

	m_sRecallNid = -1;
	m_sRecallX = -1;
	m_sRecallY = -1;

	m_sTownGuildNum = -1;
	m_sTownGuildPicId = 0;
	::ZeroMemory(m_strTownGuildName, sizeof(m_strTownGuildName));
}

CNpc::~CNpc()
{
	ClearPathFindData();

	// Delete Items
	for (int i = 0; i < m_arItem.GetSize(); i++)
	{
		if (m_arItem[i])
		{
			delete m_arItem[i];
			m_arItem[i] = NULL;
		}
	}
	m_arItem.RemoveAll();
}

bool CNpc::SetUid(int x, int y, int id)
{
	MAP* pMap = g_Zones[m_ZoneIndex];

	if (pMap->m_pMap[x][y].m_bMove == MAP_NON_MOVEABLE) return false;
	if (pMap->m_pMap[x][y].m_lUser != 0 && pMap->m_pMap[x][y].m_lUser != id)return false;

	pMap->m_pMap[x][y].m_lUser = id;

	return true;
}

CPoint CNpc::ConvertToClient(int x, int y)
{
	if (!g_Zones[m_ZoneIndex]) return CPoint(-1,-1);

	int tempx, tempy;
	int temph = g_Zones[m_ZoneIndex]->m_vMoveCell.m_vDim.cy / 2 - 1;

	if (y >= g_Zones[m_ZoneIndex]->m_sizeMap.cy || x >= g_Zones[m_ZoneIndex]->m_sizeMap.cx) return CPoint(-1,-1);

	tempx = x - temph + y;
	tempy = y - x + temph;

	return CPoint(tempx, tempy);
}

CPoint CNpc::ConvertToServer(int x, int y)
{
	if (!g_Zones[m_ZoneIndex])
	{
		return CPoint(-1,-1);
	}

	int tempx, tempy;
	int temph = g_Zones[m_ZoneIndex]->m_vMoveCell.m_vDim.cy / 2 - 1;

	if (y >= g_Zones[m_ZoneIndex]->m_vMoveCell.m_vDim.cy || x >= g_Zones[m_ZoneIndex]->m_vMoveCell.m_vDim.cx)
	{
		return CPoint(-1,-1);
	}

	if ((x+y)%2 == 0)
	{
		tempx = temph - (y / 2)+ (x / 2);

		if (x % 2)tempy = (y / 2)+ ((x / 2)+ 1);
		else        tempy = (y / 2)+ (x / 2);

		return CPoint(tempx, tempy);
	}
	else return CPoint(-1,-1);
}

bool CNpc::Init()
{
	if ((m_sMinX + m_sMinY) % 2 != 0) m_sMinX++;
	CPoint ptMin = ConvertToServer(m_sMinX, m_sMinY);
	if (ptMin.x == -1 || ptMin.y == -1)
	{
		CString szTemp;
		szTemp.Format(_T("Invalid NPC MIN AXIS : Name = %s, x = %d, y = %d"), m_strName, m_sMinX, m_sMinY);
		AfxMessageBox(szTemp);
		InterlockedIncrement(&g_CurrentNPCError);
		return false;
	}

	if ((m_sMaxX + m_sMaxY) % 2 != 0) m_sMaxX++;
	CPoint ptMax = ConvertToServer(m_sMaxX, m_sMaxY);
	if (ptMax.x == -1 || ptMax.y == -1)
	{
		CString szTemp;
		szTemp.Format(_T("Invalid NPC MAX AXIS : Name = %s, x = %d, y = %d"), m_strName, m_sMaxX, m_sMaxY);
		AfxMessageBox(szTemp);
		InterlockedIncrement(&g_CurrentNPCError);
		return false;
	}

	m_sMinDetecterX = static_cast<short>(ptMin.x);
	m_sMaxDetecterX = static_cast<short>(ptMax.x);
	m_sMinDetecterY = static_cast<short>(ptMin.y);
	m_sMaxDetecterY = static_cast<short>(ptMax.y);

	if (IsDetecter() || IsStone())
	{
		CPoint pt = ConvertToServer(m_sMoneyMin, m_sMoneyMax);
		if (pt.x == -1 || pt.y == -1)
		{
			CString szTemp;
			szTemp.Format(_T("Invalid GUARD NPC AXIS : Name = %s, x = %d, y = %d"), m_strName, m_sMoneyMin, m_sMoneyMax);
			AfxMessageBox(szTemp);
			InterlockedIncrement(&g_CurrentNPCError);
			return false;
		}

		ptMin.x = pt.x;
		ptMax.x = pt.x;
		ptMin.y = pt.y;
		ptMax.y = pt.y;
	}

	if (ptMax.x < ptMin.x)
	{
		m_sMinX = ptMax.x;
		m_sMaxX = ptMin.x;
	}
	else
	{
		m_sMinX = ptMin.x;
		m_sMaxX = ptMax.x;
	}

	if (ptMax.y < ptMin.y)
	{
		m_sMinY = ptMax.y;
		m_sMaxY = ptMin.y;
	}
	else
	{
		m_sMinY = ptMin.y;
		m_sMaxY = ptMax.y;
	}

	m_nInitMinX = m_sMinX;
	m_nInitMinY = m_sMinY;
	m_nInitMaxX = m_sMaxX;
	m_nInitMaxY = m_sMaxY;

	int iTryCount = 0;
	int x = -1;
	int y = -1;
	while (true)
	{
		iTryCount++;

		x = m_sMinX;
		if (m_sMinX != m_sMaxX) x = myrand(m_sMinX, m_sMaxX);

		y = m_sMinY;
		if (m_sMinY != m_sMaxY) y = myrand(m_sMinY, m_sMaxY);

		if (g_Zones[m_ZoneIndex] == NULL ||
			g_Zones[m_ZoneIndex]->m_pMap[x][y].m_bMove == MAP_NON_MOVEABLE ||
			g_Zones[m_ZoneIndex]->m_pMap[x][y].m_lUser != 0)
		{
			if (iTryCount >= 100)
			{
				CString szTemp;
				szTemp.Format(_T("Fail No Movecell(monsterset => %d)"), m_sMid);
				AfxMessageBox(szTemp);
				InterlockedIncrement(&g_CurrentNPCError);
				return false;
			}
			continue;
		}

		break;
	}

	// m_sCurX	= m_sOrgX = m_sTableOrgX = x;
	// m_sCurY	= m_sOrgY = m_sTableOrgY = y;
	m_sCurX = x;
	m_sCurY = y;
	m_pOrgMap = g_Zones[m_ZoneIndex]->m_pMap;
	m_bFirstClick = false;
	InitAttackType();
	m_bRandMove = false;

	return true;
}

// Determines if the NPC's target is close enough to attack
bool CNpc::IsCloseTarget(COM* pCom, int nRange)
{
	if (m_tNpcType == NPCTYPE_NPC)
		return false;

	if (CheckAIType(NPCAITYPE_DUMMY) || CheckAIType(NPCAITYPE_GUILD))
		return false;

	if (m_search_range == 0)
		return false;

	USER* pUser = GetUser(pCom, m_Target.id - USER_BAND);
	if (!CheckUser(pUser))
	{
		InitTarget();
		return false;
	}

	if (!GetDistance(pUser->m_sX, pUser->m_sY, nRange)) return false;

	m_Target.x = pUser->m_sX;
	m_Target.y = pUser->m_sY;

	return true;
}


bool CNpc::GetDistance(int xpos, int ypos, int dist)
{
	if (xpos >= g_Zones[m_ZoneIndex]->m_sizeMap.cx || xpos < 0 ||
		ypos >= g_Zones[m_ZoneIndex]->m_sizeMap.cy || ypos < 0) return false;

	if (abs(m_sCurX - xpos)> dist || abs(m_sCurY - ypos)> dist) return false;

	return true;
}

bool CNpc::IsSurround(int targetx, int targety)
{
	// Might need to make this function more efficient
	for (int i = 0; i < (sizeof(surround_x) / sizeof(surround_x[0])); i++)
	{
		if (IsMovable(targetx + surround_x[i], targety + surround_y[i])) return false;
	}

	return true;
}

bool CNpc::IsMovable(int x, int y)
{
	if (x < 0 || y < 0) return false;

	if (!g_Zones[m_ZoneIndex]) return false;
	if (!g_Zones[m_ZoneIndex]->m_pMap) return false;

	if (x >= g_Zones[m_ZoneIndex]->m_sizeMap.cx ||
		y >= g_Zones[m_ZoneIndex]->m_sizeMap.cy) return false;

	if (g_Zones[m_ZoneIndex]->m_pMap[x][y].m_bMove == MAP_NON_MOVEABLE ||
		g_Zones[m_ZoneIndex]->m_pMap[x][y].m_lUser) return false;

	return true;
}

USER* CNpc::GetUser(COM* pCom, int uid)
{
	if (!pCom) return NULL;
	//ASSERT(uid >= 0 && uid < MAX_USER);
	if (uid < 0 || uid >= MAX_USER) return NULL;
	return pCom->GetUserUid(uid);
}

void CNpc::NpcTrace(TCHAR *pMsg)
{
#ifdef _DEBUG
	CString szMsg = _T("");
	CPoint pt = ConvertToClient(m_sCurX, m_sCurY);
	szMsg.Format(_T("%s : uid = %d, name = %s, xpos = %d, ypos = %d\n"), pMsg, m_sNid, m_strName, pt.x, pt.y);
	TRACE(szMsg);
#endif
}

bool CNpc::CheckUser(USER* pUser)
{
	// The checks below make sure that the user is a valid target.
	if (!pUser) return false;
	if (pUser->m_State != STATE_GAMESTARTED) return false;
	if (!pUser->m_bLive) return false;
	if (pUser->m_bIsWarping) return false;
	if (pUser->m_bHidden) return false;
	return true;
}

bool CNpc::IsDetecter()
{
	if ((_stricmp(m_strName, "GUARD") == 0) ||
		(_stricmp(m_strName, "DEVIL GUARD") == 0))
	{
		return true;
	}
	return false;
}

bool CNpc::IsStone()
{
	if (_stricmp(m_strName, "Stone Guard") == 0)
	{
		return true;
	}
	return false;
}

bool CNpc::CheckAIType(int iAIType)
{
	return m_sTypeAI == iAIType ? true : false;
}

// Check if a user is within sight of the NPC
bool CNpc::FindUserInSight(COM* pCom)
{
	int sx = m_sCurX / SIGHT_SIZE_X;
	int sy = m_sCurY / SIGHT_SIZE_Y;

	int min_x = (sx-1)*SIGHT_SIZE_X; if (min_x < 0)min_x = 0;
	int max_x = (sx+2)*SIGHT_SIZE_X;
	int min_y = (sy-1)*SIGHT_SIZE_Y; if (min_y < 0)min_y = 0;
	int max_y = (sy+2)*SIGHT_SIZE_Y;

	MAP* pMap = pMap = g_Zones[m_ZoneIndex];
	if (!pMap) return false;

	int tmin_x = min_x;		if (tmin_x < 0) tmin_x = 0;
	int tmax_x = max_x;		if (tmax_x >= pMap->m_sizeMap.cx) tmax_x = pMap->m_sizeMap.cx - 1;
	int tmin_y = min_y;		if (tmin_y < 0) tmin_y = 0;
	int tmax_y = max_y;		if (tmax_y >= pMap->m_sizeMap.cy) tmax_y = pMap->m_sizeMap.cy - 1;

	bool bUserFound = false;
	for (int i = tmin_x; i < tmax_x; i++)
	{
		for (int j = tmin_y; j < tmax_y; j++)
		{
			int temp_uid = pMap->m_pMap[i][j].m_lUser;

			if (temp_uid < USER_BAND || temp_uid >= NPC_BAND) continue;
			else temp_uid -= USER_BAND;

			if (temp_uid >= 0 && temp_uid < MAX_USER)
			{
				USER* pUser = pCom->GetUserUid(temp_uid);
				if (pUser == NULL) continue;

				if (pUser->m_State == STATE_GAMESTARTED)
				{
					if (pUser->m_sX == i && pUser->m_sY == j && pUser->m_sZ == m_sCurZ)
					{
						bUserFound = true;
						break; // Found a user!
					}
				}
			}
		}
	}

	return bUserFound;
}

bool CNpc::CheckCanNpcLive()
{
	if (m_sRegenEvent != -1)
	{
		return false; // TODO : should call CheckServerSpecialEvent and return result of that
	}

	return true;
}

// Handles NPC chatting
void CNpc::Talking(COM *pCom)
{
	if (!pCom) return;

	// Get the amount of chat NPC has got
	int iChatNumCount = 0;
	for (int i = 0; i < MAX_NPC_CHAT_NUM; i++)
	{
		if (m_sChat[i] != -1)
		{
			++iChatNumCount;
		}
	}

	// Process if NPC has got chat
	if (iChatNumCount > 0)
	{
		// Work out which chat should be used this time round
		int iNpcChatIndex = myrand(0, 64) % iChatNumCount;
		int iChatNum = m_sChat[iNpcChatIndex];

		// Get the index of the chat within the NPC chat array
		int iChatIndex = -1;
		for (int i = 0; i < g_arNpcChat.GetSize(); i++)
		{
			if (g_arNpcChat[i]->m_sCid == iChatNum)
			{
				iChatIndex = i;
				break;
			}
		}

		// Send the chat message to the client if it was found
		// If the chat was not found then for some reason it is not in the database!
		// (Maybe should log chats that are not found so can fix the data in database).
		if (iChatIndex >= 0)
		{
			CBufferEx TempBuf;
			TempBuf.Add(PKT_CHAT);
			TempBuf.Add(NORMAL_CHAT);
			TempBuf.Add((int)(m_sNid + NPC_BAND));
			TempBuf.Add(m_sClass);
			TempBuf.Add((BYTE)0);
			TempBuf.AddString((LPTSTR)(LPCTSTR)g_arNpcChat[iChatIndex]->m_strTalk);
			SendInsight(pCom, TempBuf, TempBuf.GetLength());
		}
	}
}

CNpc* CNpc::GetNpc(int nid)
{
	if (nid < 0 || nid >= g_arNpc.GetSize()) return NULL;

	return g_arNpc[nid];
}

bool CNpc::ArrowPathFind(CPoint ptSource, CPoint ptTarget)
{
	if (m_sZone != 3 && m_sZone != 6 && m_sZone != 7 && m_sZone != 21) return true;

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

bool CNpc::IsMapMovable_C(int x, int y)
{
	CPoint pt = ConvertToServer(x, y);
	if (pt.x == -1 || pt.y == -1)
	{
		return false;
	}

	x = pt.x;
	y = pt.y;

	if (x < 0 || y < 0)return false;

	if (!g_Zones[m_ZoneIndex]) return false;
	if (!g_Zones[m_ZoneIndex]->m_pMap) return false;

	if (x >= g_Zones[m_ZoneIndex]->m_sizeMap.cx ||
		y >= g_Zones[m_ZoneIndex]->m_sizeMap.cy) return false;

	if (g_Zones[m_ZoneIndex]->m_pMap[x][y].m_bMove == MAP_NON_MOVEABLE) return false;

	return true;
}

BYTE CNpc::GetDirection(int x1, int y1, int x2, int y2)
{
	BYTE direction = 0;
	if ((y1<y2) && (x1 == x2)) direction = DIR_DOWN;
	else if ((y1<y2) && (x1>x2)) direction = DIR_DOWN_LEFT;
	else if ((y1 == y2) && (x1>x2)) direction = DIR_LEFT;
	else if ((y1>y2) && (x1>x2)) direction = DIR_UP_LEFT;
	else if ((y1>y2) && (x1 == x2)) direction = DIR_UP;
	else if ((y1>y2) && (x1<x2)) direction = DIR_UP_RIGHT;
	else if ((y1 == y2) && (x1<x2)) direction = DIR_RIGHT;
	else if ((y1<y2) && (x1<x2)) direction = DIR_DOWN_RIGHT;
	return direction;
}
