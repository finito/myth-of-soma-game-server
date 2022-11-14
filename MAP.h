#pragma once

#define GET_VALUE 0x01
#define GET_EXTRA_USAGE 0x02
#define NORM_LAYOUT 0x04
#define TRADE_LAYOUT 0x08

#define ITEM_TYPE_STORAGE NORM_LAYOUT
#define ITEM_TYPE_ME GET_VALUE | GET_EXTRA_USAGE | NORM_LAYOUT
#define ITEM_TYPE_EXCHANGE TRADE_LAYOUT

#include "ItemTable.h"
typedef CMap<int, int, CItemTable*, CItemTable*> ItemTableMap;
extern ItemTableMap	g_mapItemTable;

class ItemList
{
public:
	short sNum;
	short sAb1;
	short sAb2;
	BYTE  bArm;
	short sAt;
	short sDf;
	DWORD dwMoney;
	BYTE bGender;		// 0 = All, 1 = Female, 2 = Male
	short sHP;
	short sMP;
	short sDur;
	CString strNewName;
	short sNeedInt;
	short sNeedStr;
	short sNeedDex;
	short sNeedSkill;
	short sMagicNo; // is this correct?
	short sMagicOpt; // is this correct?
	short sPicNum;
	BYTE  bType;
	short sWgt;
	short sTime;
	short sSpecial;
	short sSpOpt[MAX_SPECIAL_OPT];
	struct PlusSpecialList
	{
		short sSpecial;
		short sSpOpt[MAX_SPECIAL_OPT];
	};
	PlusSpecialList PlusSpecial[MAX_SPECIAL_OPT];

	short sUsage;
	short sMaxUsage;
	BYTE byUpgradeLevel;

	__int64 iItemSerial;

	bool bExt; // True if item has extended values other than those in the item table

	ItemList& operator =(ItemList& other)
	{
		dwMoney = other.dwMoney;
		sNum = other.sNum;
		sUsage = other.sUsage;
		bExt = other.bExt;
		bArm = other.bArm;
		bType = other.bType;
		sPicNum = other.sPicNum;
		strNewName = other.strNewName;
		sAb1 = other.sAb1;
		sAb2 = other.sAb2;
		sWgt = other.sWgt;
		sSpecial = other.sSpecial;
		for (int i = 0; i < MAX_SPECIAL_OPT; i++)
		{
			sSpOpt[i] = other.sSpOpt[i];
		}
		for (int i = 0; i < MAX_SPECIAL_OPT; i++)
		{
			PlusSpecial[i].sSpecial = other.PlusSpecial[i].sSpecial;
			for (int j = 0; j < MAX_SPECIAL_OPT; j++)
			{
				 PlusSpecial[i].sSpOpt[j] = other.PlusSpecial[i].sSpOpt[j];
			}
		}
		sTime = other.sTime;
		sMaxUsage = other.sMaxUsage;
		byUpgradeLevel = other.byUpgradeLevel;
		bGender = other.bGender;
		sHP = other.sHP;
		sMP = other.sMP;
		sDur = other.sDur;
		sNeedInt = other.sNeedInt;
		sNeedStr = other.sNeedStr;
		sNeedDex = other.sNeedDex;
		sNeedSkill = other.sNeedSkill;
		sMagicNo = other.sMagicNo;
		sMagicOpt = other.sMagicOpt;
		return *this;
	}

	ItemList()
	{
		Init();
	};

	void Init()
	{
		bExt = false;
		dwMoney = 0;
		sNum = 0;
		bArm = bType = -1;
		sPicNum = 0;
		sUsage =  sAb1 = sAb2 = sWgt = sSpecial = sTime = sMaxUsage = byUpgradeLevel = 0;
		for (int i = 0; i < MAX_SPECIAL_OPT; i++)
		{
			sSpOpt[i] = 0;
		}
		for (int i = 0; i < MAX_SPECIAL_OPT; i++)
		{
			PlusSpecial[i].sSpecial = 0;
			for (int j = 0; j < MAX_SPECIAL_OPT; j++)
			{
				 PlusSpecial[i].sSpOpt[j] = 0;
			}
		}
		bGender = -1;
		sAt = sDf = sHP = sMP = sDur = sNeedInt = sNeedStr = sNeedDex = sNeedSkill = 0;
		sMagicNo = sMagicOpt = 0;
		strNewName = "";
		iItemSerial = 0;
	}

	inline bool IsEmpty()
	{
		bool ret = false;
		if (sNum <= 0)
		{
			ret = true;
		}
		return ret;
	}

	inline bool IsDemonItem()
	{
		bool ret = false;
		if (sNum >= DEVIL_ITEM_BAND)
		{
			ret = true;
		}
		return ret;
	}

	inline bool IsPlusSpecial()
	{
		return sSpecial == SPECIAL_PLUS	? true : false;
	}

	inline int GetAttack()
	{
		return myrand(sAb1, sAb2);
	}

	inline int GetDefense()
	{
		if (bType != TYPE_ARMOR)
		{
			return 0;
		}

		return myrand(sAb1, sAb2);
	}

	bool InitFromItemTable(short sNum)
	{
		CItemTable *pItem = NULL;
		if (!g_mapItemTable.Lookup(sNum, pItem))
		{
			return false;
		}

		Init();

		this->sNum = pItem->m_sNum;
		this->sAb1 = pItem->m_sAb1;
		this->sAb2 = pItem->m_sAb2;
		this->bArm = pItem->m_bArm;
		this->sAt = pItem->m_sAt;
		this->dwMoney = pItem->m_iCost;
		this->sDf = pItem->m_sDf;
		this->bGender = pItem->m_bGender;
		this->sHP = pItem->m_sHP;
		this->sDur = pItem->m_sDur;
		this->sMP = pItem->m_sMP;
		this->strNewName = pItem->m_strName;
		this->sNeedInt = pItem->m_sNeedInt;
		this->sNeedStr = pItem->m_sNeedStr;
		this->sNeedDex = pItem->m_sPicNum4;
		this->sNeedSkill = pItem->m_sMinExp;
		this->sMagicNo = pItem->m_sMagicNo; // ????
		this->sPicNum = pItem->m_sPicNum1;
		this->sMagicOpt = pItem->m_sMagicOpt; // ????
		this->bType = pItem->m_bType;
		this->sWgt = pItem->m_sWgt;
		this->sTime = pItem->m_sTime;
		this->sSpecial = pItem->m_sSpecial;
		for (int i = 0; i < MAX_SPECIAL_OPT; i++)
		{
			this->sSpOpt[i] = pItem->m_sSpOpt[i];
		}
		this->strNewName = pItem->m_strName;
		this->sMaxUsage = this->sUsage = pItem->m_sDur;
		return true;
	}
};

#define MAP_NON_MOVEABLE 0
#define MAP_MOVEABLE 1

class MapInfo
{
public:
	BOOL m_bMove;
	LONG m_lUser;
	BYTE m_nAreaNumber;

	int iIndex;
	LONG m_FieldUse;

	MapInfo();
	virtual ~MapInfo();
};

class MoveInfo
{
public:
	BOOL	m_bMovable;
	BYTE	m_nAreaNumber;

	MoveInfo();
	virtual ~MoveInfo();
};

class MoveCell
{
public:
	MoveCell();
	~MoveCell();

	void RemoveData();
	void Initialize(int cx, int cy);
	void ConvertMap(MapInfo** tmap);

	void Load(CArchive& ar);
	void LoadSize(CArchive& ar);

	CSize m_vDim;
	MoveInfo** m_pMoveInfo;
};

class MAP
{
public:
	void SetType(int x, int y, int nType);
	void InitTargetMap(int cx, int cy);
	BOOL LoadImf(char *szFileName);
	BOOL LoadImfSize(char* szFileName);
	BOOL LoadMapByNum(int num);
	BOOL LoadMapSizeByNum(int num);

	MoveCell m_vMoveCell;

	CSize m_sizeMap;
	MapInfo** m_pMap;
	int	m_Zone;

	MAP();
	virtual ~MAP();

protected:
	void RemoveMapData();
};
