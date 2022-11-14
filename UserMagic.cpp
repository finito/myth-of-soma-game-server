// UserMagic.cpp: implementation of the magic part of USER class.
//
//////////////////////////////////////////////////////////////////////

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

void USER::SendCharMagicData()
{
	CBufferEx MagicBuf, SpecialBuf, AbilityBuf;
	CBufferEx MagicBuf2, SpecialBuf2, AbilityBuf2;
	int magic_count = 0, special_count = 0, ability_count = 0, i;

	MagicBuf.Add(PKT_MAGIC_ALL);
	MagicBuf.Add((BYTE)1);
	SpecialBuf.Add(PKT_MAGIC_ALL);
	SpecialBuf.Add((BYTE)2);
	AbilityBuf.Add(PKT_MAGIC_ALL);
	AbilityBuf.Add((BYTE)3);

	for (i = 0; i < MAX_MAGIC_NUM; i++)
	{
		if (m_UserMagic[i].sMid == -1) break;

		switch (m_UserMagic[i].byType)
		{
		case MAGIC_TYPE_MAGIC:
			magic_count++;
			break;
		case MAGIC_TYPE_SPECIAL:
			special_count++;
			break;
		case MAGIC_TYPE_ABILITY:
			ability_count++;
			break;
		}
	}

	MagicBuf.Add((short)magic_count);
	SpecialBuf.Add((short)special_count);
	AbilityBuf.Add((short)ability_count);

	MagicBuf2.Add((short)magic_count);
	SpecialBuf2.Add((short)special_count);
	AbilityBuf2.Add((short)ability_count);

	for (i = 0; i < MAX_MAGIC_NUM; i++)
	{
		if (m_UserMagic[i].sMid == -1) break;

		switch (m_UserMagic[i].byType)
		{
		case MAGIC_TYPE_MAGIC:
			{
				MagicBuf2.Add((short)m_UserMagic[i].sMid);
				MagicBuf.AddData(m_TempBuf, GetSendMagicData(m_UserMagic[i].sMid, m_TempBuf));
			}
			break;
		case MAGIC_TYPE_SPECIAL:
			{
				SpecialBuf2.Add((short)m_UserMagic[i].sMid);
				SpecialBuf.AddData(m_TempBuf, GetSendSpecialAttackData(m_UserMagic[i].sMid, m_TempBuf));
			}
			break;
		case MAGIC_TYPE_ABILITY:
			{
				AbilityBuf2.Add((short)m_UserMagic[i].sMid);
				AbilityBuf.AddData(m_TempBuf, GetSendAbilityData(m_UserMagic[i].sMid, m_TempBuf));
			}
			break;
		}
	}
	MagicBuf.AddData((LPTSTR)MagicBuf2, MagicBuf2.GetLength());
	SpecialBuf.AddData((LPTSTR)SpecialBuf2, SpecialBuf2.GetLength());
	AbilityBuf.AddData((LPTSTR)AbilityBuf2, AbilityBuf2.GetLength());
	CBufferEx BeltBuffer;
	for (i = 0; i < MAGIC_BELT_SIZE; i++)
	{
		BeltBuffer.Add((BYTE)m_UserMagicBelt[i].byType);
		BeltBuffer.Add((short)m_UserMagicBelt[i].sMid);
	}
	MagicBuf.AddData((LPTSTR)BeltBuffer, BeltBuffer.GetLength());
	SpecialBuf.AddData((LPTSTR)BeltBuffer, BeltBuffer.GetLength());
	AbilityBuf.AddData((LPTSTR)BeltBuffer, BeltBuffer.GetLength());

	Send(MagicBuf, MagicBuf.GetLength());
	Send(SpecialBuf, SpecialBuf.GetLength());
//	Send(AbilityBuf, AbilityBuf.GetLength()); TODO: Put back when packet data is fixed...
}

int USER::GetSendMagicData(int iMid, TCHAR* pData)
{
	int index = 0;
	if (iMid <= 0 || iMid > g_arMagicTable.GetSize()) return index;
	CMagicTable* pMagic = g_arMagicTable[iMid-1];

	SetShort(pData, pMagic->m_sMid, index);
	SetByte(pData, pMagic->m_tType01, index);
	SetByte(pData, pMagic->m_tType02, index);
	SetShort(pData, pMagic->m_sStartTime, index);
	SetByte(pData, pMagic->m_tClass, index);
	SetByte(pData, pMagic->m_tTarget, index);
	SetVarString(pData, (LPTSTR)(LPCTSTR)pMagic->m_strName, pMagic->m_strName.GetLength(), index);
	SetVarString(pData, (LPTSTR)(LPCTSTR)pMagic->m_strInform, pMagic->m_strInform.GetLength(), index);

	SetShort(pData, pMagic->m_sEDist, index);
	SetShort(pData, pMagic->m_sMpdec, index);
	SetInt(pData, pMagic->m_iTime, index);
	SetShort(pData, pMagic->m_sDamage, index);
	SetShort(pData, pMagic->m_sMinMxp, index);
	SetShort(pData, pMagic->m_sMinInt, index);
	SetShort(pData, pMagic->m_sMinLevel, index); // Should be sMinLevel
	SetVarString(pData, (LPTSTR)(LPCTSTR)pMagic->m_strGetInfo, pMagic->m_strGetInfo.GetLength(), index);
	//SetShort(pData, 0, index); // SetVarString(pData, (LPTSTR)(LPCTSTR)pMagic->m_strInform, pMagic->m_strInform.GetLength(), index);
	return index;
}

int USER::GetSendSpecialAttackData(int iMid, TCHAR* pData)
{
	int index = 0;
	if (iMid < 0 || iMid > g_arSpecialAttackTable.GetSize()) return index;
	CSpecialTable* pSpecial = g_arSpecialAttackTable[iMid-1];

	SetShort(pData, pSpecial->m_sMid, index);
	SetByte(pData, pSpecial->m_tType, index);
	SetVarString(pData, (LPTSTR)(LPCTSTR)pSpecial->m_strName, pSpecial->m_strName.GetLength(), index);
	SetVarString(pData, (LPTSTR)(LPCTSTR)pSpecial->m_strInform, pSpecial->m_strInform.GetLength(), index);
	SetShort(pData, pSpecial->m_sEDist, index);
	SetShort(pData, pSpecial->m_sHpdec, index);
	SetShort(pData, pSpecial->m_sMpdec, index);
	SetShort(pData, pSpecial->m_sStmdec, index);
	SetInt(pData, pSpecial->m_iTime, index);
	SetInt(pData, pSpecial->m_iTerm, index);
	SetShort(pData, pSpecial->m_sDamage, index);
	SetShort(pData, pSpecial->m_sMinExp, index);
	SetByte(pData, pSpecial->m_tClass, index);
	SetInt(pData, pSpecial->m_iDecTerm, index);
	SetShort(pData, pSpecial->m_sTHp, index);
	SetShort(pData, pSpecial->m_sTMp, index);
	SetVarString(pData, (LPTSTR)(LPCTSTR)pSpecial->m_strGetInfo, pSpecial->m_strGetInfo.GetLength(), index);
	//SetShort(pData, 0, index); // SetVarString(pData, (LPTSTR)(LPCTSTR)pSpecial->m_strInform, pSpecial->m_strInform.GetLength(), index);
	return index;
}

int USER::GetSendAbilityData(int iMid, TCHAR* pData)
{
	int index = 0;
	if (iMid < 0 || iMid > g_arMakeSkillTable.GetSize()) return index;
	CMakeSkillTable* pNewMakeSkill = g_arMakeSkillTable[iMid-1];

	SetShort(pData, pNewMakeSkill->m_sId, index);
	SetByte(pData, (BYTE)pNewMakeSkill->m_sType, index);
	SetVarString(pData, (LPTSTR)(LPCTSTR)pNewMakeSkill->m_strName, pNewMakeSkill->m_strName.GetLength(), index);
	SetVarString(pData, (LPTSTR)(LPCTSTR)pNewMakeSkill->m_strInform, pNewMakeSkill->m_strInform.GetLength(), index);
	SetShort(pData, pNewMakeSkill->m_sMinExp, index);
	SetShort(pData, 0, index); // No Idea...
	SetShort(pData, 2, index); // Skill Use Left..
	SetShort(pData, 2, index); // Skill Use Max...
	return index;
}

void USER::ParseHaveMagicData(TCHAR *pBuf)
{
	if (pBuf[0] == 'N' && 
		pBuf[1] == 'E' &&
		pBuf[2] == 'W')
	{
		m_bNewUser = true;
		return;
	}

	int index = 0, i;
	short mid;
	m_nHaveMagicNum = 0;

	for (i = 0; i < 4; i++) // first part of magic belt
	{
		m_UserMagicBelt[i].sMid = mid = GetShort(pBuf, index);
		if (mid < 1000)
			m_UserMagicBelt[i].byType = MAGIC_TYPE_MAGIC; // Magics
		else if (mid > 1000 && mid < 2000)
		{
			m_UserMagicBelt[i].sMid-= 1000;
			m_UserMagicBelt[i].byType = MAGIC_TYPE_SPECIAL; // Specials
		}
		else
		{
			m_UserMagicBelt[i].sMid-= 2000;
			m_UserMagicBelt[i].byType = MAGIC_TYPE_ABILITY; // Ability
		}
	}

	short count = GetShort(pBuf, index);
	if (count > MAX_MAGIC_NUM)
	{
		TRACE("User %s has more than max magic\n", m_strUserId);
		count = MAX_MAGIC_NUM;
	}
	for (i = 0; i < count; i++)
	{
		mid = GetShort(pBuf, index);
		if (mid != -1)
		{
			if (mid < 1000)
			{
				m_UserMagic[i].sMid = mid;
				m_UserMagic[i].byType = MAGIC_TYPE_MAGIC; // Magics
			}
			else if (mid > 1000 && mid < 2000)
			{
				m_UserMagic[i].sMid = mid - 1000;
				m_UserMagic[i].byType = MAGIC_TYPE_SPECIAL; // Specials
			}
			else
			{
				m_UserMagic[i].sMid-= 2000;
				m_UserMagic[i].byType = MAGIC_TYPE_ABILITY; // Ability
			}
			m_nHaveMagicNum++;
		}
	}
	for (i = 4; i < MAGIC_BELT_SIZE; i++)
	{
		m_UserMagicBelt[i].sMid = mid = GetShort(pBuf, index);
		if (mid < 1000)
			m_UserMagicBelt[i].byType = MAGIC_TYPE_MAGIC; // Magics
		else if (mid > 1000 && mid < 2000)
		{
			m_UserMagicBelt[i].sMid-= 1000;
			m_UserMagicBelt[i].byType = MAGIC_TYPE_SPECIAL; // Specials
		}
		else
		{
			m_UserMagicBelt[i].sMid-= 2000;
			m_UserMagicBelt[i].byType = MAGIC_TYPE_ABILITY; // Ability
		}
	}
}

void USER::FillMagicData(TCHAR* pBuf)
{
	int index = 0;

	// Magic Belt First 4
	for (int i = 0; i < 4; i++)
	{
		if (m_UserMagicBelt[i].byType == MAGIC_TYPE_MAGIC)
		{
			SetShort(pBuf, m_UserMagicBelt[i].sMid, index);
		}
		else if (m_UserMagicBelt[i].byType == MAGIC_TYPE_SPECIAL)
		{
			SetShort(pBuf, m_UserMagicBelt[i].sMid+1000, index);
		}
		else if (m_UserMagicBelt[i].byType == MAGIC_TYPE_ABILITY)
		{
			SetShort(pBuf, m_UserMagicBelt[i].sMid+2000, index);
		}
	}

	if (m_nHaveMagicNum > MAX_MAGIC_NUM)
	{
		TRACE("User %s had more than max magic!!\n", m_strUserId);
		m_nHaveMagicNum = MAX_MAGIC_NUM;
	}

	// Magic
	SetShort(pBuf, m_nHaveMagicNum, index);
	for (int i = 0; i <	MAX_MAGIC_NUM; i++)
	{
		if (m_UserMagic[i].sMid != -1)
		{
			if (m_UserMagic[i].byType == MAGIC_TYPE_MAGIC)
			{
				SetShort(pBuf, m_UserMagic[i].sMid, index);
			}
			else if (m_UserMagic[i].byType == MAGIC_TYPE_SPECIAL)
			{
				SetShort(pBuf, m_UserMagic[i].sMid+1000, index);
			}
			else if (m_UserMagic[i].byType == MAGIC_TYPE_ABILITY)
			{
				SetShort(pBuf, m_UserMagic[i].sMid+2000, index);
			}
		}
	}

	// Magic Belt Second (8)
	for (int i = 4; i < MAGIC_BELT_SIZE; i++)
	{
		if (m_UserMagicBelt[i].byType == MAGIC_TYPE_MAGIC)
		{
			SetShort(pBuf, m_UserMagicBelt[i].sMid, index);
		}
		else if (m_UserMagicBelt[i].byType == MAGIC_TYPE_SPECIAL)
		{
			SetShort(pBuf, m_UserMagicBelt[i].sMid+1000, index);
		}
		else if (m_UserMagicBelt[i].byType == MAGIC_TYPE_ABILITY)
		{
			SetShort(pBuf, m_UserMagicBelt[i].sMid+2000, index);
		}
	}
}

bool USER::IsHaveMagic(short sMid, BYTE byMagicType)
{
	for (int i = 0; i < MAX_MAGIC_NUM; i++)
	{
		if (m_UserMagic[i].sMid == sMid  && m_UserMagic[i].byType == byMagicType)
		{
			return true;
		}
	}
	return false;
}

bool USER::IsHaveMagicMageType(int iMageType)
{
	for (int i = 0; i < MAX_MAGIC_NUM; i++)
	{
		if (m_UserMagic[i].sMid >= 1 && 
			m_UserMagic[i].sMid < 1000 && 
			m_UserMagic[i].byType == MAGIC_TYPE_MAGIC &&
			g_arMagicTable[m_UserMagic[i].sMid-1]->m_tClass == iMageType)
		{
			return true;
		}
	}
	return false;
}

DWORD USER::CheckRemainMagic(int iMagicEffectValue)
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

bool USER::CheckRemainMagicAll()
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

void USER::InitRemainMagic(int iMagicClass)
{
	// Class 4 removes all remaining magic
	if (iMagicClass == MAGIC_CLASS_ALL)
	{
		for (int i = 0; i < MAX_MAGIC_EFFECT; i++)
		{
			m_sRemainMagicNo[i] = -1;
			m_dwRemainMagicTime[i] = GetTickCount();
		}
	}
	// Class 1, 2 and 3 are Blue, White and Black Detox item
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

void USER::InitRemainSpecial()
{
	for (int i = 0; i < MAX_SPECIAL_EFFECT; i++)
	{
		m_RemainSpecial[i].sMid = -1;
		m_RemainSpecial[i].sDamage = 0;
		m_RemainSpecial[i].dwTime = GetTickCount();
	}
}

bool USER::CheckMagicDelay(CMagicTable* pMagic)
{
	if (!pMagic) return false;

	DWORD dwDelay = pMagic->m_sStartTime;
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_RAPIDCAST))
	{
		dwDelay -= (dwDelay * m_RemainSpecial[SPECIAL_EFFECT_RAPIDCAST].sDamage) / 100;
	}

	CString str;
	str.Format(IDS_GM_MSG_ATTACK_DELAY, dwDelay);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);	

	DWORD dwCurrTime = GetTickCount();
	if ((dwCurrTime - m_dwMagicCastedTime) < dwDelay)
	{
		return false;
	}

	return true;
}

int USER::GetMagicDamage(CMagicTable* pMagic, CNpc* pNpc)
{
	if (!pMagic || !pNpc) return 0;

	double dMagicExp = 0;
	switch (pMagic->m_tClass)
	{
	case MAGIC_CLASS_BLUE:
		dMagicExp = static_cast<double>(m_iBMagicExp) / CLIENT_SKILL;
		break;
	case MAGIC_CLASS_WHITE:
		dMagicExp = static_cast<double>(m_iWMagicExp) / CLIENT_SKILL;
		break;
	case MAGIC_CLASS_BLACK:
		dMagicExp = static_cast<double>(m_iDMagicExp) / CLIENT_SKILL;
		break;
	default:
		break;
	}
	
	int iDamage = pMagic->m_sDamage;	
	if (iDamage < 0)
	{
		iDamage += static_cast<int>(100 + ((dMagicExp * iDamage) / 100) - 0.5);
		return iDamage;
	}

	iDamage += static_cast<int>(
		((static_cast<double>(m_iINT) / CLIENT_BASE_STATS) + PlusFromItem(SPECIAL_INT)) +
		((dMagicExp * pMagic->m_sDamage) / 200) + 
		((static_cast<double>(m_iStaffExp) * pMagic->m_sDamage) / 80000000) + 0.5);

	iDamage += PlusFromItem(SPECIAL_MAG_ATT);
	iDamage += PlusFromItem(SPECIAL_MAG_ATTK2);

	switch (pMagic->m_tClass)
	{
	case MAGIC_CLASS_BLUE:
		iDamage += PlusFromItem(SPECIAL_BLUE_MAG_ATT);
		break;
	case MAGIC_CLASS_WHITE:
		iDamage += PlusFromItem(SPECIAL_WHITE_MAG_ATT);
		break;
	case MAGIC_CLASS_BLACK:
		iDamage += PlusFromItem(SPECIAL_BLACK_MAG_ATT);
		break;
	default:
		break;
	}

	// CheckMinus (Checks to see if attack should be taken away because of
	// human being in a devil map or devil being in a human map etc...)
	if (CheckMinus())
	{
		iDamage = (iDamage * g_iClassWarMinus) / 100;
	}

	CString str;
	str.Format(IDS_GM_MSG_MAGIC_AT, iDamage);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

	return iDamage;
}

int USER::GetMagicDamage(CMagicTable* pMagic, USER* pUser)
{
	if (!pMagic || !pUser) return 0;

	double dMagicExp = 0;
	switch (pMagic->m_tClass)
	{
	case MAGIC_CLASS_BLUE:
		dMagicExp = static_cast<double>(m_iBMagicExp) / CLIENT_SKILL;
		break;
	case MAGIC_CLASS_WHITE:
		dMagicExp = static_cast<double>(m_iWMagicExp) / CLIENT_SKILL;
		break;
	case MAGIC_CLASS_BLACK:
		dMagicExp = static_cast<double>(m_iDMagicExp) / CLIENT_SKILL;
		break;
	default:
		break;
	}
	
	int iDamage = pMagic->m_sDamage;	
	if (iDamage < 0)
	{
		iDamage = static_cast<int>((((100 + dMagicExp) * iDamage) / 100) - 0.5);
		return iDamage;
	}

	iDamage = static_cast<int>(
		((static_cast<double>(m_iINT) / CLIENT_BASE_STATS) + PlusFromItem(SPECIAL_INT)) +
		((dMagicExp * pMagic->m_sDamage) / 200) + 
		((static_cast<double>(m_iStaffExp) * pMagic->m_sDamage) / 80000000) + 0.5);

	iDamage += PlusFromItem(SPECIAL_MAG_ATT);
	iDamage += PlusFromItem(SPECIAL_MAG_ATTK2);

	switch (pMagic->m_tClass)
	{
	case MAGIC_CLASS_BLUE:
		iDamage += PlusFromItem(SPECIAL_BLUE_MAG_ATT);
		break;
	case MAGIC_CLASS_WHITE:
		iDamage += PlusFromItem(SPECIAL_WHITE_MAG_ATT);
		break;
	case MAGIC_CLASS_BLACK:
		iDamage += PlusFromItem(SPECIAL_BLACK_MAG_ATT);
		break;
	default:
		break;
	}

	// Guild Town War give extra attack to those who own the town or are an ally to the town owner
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
			iDamage += 2;
		}

		if (CheckGuildTownAlly(sTownNum))
		{
			iDamage += 1;
		}
	}
	// CheckMinus (Checks to see if attack should be taken away because of
	// human being in a devil map or devil being in a human map etc...)
	if (CheckMinus())
	{
		iDamage = (iDamage * g_iClassWarMinus) / 100;
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
		iDamage = (iDamage * iReduce) / 100;
	}

	CString str;
	str.Format(IDS_GM_MSG_MAGIC_AT, iDamage);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

	return iDamage;
}

int USER::GetMagicDefense(CMagicTable* pMagic, CNpc* pNpc)
{	
	if (!pMagic || !pNpc) return 0;

	int iMagicExp = 0;
	switch (pMagic->m_tClass)
	{
	case MAGIC_CLASS_BLUE:
		iMagicExp = pNpc->m_bmagicexp;
		break;
	case MAGIC_CLASS_WHITE:
		iMagicExp = pNpc->m_wmagicexp;
		break;
	case MAGIC_CLASS_BLACK:
		iMagicExp = pNpc->m_dmagicexp;
		break;
	default:
		break;
	}

	int iDefense = static_cast<int>(((static_cast<double>(iMagicExp) / 10) + pNpc->m_sIntel) + 0.5);

	CString str;
	str.Format(IDS_GM_MSG_MAGIC_DEF, iDefense);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

	return iDefense;
}

int USER::GetMagicDefense(CMagicTable* pMagic, USER* pUser)
{	
	if (!pMagic || !pUser) return 0;

	double dMagicExp = 0;
	switch (pMagic->m_tClass)
	{
	case MAGIC_CLASS_BLUE:
		dMagicExp = static_cast<double>(m_iBMagicExp) / CLIENT_SKILL;
		break;
	case MAGIC_CLASS_WHITE:
		dMagicExp = static_cast<double>(m_iWMagicExp) / CLIENT_SKILL;
		break;
	case MAGIC_CLASS_BLACK:
		dMagicExp = static_cast<double>(m_iDMagicExp) / CLIENT_SKILL;
		break;
	default:
		break;
	}

	double dDefense = 0;
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

	// Guild Town War give extra defense to those who own the town or are an ally to the town owner
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
			dDefense += 2;
		}

		if (CheckGuildTownAlly(sTownNum))
		{
			dDefense += 1;
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

	int iDefense = static_cast<int>(dDefense + 0.5);

	CString str;
	str.Format(IDS_GM_MSG_MAGIC_DEF, iDefense);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);

	return iDefense;
}

void USER::SendMagicReady(BYTE bySuccess, int iTargetId, short sMagicNo, BYTE byDir, DWORD dwCastDelay)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_MAGIC_READY, index);
	SetByte(m_TempBuf, bySuccess, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	SetInt(m_TempBuf, iTargetId, index);
	SetShort(m_TempBuf, static_cast<short>(m_byDir), index);
	SetShort(m_TempBuf, sMagicNo, index);
	SetByte(m_TempBuf, byDir, index);
	SetDWORD(m_TempBuf, dwCastDelay, index);
	SendInsight(m_TempBuf, index);
}

void USER::SendMagicAttack(BYTE bySuccess, int iTargetId, short sMagicNo, short sTargetHP, short sTargetMaxHP, int iContinueTime)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_ATTACK_ARROW, index);
	SetByte(m_TempBuf, bySuccess, index);
	SetShort(m_TempBuf, static_cast<short>(m_byDir), index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	SetShort(m_TempBuf, m_sHP, index);
	SetShort(m_TempBuf, m_sMP, index);
	SetDWORD(m_TempBuf, m_dwExp / 100, index);
	SetInt(m_TempBuf, iTargetId, index);
	SetShort(m_TempBuf, sMagicNo, index);
	SetShort(m_TempBuf, sTargetHP, index);
	SetShort(m_TempBuf, sTargetMaxHP, index);
	SetDWORD(m_TempBuf, iContinueTime, index);
	SendInsight(m_TempBuf, index);
}

void USER::SendMagicRailAttack(BYTE bySuccess, BYTE byDistance, short sTargetCount, int* iTargetsId, short sMagicNo, short* sTargetsHP, short* sTargetsMaxHP, int iContinueTime)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_ATTACK_MAGIC_RAIL, index);
	SetByte(m_TempBuf, bySuccess, index);
	SetShort(m_TempBuf, static_cast<short>(m_byDir), index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	SetShort(m_TempBuf, m_sHP, index);
	SetShort(m_TempBuf, m_sMP, index);
	SetDWORD(m_TempBuf, m_dwExp / 100, index);
	SetShort(m_TempBuf, sMagicNo, index);
	SetByte(m_TempBuf, byDistance, index);
	SetByte(m_TempBuf, 0, index);
	SetShort(m_TempBuf, sTargetCount, index);
	for (int i = 0; i < sTargetCount; i++)
	{
		SetInt(m_TempBuf, iTargetsId[i], index);
		SetShort(m_TempBuf, sTargetsHP[i], index);
		SetShort(m_TempBuf, sTargetsMaxHP[i], index);
	}
	SetDWORD(m_TempBuf, iContinueTime, index);
	SendInsight(m_TempBuf, index);
}

void USER::SendMagicCircleAttack(BYTE bySuccess, BYTE byDistance, short sTargetCount, int* iTargetsId, short sMagicNo, short* sTargetsHP, short* sTargetsMaxHP, int iContinueTime)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_MAGIC_CIRCLE, index);
	SetByte(m_TempBuf, bySuccess, index);
	SetShort(m_TempBuf, static_cast<short>(m_byDir), index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	SetShort(m_TempBuf, m_sHP, index);
	SetShort(m_TempBuf, m_sMP, index);
	SetDWORD(m_TempBuf, m_dwExp / 100, index);
	SetShort(m_TempBuf, sMagicNo, index);
	SetByte(m_TempBuf, byDistance, index);
	SetShort(m_TempBuf, sTargetCount, index);
	for (int i = 0; i < sTargetCount; i++)
	{
		SetInt(m_TempBuf, iTargetsId[i], index);
		SetShort(m_TempBuf, sTargetsHP[i], index);
		SetShort(m_TempBuf, sTargetsMaxHP[i], index);
	}
	SetDWORD(m_TempBuf, iContinueTime, index);
	SendInsight(m_TempBuf, index);
}

bool USER::CheckPlusType(CMagicTable* pMagic, int iType)
{
	if (!pMagic) return false;

	// PlusType of 0 returns true for all
	int iPlusType = pMagic->m_sPlusType;
	if (iPlusType == 0)
	{
		return true;
	}
	
	if (iType == 4 && (iPlusType == 1 || iPlusType == 2 || iPlusType == 4))
	{
		return true;
	}

	if (iType == 5 && (iPlusType == 2 || iPlusType == 3 || iPlusType == 5))
	{
		return true;
	}

	if (iType == 6 && (iPlusType == 1 || iPlusType == 3 || iPlusType == 6))
	{
		return true;
	}

	return false;
}

int USER::DoMagicEffect(CMagicTable* pMagic, CNpc* pNpc)
{
	if (!pNpc) return 0;

	int iDamage = 0;
	switch (pMagic->m_tEValue)
	{
	case MAGIC_EFFECT_NORMAL: // Normal Attack Magic
		{
		iDamage = GetMagicDamage(pMagic, pNpc);

		int iDefense = 0;
		if (iDamage > 1)
		{
			iDefense = GetMagicDefense(pMagic, pNpc);
			iDamage -= iDefense;
			if (iDamage < 1)
			{
				iDamage = 1;
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
		}

		// Check for weaken and multiply the damage
		if (pNpc->CheckRemainMagic(MAGIC_EFFECT_WEAKEN))
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
		
		if (iDamage > pNpc->m_sHP)
		{
			iDamage = pNpc->m_sHP;
		}
		pNpc->m_sHP -= iDamage;

		if (pNpc->CheckAIType(NPCAITYPE_DUMMY)) // Dummy
		{
			pNpc->m_sHP = pNpc->m_sMaxHp;
			SendDummyAttack(pNpc->m_sNid + NPC_BAND, iDamage);
		}

		if (pNpc->m_sHP > pNpc->m_sMaxHp)
		{
			pNpc->m_sHP = pNpc->m_sMaxHp;
		}

		DWORD dwGainedExp = static_cast<DWORD>(((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) * 100.0);
		m_dwExp += dwGainedExp;

		CString str;
		str.Format(IDS_GM_MSG_ATTACK_INFO, iDamage, dwGainedExp);
		SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);
		
		if (m_dwExp > m_dwMaxExp)
		{
			CheckLevelUp();
		}
		}
		break;
	case MAGIC_EFFECT_POISON:
		{
		// TODO: 
		//iDamage = GetMagicDamage(pMagic, pNpc);

		//int iDefense = 0;
		//if (iDamage > 1)
		//{
		//	iDefense = GetMagicDefense(pMagic, pNpc);
		//	iDamage -= iDefense;
		//	if (iDamage < 1)
		//	{
		//		iDamage = 1;
		//	}

		//	// Caps the magic damage
		//	// e.g. DragonBreath caps at 100 this is because
		//	// sDamage in magic table is 36
		//	// therefore 36 * 280 = 10080 / 100 = 100
		//	int iTemp = (pMagic->m_sDamage * 280) / 100;
		//	if (iDamage > iTemp)
		//	{
		//		iDamage = iTemp;
		//	}
		//}

		//// Check for weaken and multiply the damage
		//if (pNpc->CheckRemainMagic(MAGIC_EFFECT_WEAKEN))
		//{
		//	if (iDamage >= 0)
		//	{
		//		iDamage *= 2;
		//	}
		//	else
		//	{	
		//		// todo: implement weaken negative damage calc
		//	}
		//}
		//
		//if (iDamage > pNpc->m_sHP)
		//{
		//	iDamage = pNpc->m_sHP;
		//}
		//pNpc->m_sHP -= iDamage;

		//if (pNpc->CheckAIType(NPCAITYPE_DUMMY)) // Dummy
		//{
		//	pNpc->m_sHP = pNpc->m_sMaxHp;
		//	SendDummyAttack(pNpc->m_sNid + NPC_BAND, iDamage);
		//}

		//if (pNpc->m_sHP > pNpc->m_sMaxHp)
		//{
		//	pNpc->m_sHP = pNpc->m_sMaxHp;
		//}

		//DWORD dwGainedExp = static_cast<DWORD>(((static_cast<double>(iDamage) / pNpc->m_sMaxHp) * pNpc->m_iMaxExp) * 100.0);
		//m_dwExp += dwGainedExp;

		//CString str;
		//str.Format(IDS_GM_MSG_ATTACK_INFO, iDamage, dwGainedExp);
		//SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);
		//
		//if (m_dwExp > m_dwMaxExp)
		//{
		//	CheckLevelUp();
		//}

		//pNpc->m_dwPoisonMagicCount = pMagic->m_iTime / 1000;
		//pNpc->m_sPoisonMagicNo = pMagic->m_sMid;
		}
		break;
	case MAGIC_EFFECT_CURE:
		// Removes remaining magic on target
		if (pNpc->m_sPoisonMagicNo == -1 && pNpc->m_dwPoisonMagicCount == 0)
		{
			if (!pNpc->CheckRemainMagicAll())
			{
				switch (pMagic->m_tType01)
				{
				case MAGIC_TYPE_ARROW:
					SendMagicAttack(FAIL, pNpc->m_sNid + NPC_BAND);
					break;
				case MAGIC_TYPE_RAIL:
					SendMagicRailAttack(FAIL);
					break;
				}
				return iDamage;	
			}
		}
		pNpc->m_sPoisonMagicNo = -1;
		pNpc->m_dwPoisonMagicCount = 0;
		pNpc->InitRemainMagic(MAGIC_CLASS_ALL);
		pNpc->SendNpcStatus(m_pCom);
		break;
	case MAGIC_EFFECT_GIGGLE:
	case MAGIC_EFFECT_WEAKEN:
	case MAGIC_EFFECT_REFLECT:
	case MAGIC_EFFECT_SLOW:
	case MAGIC_EFFECT_CONFUSION:
	case MAGIC_EFFECT_BLIND:
		pNpc->m_sRemainMagicNo[pMagic->m_tEValue] = pMagic->m_sMid;
		pNpc->m_dwRemainMagicTime[pMagic->m_tEValue] = GetTickCount();
		break;	
	default:
		break;
	}

	return iDamage;
}

int USER::DoMagicEffect(CMagicTable* pMagic, USER* pUser)
{
	if (!pUser) return 0;

	// Reflect the magic if target has reflection casted
	if (pUser->CheckRemainMagic(MAGIC_EFFECT_REFLECT))
	{
		pUser = this;
	}	

	int iDamage = 0;

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

				// Caps the magic damage
				// e.g. DragonBreath caps at 100 this is because
				// sDamage in magic table is 36
				// therefore 36 * 280 = 10080 / 100 = 100
				int iCap = 280;
				if (CheckDemon(m_sClass) == CheckDemon(pUser->m_sClass))
				{
					iCap = 250;
				}
				int iTemp = (pMagic->m_sDamage * iCap) / 100;
				if (iDamage > iTemp)
				{
					iDamage = iTemp;
				}
			}
			else
			{
				// Makes the user become gray if they heal someone who is gray
				if (pUser->m_iGrayCount > 0)
				{
					SetGrayUser(GRAY_MODE_NORMAL);
				}
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
					// todo: implement weaken negative damage calc
				}
			}

			// If either of users are not within the arena 
			// and the magic is healing (negative damage) then
			// stats can be gained!
			if ((!IsMapRange(MAP_AREA_DUEL) ||
				!pUser->IsMapRange(MAP_AREA_DUEL)) &&
				pMagic->m_sDamage < 0)
			{
				bool bHasStaff = false;
				if (!m_InvItem[ARM_RHAND].IsEmpty() &&
					m_InvItem[ARM_RHAND].bType == TYPE_STAFF)
				{
					bHasStaff = true;
				}

				if ((m_Uid == pUser->m_Uid && CheckPlusType(pMagic, 4)) || 
					(m_Uid != pUser->m_Uid && CheckPlusType(pMagic, 5)))
				{
					if (pUser->m_sHP < pUser->GetMaxHP())
					{
						SetMagicExp(pMagic);

						// Increase Staff Weapon Skill Exp
						if (bHasStaff)
						{

							int iStaffInc = GetInc(INC_STAFF);
							PlusWeaponExp(6, iStaffInc);
						}

						// GetInc functions to get the inc values for
						// str, dex, int, wis and con.
						// str, dex and con only increases for dsoma characters
						int iSTRInc = GetInc(INC_STR);
						int iDEXInc = GetInc(INC_DEX);
						int iINTInc = GetInc(INC_INT);
						int iWISInc = GetInc(INC_WIS);
						int iCONInc = GetInc(INC_CON);
						double dSTRIncRate = 0;
						double dDEXIncRate = 0;
						double dINTIncRate = 1;
						double dWISIncRate = 1;
						double dCONIncRate = 0;

						// Dsoma characters can gain str, dex and con
						// when using magic in here we set the rates
						if (CheckDemon(m_sClass))
						{
							dSTRIncRate = 0.2;
							if (pMagic->m_tType02 == 1)
							{
								dDEXIncRate = 0.4;
								dINTIncRate = 1.3;
								dWISIncRate = 0.8;
								dCONIncRate = 0.3;
							}
							else
							{
								dDEXIncRate = 0.3;
								dINTIncRate = 0.8;
								dWISIncRate = 1.3;
								dCONIncRate = 0.4;
							}
						}

						iSTRInc = static_cast<int>(iSTRInc * dSTRIncRate);
						iDEXInc = static_cast<int>(iDEXInc * dDEXIncRate);
						iINTInc = static_cast<int>(iINTInc * dINTIncRate);
						iWISInc = static_cast<int>(iWISInc * dWISIncRate);
						iCONInc = static_cast<int>(iCONInc * dCONIncRate);

						// Increase following stats by their inc value 
						// STR, DEX, INT and WIS (and CON if dsoma)	
						if (CheckPlusValueByHeal(pMagic->m_sDamage, iDamage))
						{
							if (!CheckDemon(m_sClass))
							{
								PlusStr(iSTRInc);
								PlusDex(iDEXInc);
								PlusInt(iINTInc);
								PlusWis(iWISInc);
								PlusCon(iCONInc);
							}

							m_iSTRInc += iSTRInc;
							m_iDEXInc += iDEXInc;
							m_iINTInc += iINTInc;
							m_iWISInc += iWISInc;
							m_iCONInc += iCONInc;
						}	
					}
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
	case MAGIC_EFFECT_DEFENSE:
		pUser->m_sRemainMagicNo[pMagic->m_tEValue] = pMagic->m_sMid;
		pUser->m_dwRemainMagicTime[pMagic->m_tEValue] = GetTickCount();
		pUser->InitRemainSpecial();
		break;
	case MAGIC_EFFECT_POISON:
		{
			// Cannot be poisoned if user is wearing a anti-poison item
			if (pUser->CheckHaveSpecialNumInBody(SPECIAL_RESIST_POISON))
			{
				switch (pMagic->m_tType01)
				{
				case MAGIC_TYPE_ARROW:
					SendMagicAttack(FAIL, pUser->m_Uid + USER_BAND);
					break;
				case MAGIC_TYPE_RAIL:
					SendMagicRailAttack(FAIL);
					break;
				}
				return iDamage;
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

				// Caps the magic damage
				// e.g. DragonBreath caps at 100 this is because
				// sDamage in magic table is 36
				// therefore 36 * 280 = 10080 / 100 = 100
				int iCap = 280;
				if (CheckDemon(m_sClass) == CheckDemon(pUser->m_sClass))
				{
					iCap = 250;
				}
				int iTemp = (pMagic->m_sDamage * iCap) / 100;
				if (iDamage > iTemp)
				{
					iDamage = iTemp;
				}
			}
			else
			{
				// Makes the user become gray if they heal someone who is gray
				if (pUser->m_iGrayCount > 0)
				{
					SetGrayUser(GRAY_MODE_NORMAL);
				}
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
					// todo: implement weaken negative damage calc
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
			pUser->m_dwRemainMagicTime[pMagic->m_tEValue] = GetTickCount();
		}
		break;
	case MAGIC_EFFECT_CURE:
		// Removes remaining magic on target
		if (pUser->m_sRemainMagicNo[MAGIC_EFFECT_POISON] == -1)
		{
			if (!pUser->CheckRemainMagicAll())
			{
				switch (pMagic->m_tType01)
				{
				case MAGIC_TYPE_ARROW:
					SendMagicAttack(FAIL, pUser->m_Uid + USER_BAND);
					break;
				case MAGIC_TYPE_RAIL:
					SendMagicRailAttack(FAIL);
					break;
				}
				return iDamage;
			}
		}
		pUser->m_sRemainMagicNo[MAGIC_EFFECT_POISON] = -1;
		pUser->InitRemainMagic(MAGIC_CLASS_ALL);
		pUser->SendUserStatus(MAGIC_CLASS_ALL);
		break;
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

	return iDamage;
}

DWORD USER::CheckRemainSpecialAttack(int iSpecialEffectValue)
{
	if (iSpecialEffectValue >= MAX_SPECIAL_EFFECT) return 0;

	short sSpecialNo = m_RemainSpecial[iSpecialEffectValue].sMid;
	if (sSpecialNo <= 0 || sSpecialNo > g_arSpecialAttackTable.GetSize())
	{
		m_RemainSpecial[iSpecialEffectValue].sMid = -1;
		return 0;
	}

	CSpecialTable* pSpecial = g_arSpecialAttackTable[sSpecialNo-1];
	if (!pSpecial)
	{
		m_RemainSpecial[iSpecialEffectValue].sMid = -1;
		return 0;
	}

	DWORD dwCurrTime = GetTickCount();
	if (pSpecial->m_iDecTerm > 0)
	{
		int iTermCount = (dwCurrTime - m_dwSpecialAttackCastedTime) / pSpecial->m_iDecTerm;
		if (iTermCount > 0)
		{
			m_dwSpecialAttackCastedTime = dwCurrTime;

			m_sHP -= iTermCount * pSpecial->m_sTHp;
			if (m_sHP < 1) 
			{
				m_sHP = 1;
				m_RemainSpecial[iSpecialEffectValue].sMid = -1;
				SendSpecialAttackCancel(iSpecialEffectValue);
				return 0;
			}

			if (m_sHP > GetMaxHP())
			{
				m_sHP = GetMaxHP();
			}

			m_sMP -= iTermCount * pSpecial->m_sTMp;
			if (m_sMP < 0)
			{
				m_sMP = 0;
				m_RemainSpecial[iSpecialEffectValue].sMid = -1;
				SendSpecialAttackCancel(iSpecialEffectValue);
				return 0;
			}
			
			if (m_sMP > GetMaxMP())
			{
				m_sMP = GetMaxMP();
			}
		}
	}

	DWORD dwDifference = dwCurrTime - m_RemainSpecial[iSpecialEffectValue].dwTime;
	if (dwDifference >= static_cast<DWORD>(pSpecial->m_iTime))
	{
		m_RemainSpecial[iSpecialEffectValue].sMid = -1;
		SendSpecialAttackCancel(iSpecialEffectValue);
		return 0;
	}

	return static_cast<DWORD>(pSpecial->m_iTime) - dwDifference;
}

void USER::EventMagic(short sMagicNo)
{
	// Validate the magic being used
	if (sMagicNo <= 0 || sMagicNo > g_arMagicTable.GetSize())
	{
		return;
	}

	CMagicTable *pMagic = g_arMagicTable.GetAt(sMagicNo-1);
	if (!pMagic)
	{
		return;
	}

	switch (pMagic->m_tType01)
	{
	case MAGIC_TYPE_ARROW:
		EventMagicArrow(pMagic);
		break;
	default:
		break;
	}
}

void USER::EventMagicArrow(CMagicTable* pMagic)
{
	if (!pMagic)
	{
		SendMagicAttack(FAIL, m_Uid + USER_BAND);
		return;
	}

	if (!m_bLive ||
		m_State != STATE_GAMESTARTED ||
		m_bHidden ||
		m_bIsWarping)
	{
		SendMagicAttack(FAIL, m_Uid + USER_BAND);
		return;
	}

	DoMagicEffect(pMagic, this);

	SendMagicAttack(SUCCESS, m_Uid + USER_BAND, pMagic->m_sMid, m_sHP, GetMaxHP(), pMagic->m_iTime);
}

void USER::SetMagicExp(CMagicTable* pMagic)
{
	if (!pMagic) return;

	int iMagicExp = 0;
	switch (pMagic->m_tClass)
	{
	case MAGIC_CLASS_BLUE:
		iMagicExp = GetInc(INC_BLUE_MAGIC);
		break;
	case MAGIC_CLASS_WHITE:
		iMagicExp = GetInc(INC_WHITE_MAGIC);
		break;
	case MAGIC_CLASS_BLACK:
		iMagicExp = GetInc(INC_BLACK_MAGIC);
		break;
	}
	iMagicExp = iMagicExp * (pMagic->m_sMpdec * pMagic->m_sPlusExpRate) / 100;
	PlusMagicExp(pMagic->m_tClass, iMagicExp);
}

void USER::PlusMagicExp(int iMagicClass, int iPlusMagicExp)
{
	if (iPlusMagicExp == 0) return;

	int iCurExp = 0;
	int iNewExp = 0;
	CString strMagicSkill = "";

	switch (iMagicClass)
	{
	case MAGIC_CLASS_BLUE:
		iCurExp = m_iBMagicExp / SERVER_SKILL;
		m_iBMagicExp += iPlusMagicExp;
		if (m_iBMagicExp < 0)
		{
			m_iBMagicExp = 0;
		}	
		iNewExp = m_iBMagicExp / SERVER_SKILL;
		strMagicSkill.Format(IDS_USER_BLUE_MAGIC);
		break;
	case MAGIC_CLASS_WHITE:
		iCurExp = m_iWMagicExp / SERVER_SKILL;
		m_iWMagicExp += iPlusMagicExp;
		if (m_iWMagicExp < 0)
		{
			m_iWMagicExp = 0;
		}	
		iNewExp = m_iWMagicExp / SERVER_SKILL;
		strMagicSkill.Format(IDS_USER_WHITE_MAGIC);
		break;
	case MAGIC_CLASS_BLACK:
		iCurExp = m_iDMagicExp / SERVER_SKILL;
		m_iDMagicExp += iPlusMagicExp;
		if (m_iDMagicExp < 0)
		{
			m_iDMagicExp = 0;
		}	
		iNewExp = m_iDMagicExp / SERVER_SKILL;
		strMagicSkill.Format(IDS_USER_BLACK_MAGIC);
		break;
	}

	int iDiff = abs(iNewExp - iCurExp);
	if (iDiff != 0)
	{
		int iCur = iCurExp % 10; 
		if (iDiff + iCur >= 10) // Yellow skill Gain
		{
			CString str;
			if (iPlusMagicExp > 0)
			{
				str.Format(IDS_USER_INC, strMagicSkill);
				SendSpecialMsg((LPTSTR)(LPCTSTR)str, YELLOW_STAT, TO_ME); // Skill increase
			}
			else
			{
				str.Format(IDS_USER_DEC, strMagicSkill);
				SendSpecialMsg((LPTSTR)(LPCTSTR)str, YELLOW_STAT, TO_ME); // Skill Decrease
			}
		}
		else if (iDiff + iCur >= 0  && iDiff + iCur < 10) // Blue skill gain
		{
			CString str;
			if (iPlusMagicExp > 0)
			{
				str.Format(IDS_USER_INC, strMagicSkill);
				SendSpecialMsg((LPTSTR)(LPCTSTR)str, BLUE_STAT, TO_ME); // Skill increase
			}
			else
			{
				str.Format(IDS_USER_DEC, strMagicSkill);
				SendSpecialMsg((LPTSTR)(LPCTSTR)str, BLUE_STAT, TO_ME); // Skill Decrease
			}
		}

		SendCharData(INFO_MAGICEXP);
	}

	CString str;
	str.Format(IDS_GM_MSG_PLUS_MAGIC_EXP, strMagicSkill, iPlusMagicExp);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);
}

bool USER::CheckSuccessSpecialAttackByItem(CSpecialTable* pSpecial)
{
	// Cannot use special attack if not wearing the correct weapon
	if (m_InvItem[ARM_RHAND].IsEmpty() ||
		m_InvItem[ARM_RHAND].bType != pSpecial->m_tClass)
	{
		return false;
	}

	// Validate the weapon type of item being worn
	if (m_InvItem[ARM_RHAND].bType < 0 || 
		m_InvItem[ARM_RHAND].bType > TYPE_STAFF)
	{
		return false;
	}

	// Validate the skill neded for usage of the special attack
	int iWeaponExp = 0;
	switch (m_InvItem[ARM_RHAND].bType)
	{
	case TYPE_SWORD:
		iWeaponExp = m_iSwordExp / CLIENT_SKILL;
		break;
	case TYPE_AXE:
		iWeaponExp = m_iAxeExp / CLIENT_SKILL;
		break;
	case TYPE_BOW:
		iWeaponExp = m_iBowExp / CLIENT_SKILL;
		break;
	case TYPE_SPEAR:
		iWeaponExp = m_iSpearExp / CLIENT_SKILL;
		break;
	case TYPE_KNUCKLE:
		iWeaponExp = m_iKnuckleExp / CLIENT_SKILL;
		break;
	case TYPE_STAFF:
		iWeaponExp = m_iStaffExp / CLIENT_SKILL;
		break;
	default:
		break;
	}
	if (iWeaponExp < pSpecial->m_sMinExp)
	{
		return false;
	}

	// Woo !~! Special Attack can be used :)
	return true;
}

bool USER::CheckPlusType(CSpecialTable* pSpecial, int iType)
{
	if (!pSpecial) return false;

	// PlusType of 0 returns true for all
	int iPlusType = pSpecial->m_sPlusType;
	if (iPlusType == 0)
	{
		return true;
	}
	
	if (iType == 4 && (iPlusType == 1 || iPlusType == 2 || iPlusType == 4))
	{
		return true;
	}

	if (iType == 5 && (iPlusType == 2 || iPlusType == 3 || iPlusType == 5))
	{
		return true;
	}

	if (iType == 6 && (iPlusType == 1 || iPlusType == 3 || iPlusType == 6))
	{
		return true;
	}

	return false;
}

void USER::SendSpecialAttackArrow(BYTE bySuccess, short sSpecialNo, int iContinueTime, 
	int iTargetId, short sTargetHP, short sX, short sY)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_ATTACK_SPECIALARROW, index);
	SetByte(m_TempBuf, bySuccess, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	if (bySuccess == SUCCESS)
	{
		SetShort(m_TempBuf, m_sHP, index);
		SetShort(m_TempBuf, m_sMP, index);
		SetShort(m_TempBuf, m_sStm, index);
		SetShort(m_TempBuf, static_cast<short>(m_byDir), index);
		SetShort(m_TempBuf, sSpecialNo, index);
		SetDWORD(m_TempBuf, iContinueTime, index);
		SetInt(m_TempBuf, iTargetId, index);
		SetShort(m_TempBuf, sTargetHP, index);
		SetShort(m_TempBuf, sX, index);	// Trace X
		SetShort(m_TempBuf, sY, index); // Trace Y
	}
	SendInsight(m_TempBuf, index);
}

bool USER::CheckPlusValueByHeal(int iHeal, int iDamage)
{
	iDamage = abs(iDamage);
	int iNeedDamage = abs(iHeal) * 4;

	m_iHealPlusValue += iDamage;
	if (m_iHealPlusValue >= iNeedDamage)
	{
		m_iHealPlusValue -= iNeedDamage;
		return true;
	}
	return false;
}

void USER::SendMagicCasting(BYTE bySuccess, short sMagicNo, int iStartTime)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_MAGICPRECAST, index);
	SetByte(m_TempBuf, bySuccess, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	if (bySuccess == SUCCESS)
	{
		SetShort(m_TempBuf, sMagicNo, index);
		SetInt(m_TempBuf, iStartTime, index);
	}
	SendInsight(m_TempBuf, index);
}

bool USER::CheckSuccessSpecialAttack(CSpecialTable* pSpecial, USER* pTarget)
{
	if (pSpecial == NULL || pTarget == NULL)
	{
		return false;
	}
				
	if (CheckDemon(m_sClass) == CheckDemon(pTarget->m_sClass) &&
		m_BattleMode == BATTLEMODE_ATTACK &&
		m_Uid != pTarget->m_Uid &&
		pTarget->m_iGrayCount == 0 &&
		!CheckGuildWarArmy(pTarget))
	{
		return false;					
	}

	bool bCheckRemain = false;
	switch (pSpecial->m_tEValue)
	{
	case SPECIAL_EFFECT_HPREGEN:
	case SPECIAL_EFFECT_INCREASEATTACK:
	case SPECIAL_EFFECT_DEFENSE:
	case SPECIAL_EFFECT_SPEED:
	case SPECIAL_EFFECT_DODGE:
	case SPECIAL_EFFECT_MPREGEN:
	case SPECIAL_EFFECT_RAPIDCAST:
	case SPECIAL_EFFECT_MPSAVE:
	case SPECIAL_EFFECT_TRACE:
	case SPECIAL_EFFECT_MASTERTRACE:
		bCheckRemain = true;
		break;
	}

	if (m_Uid != pTarget->m_Uid && bCheckRemain)
	{
		if (pTarget->CheckRemainMagic(MAGIC_EFFECT_DEFENSE))
		{
			return false;
		}

		if (pTarget->CheckRemainSpecialAttack(SPECIAL_EFFECT_HPREGEN))
		{
			return false;
		}

		if (pTarget->CheckRemainSpecialAttack(SPECIAL_EFFECT_INCREASEATTACK))
		{
			return false;
		}

		if (pTarget->CheckRemainSpecialAttack(SPECIAL_EFFECT_DEFENSE))
		{
			return false;
		}

		if (pTarget->CheckRemainSpecialAttack(SPECIAL_EFFECT_SPEED))
		{
			return false;
		}

		if (pTarget->CheckRemainSpecialAttack(SPECIAL_EFFECT_DODGE))
		{
			return false;
		}

		if (pTarget->CheckRemainSpecialAttack(SPECIAL_EFFECT_MPREGEN))
		{
			return false;
		}

		if (pTarget->CheckRemainSpecialAttack(SPECIAL_EFFECT_RAPIDCAST))
		{
			return false;
		}

		if (pTarget->CheckRemainSpecialAttack(SPECIAL_EFFECT_MPSAVE))
		{
			return false;
		}
	}

	InitRemainSpecial();
	if (pSpecial->m_iTime <= 0)
	{
		pTarget->InitRemainSpecial();
		pTarget->m_sRemainMagicNo[MAGIC_EFFECT_DEFENSE] = -1;
	}
	return true;
}

bool USER::CheckSpecialAttackDelay(CSpecialTable* pSpecial)
{
	DWORD dwCurrTime = GetTickCount();
	if (m_dwLastSpecialTime < dwCurrTime)
	{
		if (dwCurrTime - m_dwLastSpecialTime < m_dwSpecialAttackDelay)
		{
			return false;
		}

		m_dwLastSpecialTime = dwCurrTime;
		m_dwSpecialAttackDelay = pSpecial->m_iTerm;
		m_sSpecialAttackNo = pSpecial->m_sMid;
		return true;
	}
	else if (m_dwLastSpecialTime > dwCurrTime)
	{
		m_dwLastSpecialTime = dwCurrTime;
		m_dwSpecialAttackDelay = pSpecial->m_iTerm;
		m_sSpecialAttackNo = pSpecial->m_sMid;
		return true;
	}

	return false;
}

void USER::SendSpecialAttackCancel(int iSpecialEffectValue)
{
	SendItemInfo(ARM_RHAND);

	CBufferEx TempBuf;
	TempBuf.Add(PKT_SPECIALATTACKCANCEL);
	TempBuf.Add((int)(m_Uid + USER_BAND));
	SendInsight(TempBuf, TempBuf.GetLength());

	CString str;
	str.Format(IDS_GM_MSG_SPECIAL_ATTACK_CANCEL, iSpecialEffectValue);
	SendServerMessageGameMaster((LPTSTR)(LPCTSTR)str, TO_ME);
}

void USER::EventSAttack(short sSpecialNo)
{
	// Validate the special attack being used
	if (sSpecialNo <= 0 || sSpecialNo > g_arSpecialAttackTable.GetSize())
	{
		return;
	}

	CSpecialTable *pSpecial = g_arSpecialAttackTable.GetAt(sSpecialNo-1);
	if (!pSpecial)
	{
		return;
	}

	switch (pSpecial->m_tType)
	{
	case SPECIAL_TYPE_ARROW:
		EventSAttackArrow(pSpecial);
		break;
	default:
		break;
	}
}

void USER::EventSAttackArrow(CSpecialTable* pSpecial)
{
	if (!pSpecial)
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	if (pSpecial->m_tTarget != MAGIC_TARGET_OTHER)
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	if (!m_bLive ||
		m_State != STATE_GAMESTARTED ||
		m_bHidden ||
		m_bIsWarping)
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	if (!CheckSuccessSpecialAttack(pSpecial, this))
	{
		SendSpecialAttackArrow(FAIL);
		return;
	}

	m_dwLastSpecialTime = GetTickCount();

	switch (pSpecial->m_tEValue)
	{
	case SPECIAL_EFFECT_HPREGEN:
	case SPECIAL_EFFECT_INCREASEATTACK:
	case SPECIAL_EFFECT_DEFENSE:
	case SPECIAL_EFFECT_SPEED:
	case SPECIAL_EFFECT_DODGE:
	case SPECIAL_EFFECT_MPREGEN:
	case SPECIAL_EFFECT_RAPIDCAST:
	case SPECIAL_EFFECT_MPSAVE:
		m_RemainSpecial[pSpecial->m_tEValue].sMid = pSpecial->m_sMid;
		m_RemainSpecial[pSpecial->m_tEValue].sDamage = pSpecial->m_sDamage;
		m_RemainSpecial[pSpecial->m_tEValue].dwTime = GetTickCount();
		break;
	case SPECIAL_EFFECT_DOUBLEATTACK:
		{
			int iDefense = GetDefense();
			int iAttack = GetAttack();
			int iDamage = iAttack - iDefense;
			if (iDamage <= 0)
			{
				iDamage = myrand(0, 2);
			}
			iDamage +=  static_cast<int>(((static_cast<double>(myrand(90, 110)) * pSpecial->m_sDamage) / 100.0) + 0.5);
	
			if (iDamage > m_sHP)
			{
				iDamage = m_sHP;
			}

			// Do the damage!
			SetDamage(iDamage, -1, false);
		}
		break;
	}

	SendSpecialAttackArrow(SUCCESS, pSpecial->m_sMid, (pSpecial->m_iTime / 1000), m_Uid + USER_BAND, m_sHP);
}

int USER::CalcStopTime(short sLevelSource, short sLevelTarget)
{
	static int stopTimePos[15] = {7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 12};
	static int stopTimeNeg[15] = {2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 7};

	const int iLevelDifference = sLevelSource - sLevelTarget;
	if (iLevelDifference >= 15)
	{
		return 12;
	}
	else if (iLevelDifference >= 0)
	{
		return stopTimePos[iLevelDifference];
	}
	else if (iLevelDifference >= -14)
	{
		return stopTimeNeg[iLevelDifference + 14];
	}
	else
	{
		return 1;
	}
}

void USER::SendSpecialAttackRail(BYTE bySuccess, int iTargetCount, int* iTargetsId, short sSpecialNo, short* sTargetsHP, int iContinueTime)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_ATTACK_SPECIALRAIL, index);
	SetByte(m_TempBuf, bySuccess, index);
	if (bySuccess == SUCCESS)
	{
		SetInt(m_TempBuf, m_Uid + USER_BAND, index);
		SetShort(m_TempBuf, m_sHP, index);
		SetShort(m_TempBuf, m_sMP, index);
		SetShort(m_TempBuf, m_sStm, index);
		SetShort(m_TempBuf, static_cast<short>(m_byDir), index);
		SetShort(m_TempBuf, sSpecialNo, index);
		SetByte(m_TempBuf, 0, index);
		SetDWORD(m_TempBuf, iContinueTime, index);
		SetByte(m_TempBuf, iTargetCount, index);
		for (int i = 0; i < iTargetCount; i++)
		{
			SetInt(m_TempBuf, iTargetsId[i], index);
			SetShort(m_TempBuf, sTargetsHP[i], index);
		}
	}
	SendInsight(m_TempBuf, index);
}

void USER::SendSpecialAttackCircle(BYTE bySuccess, int iTargetCount, int* iTargetsId, short sSpecialNo, short* sTargetsHP, int iContinueTime)
{
	int index = 0;
	SetByte(m_TempBuf, PKT_ATTACK_SPECIALCIRCLE, index);
	SetByte(m_TempBuf, bySuccess, index);
	if (bySuccess == SUCCESS)
	{
		SetInt(m_TempBuf, m_Uid + USER_BAND, index);
		SetShort(m_TempBuf, m_sHP, index);
		SetShort(m_TempBuf, m_sMP, index);
		SetShort(m_TempBuf, m_sStm, index);
		SetShort(m_TempBuf, static_cast<short>(m_byDir), index);
		SetShort(m_TempBuf, sSpecialNo, index);
		SetDWORD(m_TempBuf, iContinueTime, index);
		SetByte(m_TempBuf, iTargetCount, index);
		for (int i = 0; i < iTargetCount; i++)
		{
			SetInt(m_TempBuf, iTargetsId[i], index);
			SetShort(m_TempBuf, sTargetsHP[i], index);
		}
	}
	SendInsight(m_TempBuf, index);
}
