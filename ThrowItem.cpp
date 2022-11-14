#include "stdafx.h"
#include "ThrowItem.h"

CThrowItem::CThrowItem()
{
	m_pItem = NULL;
}

CThrowItem::~CThrowItem()
{
	if (m_pItem)
	{
		delete m_pItem;
		m_pItem = NULL;
	}
}
