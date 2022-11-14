// UserItem.cpp: implementation of the item part of USER class.
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

ItemList USER::StrToUserItem(TCHAR *pBuf)
{
	// TODO: NEED GET FULL STRUCTURE FOR ITEMS!- Half way there :S
	int index = 0;
	ItemList list;
	if (!pBuf) return list;
	list.sNum = GetShort(pBuf, index);
	if (list.IsEmpty()) return list;
	list.sUsage = GetShort(pBuf, index);
	list.bExt = GetByte(pBuf, index) ? true : false;
	
	CItemTable* pItem = NULL;
	if (g_mapItemTable.Lookup(list.sNum, pItem))
	{
		list.sAb1 = pItem->m_sAb1;
		list.sAb2 = pItem->m_sAb2;
		list.bArm = pItem->m_bArm;
		list.sAt = pItem->m_sAt;
		list.dwMoney = pItem->m_iCost;
		list.sDf = pItem->m_sDf;
		list.bGender = pItem->m_bGender;
		list.sHP = pItem->m_sHP;
		list.sDur = pItem->m_sDur;
		list.sMP = pItem->m_sMP;
		list.strNewName = pItem->m_strName;
		list.sNeedInt = pItem->m_sNeedInt;
		list.sNeedStr = pItem->m_sNeedStr;
		list.sNeedDex = pItem->m_sPicNum4;
		list.sNeedSkill = pItem->m_sMinExp;
		list.sMagicNo = pItem->m_sMagicNo; // ????
		list.sPicNum = pItem->m_sPicNum1;
		list.sMagicOpt = pItem->m_sMagicOpt; // ????
		list.bType = pItem->m_bType;
		list.sWgt = pItem->m_sWgt;
		list.sTime = pItem->m_sTime;
		list.sSpecial = pItem->m_sSpecial;
		for (int i = 0; i < MAX_SPECIAL_OPT; i++)
		{
			list.sSpOpt[i] = pItem->m_sSpOpt[i];
		}

		if (list.bExt) // Extra Values
		{
			int len = 0;
			index+=2; // item num again
			list.bType = GetByte(pBuf, index);
			list.bArm = GetByte(pBuf, index);
			list.sPicNum = GetShort(pBuf, index);
			int iLen = GetVarString(50, list.strNewName.GetBuffer(50), pBuf, index);
			ASSERT(iLen > 0 && iLen <= 50);
			index+=2; // dura again
			list.sAb1 = GetShort(pBuf, index);
			list.sAb2 = GetShort(pBuf, index);
			list.sDf = GetShort(pBuf, index);
			list.sMP = GetShort(pBuf, index);
			index+=2; // unknown
			index+=2; // unknown
			list.sWgt = GetShort(pBuf, index);
			list.sSpecial = GetShort(pBuf, index);
			for (int i = 0; i < MAX_SPECIAL_OPT; i++)
			{
				list.sSpOpt[i] = GetShort(pBuf, index);
			}
			index+=2; // unknown
			index+=2; // unknown
			index+=2; // unknown
			index+=1; // unknown
			list.sTime = GetShort(pBuf, index);
			index+=2; // unknown
		}
	}
	index+=2; // Something to do with repairing the item!
	list.sMaxUsage = GetShort(pBuf, index);
	if (list.sMaxUsage < 1 || list.sMaxUsage > MAX_ITEM_DURA)
	{
		list.sMaxUsage = pItem->m_sDur;
	}
	list.byUpgradeLevel = GetShort(pBuf, index);
	if (list.byUpgradeLevel < 0 || list.byUpgradeLevel > 8)
	{
		list.byUpgradeLevel = 0;
	}
	list.iItemSerial = GetInt64(pBuf, index);
	SetPlusSpecial(&list);
	return list;
}

void USER::UserItemToStr(ItemList* list, TCHAR *pBuf)
{
	// TODO: NEED GET FULL STRUCTURE FOR ITEMS!- Half way there :S
	if (!list) return;

	int index = 0;
	SetShort(pBuf, list->sNum, index);
	SetShort(pBuf, list->sUsage, index);
	if (list->bExt) // Extended item information
	{
		SetByte(pBuf, 1, index);
		SetShort(pBuf, list->sNum, index);
		SetByte(pBuf, list->bType, index);
		SetByte(pBuf, list->bArm, index);
		SetShort(pBuf, list->sPicNum, index);
		SetVarString(pBuf, (LPTSTR)(LPCTSTR)list->strNewName, list->strNewName.GetLength(), index);
		SetShort(pBuf, list->sUsage, index); // Unknown
		SetShort(pBuf, list->sAb1, index);
		SetShort(pBuf, list->sAb2, index);
		SetShort(pBuf, list->sDf, index);
		SetShort(pBuf, list->sMP, index);
		SetShort(pBuf, 0, index); // unknown
		SetShort(pBuf, 0, index); // unknown
		SetShort(pBuf, list->sWgt, index);
		SetShort(pBuf, list->sSpecial, index);
		for (int i = 0; i < MAX_SPECIAL_OPT; i++)
		{
			SetShort(pBuf, list->sSpOpt[i], index);
		}
		SetShort(pBuf, 0, index); // unknown
		SetShort(pBuf, 0, index); // unknown
		SetShort(pBuf, 0, index); // unknown
		SetByte(pBuf, 0, index); // unknown
		SetShort(pBuf, list->sTime, index);
		SetShort(pBuf, 0, index); // unknown

		SetShort(pBuf, 0, index); // Something to do with repairing the item!
		SetShort(pBuf, list->sMaxUsage, index);
		SetShort(pBuf, list->byUpgradeLevel, index);
		SetInt64(pBuf, list->iItemSerial, index);
	}
	else
	{
		SetByte(pBuf, 0, index);
		SetShort(pBuf, 0, index); // Something to do with repairing the item!
		SetShort(pBuf, list->sMaxUsage, index);
		SetShort(pBuf, list->byUpgradeLevel, index);
		SetInt64(pBuf, list->iItemSerial, index);
	}	
}

short USER::GetItemClass(short sNum)
{
	return sNum >= 2000 ? DEVIL : HUMAN;
}

void USER::RecalcWgt()
{
	int i;
	short Weight = 0;
	for (i = 0; i < INV_ITEM_NUM; i++)
	{
		if (m_InvItem[i].bType > TYPE_ACC)
		{
			Weight += m_InvItem[i].sUsage * m_InvItem[i].sWgt;
		}
		else
		{
			Weight += m_InvItem[i].sWgt;
		}
	}
	for (i = 0; i < BELT_ITEM_NUM; i++)
	{
		Weight += m_BeltItem[i].sUsage * m_BeltItem[i].sWgt;
	}

	m_sWgt = Weight;
}

bool USER::GetEquipItemPid(TCHAR *pItemBuf, TCHAR *pBuf)
{
	if (pItemBuf == NULL) return false;
	
//	if (EQUIP_ITEM_NUM > nSize) return FALSE;

	short EquipItemIds[EQUIP_ITEM_NUM];

	int index = 0;
	for (int i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		EquipItemIds[i] = GetShort(pItemBuf, index);
		index += ITEM_DB_SIZE - 2; // Skip rest of item data because 
								   // only interested in the id number
	}	
	
	short sSid = 0;
	short sPid = 0;

	index = 0;
	for (int i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		sSid = EquipItemIds[i];
		if (sSid > 0)
		{
			CItemTable* pItemTable;
			if (g_mapItemTable.Lookup(sSid, pItemTable))
			{
				sPid = pItemTable->m_sPicNum1;
			}
			else
			{
				sPid = 0;
			}
		}
		else 
		{
			sPid = 0;
		}
		
		SetShort(pBuf, sPid, index);
	}

	return true;
}

int USER::GetSendItemData(ItemList item, TCHAR* pData, BYTE bFlags)
{
	int index = 0;
	if (item.sNum <= 0) return 0;
	CItemTable* pItem =	g_mapItemTable[item.sNum];
	ASSERT(pItem != NULL);

	SetByte(pData, item.bType, index);
	SetByte(pData, item.bArm, index);
	short sClass = 0;
	if (item.sNum > 2000)
		sClass = 10;
	SetShort(pData, sClass, index);
	SetShort(pData, item.sPicNum, index);
	if (bFlags & TRADE_LAYOUT)
	{
		SetShort(pData, item.sUsage, index);
	}
	SetByte(pData, (BYTE)pItem->m_bGender, index);
	SetShort(pData, item.sAb1, index);
	SetShort(pData, item.sAb2, index);
	SetShort(pData, item.sWgt, index);
	SetShort(pData, item.sTime, index);
	if (bFlags & GET_VALUE)
	{
		SetDWORD(pData, item.dwMoney, index);
	}
	SetShort(pData, pItem->m_sNeedStr, index);
	SetShort(pData, pItem->m_sNeedInt, index);
	SetShort(pData, pItem->m_sPicNum4, index);
	SetShort(pData, pItem->m_sMinExp, index);
	SetShort(pData, pItem->m_sHP, index);
	SetShort(pData, pItem->m_sMP, index);
	if (bFlags & NORM_LAYOUT)
	{
		SetShort(pData, item.sUsage, index);
	}
	if (bFlags & GET_EXTRA_USAGE)
	{
		SetShort(pData, item.sMaxUsage, index);
		SetShort(pData, pItem->m_sDur, index);
	}
	SetVarString(pData, (LPTSTR)(LPCTSTR)item.strNewName, item.strNewName.GetLength(), index);
	SetByte(pData, item.byUpgradeLevel, index);
	index += GetSpecialItemData(item.sNum, pData + index);
	
	// TODO: Check if this is needed
	// 1p1 sends the user attack delay 
	// checks for remaining speed up aura
	// gets current attack delay
	// or default (900)

	return index;
}

int USER::GetSpecialItemData(short nItemNum, TCHAR* pData)
{
	int index = 0;
	int sOpt = -1;
	if (nItemNum <= 0) return 0;
	CItemTable* pItem =	g_mapItemTable[nItemNum];
	ASSERT (pItem != NULL);

	switch (pItem->m_sSpecial)
	{
		case SPECIAL_PLUS:
			for (int i = 0; i < MAX_SPECIAL_OPT; i++)
			{
				index += AddPlusSpecialData(pItem->m_sSpOpt[i], pData + index);
			}
			break;
		default:
			SetShort(pData, pItem->m_sSpecial, index);
			for (int i = 0; i < MAX_SPECIAL_OPT; i++)
			{
				SetShort(pData, pItem->m_sSpOpt[i], index);
			}
			for (int i = 0; i < 8; i++)
			{
				SetShort(pData, 0, index); // Other special opts when it uses other table to get more
			}
			break;
	}
	return index;
}

int USER::AddPlusSpecialData(short sIndex, TCHAR* pData)
{
	int index = 0;
	if (sIndex >= 0 && sIndex < g_arPlusSpecialTable.GetSize())
	{
		SetShort(pData, g_arPlusSpecialTable[sIndex]->m_sSpecial, index);
		for (int i = 0; i < MAX_SPECIAL_OPT; i++)
		{
			SetShort(pData, g_arPlusSpecialTable[sIndex]->m_sSpOpt[i], index);
		}
	}
    return index;
}

short USER::PushItemInventory(ItemList* pItem)
{
	int i;
	short nInvSlotEmpty = -1, nInvSlotSame = -1;
	if (pItem == NULL) return -1;

	if (pItem->bType > TYPE_ACC)
	{
		for (i = EQUIP_ITEM_NUM; i < INV_ITEM_NUM; i++)
		{
			if (m_InvItem[i].sNum == pItem->sNum)
			{
				if (m_InvItem[i].sUsage >= MAX_ITEM_DURA) 
				{
					SendServerChatMessage(IDS_INV_FULL, TO_ME);
					return -1;
				}
				nInvSlotSame = i;
				break;
			}
		}

		if (nInvSlotSame >= 0)
		{
			if (m_InvItem[nInvSlotSame].sUsage + pItem->sUsage > MAX_ITEM_DURA)
			{
				short sItemCountLeft = m_InvItem[nInvSlotSame].sUsage + pItem->sUsage - MAX_ITEM_DURA;
				pItem->sUsage = sItemCountLeft;
				m_InvItem[nInvSlotSame].sUsage = MAX_ITEM_DURA;
			}
			else
			{
				PlusItemDur(&m_InvItem[nInvSlotSame], pItem->sUsage);
				pItem->sUsage = 0;
			}
			// TODO: Update item in database here
			SendItemInfoChange(BASIC_INV, nInvSlotSame, INFO_DUR);
			return nInvSlotSame;
		}
	}

	for (i = EQUIP_ITEM_NUM; i < INV_ITEM_NUM; i++)
	{
		if (m_InvItem[i].IsEmpty())
		{
			nInvSlotEmpty = i;
			break;
		}
	}

	if (nInvSlotEmpty >= 0)
	{
		m_InvItem[nInvSlotEmpty] = *pItem;
		pItem->sUsage = 0;
		// TODO: Update item in database here
		return nInvSlotEmpty;
	}

	SendServerChatMessage(IDS_INV_FULL, TO_ME);
	return -1;
}

void USER::PlusItemDur(ItemList* pItem, short Dura, bool bSubtract /*=false*/)
{
	ASSERT(pItem != NULL);
	if (pItem->sUsage <= 0) { pItem->sUsage = 0; return; }
	if (pItem->sUsage >= MAX_ITEM_DURA+1 && pItem->bType > TYPE_ACC) return; // max dura

	short ItemDura = pItem->sUsage;

	if (bSubtract) ItemDura -= Dura;
	else ItemDura += Dura;

	if (ItemDura <= MAX_ITEM_DURA)
	{
		pItem->sUsage = ItemDura;
	}
	else
	{
		pItem->sUsage = MAX_ITEM_DURA;
	}
}

void USER::SetPlusSpecial(ItemList* item)
{
	for (int i = 0; i < MAX_SPECIAL_OPT; i++)
	{
		item->PlusSpecial[i].sSpecial = 0;
		for (int j = 0; j < MAX_SPECIAL_OPT; j++)
		{
			item->PlusSpecial[i].sSpOpt[j] = 0;
		}
	}

	if (item->sSpecial != SPECIAL_PLUS)
	{
		return;
	}

	for (int i = 0; i < MAX_SPECIAL_OPT; i++)
	{
		if (item->sSpOpt[i] >= 0 && item->sSpOpt[i] < g_arPlusSpecialTable.GetSize())
		{
			item->PlusSpecial[i].sSpecial = g_arPlusSpecialTable[item->sSpOpt[i]]->m_sSpecial;
			for (int j = 0; j < MAX_SPECIAL_OPT; j++)
			{
				item->PlusSpecial[i].sSpOpt[j] = g_arPlusSpecialTable[item->sSpOpt[i]]->m_sSpOpt[j];
			}
		}
	}
}

bool USER::UserThrowItem(ItemList* pThrowItem, BYTE  MaxArea)
{
	if (m_iZoneIndex < 0 || m_iZoneIndex >= g_Zones.GetSize()) false;

	CPoint t;
	int i;
	int iX, iY;

	MAP *pMap = g_Zones[m_iZoneIndex];
	if (!pMap) return false;
	if (!pMap->m_pMap) return false;

	int dir[9][2];
	//	X					Y
	dir[0][0]  =  0;		dir[0][1] =  0;
	dir[1][0]  = -1;		dir[1][1] =  1;
	dir[2][0]  = -1;		dir[2][1] =  0;
	dir[3][0]  = -1;		dir[3][1] = -1;
	dir[4][0]  =  0;		dir[4][1] = -1;
	dir[5][0]  =  1;		dir[5][1] = -1;
	dir[6][0]  =  1;		dir[6][1] =  0;
	dir[7][0]  =  1;		dir[7][1] =  1;
	dir[8][0]  =  0;		dir[8][1] =  1;

	for (i = 0; i < 9; i++)
	{
		iX = m_sX + dir[i][0];
		iY = m_sY + dir[i][1];

		if (iX >= pMap->m_sizeMap.cx || iX < 0 || iY >= pMap->m_sizeMap.cy || iY < 0) continue;
		if (IsMovable_S(iX, iY))
		{
			if (g_Zones[m_iZoneIndex]->m_pMap[iX][iY].iIndex == -1)
			{
				if (InterlockedCompareExchangePointer((PVOID*)&g_Zones[m_iZoneIndex]->m_pMap[iX][iY].m_FieldUse, (PVOID)1, (PVOID)0) == (PVOID*)0)
				{
					m_pCom->DelThrowItem();
					if (!m_pCom->SetThrowItem(pThrowItem, iX, iY, m_iZoneIndex))
					{
						::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[iX][iY].m_FieldUse, 0);
						continue;
					}
					else
					{
						::InterlockedExchange(&g_Zones[m_iZoneIndex]->m_pMap[iX][iY].m_FieldUse, 0);
						goto Success_;
					}
				}
				else
				{
					continue;
				}
			}
		}
	}
	return false;

Success_:
	if (pThrowItem->bType == TYPE_MONEY)
	{
			CString str;
			str.Format(IDS_THROW_BARR, pThrowItem->dwMoney);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
	}
	else
	{
		if (pThrowItem->bType <= TYPE_ACC)
		{
			CString str;
			str.Format(IDS_THROW_ITEM, pThrowItem->strNewName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
		}
		else
		{
			CString str;
			str.Format(IDS_THROW_ITEMS, pThrowItem->sUsage, pThrowItem->strNewName);
			SendServerChatMessage((LPTSTR)(LPCTSTR)str, TO_ME);
		}
	}
	return true;
}

void USER::SendItemInfo(short nSlot)
{
	CBufferEx TempBuf;
	TempBuf.Add(PKT_ITEM_INFO);
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add(nSlot);
	//if (m_InvItem[nSlot].sPicNum != 0)
	//{
	//
	//
	TempBuf.Add(m_InvItem[nSlot].sPicNum);
	//}
	//else
	//{
	//TempBuf.Add((short)-1);
	//}

	// Need to check for remaining special attack (4) - Speed Up Aura
	if (CheckRemainSpecialAttack(SPECIAL_EFFECT_SPEED))
	{
		TempBuf.Add(m_RemainSpecial[SPECIAL_EFFECT_SPEED].sDamage);
	}
	else
	{
		if (m_InvItem[ARM_RHAND].IsEmpty())
		{
			TempBuf.Add((short)DEFAULT_ATTACK_DELAY);
		}
		else 
		{
			TempBuf.Add((short)m_InvItem[ARM_RHAND].sTime);
		}
	}

	SendInsight(TempBuf, TempBuf.GetLength());
}

void USER::SendItemInfoChange(BYTE Belt, short Slot, BYTE Type)
{
	if (Type <= 0) return;

	ItemList Item;
	if (Belt == BELT_INV)
	{
		if (Slot < 0 || Slot >= BELT_ITEM_NUM) return;
		Item = m_BeltItem[Slot];
	}
	else if (Belt == BASIC_INV)
	{
		if (Slot < 0 || Slot >= INV_ITEM_NUM) return;
		Item = m_InvItem[Slot];
	}
	else
	{
		return;
	}

	CItemTable* pItemTable;
	if (!g_mapItemTable.Lookup(Item.sNum, pItemTable)) return;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_ITEM_CHANGE_INFO);
	TempBuf.Add(Belt);
	TempBuf.Add(Slot);
	TempBuf.Add(Type);
	
	if (Type & INFO_TYPE)
	{
		TempBuf.Add(Item.bType);
		TempBuf.Add(pItemTable->m_bGender);
	}
	if (Type & INFO_ARM)
	{
		TempBuf.Add(Item.bArm);
		TempBuf.Add(Item.sWgt); // May be in info picnum
		TempBuf.Add(Item.sTime);
	}
	if (Type & INFO_PICNUM)
	{
		TempBuf.Add(GetItemClass(Item.sNum));
		TempBuf.Add(Item.sPicNum);
	}
	if (Type & INFO_DUR)
	{
		TempBuf.Add(Item.sUsage);
	}
	if (Type & INFO_NAME)
	{
		TempBuf.AddString((LPTSTR)(LPCTSTR)Item.strNewName);
		TempBuf.Add(Item.byUpgradeLevel);
	}
	if (Type & INFO_DAMAGE)
	{
		TempBuf.Add(pItemTable->m_sAb1);
		TempBuf.Add(pItemTable->m_sAb2);
	}
	if (Type & INFO_LIMIT)
	{
		TempBuf.Add(Item.sMaxUsage);
		TempBuf.Add(pItemTable->m_sDur);
		TempBuf.Add(Item.dwMoney);
		TempBuf.Add(pItemTable->m_sNeedStr);
		TempBuf.Add(pItemTable->m_sNeedInt);
		TempBuf.Add(pItemTable->m_sPicNum4);
		TempBuf.Add(pItemTable->m_sMinExp);
	}
	if (Type & INFO_SPECIAL)
	{
		TempBuf.Add(pItemTable->m_sHP);
		TempBuf.Add(pItemTable->m_sMP);
		int index = 0;
		index += GetSpecialItemData(Item.sNum, m_TempBuf);
		TempBuf.AddData(m_TempBuf, index);
	}

	Send(TempBuf, TempBuf.GetLength());
}

void USER::SendChangeWgt()
{	
	RecalcWgt();

	CBufferEx TempBuf;
	TempBuf.Add(PKT_WEIGHT);
	TempBuf.Add(m_sWgt);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::SendMoneyChanged()
{
	CBufferEx TempBuf;

	TempBuf.Add(PKT_MONEYCHANGE);
	TempBuf.Add(m_dwBarr);

	Send(TempBuf, TempBuf.GetLength());
}

void USER::SendItemFieldInfoToMe()
{
	int index = 0;

	CBufferEx TempBuf;

	if (m_ItemFieldInfoCount <= 0)
	{
		return;
	}

	if (m_ItemFieldInfoIndex <= 0)
	{
		return;
	}

	TempBuf.Add(PKT_ITEM_FIELD_INFO);
	TempBuf.Add((short)m_ItemFieldInfoCount);
	TempBuf.AddData(m_ItemFieldInfoBuf, m_ItemFieldInfoIndex);

	Send(TempBuf, TempBuf.GetLength());

	m_ItemFieldInfoCount = 0;
	memset(m_ItemFieldInfoBuf, NULL, 8192);
	m_ItemFieldInfoIndex = 0;
}

void USER::AddItemFieldInfoToMe(ItemList *pItem, BYTE type, int x, int y)
{
	if (!pItem && type == ITEM_INFO_MODIFY)
	{
		return;
	}

	if (m_ItemFieldInfoIndex >= 8000)
	{
		SendItemFieldInfoToMe();
	}

	SetByte(m_ItemFieldInfoBuf, type, m_ItemFieldInfoIndex);

	SetShort(m_ItemFieldInfoBuf, x, m_ItemFieldInfoIndex);
	SetShort(m_ItemFieldInfoBuf, y, m_ItemFieldInfoIndex);

	if (type == ITEM_INFO_MODIFY)
	{
		if (pItem->bType == TYPE_MONEY)
		{
			if (pItem->dwMoney >= 100 && pItem->dwMoney < 1000) pItem->sPicNum = 178;
			else if (pItem->dwMoney >= 1000) pItem->sPicNum = 179;
			SetShort(m_ItemFieldInfoBuf, pItem->sPicNum, m_ItemFieldInfoIndex);
			SetByte(m_ItemFieldInfoBuf, pItem->bType, m_ItemFieldInfoIndex);
			SetDWORD(m_ItemFieldInfoBuf, pItem->dwMoney, m_ItemFieldInfoIndex);
			SetVarString(m_ItemFieldInfoBuf, (LPTSTR)(LPCTSTR)pItem->strNewName, pItem->strNewName.GetLength(), m_ItemFieldInfoIndex);
		}
		else
		{
			SetShort(m_ItemFieldInfoBuf, pItem->sPicNum, m_ItemFieldInfoIndex);
			SetByte(m_ItemFieldInfoBuf, pItem->bType, m_ItemFieldInfoIndex);
			SetDWORD(m_ItemFieldInfoBuf, pItem->sUsage, m_ItemFieldInfoIndex);
			SetVarString(m_ItemFieldInfoBuf, (LPTSTR)(LPCTSTR)pItem->strNewName, pItem->strNewName.GetLength(), m_ItemFieldInfoIndex);
		}
	}

	m_ItemFieldInfoCount++;
}

void USER::SendItemFieldInfo(BYTE type, BYTE towho, ItemList *pItem, int x, int y)
{
	if (!pItem) return;

	CPoint t = ConvertToClient(x, y);
	if (t.x == -1 || t.y == -1)return;

	CBufferEx TempBuf;

	TempBuf.Add(PKT_ITEM_FIELD_INFO);
	TempBuf.Add((short)0x01);
	TempBuf.Add(type);
	TempBuf.Add((short)t.x);
	TempBuf.Add((short)t.y);

	if (type == ITEM_INFO_MODIFY)
	{
		if (pItem->bType == TYPE_MONEY)	
		{
			if (pItem->dwMoney >= 100 && pItem->dwMoney < 1000) pItem->sPicNum = 178;
			else if (pItem->dwMoney >= 1000) pItem->sPicNum = 179;

			TempBuf.Add(pItem->sPicNum);
			TempBuf.Add(pItem->bType);
			TempBuf.Add(pItem->dwMoney);
			TempBuf.Add((BYTE)pItem->strNewName.GetLength());
			TempBuf.AddData((LPTSTR)(LPCTSTR)pItem->strNewName, pItem->strNewName.GetLength());
		}
		else
		{
			TempBuf.Add(pItem->sPicNum);
			TempBuf.Add(pItem->bType);
			TempBuf.Add((int)pItem->sUsage);
			TempBuf.Add((BYTE)pItem->strNewName.GetLength());
			TempBuf.AddData((LPTSTR)(LPCTSTR)pItem->strNewName, pItem->strNewName.GetLength());
		}
	}

	// if (type == ITEM_INFO_PICKUP) TempBuf.Add((int)(m_uid + USER_BAND));

	switch (towho)
	{

	case TO_ALL:
		SendAll(TempBuf, TempBuf.GetLength());
		break;

	case TO_ME:
		Send(TempBuf, TempBuf.GetLength());
		break;

	case TO_ZONE:
		SendZone(TempBuf, TempBuf.GetLength());
		break;

	case TO_INSIGHT:
		SendInsight(TempBuf, TempBuf.GetLength());
		break;

	default:
		SendInsight(TempBuf, TempBuf.GetLength());
		break;

	}
}

bool USER::SpecialItemUse(short sItemNum, short sSlot, short sSpecial, short sOp1, short sOp2, short sOp3, bool &bItemUsed, int &iLightItemTime)
{
	// TODO: FINISH IMPLEMENTING ALL FUNCTIONS CALLED!
	CItemTable* pItemTable;
	if (!g_mapItemTable.Lookup(sItemNum, pItemTable)) return false;

	bool bShowPotionEffect = true;

	switch (sSpecial)
	{
	case SPECIAL_RUN_EVENT:
		{
			EVENT* pEvent = GetEventInCurrentZone();	if (!pEvent)break;

			if (!pEvent->m_arEvent[sOp1])break;

			if (!CheckEventLogic(pEvent->m_arEvent[sOp1])) break;

			for (int i = 0; i < pEvent->m_arEvent[sOp1]->m_arExec.GetSize(); i++)
			{
				if (!RunNpcEvent(NULL, pEvent->m_arEvent[sOp1]->m_arExec[i]))
				{
					return false;
				}
			}
		}
		break;
	case SPECIAL_LIGHT:
		m_iLightItemNo = sItemNum;
		m_dwLightItemTime = GetTickCount();
		iLightItemTime = sOp1;
		if (sOp2 == 1)
		{
			EffectFire();
		}
		bShowPotionEffect = false;
		break;
	case SPECIAL_ANTIDOTE:
		// Cure any poisons here...
		m_sRemainMagicNo[MAGIC_EFFECT_POISON] = -1;
		m_dwRemainMagicTime[MAGIC_EFFECT_POISON] = 0;
		SendUserStatus(5);
		break;
	case SPECIAL_PORTAL:
		if (!CheckTownPortal(sItemNum)) return false;
		// TODO : CheckTownPortalLevel
		if (m_bIsUsingWarpItem) return false;
		if (CheckRemainSpecialAttack(SPECIAL_EFFECT_PARALYZE)) return false;
		if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION)) return false;
		m_bIsUsingWarpItem = true;
		TownPortalStart(0, sItemNum);
		break;
	case SPECIAL_TOWN_PORTAL:
		if (!CheckTownPortalFixed()) return false;
		// TODO : CheckTownPortalLevel
		if (m_bIsUsingWarpItem) return false;
		if (CheckRemainSpecialAttack(SPECIAL_EFFECT_PARALYZE)) return false;
		if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION)) return false;
		m_bIsUsingWarpItem = true;
		TownPortalStart(1);
		break;
	case SPECIAL_MORAL_ADD:
		{
		if (pItemTable->m_sAb1 <= 0) return false;
		int iMoral = m_iMoral / CLIENT_MORAL;
		if (iMoral >= 0)
		{
			SendServerChatMessage(IDS_ITEM_MORAL_HIGH, TO_ME);
			return false;
		}
		int iMoralAdd = pItemTable->m_sAb1;
		if (iMoral + iMoralAdd > 0)
			iMoralAdd = abs(iMoral);
		if (SetMoral(iMoralAdd * CLIENT_MORAL))
		{
			SendCharData(INFO_BASICVALUE);
			SendMyInfo(TO_INSIGHT, INFO_MODIFY);
		}
		}
		break;
	case SPECIAL_COMPASS:
		return false;
		break;
	//case SPECIAL_POS_GEM:
	//	break;
	case SPECIAL_GBIRD_FEATHER:
		{
			if (IsMapRange(MAP_AREA_DUEL)) return false;
			if (IsMapRange(MAP_AREA_HSOMA_GV)) return false;
			if (IsMapRange(MAP_AREA_DSOMA_GV)) return false;
			if (CheckRemainSpecialAttack(SPECIAL_EFFECT_PARALYZE)) return false;
			if (CheckRemainSpecialAttack(SPECIAL_EFFECT_CONCUSSION)) return false;
			
			// Find a random position based on the item's special options
			CPoint pt = ConvertToClient(m_sX, m_sY);
			if (pt.x == -1 || pt.y == -1) return false;
			short sNewX = -1;
			short sNewY = -1;
			for (int i = 0; i < 10; i++)
			{
				short sRandX = myrand(pt.x - sOp1, pt.x + sOp1);
				short sRandY = myrand(pt.y - sOp1, pt.y + sOp1);
				
				if (sRandX < pt.x)
				{
					if (sRandX > pt.x - sOp2)
					{
						sRandX = myrand(pt.x - sOp1, pt.x - sOp2);
					}
				}
				else 
				{
					if (sRandX < (pt.x + sOp2))
					{
						sRandX = myrand(pt.x + sOp2, pt.x + sOp1);
					}
				}

				if (sRandY < pt.y)
				{
					if (sRandY > pt.y - sOp2)
					{
						sRandY = myrand(pt.y - sOp1, pt.y - sOp2);
					}
				}
				else 
				{
					if (sRandY < (pt.y + sOp2))
					{
						sRandY = myrand(pt.y + sOp2, pt.y + sOp1);
					}
				}

				if (IsMovable_C(sRandX, sRandY))
				{
					CPoint pt = ConvertToServer(sRandX, sRandY);
					if (pt.x != -1 && pt.y != -1)
					{
						if (!IsMapRange(MAP_AREA_HSOMA_GV, sNewX, sNewY) && 
							!IsMapRange(MAP_AREA_DSOMA_GV, sNewX, sNewY))
						{
							sNewX = sRandX;
							sNewY = sRandY;
							break;
						}
					}
				}
			}
			
			// Failed to find a place to move to
			if (sNewX == -1 || sNewY == -1)
			{
				return false;
			}

			// Success, now move the user
			LinkToSameZone(sNewX, sNewY);
			ShowMagic(29, 0);
			bShowPotionEffect = false;
		}
		break;
	case SPECIAL_STAT_MOD_HP:
		ChangeExtStat(m_sMaxHP, sOp1);
		break;
	case SPECIAL_STAT_MOD_MP:
		ChangeExtStat(m_sMaxMP, sOp1);
		break;
	case SPECIAL_STAT_MOD_STM:
		ChangeExtStat(m_sMaxStm, sOp1);
		break;
	case SPECIAL_STAT_MOD_WGT:
		ChangeExtStat(m_sMaxWgt, sOp1);
		break;
	case SPECIAL_LOTTERY:
		return false;
		break;
	case SPECIAL_STM_RECOVER:
		CheckMaxValue(m_sStm, sOp1);
		if (m_sStm < 0) m_sStm = 0;
		if (m_sStm > GetMaxStm()) m_sStm = GetMaxStm();
		SendCharData(INFO_EXTVALUE);
		break;
	case SPECIAL_DETOX_BLUE:
		InitRemainMagic(MAGIC_CLASS_BLUE);
		SendUserStatus(MAGIC_CLASS_BLUE);
		break;
	case SPECIAL_DETOX_WHITE:
		InitRemainMagic(MAGIC_CLASS_WHITE);
		SendUserStatus(MAGIC_CLASS_WHITE);
		break;
	case SPECIAL_DETOX_BLACK:
		InitRemainMagic(MAGIC_CLASS_BLACK);
		SendUserStatus(MAGIC_CLASS_BLACK);
		break;
	//case SPECIAL_REVIVE:
	//	break;
	//case SPECIAL_REPAIR:
	//	break;
	//case SPECIAL_SMELTING:
	//	break;
	case SPECIAL_COOKING:
		{
			int iMakers[6] = {sOp1, sOp2, -1, -1, -1, -1};
			SendMakerOpen(iMakers);
		}
		break;
	//case SPECIAL_UPGRADE:
	//	break;
	//case SPECIAL_SPEC_REPAIR:
	//	break;
	case SPECIAL_FORTUNE_POUCH:
		return ChangeRandItem(sOp1);
		break;
	//case SPECIAL_PLUS: // TODO: Looks up table plus special..
	//	{
	//		int sOp = sOp1;
	//		if (sOp >= 0 && sOp < g_arPlusSpecialTable.GetSize())
	//		{
	//			SpecialItemUse(sItemNum, g_arPlusSpecialTable[sOp]->m_sSpecial, g_arPlusSpecialTable[sOp]->m_sSpOpt[0],
	//					g_arPlusSpecialTable[sOp]->m_sSpOpt[1], g_arPlusSpecialTable[sOp]->m_sSpOpt[2]);
	//		}
	//		sOp = sOp2;
	//		if (sOp >= 0 && sOp < g_arPlusSpecialTable.GetSize())
	//		{
	//			SpecialItemUse(sItemNum, g_arPlusSpecialTable[sOp]->m_sSpecial, g_arPlusSpecialTable[sOp]->m_sSpOpt[0],
	//					g_arPlusSpecialTable[sOp]->m_sSpOpt[1], g_arPlusSpecialTable[sOp]->m_sSpOpt[2]);
	//		}
	//		sOp = sOp3;
	//		if (sOp >= 0 && sOp < g_arPlusSpecialTable.GetSize())
	//		{
	//			SpecialItemUse(sItemNum, g_arPlusSpecialTable[sOp]->m_sSpecial, g_arPlusSpecialTable[sOp]->m_sSpOpt[0],
	//					g_arPlusSpecialTable[sOp]->m_sSpOpt[1], g_arPlusSpecialTable[sOp]->m_sSpOpt[2]);
	//		}
	//	}
	//	break;
	case SPECIAL_GENDER_CHANGE:
		return ChangeGender();
		break;
	case SPECIAL_SKIN_CHANGE:
		return false;
		break;
	//case SPECIAL_SHOUT:
	//	break;
	case SPECIAL_SPEC_GEM:
		bItemUsed = false;
		m_sChangeOtherItemSlot = sSlot;
		return OpenChangeOtherItem(sOp1);
		break;
	//case SPECIAL_SINGLE_SHOUT:
	//	break;
	//case SPECIAL_HAIR_DYE:
	//	break;
	case SPECIAL_RANGE_CHANGE:
		return false;
		break;
	default:
		break;
	}	

	if (pItemTable->m_bType == TYPE_POTION && bShowPotionEffect)
	{
		BYTE byR = static_cast<BYTE>(sOp1);
		BYTE byG = static_cast<BYTE>(sOp2);
		BYTE byB = static_cast<BYTE>(sOp3);
		EffectPotion(byR, byG, byB);
	}

	return true;
}

void USER::EffectPotion(BYTE byR, BYTE byG, BYTE byB)
{
	CBufferEx TempBuf;
	TempBuf.Add(PKT_EFFECT);
	TempBuf.Add(static_cast<BYTE>(1)); // type potion
	TempBuf.Add(m_Uid + USER_BAND);
	TempBuf.Add(byR);
	TempBuf.Add(byG);
	TempBuf.Add(byB);
	Send(TempBuf, TempBuf.GetLength());
}

void USER::SendGetItem(short sSlot)
{
	CItemTable* pItem = NULL;
	if (sSlot < 0 || sSlot >= INV_ITEM_NUM) return;
	if (!g_mapItemTable.Lookup(m_InvItem[sSlot].sNum, pItem)) return;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_GETITEM);
	TempBuf.Add(sSlot);
	TempBuf.Add(m_InvItem[sSlot].bType);
	TempBuf.Add(m_InvItem[sSlot].bArm);
	TempBuf.Add(GetItemClass(m_InvItem[sSlot].sNum)); // class
	TempBuf.Add(m_InvItem[sSlot].sPicNum);
	TempBuf.Add(m_InvItem[sSlot].bGender);
	TempBuf.Add(m_InvItem[sSlot].sAb1);
	TempBuf.Add(m_InvItem[sSlot].sAb2);
	TempBuf.Add(m_InvItem[sSlot].sWgt);
	TempBuf.Add(m_InvItem[sSlot].sTime);
	TempBuf.Add(m_InvItem[sSlot].dwMoney);
	TempBuf.Add(m_InvItem[sSlot].sNeedStr);
	TempBuf.Add(m_InvItem[sSlot].sNeedInt);
	TempBuf.Add(m_InvItem[sSlot].sNeedDex);
	TempBuf.Add(m_InvItem[sSlot].sNeedSkill);
	TempBuf.Add(m_InvItem[sSlot].sHP);
	TempBuf.Add(m_InvItem[sSlot].sMP);
	TempBuf.Add(m_InvItem[sSlot].sUsage);
	TempBuf.Add(m_InvItem[sSlot].sMaxUsage);
	TempBuf.Add(m_InvItem[sSlot].sDur);
	TempBuf.Add((BYTE)m_InvItem[sSlot].strNewName.GetLength());
	TempBuf.AddData((LPTSTR)(LPCTSTR)m_InvItem[sSlot].strNewName, m_InvItem[sSlot].strNewName.GetLength());
	TempBuf.Add(m_InvItem[sSlot].byUpgradeLevel); // Upgrade level
	TempBuf.AddData(m_TempBuf, GetSpecialItemData(m_InvItem[sSlot].sNum, m_TempBuf));
	Send(TempBuf, TempBuf.GetLength());
}

///////////////////////////////////////////////////////////////////////////////
//	Returns the index of an item based on nSlot that is the same as Item and is stackable.
//
int USER::GetSameItem(ItemList Item, int nSlot)
{
	// TODO : Implement the other slots...
	int i = 0;

	CItemTable* pItem = NULL;

	if (!g_mapItemTable.Lookup(Item.sNum, pItem)) return -1;
	if (pItem->m_bType <= TYPE_ACC || pItem->m_bType == TYPE_UNSTACKABLE) return -1; // Equipment or non stackable... so no need to find the same item to stack.

	switch (nSlot)
	{
	case INVENTORY_SLOT:
		for (i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INV_ITEM_NUM; i++)
		{
			if (m_InvItem[i].sNum == Item.sNum && m_InvItem[i].sUsage < MAX_ITEM_DURA)  // sNum is a unique identifier.. so should only need to do this 1 comparison..
			{
				return i;
			}
		}
		break;
	case TRADE_SLOT:
		EnterCriticalSection(&m_CS_ExchangeItem);
		for (i = 0; i < m_arExchangeItem.GetSize(); i++)
		{
			if (m_arExchangeItem[i] == NULL)continue;

			if (m_InvItem[m_arExchangeItem[i]->sSlot].sNum == Item.sNum && m_InvItem[m_arExchangeItem[i]->sSlot].sUsage < MAX_ITEM_DURA)
			{
				LeaveCriticalSection(&m_CS_ExchangeItem);
				return i;
			}
		}
		LeaveCriticalSection(&m_CS_ExchangeItem);
		break;
	case STORAGE_SLOT:
		for (i = 0; i < STORAGE_ITEM_NUM; i++)
		{
			if (m_StorageItem[i].sNum == Item.sNum && m_StorageItem[i].sUsage < MAX_ITEM_DURA)  
			{
				return i;
			}
		}
		break;
	case GUILD_SLOT:
		for (i = 0; i < GUILD_STORAGE_ITEM_NUM; i++)
		{
			if (m_GuildItem[i].sNum == Item.sNum && m_GuildItem[i].sUsage < MAX_ITEM_DURA)  
			{
				return i;
			}
		}
		break;
	default:
		break;
	}

	return -1;
}

int USER::GetEmptySlot(int nSlot)
{
	int i = 0;

	switch (nSlot)
	{
	case EQUIP_SLOT:
		for (i = 0; i < EQUIP_ITEM_NUM; i++)
		{
			if (m_InvItem[i].IsEmpty()) return i;
		}
		break;
	case INVENTORY_SLOT:
		for (i = EQUIP_ITEM_NUM; i < EQUIP_ITEM_NUM + INV_ITEM_NUM; i++)
		{
			if (m_InvItem[i].IsEmpty()) return i;
		}
		break;
	case STORAGE_SLOT:
		for (i = 0; i < STORAGE_ITEM_NUM; i++)
		{
			if (m_StorageItem[i].IsEmpty()) return i;
		}
		break;
	case GUILD_SLOT:
		for (i = 0; i < GUILD_STORAGE_ITEM_NUM; i++)
		{
			if (m_GuildItem[i].IsEmpty()) return i;
		}
		break;
	default:
		break;
	}

	return -1;
}

void USER::SendDeleteItem(BYTE Belt, short Slot)
{
	CBufferEx TempBuf;
	TempBuf.Add(PKT_OUTITEM);
	TempBuf.Add(Belt);
	TempBuf.Add(Slot);
	Send(TempBuf, TempBuf.GetLength());
}

int USER::GetMaxStorageCount()
{
	if (m_sLevel < 10)
	{
		return 40;
	}
	else if (m_sLevel >= 10 && m_sLevel < 40)
	{
		return 42 + 2 * ((m_sLevel-10) / 6);
	}
	else if (m_sLevel >= 40 && m_sLevel < 80)
	{
		return 52 + 2 * ((m_sLevel-40) / 5);
	}
	else if (m_sLevel >= 80 && m_sLevel < 100)
	{
		return 68 + 2 * ((m_sLevel-80) / 4);
	}
	else
	{
		return 78;
	}
}

CStore* USER::GetStore(int nStore)
{
	for (int i = 0; i < g_arStore.GetSize(); i++)
	{
		if (g_arStore[i] == NULL) continue;

		if (g_arStore[i]->m_sStoreID == (short)nStore)
		{
			return g_arStore[i];
		}
	}

	return NULL;	
}

int USER::GetItemWeight(int sNum, int sCount)
{
	CItemTable* pItem = NULL;
	if (sNum > 0)
	{
		if (g_mapItemTable.Lookup(sNum, pItem))
		{
			if (pItem->m_bType > TYPE_ACC)
			{
				return pItem->m_sWgt * sCount;
			}
			else
			{
				return pItem->m_sWgt;
			}
		}
	}
	return 0;
}

bool USER::CanEquipItem(ItemList* pItem)
{
	// Gender
	if (pItem->bGender == ITEM_GENDER_FEMALE && m_sGender != GENDER_FEMALE) return false;
	if (pItem->bGender == ITEM_GENDER_MALE && m_sGender != GENDER_MALE) return false;

	// Stats
	int iStr = m_iSTR / CLIENT_BASE_STATS;
	if (pItem->sNeedStr > iStr) return false;

	int iInt = m_iINT / CLIENT_BASE_STATS;
	if (pItem->sNeedInt > iInt) return false;

	int iDex = m_iDEX / CLIENT_EXT_STATS;
	if (pItem->sNeedDex > iDex) return false;

	int iSkill = 0;

	switch (pItem->bType)
	{
	case TYPE_SWORD:
		iSkill = m_iSwordExp / CLIENT_SKILL;
		break;
	case TYPE_AXE:
		iSkill = m_iAxeExp / CLIENT_SKILL;
		break;
	case TYPE_BOW:
		iSkill = m_iBowExp / CLIENT_SKILL;
		break;
	case TYPE_SPEAR:
		iSkill = m_iSpearExp / CLIENT_SKILL;
		break;
	case TYPE_KNUCKLE:
		iSkill = m_iKnuckleExp / CLIENT_SKILL;
		break;
	case TYPE_STAFF:
		iSkill = m_iStaffExp / CLIENT_SKILL;
		break;
	default:
		break;
	}

	if (pItem->sNeedSkill > iSkill) return false;

	return true;
}

int USER::PlusFromItem(int specialNo)
{
	// Loop through all equipment items looking for all items with special no
	// values are added and then returned.
	
	int valueAdd = 0;

	for (int i = 0; i < EQUIP_ITEM_NUM; i++)
	{
		// Special stats are not added if wearing another race's item.
		if (CheckDemon(m_sClass) != m_InvItem[i].IsDemonItem()) continue;

		// If the item has a specialNo of 'SPECIAL_PLUS' we need to check the plus special instead.
		if (!m_InvItem[i].IsPlusSpecial())
		{
			if (m_InvItem[i].sSpecial == specialNo)
			{
				int temp = 0;
				if (m_InvItem[i].sSpOpt[1] != 0)
				{
					temp += myrand(m_InvItem[i].sSpOpt[0], m_InvItem[i].sSpOpt[1]);
				}
				else
				{
					temp += m_InvItem[i].sSpOpt[0];
				}

				if (m_InvItem[i].sSpOpt[2] != 0)
				{
					int rand = myrand(1, 100);
					if (rand > m_InvItem[i].sSpOpt[2])
					{
						temp = 0;
					}
				}

				valueAdd += temp;
			}
		}
		else
		{
			for (int j = 0; j < MAX_SPECIAL_OPT; j++)
			{
				if (m_InvItem[i].PlusSpecial[j].sSpecial == specialNo)
				{
					int temp = 0;
					if (m_InvItem[i].PlusSpecial[j].sSpOpt[1] != 0)
					{
						temp += myrand(m_InvItem[i].PlusSpecial[j].sSpOpt[0], m_InvItem[i].PlusSpecial[j].sSpOpt[1]);
					}
					else
					{
						temp += m_InvItem[i].PlusSpecial[j].sSpOpt[0];
					}

					if (m_InvItem[i].PlusSpecial[j].sSpOpt[2] != 0)
					{
						int rand = myrand(1, 100);
						if (rand > m_InvItem[i].PlusSpecial[j].sSpOpt[2])
						{
							temp = 0;
						}
					}

					valueAdd += temp;
				}
			}
		}
	}

	return valueAdd;
}

void USER::NpcThrowItem(CNpc *pNpc)
{	
	if (!pNpc) return;
	if (pNpc->m_ZoneIndex < 0 || pNpc->m_ZoneIndex >= g_Zones.GetSize()) return;

	CPoint t;
	int iX, iY;

	MAP *pMap = g_Zones[pNpc->m_ZoneIndex];
	if (!pMap) return;
	if (!pMap->m_pMap) return;

	int dir[25][2];
	//	X					Y
	dir[0][0]  =  0;		dir[0][1] =  0;
	dir[1][0]  = -1;		dir[1][1] =  1;
	dir[2][0]  = -1;		dir[2][1] =  0;
	dir[3][0]  = -1;		dir[3][1] = -1;
	dir[4][0]  =  0;		dir[4][1] = -1;
	dir[5][0]  =  1;		dir[5][1] = -1;
	dir[6][0]  =  1;		dir[6][1] =  0;
	dir[7][0]  =  1;		dir[7][1] =  1;
	dir[8][0]  =  0;		dir[8][1] =  1;
	dir[9][0]  =  -2;		dir[9][1] =  2;
	dir[10][0]  =  -2;		dir[10][1] =  1;
	dir[11][0]  =  -2;		dir[11][1] =  0;
	dir[12][0]  =  -2;		dir[12][1] =  -1;
	dir[13][0]  =  -2;		dir[13][1] =  -2;
	dir[14][0]  =  -1;		dir[14][1] =  -2;
	dir[15][0]  =  0;		dir[15][1] =  -2;
	dir[16][0]  =  1;		dir[16][1] =  -2;
	dir[17][0]  =  2;		dir[17][1] =  -2;
	dir[18][0]  =  2;		dir[18][1] =  -1;
	dir[19][0]  =  2;		dir[19][1] =  0;
	dir[20][0]  =  2;		dir[20][1] =  1;
	dir[21][0]  =  2;		dir[21][1] =  2;
	dir[22][0]  =  1;		dir[22][1] =  2;
	dir[23][0]  =  0;		dir[23][1] =  2;
	dir[24][0]  =  -1;		dir[24][1] =  2;

	for (int i = 0; i < pNpc->m_arItem.GetSize(); i++)
	{
		for (int j = 0; j < 25; j++)
		{
			iX = pNpc->m_sCurX + dir[j][0];
			iY = pNpc->m_sCurY + dir[j][1];

			if (iX >= pMap->m_sizeMap.cx || iX < 0 || iY >= pMap->m_sizeMap.cy || iY < 0) continue;
			if (IsMovable_S(iX, iY))
			{
				if (g_Zones[pNpc->m_ZoneIndex]->m_pMap[iX][iY].iIndex == -1)
				{
					if (InterlockedCompareExchangePointer((PVOID*)&g_Zones[pNpc->m_ZoneIndex]->m_pMap[iX][iY].m_FieldUse, (PVOID)1, (PVOID)0) == (PVOID*)0)
					{
						ItemList* pThrowItem = new ItemList;
						*pThrowItem = *pNpc->m_arItem[i];

						m_pCom->DelThrowItem();
						if (!m_pCom->SetThrowItem(pThrowItem, iX, iY, pNpc->m_ZoneIndex))
						{
							::InterlockedExchange(&g_Zones[pNpc->m_ZoneIndex]->m_pMap[iX][iY].m_FieldUse, 0);
							delete pThrowItem;
							pThrowItem = NULL;
							continue;
						}
						else
						{
							::InterlockedExchange(&g_Zones[pNpc->m_ZoneIndex]->m_pMap[iX][iY].m_FieldUse, 0);
							break;
						}
					}
					else
					{
						continue;
					}
				}
			}
		}
	}

	// Delete NPC Items
	for (int i = 0; i <pNpc-> m_arItem.GetSize(); i++)
	{
		if (pNpc->m_arItem[i]) 
		{
			delete pNpc->m_arItem[i];
		}
	}
	pNpc->m_arItem.RemoveAll();
}

void USER::NpcThrowMoney(CNpc *pNpc)
{	
	// Validate Npc
	if (!pNpc) return;
	if (pNpc->IsDetecter() || pNpc->IsStone()) return;
	
	// Calculate if money is going to drop
	int MoneyRate = myrand(1, 100);
	if (MoneyRate > pNpc->m_sMoneyRate) return;

	// Calculate amount of money to be dropped
	DWORD DropMoney = myrand(pNpc->m_sMoneyMin, pNpc->m_sMoneyMax);
	if (DropMoney == 0) return;

	// Is the drop type of the money a pile? (more than one)
	int MaxMoneyPile = 1;
	if (pNpc->m_sMoneyType == 1) 
	{
		MaxMoneyPile = myrand(3, 10);
	}
	DropMoney /= MaxMoneyPile;

	if (pNpc->m_ZoneIndex < 0 || pNpc->m_ZoneIndex >= g_Zones.GetSize()) return;

	CPoint t;
	int iX, iY;

	MAP *pMap = g_Zones[pNpc->m_ZoneIndex];
	if (!pMap) return;
	if (!pMap->m_pMap) return;

	int dir[25][2];
	//	X					Y
	dir[0][0]  =  0;		dir[0][1] =  0;
	dir[1][0]  = -1;		dir[1][1] =  1;
	dir[2][0]  = -1;		dir[2][1] =  0;
	dir[3][0]  = -1;		dir[3][1] = -1;
	dir[4][0]  =  0;		dir[4][1] = -1;
	dir[5][0]  =  1;		dir[5][1] = -1;
	dir[6][0]  =  1;		dir[6][1] =  0;
	dir[7][0]  =  1;		dir[7][1] =  1;
	dir[8][0]  =  0;		dir[8][1] =  1;
	dir[9][0]  =  -2;		dir[9][1] =  2;
	dir[10][0]  =  -2;		dir[10][1] =  1;
	dir[11][0]  =  -2;		dir[11][1] =  0;
	dir[12][0]  =  -2;		dir[12][1] =  -1;
	dir[13][0]  =  -2;		dir[13][1] =  -2;
	dir[14][0]  =  -1;		dir[14][1] =  -2;
	dir[15][0]  =  0;		dir[15][1] =  -2;
	dir[16][0]  =  1;		dir[16][1] =  -2;
	dir[17][0]  =  2;		dir[17][1] =  -2;
	dir[18][0]  =  2;		dir[18][1] =  -1;
	dir[19][0]  =  2;		dir[19][1] =  0;
	dir[20][0]  =  2;		dir[20][1] =  1;
	dir[21][0]  =  2;		dir[21][1] =  2;
	dir[22][0]  =  1;		dir[22][1] =  2;
	dir[23][0]  =  0;		dir[23][1] =  2;
	dir[24][0]  =  -1;		dir[24][1] =  2;

	for (int i = 0; i < MaxMoneyPile; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			iX = pNpc->m_sCurX + dir[j][0];
			iY = pNpc->m_sCurY + dir[j][1];

			if (iX >= pMap->m_sizeMap.cx || iX < 0 || iY >= pMap->m_sizeMap.cy || iY < 0) continue;
			if (IsMovable_S(iX, iY))
			{
				if (g_Zones[pNpc->m_ZoneIndex]->m_pMap[iX][iY].iIndex == -1)
				{
					if (InterlockedCompareExchangePointer((PVOID*)&g_Zones[pNpc->m_ZoneIndex]->m_pMap[iX][iY].m_FieldUse, (PVOID)1, (PVOID)0) == (PVOID*)0)
					{
						// Create the money item
						ItemList* pThrowItem = new ItemList;
						if (pThrowItem != NULL && !pThrowItem->InitFromItemTable(TYPE_MONEY_NUM))
						{
							continue;
						}

						// Adds money to the money item to be put onto the map and checks that it doesn't go over the max etc
						CheckMaxValue((DWORD &)pThrowItem->dwMoney, DropMoney);

						m_pCom->DelThrowItem();
						if (!m_pCom->SetThrowItem(pThrowItem, iX, iY, pNpc->m_ZoneIndex))
						{
							::InterlockedExchange(&g_Zones[pNpc->m_ZoneIndex]->m_pMap[iX][iY].m_FieldUse, 0);
							delete pThrowItem;
							pThrowItem = NULL;
							continue;
						}
						else
						{
							::InterlockedExchange(&g_Zones[pNpc->m_ZoneIndex]->m_pMap[iX][iY].m_FieldUse, 0);
							break;
						}
					}
					else
					{
						continue;
					}
				}
			}
		}
	}
}

void USER::GiveBasicItem()
{
	short sWeaponItemNum = 0;
	switch (m_sClass)
	{
	case 0:
		if (m_sStartStr == 11 && m_sStartDex == 12)
		{
			sWeaponItemNum = 172; // Training Knuckle
		}
		else if (m_sStartStr == 12 && m_sStartDex == 11)
		{
			sWeaponItemNum = 168; // Training Spear
		}
		else if (m_sStartDex >= 12)
		{
			sWeaponItemNum = 169; // Training Bow
		}
		else if (m_sStartInt >= 12)
		{
			sWeaponItemNum = 170; // Training Staff
		}
		else if (m_sStartWis >= 12)
		{
			sWeaponItemNum = 170; // Training Staff
		}
		else if (m_sStartStr == 13)
		{
			sWeaponItemNum = 171; // Training Axe
		}
		else
		{
			sWeaponItemNum = 168; // Training Sword
		}
		break;
	case 11:
		sWeaponItemNum = 2001; // Training Sword
		break;
	case 12:
		sWeaponItemNum = 2099; // Training Spear
		break;
	case 13:
		sWeaponItemNum = 2124; // Training Bow
		break;
	case 14:
		sWeaponItemNum = 2050; // Training Axe
		break;
	case 15:
		sWeaponItemNum = 2173; // Training Knuckle
		break;
	case 16:
		sWeaponItemNum = 2198; // Training Staff
		break;
	default:
		break;
	}

	// Give Weapon Item
	ItemList GiveItem;
	GiveItem.InitFromItemTable(sWeaponItemNum);
	PushItemInventory(&GiveItem);

	if (!CheckDemon(m_sClass))
	{
		// Give HSOMA Armor
		if (m_sGender == GENDER_FEMALE)
		{
			short sArmorItemNum[3] = {176, 177, 178}; 
			for (int i = 0; i < 3; i++)
			{
				ItemList GiveItem;
				GiveItem.InitFromItemTable(sArmorItemNum[i]);
				PushItemInventory(&GiveItem);
			}
		}
		else if (m_sGender == GENDER_MALE)
		{
			short sArmorItemNum[3] = {173, 174, 175}; 
			for (int i = 0; i < 3; i++)
			{
				ItemList GiveItem;
				GiveItem.InitFromItemTable(sArmorItemNum[i]);
				PushItemInventory(&GiveItem);
			}
		}

		// Give HSOMA Potions
		short sPotionItemNum[2] = {1097, 1098}; 
		for (int i = 0; i < 2; i++)
		{
			ItemList GiveItem;
			GiveItem.InitFromItemTable(sPotionItemNum[i]);
			GiveItem.sUsage = 15;
			PushItemInventory(&GiveItem);
		}
	}
	else
	{
		// Give DSOMA Potions
		short sPotionItemNum[2] = {2500, 2501}; 
		for (int i = 0; i < 2; i++)
		{
			ItemList GiveItem;
			GiveItem.InitFromItemTable(sPotionItemNum[i]);
			GiveItem.sUsage = 15;
			PushItemInventory(&GiveItem);
		}
	}

	SendChangeWgt();
}

void USER::DeadThrowItem(bool bCheckMoral, bool bSendItemDelete)
{
	if (g_bClassWar) return;

	int iMoral = m_iMoral / CLIENT_MORAL;
	int iRandMax = 0;
	
	// bEquipLessChance if true will reduce the chance of equipment being dropped
	// because the max rand is increased.
	if (bCheckMoral)
	{
		if (!CheckDemon(m_sClass))
		{
			if (iMoral <= -50)
			{
				iRandMax = 3;
			}
			else if (iMoral <= -31)
			{
				iRandMax = 4;
			}
			else if (iMoral <= -11)
			{
				iRandMax = 5;
			}
			else if (iMoral <= -4)
			{
				iRandMax = 6;
			}
			else if (iMoral <= 3)
			{
				iRandMax = 7;
			}
			else if (iMoral <= 10)
			{
				iRandMax = 8;
			}
			else if (iMoral <= 30)
			{
				iRandMax = 9;
			}
			else if (iMoral <= 49)
			{
				iRandMax = 10;
			}
			else if (iMoral <= 74)
			{
				iRandMax = 11;
			}
		}
		else
		{
			if (iMoral <= -50)
			{
				iRandMax = 11;
			}
			else if (iMoral <= -31)
			{
				iRandMax = 10;
			}
			else if (iMoral <= -11)
			{
				iRandMax = 9;
			}
			else if (iMoral <= -4)
			{
				iRandMax = 8;
			}
			else if (iMoral <= 3)
			{
				iRandMax = 7;
			}
			else if (iMoral <= 10)
			{
				iRandMax = 6;
			}
			else if (iMoral <= 30)
			{
				iRandMax = 5;
			}
			else if (iMoral <= 49)
			{
				iRandMax = 4;
			}
			else if (iMoral <= 74)
			{
				iRandMax = 3;
			}
		}
	}

	iRandMax += 9;

	// Drop items from equipment
	int iEquipSlot = myrand(0, iRandMax);
	if (iEquipSlot < 10)
	{
		if (!m_InvItem[iEquipSlot].IsEmpty())
		{
			if (CanTradeItem(m_InvItem[iEquipSlot]))
			{
				ItemList* pMapItem = new ItemList;
				*pMapItem = m_InvItem[iEquipSlot];

				if (UserThrowItem(pMapItem, 25))
				{
					// TODO: DeadThrowItem: UpdateItemIndex and write item log
					m_InvItem[iEquipSlot].Init();
					if (bSendItemDelete)
					{
						SendDeleteItem(BASIC_INV, iEquipSlot);
					}
					SendItemInfo(iEquipSlot);
				}
				else
				{
					delete pMapItem;
					pMapItem = NULL;
				}
			}
		}
	}

	// Drop Items from inventory
	iRandMax = 0;
	if (bCheckMoral)
	{
		if (iMoral <= -9)
		{
			iRandMax = 3;
		}
		else if (iMoral >= -8 && iMoral <= 10)
		{
			iRandMax = 2;
		}
		else
		{
			iRandMax = 1;
		}
	}	
	iRandMax += 49;

	int iInvSlot = myrand(10, iRandMax);
	if (iInvSlot < 50)
	{
		if (!m_InvItem[iInvSlot].IsEmpty())
		{
			if (CanTradeItem(m_InvItem[iInvSlot]))
			{
				ItemList* pMapItem = new ItemList;
				*pMapItem = m_InvItem[iInvSlot];

				if (m_InvItem[iInvSlot].bType > TYPE_ACC)
				{
					if (m_InvItem[iInvSlot].sUsage > 5)
					{
						int iUsage = ((myrand(20, 50) * m_InvItem[iInvSlot].sUsage) / 100);
						pMapItem->sUsage = iUsage;
						PlusItemDur(&m_InvItem[iInvSlot], iUsage, true); // Sets dura of item minus the amount being droped					
					}
					else
					{
						m_InvItem[iInvSlot].Init();
					}
				}
				else
				{
					m_InvItem[iInvSlot].Init();
				}

				if (UserThrowItem(pMapItem, 25))
				{
					if (bSendItemDelete)
					{
						SendDeleteItem(BASIC_INV, iInvSlot);
					}
					SendItemInfo(iInvSlot);
				}
				else
				{
					delete pMapItem;
					pMapItem = NULL;
				}
			}
		}
	}
	else
	{
		int iBeltSlot = iInvSlot - 50;
		if (iBeltSlot >= 0 && iBeltSlot < 4)
		{
			if (!m_BeltItem[iBeltSlot].IsEmpty())
			{
				if (CanTradeItem(m_BeltItem[iBeltSlot]))
				{
					ItemList* pMapItem = new ItemList;
					*pMapItem = m_BeltItem[iBeltSlot];

					if (m_BeltItem[iBeltSlot].bType > TYPE_ACC)
					{
						if (m_BeltItem[iBeltSlot].sUsage > 5)
						{
							int iUsage = ((myrand(20, 50) * m_BeltItem[iBeltSlot].sUsage) / 100);
							pMapItem->sUsage = iUsage;
							PlusItemDur(&m_BeltItem[iBeltSlot], iUsage, true); // Sets dura of item minus the amount being droped					
						}
						else
						{
							m_BeltItem[iBeltSlot].Init();
						}
					}
					else
					{
						m_BeltItem[iBeltSlot].Init();
					}

					if (UserThrowItem(pMapItem, 25))
					{
						if (bSendItemDelete)
						{
							SendDeleteItem(BELT_INV, iBeltSlot);
						}
						SendItemInfo(iBeltSlot);
					}
					else
					{
						delete pMapItem;
						pMapItem = NULL;
					}
				}
			}
		}
	}
}

void USER::DeadThrowMoney(bool bSendMoneyChanged)
{
	// DeadThrowMoney - Fix for dsoma. Has been Fixed.
	// Currently this does not work for dsoma because the moral values
	// for dsoma get negative when get better moral
	// so those with e.g. wicked moral on dsoma will lose more money than those
	// who PK and get virtuous.

	if (g_bClassWar) return;

	DWORD dwMoney = m_dwBarr;
	DWORD dwMoneyThrow = 0;

	// Calculate the amount of money player will be losing
	int iMoral = m_iMoral / CLIENT_MORAL;
	if (!CheckDemon(m_sClass))
	{
		if (iMoral <= -9)
		{
			dwMoneyThrow = (m_dwBarr * myrand(30, 50)) / 100;
		}
		else if (iMoral >= -8 && iMoral <= 10)
		{
			dwMoneyThrow = (m_dwBarr * myrand(10, 30)) / 100;
		}
		else
		{
			dwMoneyThrow = (m_dwBarr * myrand(0, 10)) / 100;
		}
	}
	else 
	{
		if (iMoral >= 9)
		{
			dwMoneyThrow = (m_dwBarr * myrand(30, 50)) / 100;
		}
		else if (iMoral <= 8 && iMoral >= -10)
		{
			dwMoneyThrow = (m_dwBarr * myrand(10, 30)) / 100;
		}
		else
		{
			dwMoneyThrow = (m_dwBarr * myrand(0, 10)) / 100;
		}
	}

	// No need to continue if there is going to be no money thrown
	if (dwMoneyThrow <= 0)
	{
		return;
	}

	// Adjust the amount of money player now has left
	if (dwMoney <= dwMoneyThrow) 
	{
		dwMoney = 0;
	}
	else
	{
		dwMoney -= dwMoneyThrow;
	}
	// TODO: DeadThrowMoney - UpdateUserData
	m_dwBarr = dwMoney;

	int iMoneyPileCount = myrand(3, 10);
	DWORD dwMoneyPileEach = dwMoneyThrow / iMoneyPileCount;

	for (int i = 0; i < iMoneyPileCount; i++)
	{
		if (dwMoneyThrow <= 0)
		{
			break;
		}

		DWORD dwMoneyPile = 0;
		if (myrand(0, 1))
		{
			dwMoneyPile = dwMoneyPileEach - (dwMoneyPileEach * myrand(0, 20)) / 100;
		}
		else
		{
			dwMoneyPile = dwMoneyPileEach + (dwMoneyPileEach * myrand(0, 20)) / 100;
		}

		if (dwMoneyThrow <= dwMoneyPile)
		{
			dwMoneyPile = dwMoneyThrow;
			dwMoneyThrow = 0;
		}
		else
		{
			dwMoneyThrow -= dwMoneyPile;
		}

		int iTryCount = 0;
		int iThrowX = -1;
		int iThrowY = -1;
		while (true)
		{
			iThrowX = m_sX + myrand(-2, 2);
			iThrowY = m_sY + myrand(-2, 2);

			if (IsMovable_S(iThrowX, iThrowY))
			{
				break;
			}

			iTryCount++;
			if (iTryCount >= 9)
			{
				return;
			}
		}

		CPoint pt = ConvertToClient(iThrowX, iThrowY);
		if (pt.x == -1 || pt.y == -1)
		{
			continue;
		}

		// Create the money item from item table loaded from database
		ItemList* pMapItem = new ItemList;
		if (pMapItem == NULL) continue;

		if (!pMapItem->InitFromItemTable(TYPE_MONEY_NUM)) continue;

		// Adds money to the money item to be put onto the map and checks that it doesn't go over the max etc
		CheckMaxValue((DWORD &)pMapItem->dwMoney, (DWORD)dwMoneyPile);

		if (!UserThrowItem(pMapItem, 25))
		{
			delete pMapItem;
			pMapItem = NULL;
			continue;
		}

		// TRACE("Dead Throw money: %d\n", pMapItem->dwMoney);
	}
		
	if (bSendMoneyChanged)
	{
		SendMoneyChanged();
	}
}

bool USER::CanTradeItem(const ItemList& item)
{
	if (item.bType == TYPE_UNTRADABLE)
	{
		return false;
	}

	if (item.sSpecial == SPECIAL_POS_GEM)
	{
		return false;
	}

	// FIXME: Hardcoded check for hsoma hero armor
	if (item.sNum >= 1850 && item.sNum <= 1865)
	{
		return false;
	}
	return true;
}

int USER::GetStoreItemData(short ItemNum, short Quantity, TCHAR* pData, int iPosValue, CString strName)
{
	int index = 0;
	if (ItemNum <= 0) return 0;
	CItemTable* pItem =	g_mapItemTable[ItemNum];
	if (pItem == NULL) return 0;
	
	SetShort(pData, ItemNum, index);
	SetShort(pData, Quantity, index); // Qauntity -1 = Unlimited

	SetByte(pData, pItem->m_bType, index);
	SetShort(pData, GetItemClass(ItemNum), index);
	SetShort(pData, pItem->m_sPicNum1, index);
	SetByte(pData, (BYTE)pItem->m_bGender, index);
	
	SetShort(pData, pItem->m_sAb1, index);
	SetShort(pData, pItem->m_sAb2, index);
	SetShort(pData, pItem->m_sWgt, index);
	SetShort(pData, pItem->m_sTime, index);
	SetShort(pData, pItem->m_sNeedStr, index);
	SetShort(pData, pItem->m_sNeedInt, index);
	SetShort(pData, pItem->m_sPicNum4, index);
	SetShort(pData, pItem->m_sMinExp, index);
	SetShort(pData, pItem->m_sHP, index);
	SetShort(pData, pItem->m_sMP, index);
	SetShort(pData, pItem->m_sDur, index);
	if (iPosValue != -1)
		SetDWORD(pData, (DWORD)iPosValue, index);
	else
    	SetDWORD(pData, (DWORD)pItem->m_iCost, index);
	if (strName.GetLength() != 0)
		SetVarString(pData, (LPTSTR)(LPCTSTR)strName, strName.GetLength(), index);
	else
		SetVarString(pData, (LPTSTR)(LPCTSTR)pItem->m_strName, pItem->m_strName.GetLength(), index);
	SetByte(pData, (BYTE)0, index); // item upgrade level
	index += GetSpecialItemData(ItemNum, pData + index);
	return index;
}

int USER::GetTraderItemData(CTraderExchangeTable* pTraderExchange, TCHAR* pData)
{
	int index = 0;
	short sItemNum = pTraderExchange->m_sItem;
	if (sItemNum <= 0) 
		return 0;
	CItemTable* pItem =	g_mapItemTable[sItemNum];
	if (pItem == NULL) 
		return 0;
					
	SetShort(pData, pTraderExchange->m_sId, index);
	SetShort(pData, pTraderExchange->m_sMoney, index);	
	for (int i = 0; i < 5; i++)
	{
		int sNeedItemNum = pTraderExchange->m_sNeedItem[i];
		if (sNeedItemNum > 0)
		{
			CItemTable* pNeedItem =	g_mapItemTable[sNeedItemNum];
			if (pNeedItem == NULL)
			{
				SetByte(pData, 0, index);
			}
			else
			{
				SetVarString(pData, (LPTSTR)(LPCTSTR)pNeedItem->m_strName, pNeedItem->m_strName.GetLength(), index);
				SetShort(pData, pTraderExchange->m_sNeedItemNum[i], index);
			}
		}
		else
		{
			SetByte(pData, 0, index);
		}
	}
	SetShort(pData, sItemNum, index);
	SetByte(pData, pItem->m_bType, index);
	SetShort(pData, GetItemClass(sItemNum), index);
	SetShort(pData, pItem->m_sPicNum1, index);
	SetByte(pData, (BYTE)pItem->m_bGender, index);
	SetShort(pData, pItem->m_sAb1, index);			// Min Damage
	SetShort(pData, pItem->m_sAb2, index);			// Max Damage
	SetShort(pData, pItem->m_sWgt, index);
	SetShort(pData, pItem->m_sTime, index);			// Speed
	SetShort(pData, pItem->m_sNeedStr, index);
	SetShort(pData, pItem->m_sNeedInt, index);
	SetShort(pData, pItem->m_sPicNum4, index);		// Need Dex
	SetShort(pData, pItem->m_sMinExp, index);		// Need Skill
	SetShort(pData, pItem->m_sHP, index);
	SetShort(pData, pItem->m_sMP, index);
	SetShort(pData, pItem->m_sDur, index);			// Usage
	SetDWORD(pData, (DWORD)pItem->m_iCost, index);
	SetVarString(pData, (LPTSTR)(LPCTSTR)pItem->m_strName, pItem->m_strName.GetLength(), index);
	SetByte(pData, (BYTE)0, index); // item upgrade level
	index += GetSpecialItemData(sItemNum, pData + index);
	return index;
}

void USER::SendRepairItem(BYTE bySuccess, BYTE bySubResult)
{	
	int index = 0;
	SetByte(m_TempBuf, PKT_REPAIR_REQ, index);
	SetByte(m_TempBuf, bySuccess, index);
	if (bySuccess == FAIL)
	{
		SetByte(m_TempBuf, bySubResult, index);
	}
	Send(m_TempBuf, index);
}

void USER::EffectFire()
{
	int index = 0;
	SetByte(m_TempBuf, PKT_EFFECT, index);
	SetByte(m_TempBuf, 2, index); // Type Light
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	Send(m_TempBuf, index);
}

int USER::CheckRemainFire()
{
	if (m_iLightItemNo < 0)
	{
		m_iLightItemNo = -1;
		m_dwLightItemTime = 0;
		return 0;
	}

	CItemTable* pItem = NULL;
	if (!g_mapItemTable.Lookup(m_iLightItemNo, pItem))
	{
		m_iLightItemNo = -1;
		m_dwLightItemTime = 0;
		return 0;
	}

	DWORD dwCurrTime = GetTickCount();
	DWORD dwTimeDiff = (dwCurrTime - m_dwLightItemTime) / 1000;
	if (dwTimeDiff <= static_cast<DWORD>(pItem->m_sSpOpt[0]))
	{
		m_dwLightItemTime = dwCurrTime;
		return pItem->m_sSpOpt[0] - dwTimeDiff;
	}

	m_iLightItemNo = -1;
	m_dwLightItemTime = 0;
	return 0;
}

void USER::SendExchangeItemFail()
{
	int index = 0;
	SetByte(m_TempBuf, PKT_TRADE_ADDITEM, index);
	SetByte(m_TempBuf, FAIL, index);
	Send(m_TempBuf, index);
}

void USER::SendExchangeItem(USER* pTradeUser, short sSlot, short sCount)
{
	if (!pTradeUser || pTradeUser->m_State != STATE_GAMESTARTED)
		return;

	CBufferEx TempBuf;
	TempBuf.Add(PKT_TRADE_ADDITEM);
	TempBuf.Add((BYTE)3);
	TCHAR pData[150];
	TempBuf.Add((short)sSlot);
	ItemList tradeItem;
	tradeItem = m_InvItem[sSlot];
	if (tradeItem.bType > TYPE_ACC)
		tradeItem.sUsage = sCount;
	TempBuf.AddData(pData, GetSendItemData(tradeItem, pData, ITEM_TYPE_EXCHANGE));
	pTradeUser->Send(TempBuf, TempBuf.GetLength());

	int index = 0;
	SetByte(m_TempBuf, PKT_TRADE_ADDITEM, index);
	SetByte(m_TempBuf, SUCCESS, index);
	Send(m_TempBuf, TempBuf.GetLength());
}
