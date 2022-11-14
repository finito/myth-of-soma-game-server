#pragma once

#include "Map.h"

class CThrowItem
{
public:
	int	m_z;
	int	m_x;
	int	m_y;
	DWORD dwTime;
	ItemList* m_pItem;

	CThrowItem();
	virtual ~CThrowItem();
};
