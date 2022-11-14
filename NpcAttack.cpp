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

void CNpc::InitTarget()
{
	m_Target.id = 0;
	m_Target.x = 0;
	m_Target.y = 0;
}

void CNpc::InitUserList()
{
	m_iAttackUserListIndex = 0;
	for (int i = 0; i < NPC_HAVE_USER_LIST; i++)
	{
		::ZeroMemory(m_AttackUserList[i].strUserID, sizeof(m_AttackUserList[i].strUserID));
	}
}

void CNpc::InitAttackType()
{
	for (int i = 0; i < 100; i++)
	{
		m_sAttackType[i] = 0; 
	}
	
	m_iAttackTypeMagicRange = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < m_sMagicRate[i]; j++)
		{
			if (m_iAttackTypeMagicRange >= 100) break;
			m_sAttackType[m_iAttackTypeMagicRange] = m_sMagicNum[i];
			++m_iAttackTypeMagicRange;
		}
	}	
}

short CNpc::SelectAttackType()
{
	int rand = myrand(0, 99);
	return m_sAttackType[rand];
}

short CNpc::SelectMagicType()
{
	int rand = myrand(0, m_iAttackTypeMagicRange);
	return m_sAttackType[rand];
}

bool CNpc::CheckAttackSuccess(USER* pUser)
{
	if (!pUser) return false;

	// Get the amount of attacking dex 
	double dDexAttack = static_cast<double>(m_sDex_at);

	// Get the defense dex
	double dDexDefense = static_cast<double>(pUser->m_iDEX) / CLIENT_EXT_STATS;
	int iPlusDex = pUser->PlusFromItem(SPECIAL_DEX);
	dDexDefense += iPlusDex;

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

	// Subtract any evasion gained from dodging aura 
	if (pUser->CheckRemainSpecialAttack(SPECIAL_EFFECT_DODGE))
	{
		iDexAttack -= pUser->m_RemainSpecial[SPECIAL_EFFECT_DODGE].sDamage;
	}

	// Add on any accuracy that is gained from wearing items
	iDexAttack -= pUser->PlusFromItem(SPECIAL_EVASION);

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

bool CNpc::IsAttackList(USER* pUser)
{
	if (pUser == NULL || pUser->m_State != STATE_GAMESTARTED) return false;

	int sLen = strlen(pUser->m_strUserId);

	if (sLen < 0 || sLen > NAME_LENGTH) return false;

	for (int i = 0; i < NPC_HAVE_USER_LIST; i++)
	{
		if (strcmp(m_AttackUserList[i].strUserID, pUser->m_strUserId) == 0) return true;
	}

	return false;
}

void CNpc::AttackListAdd(USER* pUser)
{
	if (!IsAttackList(pUser))
	{
		int i = m_iAttackUserListIndex;

		ZeroMemory(m_AttackUserList[i].strUserID, sizeof(m_AttackUserList[i].strUserID));
		strcpy(m_AttackUserList[i].strUserID, pUser->m_strUserId);

		m_iAttackUserListIndex++;
		if (m_iAttackUserListIndex >= NPC_HAVE_USER_LIST)
		{
			m_iAttackUserListIndex = 0;
		}
	}
}

void CNpc::AttackListRemove(USER* pUser)
{
	for (int i = 0; i < NPC_HAVE_USER_LIST; i++)
	{
		if (strcmp(m_AttackUserList[i].strUserID, pUser->m_strUserId) == 0)
		{
			ZeroMemory(m_AttackUserList[i].strUserID, sizeof(m_AttackUserList[i].strUserID));
			break;
		}
	}
}

int CNpc::GetDefense()
{
	return m_sAStr + m_sStr;
}

int CNpc::GetAttack()
{
	return m_sWStr + m_sStr;
}

bool CNpc::FindEnemy(COM *pCom)
{
	// BOOL bSearch = FALSE;

	if (m_tNpcType == NPCTYPE_NPC)
		return false;

	if (CheckAIType(NPCAITYPE_DUMMY) || CheckAIType(NPCAITYPE_GUILD)) 
		return false;
	
	if (m_search_range == 0) 
		return false; // 0 Attack distance so can't attack

	if (m_iAttackedUid != 0) // Has someone attacked the NPC?
	{
		// Don't need to find an enemy if someone attacked the NPC
		m_Target.id = m_iAttackedUid;
		m_iAttackedUid = 0;
		return false;
	}

	BYTE bySearchRange = m_search_range; 
	if (!IsInRange())
		bySearchRange = static_cast<BYTE>((m_sRangeRate / 100.0) * bySearchRange);

	int min_x = m_sCurX - bySearchRange; if (min_x < 0)min_x = 0;
	int min_y = m_sCurY - bySearchRange; if (min_y < 0)min_y = 0;
	int max_x = m_sCurX + bySearchRange;
	int max_y = m_sCurY + bySearchRange;

	if (max_x >= g_Zones[m_ZoneIndex]->m_sizeMap.cx) max_x = g_Zones[m_ZoneIndex]->m_sizeMap.cx - 1;
	if (max_y >= g_Zones[m_ZoneIndex]->m_sizeMap.cy) max_y = g_Zones[m_ZoneIndex]->m_sizeMap.cy - 1;

	for (int ix = min_x; ix <= max_x; ix++)
	{
		for (int iy = min_y; iy <= max_y; iy++)
		{
			int target_uid = m_pOrgMap[ix][iy].m_lUser;
			if (target_uid < USER_BAND || target_uid >= NPC_BAND) continue;

			USER* pUser = pCom->GetUserUid(target_uid - USER_BAND);
			if (CheckUser(pUser))
			{
				if (ix != pUser->m_sX || iy != pUser->m_sY)
				{
				//	TRACE("User: %s at: %d %d ... not at: %d %d\n", pUser->m_strUserID, pUser->m_sLX, pUser->m_sLY, ix, iy);
				//	TRACE("User not in same co-ordinates as monster thinks!\n");
					m_pOrgMap[ix][iy].m_lUser = 0;
					continue;
				}

				if (m_sTownGuildNum != -1 && m_sTownGuildNum == pUser->m_sGroup)
				{
					continue;
				}

				if (m_sGuild != -1 && pUser->CheckGuildTownAlly(m_sTownGuildNum))
				{
					continue;
				}

				if (IsDetecter() || IsStone())
				{
					int iMoral = pUser->m_iMoral / CLIENT_SKILL;
					if (!pUser->CheckDemon(pUser->m_sClass))
					{
						if (iMoral <= -50)
						{
							m_Target.id	= target_uid;
							m_Target.x	= ix;
							m_Target.y	= iy;
							return true;
						}
					}
					else
					{
						if (iMoral >= 50)
						{
							m_Target.id	= target_uid;
							m_Target.x	= ix;
							m_Target.y	= iy;
							return true;
						}
					}
				}

				switch (m_tNpcAttType)
				{
				case NPC_AT_PASSIVE:
					if (IsAttackList(pUser))
					{
						m_Target.id	= target_uid;
						m_Target.x	= ix;
						m_Target.y	= iy;
						return true;
					}
					break;
				case NPC_AT_AGGRO:
					if (IsSurround(ix, iy)) continue;
					m_Target.id	= target_uid;
					m_Target.x	= ix;
					m_Target.y	= iy;
					return true;
					break;
				default:
					break;
				}
			}
		//	else if (target_uid >= NPC_BAND) // ADD FOR PETS LATER
		}
	}

	// InitUserList();
	// InitTarget();

	return false;
}

void CNpc::Attack(COM *pCom)
{
	if (!pCom) return;
	//TRACE("MOB Attacking\n");

	if (m_Target.id >= USER_BAND && m_Target.id < NPC_BAND)
	{
		USER* pUser = GetUser(pCom, m_Target.id - USER_BAND);
		
		if (!CheckUser(pUser))
		{
			SendAttackFail(pCom, m_Target.id);
			InitTarget();
			m_NpcState = NPC_STANDING;
			m_iAttackedUid = 0;
			return;
		}

		if (!CheckAttackSuccess(pUser))					
		{
			SendAttackMiss(pCom, m_Target.id);
			return;
		}

		// Calculate the damage to be dealt
		int iDefense = pUser->GetDefense();
		int iAttack = GetAttack();

		int iDamage = iAttack - iDefense;
		if (iDamage <= m_sMinDamage)
		{
			iDamage = myrand(1, m_sMinDamage);
		}

		if (iDamage <= 0)
		{
			iDamage = 1;
		}
		if (iDamage > pUser->m_sHP)
		{
			iDamage = pUser->m_sHP;
		}

		// TODO : Set who attacked user
		pUser->SetDamage(iDamage, -1);

		if (pUser->m_lDeadUsed == 1)
		{
			if (IsDetecter())
			{
				pUser->m_iDeathType |= DEATH_TYPE_GUARD;
			}

			InitTarget();
			m_NpcState = NPC_STANDING;
			m_iAttackedUid = 0;

			if (m_tNpcAttType == 0) // Peaceful NPC
			{
				AttackListRemove(pUser);
			}
		}

		//CPoint pt1 = ConvertToClient(m_sCurX, m_sCurY);
		//CPoint pt2 = ConvertToClient(pUser->m_sX, pUser->m_sY);
		//BYTE byDir = GetDirection(pt1.x, pt1.y, pt2.x, pt2.y);
		//SendSpecialAttackArrow(pCom, SUCCESS, byDir, 12, 0, m_Target.id, pUser->m_sHP);

		SendAttackSuccess(pCom, pUser->m_Uid + USER_BAND, pUser->m_sHP, pUser->GetMaxHP());
	}
	else
	{
		m_iAttackedUid = 0;
		m_NpcState = NPC_STANDING;
	}
}

void CNpc::SendAttackSuccess(COM *pCom, int tuid, short sHP, short sMaxHP)
{
	if (pCom == NULL) return;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_ATTACK);
	TempBuf.Add(ATTACK_SUCCESS);
	TempBuf.Add((int)(m_sNid + NPC_BAND));
	TempBuf.Add(tuid);
	TempBuf.Add((BYTE)m_can_escape); // Arrow Attack animation
	TempBuf.Add((short)0); // Direction

	for (int i = 0; i < 4; i++)
	{
		TempBuf.Add((short)-1); // User Item and durability change...
		TempBuf.Add((short)0);
	}

	TempBuf.Add((short)sHP);
	TempBuf.Add((short)sMaxHP);
	TempBuf.Add((DWORD)0); // Exp
	TempBuf.Add((BYTE)0); // unknown
	TempBuf.Add((BYTE)0); // unknown
	SendInsight(pCom, TempBuf, TempBuf.GetLength());
}

void CNpc::SendAttackMiss(COM *pCom, int tuid)
{
	CBufferEx TempBuf;
	
	TempBuf.Add(PKT_ATTACK);
	TempBuf.Add(ATTACK_MISS);
	TempBuf.Add((int)(m_sNid + NPC_BAND));
	TempBuf.Add(tuid);
	TempBuf.Add((BYTE)m_can_escape); // Arrow Attack animation
	TempBuf.Add((short)0); // Direction
	for (int i = 0; i < 4; i++)
	{
		TempBuf.Add((short)-1); // User Item and durability change...
		TempBuf.Add((short)0);
	}
	TempBuf.Add((BYTE)0); // unknown
	TempBuf.Add((BYTE)0); // unknown
	SendInsight(pCom, TempBuf, TempBuf.GetLength());
}

void CNpc::SendAttackFail(COM *pCom, int tuid)
{
	CBufferEx TempBuf;
	
	TempBuf.Add(PKT_ATTACK);
	TempBuf.Add(ATTACK_FAIL);
	TempBuf.Add((int)(m_sNid + NPC_BAND));
	TempBuf.Add(tuid);
	TempBuf.Add((BYTE)m_can_escape); // Arrow Attack animation
	TempBuf.Add((short)0); // Direction
	SendInsight(pCom, TempBuf, TempBuf.GetLength());
}

void CNpc::SetDead(COM* pCom, int iUid, bool bBroadCast)
{
	// Set NPC state as dead and remove from map
	m_NpcState = NPC_DEAD;
	if (g_Zones[m_ZoneIndex]->m_pMap[m_sCurX][m_sCurY].m_lUser == m_sNid + NPC_BAND) 
	{
		InterlockedExchange(&g_Zones[m_ZoneIndex]->m_pMap[m_sCurX][m_sCurY].m_lUser, 0);
	}

	// Remove remaining magic and special
	InitRemainMagic(MAGIC_CLASS_ALL);
	InitRemainSpecial();

	// Remove poison status from NPC
	m_sPoisonMagicNo = -1;
	m_dwPoisonMagicCount = 0;

	if (!pCom) return;
	if (!bBroadCast) return;
	SendNpcStatus(pCom);

	// Tell client an NPC has died, where and by who.
	CBufferEx TempBuf;
	TempBuf.Add(PKT_DEAD);
	TempBuf.Add((int)(m_sNid + NPC_BAND));
	CPoint pt = ConvertToClient(m_sCurX, m_sCurY);
	TempBuf.Add((short)pt.x);
	TempBuf.Add((short)pt.y); 
	TempBuf.Add(iUid + USER_BAND); // Who killed the NPC
	SendInsight(pCom, TempBuf, TempBuf.GetLength());
}

void CNpc::Magic(COM *pCom, short sMagicNo)
{
	if (!pCom) return;

	if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
	{
		return;
	}

	CMagicTable* pMagic = g_arMagicTable[sMagicNo-1];
	if (!pMagic)
	{
		return;
	}

	switch (pMagic->m_tType01)
	{
	case MAGIC_TYPE_ARROW: // Magic Arrow
		MagicArrow(pCom, sMagicNo);
		break;
	case MAGIC_TYPE_RAIL: // Magic Rail
		break;
	case MAGIC_TYPE_CIRCLE: // Magic Circle
		break;
	}
}

void CNpc::MagicArrow(COM *pCom, short sMagicNo)
{
	if (!pCom) return;

	int iTargetId = m_Target.id;

	if (sMagicNo <= 0  || sMagicNo > g_arMagicTable.GetSize())
	{
		SendMagicArrowAttackFail(pCom, iTargetId, sMagicNo);
		return;
	}

	CMagicTable* pMagic = g_arMagicTable[sMagicNo-1];
	if (!pMagic)
	{
		SendMagicArrowAttackFail(pCom, iTargetId, sMagicNo);
		return;
	}

	if (iTargetId < USER_BAND || iTargetId >= INVALID_BAND)	
	{
		SendMagicArrowAttackFail(pCom, iTargetId, sMagicNo);
		return;
	}

	int iDistance = m_sDistance;
	if (m_sDistance <= 1)
	{
		iDistance = pMagic->m_sEDist;
	}

	USER *pUser = pUser = GetUser(pCom, iTargetId - USER_BAND);
	if (!pUser)
	{
		SendMagicArrowAttackFail(pCom, iTargetId, sMagicNo);
		return;
	}

	if (!CheckUser(pUser))
	{
		SendMagicArrowAttackFail(pCom, iTargetId, sMagicNo);
		return;
	}
	
	if (!CheckDistance(pUser, iDistance))
	{
		SendMagicArrowAttackFail(pCom, iTargetId, sMagicNo);
		return;
	}

	int iDamage = pMagic->m_sDamage; // Base Damage

	switch (pMagic->m_tEValue)
	{
	case MAGIC_EFFECT_NORMAL: // Normal Attack Magic
		{
			iDamage = GetMagicDamage(pMagic, pUser);

			int iDefense = 0;
			if (iDamage > 1)
			{
				iDefense = GetMagicDefense(pMagic, pUser);
				iDamage -= iDefense;
				if (iDamage < 1)
				{
					iDamage = 1;
				}
			}

			// Caps the magic damage
			// e.g. DragonBreath caps at 100 this is because
			// sDamage in magic table is 36
			// therefore 36 * 280 = 10080 / 100 = 100
			int iTemp = (pMagic->m_sDamage * 280) / 100;
			if (iDamage > iTemp)
			{
				iDamage = iTemp;
			}

			// Check for weaken and multiply the damage
			if (pUser->CheckRemainMagic(MAGIC_EFFECT_WEAKEN))
			{
				if (iDamage >= 0)
				{
					iDamage *= 2;
				}
				else
				{	
					// TODO: implement weaken negative damage calculation
				}
			}
			
			if (iDamage < 0)
			{
				pUser->m_sHP -= iDamage;
				if (pUser->m_sHP > pUser->GetMaxHP())
				{
					pUser->m_sHP = pUser->GetMaxHP();
				}
			}
			else
			{
				pUser->SetDamage(iDamage, -1, false);
			}

			if (pUser->m_lDeadUsed == 1)
			{
				if (IsDetecter())
				{
					pUser->m_iDeathType |= DEATH_TYPE_GUARD;
				}

				InitTarget();
				m_NpcState = NPC_STANDING;
				m_iAttackedUid = 0;

				if (m_tNpcAttType == 0) // Peaceful NPC
				{
					AttackListRemove(pUser);
				}
			}
			//pUser->m_sHP -= iDamage;
			//if (pUser->m_sHP > pUser->GetMaxHP())
			//{
			//	pUser->m_sHP = pUser->GetMaxHP();
			//}

			//if (pUser->m_sHP < 0)
			//{
			//	pUser->m_sHP = 0;
			//}
		}
		break;
	case MAGIC_EFFECT_POISON:
		{
			// Cannot be poisoned if user is wearing a anti-poison item
			if (pUser->CheckHaveSpecialNumInBody(SPECIAL_RESIST_POISON))
			{
				SendMagicArrowAttackFail(pCom, iTargetId, sMagicNo);
				return;
			}

			iDamage = GetMagicDamage(pMagic, pUser);

			int iDefense = 0;
			if (iDamage > 1)
			{
				iDefense = GetMagicDefense(pMagic, pUser);
				iDamage -= iDefense;
				if (iDamage < 1)
				{
					iDamage = 1;
				}
			}

			// Caps the magic damage
			// e.g. DragonBreath caps at 100 this is because
			// sDamage in magic table is 36
			// therefore 36 * 280 = 10080 / 100 = 100
			int iTemp = (pMagic->m_sDamage * 280) / 100;
			if (iDamage > iTemp)
			{
				iDamage = iTemp;
			}

			// Check for weaken and multiply the damage
			if (pUser->CheckRemainMagic(MAGIC_EFFECT_WEAKEN))
			{
				if (iDamage >= 0)
				{
					iDamage *= 2;
				}
				else
				{	
					// TODO: implement weaken negative damage calculation

				}
			}

			if (iDamage < 0)
			{
				pUser->m_sHP -= iDamage;
				if (pUser->m_sHP > pUser->GetMaxHP())
				{
					pUser->m_sHP = pUser->GetMaxHP();
				}
			}
			else
			{
				pUser->SetDamage(iDamage, -1, false);

			}

			if (pUser->m_lDeadUsed == 1)
			{
				if (IsDetecter())
				{
					pUser->m_iDeathType |= DEATH_TYPE_GUARD;
				}

				InitTarget();
				m_NpcState = NPC_STANDING;
				m_iAttackedUid = 0;

				if (m_tNpcAttType == 0) // Peaceful NPC
				{
					AttackListRemove(pUser);
				}
			}
			//pUser->m_sHP -= iDamage;
			//if (pUser->m_sHP > pUser->GetMaxHP())
			//{
			//	pUser->m_sHP = pUser->GetMaxHP();
			//}

			//if (pUser->m_sHP < 0)
			//{
			//	pUser->m_sHP = 0;
			//}

			pUser->m_sRemainMagicNo[pMagic->m_tEValue] = pMagic->m_sMid;
			pUser->m_dwRemainMagicTime[pMagic->m_tEValue] = pMagic->m_iTime;		
		}
		break;
	case MAGIC_EFFECT_CURE:
		// Removes remaining magic on target
		if (pUser->m_sRemainMagicNo[MAGIC_EFFECT_POISON] == -1)
		{
			if (!pUser->CheckRemainMagicAll())
			{
				SendMagicArrowAttackFail(pCom, iTargetId, sMagicNo);
				return;	
			}
		}
		pUser->m_sRemainMagicNo[MAGIC_EFFECT_POISON] = -1;
		pUser->InitRemainMagic(MAGIC_CLASS_ALL);
		pUser->SendUserStatus(MAGIC_CLASS_ALL);
		break;
	case MAGIC_EFFECT_DEFENSE:
	case MAGIC_EFFECT_GIGGLE:
	case MAGIC_EFFECT_WEAKEN:
	case MAGIC_EFFECT_REFLECT:
	case MAGIC_EFFECT_SLOW:
	case MAGIC_EFFECT_CONFUSION:
	case MAGIC_EFFECT_BLIND:
		pUser->m_sRemainMagicNo[pMagic->m_tEValue] = pMagic->m_sMid;
		pUser->m_dwRemainMagicTime[pMagic->m_tEValue] = GetTickCount();
		break;	
	default:
		break;
	}

	SendMagicArrowAttackSuccess(pCom, iTargetId, sMagicNo, pMagic->m_iTime, pUser->m_sHP, pUser->GetMaxHP());
}

int CNpc::GetMagicDamage(CMagicTable *pMagic, USER *pUser)
{
	if (!pMagic || !pUser)
	{
		return 0;
	}

	int iDamage = pMagic->m_sDamage;
	int iMagicExp = 0;

	switch (pMagic->m_tClass)
	{
	case MAGIC_CLASS_BLUE:
		iMagicExp = m_bmagicexp;
		break;
	case MAGIC_CLASS_WHITE:
		iMagicExp = m_wmagicexp;
		break;
	case MAGIC_CLASS_BLACK:
		iMagicExp = m_dmagicexp;
		break;
	default:
		break;
	}

	if (iDamage < 0)
	{
		iDamage += 100 + ((iMagicExp * iDamage) / 100);
	}
	else
	{
		iDamage += m_sIntel + ((iMagicExp * iDamage) / 200);
	}

	return iDamage;
}

int CNpc::GetMagicDefense(CMagicTable *pMagic, USER *pUser)
{
	if (!pMagic || !pUser)
	{
		return 0;
	}

	double dDefense = 0;
	double dMagicExp = 0;

	switch (pMagic->m_tClass)
	{
	case MAGIC_CLASS_BLUE:
		dMagicExp = static_cast<double>(pUser->m_iBMagicExp) / CLIENT_SKILL;
		break;
	case MAGIC_CLASS_WHITE:
		dMagicExp = static_cast<double>(pUser->m_iWMagicExp) / CLIENT_SKILL;
		break;
	case MAGIC_CLASS_BLACK:
		dMagicExp = static_cast<double>(pUser->m_iDMagicExp) / CLIENT_SKILL;
		break;
	default:
		break;
	}

	if (!pUser->CheckDemon(pUser->m_sClass))
	{
		dDefense += (static_cast<double>(pUser->m_iINT) / CLIENT_BASE_STATS) + pUser->PlusFromItem(SPECIAL_INT);
	}
	else
	{
		dDefense += (static_cast<double>(pUser->m_iWIS) / CLIENT_EXT_STATS) + pUser->PlusFromItem(SPECIAL_WIS);
	}

	dDefense += pUser->PlusFromItem(SPECIAL_MAG_DEF);
	dDefense += pUser->PlusFromItem(SPECIAL_MAG_DEF2);
	dDefense += dMagicExp / 10.0;

	switch (pMagic->m_tClass)
	{
	case MAGIC_CLASS_BLUE:
		dDefense += pUser->PlusFromItem(SPECIAL_BLUE_MAG_DEF);
		break;
	case MAGIC_CLASS_WHITE:
		dDefense += pUser->PlusFromItem(SPECIAL_WHITE_MAG_DEF);
		break;
	case MAGIC_CLASS_BLACK:
		dDefense += pUser->PlusFromItem(SPECIAL_BLACK_MAG_DEF);
		break;
	default:
		break;
	}

	if (pUser->CheckDemon(pUser->m_sClass))
	{
		dDefense += pUser->m_sLevel / 5.0;
	}

	pUser->DecreaseArmorItemDura();

	// CheckMinus (Checks to see if defense should be taken away because of
	// human being in a devil map or devil being in a human map etc...)
	if (pUser->CheckMinus())
	{
		dDefense = (dDefense * g_iClassWarMinus) / 100.0;
	}

	return static_cast<int>(dDefense + 0.5);
}

void CNpc::SendMagicArrowAttackFail(COM *pCom, int iTargetId, short sMagicNo)
{
	if (!pCom) return;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_ATTACK_ARROW);
	TempBuf.Add(FAIL);
	TempBuf.Add((short)0); // Direction
	TempBuf.Add((int)(m_sNid + NPC_BAND));
	TempBuf.Add(m_sHP);
	TempBuf.Add(m_sMP);
	TempBuf.Add((int)0); // Exp
	TempBuf.Add(iTargetId);
	TempBuf.Add(sMagicNo);
	TempBuf.Add((short)0);
	TempBuf.Add((short)0);
	SendInsight(pCom, TempBuf, TempBuf.GetLength());
}

void CNpc::SendMagicArrowAttackSuccess(COM *pCom, int iTargetId, short sMagicNo, DWORD dwConTime,
	short sHP, short sMaxHP)
{
	if (!pCom) return;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_ATTACK_ARROW);
	TempBuf.Add(SUCCESS);
	TempBuf.Add((short)0); // Direction
	TempBuf.Add((int)(m_sNid + NPC_BAND));
	TempBuf.Add(m_sHP);
	TempBuf.Add(m_sMP);
	TempBuf.Add((int)0); // Exp
	TempBuf.Add(iTargetId);
	TempBuf.Add(sMagicNo);
	TempBuf.Add(sHP);
	TempBuf.Add(sMaxHP);
	TempBuf.Add(dwConTime);
	SendInsight(pCom, TempBuf, TempBuf.GetLength());
}

bool CNpc::CheckDistance(USER *pUser, int iDistance)
{
	if (!pUser) return false;
	if (pUser->m_sZ != m_sZone) return false;

	// MAP* pMap = g_Zones[m_ZoneIndex];
	// if (!g_Zones[m_ZoneIndex]) return false;

	// Certain maps have walls and these need to be checked for
	// and prevent attacks through them.
	CPoint ptPos = ConvertToClient(m_sCurX, m_sCurY);
	CPoint ptPosTarget = ConvertToClient(pUser->m_sX, pUser->m_sY);
	if (!ArrowPathFind(ptPos, ptPosTarget))
	{
		return false;
	}

	if (abs(m_sCurX - pUser->m_sX) > iDistance || abs(m_sCurY - pUser->m_sY) > iDistance)
	{
		return false;
	}

	return true;
}

DWORD CNpc::CheckRemainMagic(int iMagicEffectValue)
{
	if (iMagicEffectValue >= MAX_MAGIC_EFFECT) return 0;

	short sMagicNo = m_sRemainMagicNo[iMagicEffectValue];
	if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
	{
		return 0;
	}

	CMagicTable* pMagic = g_arMagicTable[sMagicNo-1];
	if (!pMagic)
	{
		m_sRemainMagicNo[iMagicEffectValue] = -1;
		return 0;
	}

	DWORD dwDifference = GetTickCount() - m_dwRemainMagicTime[iMagicEffectValue];
	if (dwDifference >= static_cast<DWORD>(pMagic->m_iTime))
	{
		m_sRemainMagicNo[iMagicEffectValue] = -1;
		return 0;
	}

	return static_cast<DWORD>(pMagic->m_iTime) - dwDifference;
}

bool CNpc::CheckRemainMagicAll()
{
	for (int i = 0; i < MAX_MAGIC_EFFECT; i++)
	{
		if (CheckRemainMagic(i))
		{
			return true;
		}
	}
	return false;
}

void CNpc::InitRemainMagic(int iMagicClass)
{
	// Class 4 (MAGIC_CLASS_ALL) removes all remaining magic
	if (iMagicClass == MAGIC_CLASS_ALL)
	{
		for (int i = 0; i < MAX_MAGIC_EFFECT; i++)
		{
			m_sRemainMagicNo[i] = -1;
			m_dwRemainMagicTime[i] = GetTickCount();
		}
	}
	// Class 1, 2 and 3 are e.g. Blue, White and Black Detox item
	// and therefore removes those class of magic
	else if (iMagicClass == MAGIC_CLASS_BLUE || 
		iMagicClass == MAGIC_CLASS_WHITE || 
		iMagicClass == MAGIC_CLASS_BLACK)
	{
		for (int i = 0; i < MAX_MAGIC_EFFECT; i++)
		{
			short sMagicNo = m_sRemainMagicNo[i];
			if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
			{
				continue;
			}

			CMagicTable* pMagic = g_arMagicTable[sMagicNo-1];
			if (!pMagic)
			{
				continue;
			}	

			if (pMagic->m_tClass == iMagicClass && pMagic->m_sDamage > 0)
			{
				m_sRemainMagicNo[i] = -1;
				m_dwRemainMagicTime[i] = GetTickCount();
			}
		}
	}
}

void CNpc::SendNpcStatus(COM *pCom)
{
	if (!pCom) return;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_STATUS_INFO);
	TempBuf.Add((int)(m_sNid + NPC_BAND));
	if (m_dwPoisonMagicCount <= 0)
	{
		TempBuf.Add((int)0);
	}
	else
	{
		TempBuf.Add((int)1);
	}
	SendInsight(pCom, TempBuf, TempBuf.GetLength());
}

void CNpc::PoisonMagic(COM *pCom)
{
	if (!pCom) return;

	short sMagicNo = m_sPoisonMagicNo;
	if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
	{
		m_sPoisonMagicNo = -1;
		m_dwPoisonMagicCount = 0;
		return;
	}

	CMagicTable* pMagic = g_arMagicTable[sMagicNo-1];
	if (!pMagic)
	{
		m_sPoisonMagicNo = -1;
		m_dwPoisonMagicCount = 0;
		return;
	}	

	if (pMagic->m_tEValue != MAGIC_EFFECT_POISON)
	{
		m_sPoisonMagicNo = -1;
		m_dwPoisonMagicCount = 0;
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
			// TODO: implement weaken negative damage calculation
		}
	}

	m_sHP -= iDamage;
	if (m_sHP > m_sMaxHp)
	{
		m_sHP = m_sMaxHp;
	}

	if (m_sHP < 0)
	{
		m_sHP = 0;
	}

	CBufferEx TempBuf;
	TempBuf.Add(PKT_POISON);
	TempBuf.Add((int)(m_sNid + NPC_BAND));
	TempBuf.Add(m_sHP);
	TempBuf.Add(m_sPoisonMagicNo);
	TempBuf.Add(m_dwPoisonMagicCount * 1000);
	SendInsight(pCom, TempBuf, TempBuf.GetLength());

	if (m_dwPoisonMagicCount <= 0)
	{
		m_sPoisonMagicNo = -1;
	}

	SendNpcStatus(pCom);
}

DWORD CNpc::CheckRemainSpecialAttack(COM *pCom, int iSpecialEffectValue)
{
	if (iSpecialEffectValue >= MAX_SPECIAL_EFFECT) return 0;

	short sSpecialNo = m_sRemainSpecialNo[iSpecialEffectValue];
	if (sSpecialNo <= 0 || sSpecialNo > g_arSpecialAttackTable.GetSize())
	{
		m_sRemainSpecialNo[iSpecialEffectValue] = -1;
		return 0;
	}

	CSpecialTable* pSpecial = g_arSpecialAttackTable[sSpecialNo-1];
	if (!pSpecial)
	{
		m_sRemainSpecialNo[iSpecialEffectValue] = -1;
		return 0;
	}

	int specialTime = pSpecial->m_iTime;
	if (iSpecialEffectValue == SPECIAL_EFFECT_PARALYZE || 
		iSpecialEffectValue == SPECIAL_EFFECT_CONCUSSION)
	{
		specialTime = m_sCon * 1000;
	}

	DWORD dwDifference = GetTickCount() - m_dwRemainSpecialTime[iSpecialEffectValue];
	if (dwDifference >= static_cast<DWORD>(specialTime))
	{
		m_sRemainSpecialNo[iSpecialEffectValue] = -1;
		SendSpecialAttackCancel(pCom);
		return 0;
	}

	return static_cast<DWORD>(pSpecial->m_iTime) - dwDifference;
}

void CNpc::InitRemainSpecial()
{
	for (int i = 0; i < MAX_SPECIAL_EFFECT; i++)
	{
		m_sRemainSpecialNo[i] = -1;
		m_dwRemainSpecialTime[i] = GetTickCount();
	}
}

void CNpc::SendSpecialAttackCancel(COM *pCom)
{
	CBufferEx TempBuf;
	TempBuf.Add(PKT_SPECIALATTACKCANCEL);
	TempBuf.Add((int)(m_sNid + NPC_BAND));
	SendInsight(pCom, TempBuf, TempBuf.GetLength());	
}


void CNpc::SendSpecialAttackArrow(COM* pCom, BYTE bySuccess, BYTE byDir, short sSpecialNo, int iContinueTime, 
	int iTargetId, short sTargetHP)
{
	if (!pCom) return;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_ATTACK_SPECIALARROW);
	TempBuf.Add(bySuccess);
	TempBuf.Add((int)(m_sNid + NPC_BAND));
	if (bySuccess == SUCCESS)
	{
		TempBuf.Add(m_sHP);
		TempBuf.Add(m_sMP);
		TempBuf.Add(static_cast<short>(0));
		TempBuf.Add(static_cast<short>(byDir));
		TempBuf.Add(sSpecialNo);
		TempBuf.Add(iContinueTime);
		TempBuf.Add(iTargetId);
		TempBuf.Add(sTargetHP);
		TempBuf.Add(static_cast<short>(-1));
		TempBuf.Add(static_cast<short>(-1));
	}
	SendInsight(pCom, TempBuf, TempBuf.GetLength());
}
