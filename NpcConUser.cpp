#include "stdafx.h"
#include "USER.h"
#include "Npc.h"

#include "Extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CNpc::Send(USER *pUser, TCHAR *pBuf, int nLength)
{
	if (!pUser)return;

	pUser->Send(pBuf, nLength);
}


void CNpc::SightRecalc(COM* pCom)
{
	int sx, sy;

	sx = m_sCurX / SIGHT_SIZE_X;
	sy = m_sCurY / SIGHT_SIZE_Y;

	int dir_x = 0;
	int dir_y = 0;

	if (sx == m_presx && sy == m_presy)return;

	if (m_presx == -1 || m_presy == -1)
	{
		dir_x = 0;
		dir_y = 0;
	}
	else
	{
		if (sx > m_presx && abs(sx-m_presx) == 1)		dir_x = DIR_H;
		if (sx < m_presx && abs(sx-m_presx) == 1)		dir_x = DIR_L;
		if (sy > m_presy && abs(sy-m_presy) == 1)		dir_y = DIR_H;
		if (sy < m_presy && abs(sy-m_presy) == 1)		dir_y = DIR_L;
		if (abs(sx-m_presx) > 1)						dir_x = DIR_OUTSIDE;
		if (abs(sy-m_presy) > 1)						dir_y = DIR_OUTSIDE;
	}

	int prex = m_presx;
	int prey = m_presy;
	m_presx = sx;
	m_presy = sy;

	SendUserInfoBySightChange(dir_x, dir_y, prex, prey, pCom);
}


void CNpc::SendUserInfoBySightChange(int dir_x, int dir_y, int prex, int prey, COM *pCom)
{
	int min_x = 0, min_y = 0;
	int max_x = 0, max_y = 0;

	int sx = m_presx;
	int sy = m_presy;

	int modify_index = 0;
	char modify_send[1024];		::ZeroMemory(modify_send, sizeof(modify_send));
	FillNpcInfo(modify_send, modify_index, INFO_MODIFY, pCom);

	int delete_index = 0;
	char delete_send[1024];		::ZeroMemory(delete_send, sizeof(delete_send));
	FillNpcInfo(delete_send, delete_index, INFO_DELETE, pCom);

	if (prex == -1 || prey == -1)
	{
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendToRange(pCom, modify_send, modify_index, min_x, min_y, max_x, max_y);
		return;
	}
	if (dir_x == DIR_OUTSIDE || dir_y == DIR_OUTSIDE)
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendToRange(pCom, delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendToRange(pCom, modify_send, modify_index, min_x, min_y, max_x, max_y);
		return;
	}
	if (dir_x > 0)
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendToRange(pCom, delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx+1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendToRange(pCom, modify_send, modify_index, min_x, min_y, max_x, max_y);
	}
	if (dir_y > 0)
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey)*SIGHT_SIZE_Y;
		SendToRange(pCom, delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy+1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendToRange(pCom, modify_send, modify_index, min_x, min_y, max_x, max_y);
	}
	if (dir_x < 0)
	{
		min_x = (prex+1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey-1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendToRange(pCom, delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy+2)*SIGHT_SIZE_Y;
		SendToRange(pCom, modify_send, modify_index, min_x, min_y, max_x, max_y);
	}
	if (dir_y < 0)
	{
		min_x = (prex-1)*SIGHT_SIZE_X;
		max_x = (prex+2)*SIGHT_SIZE_X;
		min_y = (prey+1)*SIGHT_SIZE_Y;
		max_y = (prey+2)*SIGHT_SIZE_Y;
		SendToRange(pCom, delete_send, delete_index, min_x, min_y, max_x, max_y);
		min_x = (sx-1)*SIGHT_SIZE_X;
		max_x = (sx+2)*SIGHT_SIZE_X;
		min_y = (sy-1)*SIGHT_SIZE_Y;
		max_y = (sy)*SIGHT_SIZE_Y;
		SendToRange(pCom, modify_send, modify_index, min_x, min_y, max_x, max_y);
	}
}

void CNpc::SendToRange(COM *pCom, char *temp_send, int index, int min_x, int min_y, int max_x, int max_y)
{
	if (index <= 0 || index >= MAX_PACKET_SIZE)return;

	MAP* pMap = g_Zones[m_ZoneIndex];
	if (!pMap)return;

	int tmin_x = min_x;		if (tmin_x < 0)tmin_x = 0;
	int tmax_x = max_x;		if (tmax_x >= pMap->m_sizeMap.cx)tmax_x = pMap->m_sizeMap.cx - 1;
	int tmin_y = min_y;		if (tmin_y < 0)tmin_y = 0;
	int tmax_y = max_y;		if (tmax_y >= pMap->m_sizeMap.cy)tmax_y = pMap->m_sizeMap.cy - 1;

	int temp_uid;
	USER* pUser = NULL;

	for (int i = tmin_x; i < tmax_x; i++)
	{
		for (int j = tmin_y; j < tmax_y; j++)
		{
			temp_uid = pMap->m_pMap[i][j].m_lUser;

			if (temp_uid < USER_BAND || temp_uid >= NPC_BAND) continue;
			else temp_uid -= USER_BAND;

			if (temp_uid >= 0 && temp_uid < MAX_USER)
			{
				pUser = pCom->GetUserUid(temp_uid);
				if (pUser == NULL)continue;

				if (pUser->m_State == STATE_GAMESTARTED)
				{
					if (pUser->m_sX == i && pUser->m_sY == j && pUser->m_sZ == m_sCurZ)
					{
						Send(pUser, temp_send, index);
					}
				}
			}
		}
	}
}

void CNpc::SendInsight(COM* pCom, TCHAR *pBuf, int nLength)
{
	if (nLength <= 0 || nLength >= MAX_PACKET_SIZE) return;

	int sx = m_sCurX / SIGHT_SIZE_X;
	int sy = m_sCurY / SIGHT_SIZE_Y;

	int min_x = (sx-1)*SIGHT_SIZE_X; if (min_x < 0)min_x = 0;
	int max_x = (sx+2)*SIGHT_SIZE_X;
	int min_y = (sy-1)*SIGHT_SIZE_Y; if (min_y < 0)min_y = 0;
	int max_y = (sy+2)*SIGHT_SIZE_Y;

	MAP* pMap = g_Zones[m_ZoneIndex];
	if (!pMap)return;

	if (max_x >= pMap->m_sizeMap.cx)max_x = pMap->m_sizeMap.cx - 1;
	if (max_y >= pMap->m_sizeMap.cy)max_y = pMap->m_sizeMap.cy - 1;

	int temp_uid;
	USER* pUser = NULL;

	for (int i = min_x; i < max_x; i++)
	{
		for (int j = min_y; j < max_y; j++)
		{
			temp_uid = pMap->m_pMap[i][j].m_lUser;

			if (temp_uid < USER_BAND || temp_uid >= NPC_BAND) continue;
			else temp_uid -= USER_BAND;

			if (temp_uid >= 0 && temp_uid < MAX_USER)
			{
				pUser = pCom->GetUserUid(temp_uid);
				if (pUser == NULL)continue;

				if (pUser->m_State == STATE_GAMESTARTED)
				{
					if (pUser->m_sX == i && pUser->m_sY == j && pUser->m_sZ == m_sCurZ)
					{
						Send(pUser, pBuf, nLength);
					}
				}
			}
		}
	}
}

void CNpc::FillNpcInfo(char *temp_send, int &index, BYTE flag, COM* pCom)
{
	CPoint t;
	BYTE len = 0;

	t = ConvertToClient(m_sCurX, m_sCurY);

	SetByte(temp_send, PKT_USERMODIFY, index);
	SetByte(temp_send, flag, index);
	SetInt( temp_send, (int)(m_sNid + NPC_BAND), index);
	SetShort(temp_send, m_sClass, index); // class
	SetShort(temp_send, t.x, index);
	SetShort(temp_send, t.y, index);

	if (flag != INFO_MODIFY)
	{
		return;
	}

	SetByte(temp_send, m_tNpcType, index); // bDead Enable

	len = strlen(m_strName);
	SetByte(temp_send, (BYTE)len, index);
	if (len > 0)
	{
		SetString(temp_send, m_strName, len, index);
	}
	SetByte(temp_send, 0, index); // not sure
	//len = 0;
	//SetByte(temp_send, (BYTE)len, index); // Guild name len
	SetVarString(temp_send, m_strTownGuildName, strlen(m_strTownGuildName), index);
	SetShort(temp_send, m_sTownGuildPicId, index); // Guild pic id
	SetByte(temp_send, 0, index); // Guild Rank
	SetByte(temp_send, 0, index); // Guild War .. 1 = in war 0 = no war
	SetShort(temp_send, -1, index); // How to display name at GVW
	SetShort(temp_send, m_sTownGuildNum, index);
	SetShort(temp_send, -1, index); // The guild the user is fighting against
	SetByte(temp_send, 0, index); // Will turn name black during GW if 1

	SetByte(temp_send, 0, index); // Party leader string length

	SetByte(temp_send, 0, index); // 1 if in a place where wotw is on
	SetShort(temp_send, m_sMaxHp, index);
	SetShort(temp_send, m_sHP, index);
	SetShort(temp_send, (short)0, index);
	SetShort(temp_send, 0, index); // hair mode
	SetShort(temp_send, (short)m_sBlood, index);
	SetShort(temp_send, (short)m_sPid, index);
	SetShort(temp_send, 0, index); // Morale
	if (m_NpcState == NPC_DEAD)
	{
		SetByte(temp_send, 0, index); // bDead Enable
	}
	else
	{
		SetByte(temp_send, 1, index); // bDead Enable
	}
	SetByte(temp_send, 1, index); // no idea
	SetByte(temp_send, 0, index); // npcnbattlemode

	SetShort(temp_send, -1, index); // attack speed
	SetShort(temp_send, (short)0, index); // NPC Direction
	SetByte(temp_send, 0, index); // Grey mode

	for (int i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		SetShort(temp_send, (short)-1, index);
		SetShort(temp_send, (short)-1, index); // B Type
		SetShort(temp_send, (short)-1, index); // B Arm
	}
	SetShort(temp_send, (short)m_can_find_our, index); // Status

	if (m_sPoisonMagicNo != -1)
	{
		SetByte(temp_send, 1, index); // Tell client there is a magic!
		SetShort(temp_send, m_sPoisonMagicNo, index);
		SetDWORD(temp_send, m_dwPoisonMagicCount * 1000, index);
	}

	for (int i = 0; i < MAX_MAGIC_EFFECT; i++)
	{
		DWORD dwConTime = CheckRemainMagic(i);
		if (dwConTime)
		{
			SetByte(temp_send, 1, index); // Tell client there is a magic!
			SetShort(temp_send, m_sRemainMagicNo[i], index);
			SetDWORD(temp_send, dwConTime, index);
		}
	}
	SetByte(temp_send, 0, index); // tells client there are no more magic to read
	SetShort(temp_send, (short)0, index); // no idea

	for (int i = 0; i < MAX_SPECIAL_EFFECT; i++)
	{
		DWORD dwConTime = CheckRemainSpecialAttack(pCom, i);
		if (dwConTime)
		{
			SetByte(temp_send, 1, index); // Tell client there is a magic!
			SetShort(temp_send, m_sRemainSpecialNo[i], index);
			SetDWORD(temp_send, dwConTime, index);
		}
	}
	SetByte(temp_send, 0, index);
}
