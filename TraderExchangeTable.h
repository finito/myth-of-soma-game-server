#pragma once

class CTraderExchangeTable  
{
public:
	short m_sId;
	short m_sType;
	short m_sItem;
	short m_sMoney;
	short m_sNeedItem[5];
	short m_sNeedItemNum[5];

	CTraderExchangeTable();
	~CTraderExchangeTable();
};
