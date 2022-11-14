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

void CNpc::NpcLive(COM *pCom)
{
	if (!CheckCanNpcLive())
	{
		m_NpcState = NPC_DEAD;
		m_Delay = 0;
	}
	else
	{
		if (SetLive(pCom))
		{
			// Send NPC info to clients
			int modify_index = 0;
			char modify_send[2048];
			::ZeroMemory(modify_send, sizeof(modify_send));
			FillNpcInfo(modify_send, modify_index, INFO_MODIFY, pCom);
			SendInsight(pCom, modify_send, modify_index);

			// Recalculate sight
			m_presx = -1;
			m_presy = -1;

 			SightRecalc(pCom);

			m_Delay = m_standing_time;

			if (m_sGroup != -1)
			{
				// Recall Force spawned NPC if not already spawned and within same group as the NPC being spawned
				for (int i = 0; i < g_arNpcTypeNoLive.GetSize(); i++)
				{
					NPC_TYPE_NOLIVE *NoLive =  g_arNpcTypeNoLive[i];

					if (!NoLive) continue;
					if (NoLive->sGroup != m_sGroup) continue;

					CNpc *pRecall = GetNpc(NoLive->sNId);
					if (!pRecall) continue;

					if (pRecall->m_sZone != m_sZone) continue;
					if (pRecall->m_NpcState == NPC_LIVE ||
						pRecall->m_NpcState == NPC_DEAD)
					{
						pRecall->m_sRecallNid = m_sNid;
						pRecall->m_sRecallX = m_sCurX;
						pRecall->m_sRecallY = m_sCurY;
						pRecall->m_Delay = 3000;
						pRecall->m_NpcState = NPC_RECALL;
					}
				}
			}
		}
	}
}

bool CNpc::SetLive(COM* pCom)
{
	MAP* pMap = g_Zones[m_ZoneIndex];
	m_pOrgMap = pMap->m_pMap;

	m_sMinX = m_nInitMinX;
	m_sMinY = m_nInitMinY;
	m_sMaxX = m_nInitMaxX;
	m_sMaxY = m_nInitMaxY;

	int iTryCount = 0;
	int x = -1;
	int y = -1;
	while (true)
	{
		iTryCount++;
		if (iTryCount > 10) return false;

		x = m_sMinX;
		if (m_sMinX != m_sMaxX) x = myrand(m_sMinX, m_sMaxX);

		y = m_sMinY;
		if (m_sMinY != m_sMaxY) y = myrand(m_sMinY, m_sMaxY);

		if (pMap->m_pMap[x][y].m_bMove == MAP_NON_MOVEABLE) continue;

		if (pMap->m_pMap[x][y].m_lUser != 0)
		{
			// Extra checks to determine if the object on the map is valid
			// if it isn't valid then we can perform some cleanup and reset the object id in map to 0
			// this will ensure that the position on map is free when for some reason an invalid object was left there.
			if (pMap->m_pMap[x][y].m_lUser >= USER_BAND && pMap->m_pMap[x][y].m_lUser < NPC_BAND)
			{
				USER* pUser = GetUser(pCom, pMap->m_pMap[x][y].m_lUser - USER_BAND);
				if (pUser == NULL)
				{
					pMap->m_pMap[x][y].m_lUser = 0;
				}
				else if (pUser->m_State != STATE_GAMESTARTED)
				{
					pMap->m_pMap[x][y].m_lUser = 0;
				}
				else if (pUser->m_sX != x || pUser->m_sY != y)
				{
					pMap->m_pMap[x][y].m_lUser = 0;
				}
			}
			else if (pMap->m_pMap[x][y].m_lUser >= NPC_BAND)
			{
				CNpc* pNpc = GetNpc(pMap->m_pMap[x][y].m_lUser - NPC_BAND);
				if (pNpc == NULL)
				{
					pMap->m_pMap[x][y].m_lUser = 0;
				}
				else if (pNpc->m_sCurX != x || pNpc->m_sCurY != y)
				{
					pMap->m_pMap[x][y].m_lUser = 0;
				}
			}

			continue;
		}

		break;
	}

	// Place NPC on the map
	m_sCurX = x;
	m_sCurY = y;
	SetUid(m_sCurX, m_sCurY, m_sNid + NPC_BAND);

	short sMinX = m_sCurX - (abs(m_sMinX - m_sMaxX) / 2);
	short sMaxX = m_sCurX + (abs(m_sMinX - m_sMaxX) / 2);
	short sMinY = m_sCurY - (abs(m_sMinY - m_sMaxY) / 2);
	short sMaxY = m_sCurY + (abs(m_sMinY - m_sMaxY) / 2);

	m_sMinX = sMinX;
	if (m_sMinX < 0) m_sMinX = 0;
	m_sMaxX = sMaxX;
	if (m_sMaxX >= pMap->m_sizeMap.cx) m_sMaxX -= 1;

	m_sMinY = sMinY;
	if (m_sMinY < 0) m_sMinY = 0;
	m_sMaxY = sMaxY;
	if (m_sMaxY >= pMap->m_sizeMap.cy) m_sMaxY -= 1;

	// Initialize Items for NPC
	// TODO : The way items for NPC is done needs improvement.

	// Delete Array of current items
	for (int i = 0; i < m_arItem.GetSize(); i++)
	{
		if (m_arItem[i])
		{
			delete m_arItem[i];
		}
	}
	m_arItem.RemoveAll();

	// Create the lookup table for working out items to be dropped
	int ItemTable[30000] = {-1, };
	// ZeroMemory(ItemTable, 30000);
	int iTableIdx = 0;
	for (int i = 0; i < MAX_NPC_ITEM_NUM; i++)
	{
		for (int j = 0; j < m_sarrItemRand[i]; j++)
		{
			if (iTableIdx >= 30000)
			{
				break;
			}
			ItemTable[iTableIdx] = m_sarrItem[i];
			iTableIdx++;
		}
	}

	// Add items to the NPC
	for (int i = 0; i < m_sMaxItemNum; i++)
	{
		int rand = myrand(0, 29999);
		if (ItemTable[rand] == -1)
		{
			continue;
		}

		ItemList *pNpcItem = new ItemList;
		if (pNpcItem == NULL)
		{
			continue;
		}

		// Check for Special Items
		if (ItemTable[rand] <= 10000)
		{
			// Create normal item
			if (!pNpcItem->InitFromItemTable(ItemTable[rand]))
			{
				delete pNpcItem;
				pNpcItem = NULL;
				continue;
			}
		}
		else
		{
			// Create special item
			short sSpecialItemNum = ItemTable[rand]-10000;
			int iSpecialItemIndex = -1;
			for (int i = 0; i < g_arSpecialItemData.GetSize(); i++)
			{
				if (g_arSpecialItemData[i]->sItemNum == sSpecialItemNum)
				{
					iSpecialItemIndex = i;
					break;
				}
			}

			if (iSpecialItemIndex == -1)
			{
				delete pNpcItem;
				pNpcItem = NULL;
				continue;
			}

			SpecialItemData* s = g_arSpecialItemData[iSpecialItemIndex];
			if (!s)
			{
				delete pNpcItem;
				pNpcItem = NULL;
				continue;
			}

			if (!pNpcItem->InitFromItemTable(s->sItemNum))
			{
				delete pNpcItem;
				pNpcItem = NULL;
				continue;
			}

			pNpcItem->bExt = true;
			pNpcItem->sAb1 = myrand(s->sMinAb1, s->sMaxAb1);
			pNpcItem->sAb2 = myrand(s->sMinAb2, s->sMaxAb2);
			pNpcItem->sMaxUsage = pNpcItem->sUsage = pNpcItem->sDur = myrand(s->sMinDur, s->sMaxDur);
			pNpcItem->sTime = myrand(s->sMinTime, s->sMaxTime);
			pNpcItem->sWgt = myrand(s->sMinWg, s->sMaxWg);
		}

		m_arItem.Add(pNpcItem);
	}

	// Restore HP/MP to max
	m_sHP = m_sMaxHp;
	m_sMP = m_sMaxMp;

	// Reset other NPC details
	InitRemainMagic(MAGIC_CLASS_ALL);
	InitRemainSpecial();
	m_sPoisonMagicNo = -1;
	m_dwPoisonMagicCount = 0;
	m_bFirstClick = false;
	InitTarget();
	InitUserList();

	m_NpcState = NPC_STANDING;
	return true;
}

void CNpc::NpcStanding(COM *pCom)
{
//	NpcTrace(_T("NpcStanding()"));

	// Check for remaining paralyze and concussion special attack on NPC
	// if found the NPC is made to keep standing...
	if (CheckRemainSpecialAttack(pCom, SPECIAL_EFFECT_PARALYZE) ||
		CheckRemainSpecialAttack(pCom, SPECIAL_EFFECT_CONCUSSION))
	{
		m_NpcState = NPC_STANDING;
		m_Delay = 750;
		return;
	}

	// Attempt random movement
	if (RandomMove(pCom))
	{
		m_NpcState = NPC_MOVING;
		m_Delay = 750;
		m_iAttackedUid = 0;
		m_bFirstMove = true;
		return;
	}

	// Continues to stand if random move failed
	InitTarget();
	m_NpcState = NPC_STANDING;
	m_Delay = m_standing_time;
	m_iAttackedUid = 0;
	Talking(pCom);
}

void CNpc::NpcMoving(COM *pCom)
{
//	NpcTrace(_T("NpcMoving()"));

	// Check for remaining paralyze and concussion special attack on NPC
	// if found the NPC is made to stop moving...
	if (CheckRemainSpecialAttack(pCom, SPECIAL_EFFECT_PARALYZE) ||
		CheckRemainSpecialAttack(pCom, SPECIAL_EFFECT_CONCUSSION))
	{
		m_NpcState = NPC_STANDING;
		m_Delay = 750;
		return;
	}

	if (FindEnemy(pCom))
	{
		m_NpcState = NPC_ATTACKING;
		m_Delay = 750; // m_sSpeed;
		return;
	}

	if (IsMovingEnd())
	{
		InitTarget();
		m_NpcState = NPC_STANDING;
		m_iAttackedUid = 0;

		if (IsInRange())
		{
			m_Delay = m_standing_time;
		}
		else
		{
			m_Delay = 750;
		}

		Talking(pCom);
		return;
	}

	if (!StepMove(pCom, 1))
	{
		InitTarget();
		m_NpcState = NPC_STANDING;
		m_Delay = m_standing_time;
		m_iAttackedUid = 0;
		return;
	}

	m_Delay = 750; // m_sSpeed;

	CBufferEx TempBuf;
	if (IsStepEnd())
	{
		TempBuf.Add(PKT_MOVEEND);
	}
	else
	{
		TempBuf.Add(PKT_MOVEFIRST);
	}

	TempBuf.Add(SUCCESS);
	TempBuf.Add((int)(NPC_BAND + m_sNid));
	TempBuf.Add((short)-1);
	CPoint pt = ConvertToClient(m_sCurX, m_sCurY);
	//if (m_bFirstMove)
	//{
	//	m_bFirstMove = false;
	TempBuf.Add((short)pt.x);
	TempBuf.Add((short)pt.y);
	//}
	TempBuf.Add((short)pt.x);
	TempBuf.Add((short)pt.y);
	TempBuf.Add((short)0);	// Stamina - NPC does not use.
	TempBuf.Add((short)0);  // Direction - NPC does not use.
	SendInsight(pCom, TempBuf, TempBuf.GetLength());
}

void CNpc::NpcAttacking(COM *pCom)
{
	// Check for remaining concussion special attack on NPC
	// if found the NPC is made to stop attacking...
	if (CheckRemainSpecialAttack(pCom, SPECIAL_EFFECT_CONCUSSION))
	{
		m_NpcState = NPC_STANDING;
		m_Delay = 750;
		return;
	}

	NpcTrace(_T("NpcAttacking()"));

	int ret = 0;

	TRACE("1 NPC Target: %d\n", m_Target.id);
	if (IsCloseTarget(pCom, m_sDistance))
	{
		NpcTrace(_T("NPC Fighting!"));
		m_NpcState = NPC_FIGHTING;
		m_Delay = 0;
		return;
	}
	TRACE("2 NPC Target: %d\n", m_Target.id);

	if (!GetTargetPath(pCom))
	{
		NpcTrace(_T("NPC Couldn't find target path"));
		if (!RandomMove(pCom))
		{
			InitTarget();
			m_NpcState = NPC_STANDING;
			m_Delay = m_standing_time;
			m_iAttackedUid = 0;
			return;
		}

		// m_bFirstMove = true;
		m_NpcState = NPC_MOVING;
		m_Delay = 750;//m_sSpeed;
		return;
	}

	// m_bFirstMove = true;
	m_NpcState = NPC_TRACING;
	m_Delay = 750;//m_sSpeed;
}

void CNpc::NpcTracing(COM *pCom)
{
	NpcTrace(_T("NpcTracing()"));

	// Check for remaining paralyze and concussion special attack on NPC
	// if found the NPC is made to stop tracing...
	if (CheckRemainSpecialAttack(pCom, SPECIAL_EFFECT_PARALYZE) ||
		CheckRemainSpecialAttack(pCom, SPECIAL_EFFECT_CONCUSSION))
	{
		m_NpcState = NPC_STANDING;
		m_Delay = 750;
		return;
	}

	if (IsMovingEnd())
	{
		// TODO : Implement IsCloseTargetNonRecalc
	}
	else
	{
		// TODO : Implement IsCloseTargetRecalc
	}

//	if (m_tNpcType == NPCTYPE_GUARD || m_tNpcType == NPCTYPE_GUILD_GUARD) return;

	if (GetUser(pCom, (m_Target.id - USER_BAND)) == NULL)	// Target User Not found
	{
		InitTarget();
		m_NpcState = NPC_STANDING;
		m_Delay = m_standing_time;
		m_iAttackedUid = 0;
		return;
	}

	if (IsCloseTarget(pCom, m_sDistance)) // Is target now in range of attack?
	{
		NpcTrace(_T("NPC Fighting!"));
		m_NpcState = NPC_FIGHTING;
		m_Delay = 0;
		return;
	}

	if (IsSurround(m_Target.x, m_Target.y))	// Is target now surrounded?
	{
		InitTarget();
		m_NpcState = NPC_STANDING;
		m_Delay = m_standing_time;
		m_iAttackedUid = 0;
		return;
	}

	if (IsChangePath(pCom))	// Has target changed pos?
	{
		NpcTrace(_T("NPC Target has changed pos!"));
		if (ResetPath(pCom) == FALSE)// && !m_tNpcTraceType)
		{
			InitTarget();
			m_NpcState = NPC_STANDING;
			m_Delay = m_standing_time;
			m_iAttackedUid = 0;
			return;
		}
	}

	if (!StepMove(pCom, 1))
	{
		NpcTrace(_T("NPC Failed moving to target!!"));
		InitTarget();
		m_NpcState = NPC_STANDING;
		m_Delay = m_standing_time;
		m_iAttackedUid = 0;
		return;
	}

	// m_bFirstMove = true;
	m_Delay = 750;//m_sSpeed;

	//NpcTrace(_T("NPC Moving towards target"));

	CBufferEx TempBuf;

	//if (m_bFirstMove)
	//{
	//	TempBuf.Add(PKT_MOVEFIRST);
	//}
	if (IsStepEnd())
	{
		TempBuf.Add(PKT_MOVEEND);
	}
	else
	{
		TempBuf.Add(PKT_MOVEFIRST);
	}

	TempBuf.Add(SUCCESS);
	TempBuf.Add((int)(NPC_BAND + m_sNid));
	TempBuf.Add((short)-1);
	CPoint pt = ConvertToClient(m_sCurX, m_sCurY);
	//if (m_bFirstMove)
	//{
	//	m_bFirstMove = false;
	//TempBuf.Add((short)pt.x);
	//TempBuf.Add((short)pt.y);
	//}
	TempBuf.Add((short)pt.x);
	TempBuf.Add((short)pt.y);
	TempBuf.Add((short)0);	// Stamina - NPC does not use.
	TempBuf.Add((short)0);  // Direction - NPC does not use.
	SendInsight(pCom, TempBuf, TempBuf.GetLength());
}


void CNpc::NpcFighting(COM *pCom)
{
	// NpcTrace(_T("NpcFighting()"));

	// Check for remaining concussion special attack on NPC
	// if found the NPC is made to stop fighting...
	if (CheckRemainSpecialAttack(pCom, SPECIAL_EFFECT_CONCUSSION))
	{
		m_NpcState = NPC_STANDING;
		m_Delay = 750;
		return;
	}

	int iOldTargetId = 0;
	if (m_iAttackedUid != 0 && m_sHowTarget == 0) // Change the target to the last user to attack NPC
	{
		iOldTargetId = m_Target.id;
		m_Target.id = m_iAttackedUid;
		m_iAttackedUid = 0;
	}

	if (!IsCloseTarget(pCom, m_sDistance))
	{
		m_Target.id = iOldTargetId;
		if (!IsCloseTarget(pCom, m_sDistance))
		{
			m_NpcState = NPC_TRACING;
			return;
		}
	}

	if (SelectAttackType() >= 1) // Magic Attack
	{
		Magic(pCom, SelectMagicType());
	}
	else // Normal Attack
	{
		Attack(pCom);
	}

	if (CheckRemainMagic(MAGIC_EFFECT_SLOW))
	{
		m_Delay = DEFAULT_MOB_ATTACK_SPEED * 2;
	}
	else
	{
		m_Delay = DEFAULT_MOB_ATTACK_SPEED;
	}
}

void CNpc::NpcDead(COM *pCom)
{
	if (m_bFirstLive)
	{
		m_bFirstLive = false;
		m_Delay = myrand(0, m_sRegenTime);
		m_NpcState = NPC_LIVE;
	}
	else
	{
		m_Delay = m_sRegenTime;
		m_NpcState = NPC_LIVE;
	}
}

void CNpc::NpcRecall(COM *pCom)
{
	if (m_sRecallX == -1 || m_sRecallY == -1) return;

	CPoint pt = FindNearAvailablePoint_S(m_sRecallX, m_sRecallY, 5);
	if (pt.x == -1 || pt.y == -1) return;

	if (SetLive(pCom))
	{
		m_sMinX = pt.x - 10;
		m_sMaxX = pt.x + 10;
		m_sMinY = pt.y - 10;
		m_sMaxY = pt.y + 10;
		MoveNpc(static_cast<short>(pt.x), static_cast<short>(pt.y), pCom);
	}
}
