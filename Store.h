#pragma once

struct StoreItem
{
	short sItemNo;
	short sQuantity;			// Holds quantity of item
	short sOriginalQuantity;
	short sRegen;				// Holds number of times before a big regen which is its value * 24 game hour
	short sRegenRate;
	short sOriginalRegen;
	short sMRegen;				// Holds number of times before a minor regen which is its value * 1 game hour
	short sMRegenRate;
	short sOriginalMRegen;
};

typedef CTypedPtrArray <CPtrArray, StoreItem*> ItemArray;

class CStore  
{
public:
	short m_sStoreID;
	BYTE m_tStoreType;

	ItemArray m_arItems;

	CStore();
	~CStore();
};
